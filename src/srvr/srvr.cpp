

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <thread>
#include <optional>
#include <memory>
#include <sstream>

#include "srvr.h"
#include "commands_srvr.h"
#include "logger.h"


std::unique_ptr<SrvrCmdMgr>	Srvr::m_CMD_MGR{new SrvrCmdMgr{}};

/*
-- class Srvr
*/

bool Srvr::activate()
{
	Logger::instance().start("rediscc_srvr");
	bool rc{};
	m_listenFd = open_socket();
	rc = assign_protocol_address();
	if (!rc) {
		close_socket(m_listenFd);
		return false;
	}
	make_socket_passive_listen();
	rc = acceptor();
	if (!rc) {
		close_socket(m_listenFd);
		return false;
	}
	return rc;
}

bool RedisSrvr::acceptor()
{
	struct sockaddr_in clntaddr;
	socklen_t clntlen = sizeof(clntaddr);

	fd_set	read_fdset;
	int maxfd = m_listenFd;
	
	for ( ; ; ) {
		
		FD_ZERO(&read_fdset);
		FD_SET(m_listenFd,&read_fdset);

		int nready = select(maxfd+1,&read_fdset,NULL,NULL,NULL);
		if (nready ==-1) {
			Logger::instance().log_error(" select ready sockets number is: ", std::to_string(nready));
			continue;	
		}
		if (nready > 0) {
			Logger::instance().log_info(" select ready sockets number is: ", std::to_string(nready));
		}

		if (FD_ISSET(m_listenFd, &read_fdset)) {

			errno = 0;
			int connfd = accept(m_listenFd
								,reinterpret_cast<sockaddr*>(&clntaddr)
								,&clntlen);
			if (connfd != -1) {
				//fcntl(connfd,F_SETFL,O_NONBLOCK);
			} else {
				if (errno==EAGAIN || errno==EWOULDBLOCK)
					break;
				else {
					close_socket(m_listenFd);
					return false;
				}
			}

			std::thread*  pconnmgr=new std::thread(&RedisSrvr::conn_mgr,this
													,connfd
													,std::ref(*m_CMD_MGR));
			m_conn_thrds.push_back(pconnmgr);
		}
	}
	
	for (auto pconnmgr : m_conn_thrds) {
		pconnmgr->join();
	}

	return false;
}


bool RedisSrvr::recv_and_respond(int connfd,SrvrCmdMgr& cmdMgr)
{
	int flags{};
	int BUFF_SZ {4096};
	int cycles{};

	std::stringstream ss;
	ss << std::this_thread::get_id();
	Logger::instance().log_info(" recv_and_respond threadid:", ss.str()," connfd:", std::to_string(connfd));

	while (true) {

		char buf[BUFF_SZ];
		int ret_data = recv(connfd,buf,BUFF_SZ,flags);

		if (ret_data == 0 && !cycles) 
			return false;

		if (ret_data == 0 && cycles) 
			return true;


		++cycles;
		std::optional<std::string> rspns=cmdMgr.process_and_respond(buf
																	,BUFF_SZ
																	,*this);

		if (rspns) {

				errno = 0;
				ssize_t ret_snd=send(connfd
									,(*rspns).c_str()
									,(*rspns).length()
									,flags);
				if (ret_snd != static_cast<ssize_t>((*rspns).length())){
					Logger::instance().log_error(" msg was not sent completly msg is:", *rspns);
					if (ret_snd == -1) {
						Logger::instance().log_error(" errno of send:", std::to_string(errno));
				}
			}
		}

	}

	return true;
}

void RedisSrvr::conn_mgr(int connfd,SrvrCmdMgr& cmdMgr)
{

	fd_set	rd_fdset;
	fd_set	wrt_fdset;
	fd_set	err_fdset;
	int maxfd = connfd;
	int numready{};

	for ( ; ; ) {

		FD_ZERO(&rd_fdset);
		FD_SET(connfd,&rd_fdset);
		FD_ZERO(&wrt_fdset);
		FD_SET(connfd,&wrt_fdset);
		FD_ZERO(&err_fdset);
		FD_SET(connfd,&err_fdset);

		numready = select(maxfd+1,&rd_fdset,&wrt_fdset,&err_fdset,NULL);
		if (numready == -1) {
			Logger::instance().log_error(" conn_mgr select ready sockets number is: ", std::to_string(numready));
			continue;
		}
		if (numready > 0) {
			Logger::instance().log_info(" conn_mgr select ready sockets number is: ", std::to_string(numready));
		}

		if (FD_ISSET(connfd, &rd_fdset)) {
			if (!recv_and_respond(connfd,cmdMgr)) {
				break;
			}
		}
		if (FD_ISSET(connfd, &wrt_fdset)) {
		}
		if (FD_ISSET(connfd, &err_fdset)) {
		}
	}

	close_socket(connfd);
	Logger::instance().log_info(" close socket in thread. connfd:", std::to_string(connfd));
}

bool RedisSrvr::insert2memdb(std::string key,std::string val)
{
	return m_memDB.insert(key,val);
}
bool RedisSrvr::insert2memdb(std::string key,std::string val,int seconds)
{
	return m_memDB.insert(key,val,seconds);
}
std::optional<MemDB::ValueVariant_t> RedisSrvr::retrieve_from_memdb(std::string key)
{
	return m_memDB.retrieve(key);
}
/*
-- class TcpSrvr
*/
int TcpSrvr::open_socket()
{
	int listenfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (listenfd == -1) {
		Logger::instance().log_error(" could not open socket");
	}

/*	int opt=1;
	if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR
					,reinterpret_cast<char*>(&opt)
					,sizeof(opt)) == -1) {
		std::cout << "ERR:cant setsockopt REUSEADDR on "<<listenfd<<std::endl;
		close_socket(listenfd);
	}
*/	
	return listenfd;
}

bool TcpSrvr::make_socket_passive_listen() 
{
	errno=0;
	if (listen(m_listenFd,m_backlog)) {
		Logger::instance().log_error(" listen failed on ", std::to_string(m_listenFd)," errno is:", std::to_string(errno));
		close_socket(m_listenFd);
		return false;
	}
	//fcntl (m_listenFd,F_SETFL,O_NONBLOCK);
	return true;
}

/*
-- class RedisSrvr
*/
bool RedisSrvr::assign_protocol_address()
{
	struct sockaddr_in	srvraddr;
	srvraddr.sin_family = AF_INET;
	srvraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvraddr.sin_port = htons(RedisSrvr::REDIS_PORT);
	if (!bind(m_listenFd
				,reinterpret_cast<struct sockaddr*>(&srvraddr)
				,sizeof(srvraddr))) 
		return true;
	return false;
}


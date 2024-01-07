

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <thread>
#include <optional>

#include "../../include/srvr.h"


CmdMgr	Srvr::m_CMD_MGR{};

/*
-- class Srvr
*/

bool Srvr::activate()
{
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

bool Srvr::acceptor()
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
			std::cout << "ERR: select ready sockets number is: "<<nready<<std::endl;
			continue;	
		}
		if (nready > 0) {
			std::cout << "INFO: select ready sockets number is: "<<nready<<std::endl;
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

			std::thread*  pconnmgr=new std::thread(&Srvr::conn_mgr,this
													,connfd
													,std::ref(m_CMD_MGR));
			m_conn_thrds.push_back(pconnmgr);
		}
	}
	
	for (auto pconnmgr : m_conn_thrds) {
		pconnmgr->join();
	}

	return false;
}


bool Srvr::recv_and_respond(int connfd,CmdMgr& cmdMgr)
{
	int flags{};
	int BUFF_SZ {4096};
	int cycles{};

	std::cout << "INFO: recv_and_respond threadid:" << std::this_thread::get_id() << " connfd:" << connfd << std::endl;

	while (true) {

		char buf[BUFF_SZ];
		int ret_data = recv(connfd,buf,BUFF_SZ,flags);

		if (ret_data == 0 && !cycles) 
			return false;

		if (ret_data == 0 && cycles) 
			return true;


		++cycles;
		std::optional<std::string> rspns=cmdMgr.process_and_respond(buf
																	,BUFF_SZ);

		if (rspns) {

				errno = 0;
				ssize_t ret_snd=send(connfd
									,(*rspns).c_str()
									,(*rspns).length()
									,flags);
				if (ret_snd != static_cast<ssize_t>((*rspns).length())){
					std::cout << "ERR: msg was not sent completly msg is:" << *rspns << std::endl;
					if (ret_snd == -1) {
						std::cout << " ERR: errno of send:" << errno << std::endl;
				}
				else {
					std::cout << "INFO: msg sent is:" << *rspns << std::endl;
				}
			}
		}

	}

	return true;
}

void Srvr::conn_mgr(int connfd,CmdMgr& cmdMgr)
{
	//recv_and_respond(connfd,cmdMgr);

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
			std::cout << "ERR: conn_mgr select ready sockets number is: "<<numready<<std::endl;
			continue;
		}
		if (numready > 0) {
			std::cout << "INFO: conn_mgr select ready sockets number is: "<<numready<<std::endl;
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
	std::cout << "INFO: close socket in thread. connfd:" << connfd << std::endl;
}
/*
-- class TcpSrvr
*/
int TcpSrvr::open_socket()
{
	int listenfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (listenfd == -1) {
		std::cout << "ERR: could not open socket" << std::endl; 
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
		std::cout << "ERR: listen failed on " << m_listenFd << " errno is:" << errno << "backlog is:" << m_backlog << std::endl;
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



#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>

#include "clnt.h"
#include "resp.h"
#include "commands_clnt.h"

ClntCmdMgr RedisClnt::m_CMD_MGR;

bool Clnt::activate()
{
	if (!open_socket())
		return false;
	if (!connect_to_server()) {
		return false;
	}
 	return true;	
}

bool TcpClnt::open_socket()
{
	m_connFd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_connFd == -1)
		return false;
	return true;
}

bool RedisClnt::connect_to_server()
{
	struct sockaddr_in srvraddr;
	srvraddr.sin_family = AF_INET;
	srvraddr.sin_port = htons(RedisClnt::REDIS_PORT);
	struct hostent	*localhost=gethostbyname("127.0.0.1");
	srvraddr.sin_addr = *(reinterpret_cast<struct in_addr*>(localhost->h_addr));
	errno=0;
	int conn_rc=connect(m_connFd
						,reinterpret_cast<struct sockaddr*>(&srvraddr)
						,sizeof(srvraddr));
	if (conn_rc==-1) {
		std::cout<<"ERR: connect fail on connfd:" << m_connFd << " with errno:" << errno << std::endl;
		close(m_connFd);
		return false;
	}

	return true;
}

bool RedisClnt::activate_and_process_ping()
{
	std::optional<std::string> pingreq=m_CMD_MGR.build_ping_request();
	int flags{};
	errno = 0;
    int ret_snd=send(m_connFd
                     ,(*pingreq).c_str()
                     ,(*pingreq).length()
                     ,flags);
	if (ret_snd != static_cast<int>((*pingreq).length())) {
		std::cout << "ERR: not send all buff:" << (*pingreq) << std::endl;
	}

    int BUFF_SZ {4096};
	char buf[BUFF_SZ];
    int ret_data = recv(m_connFd,buf,BUFF_SZ,flags);
	if (ret_data <= 0) return false;
	std::optional<std::string> pong=m_CMD_MGR.process_response(buf,BUFF_SZ);
	if (!pong) return false;
	std::cout << *pong << std::endl;
	return true;
	

}
bool RedisClnt::activate_and_process_set(std::string key,std::string value)
{
	std::pair<std::string,std::string> kvpair{key,value};
	std::optional<std::string> setreq=m_CMD_MGR.build_set_request(kvpair);
	int flags{};
	errno = 0;
    int ret_snd=send(m_connFd
                     ,(*setreq).c_str()
                     ,(*setreq).length()
                     ,flags);
	if (ret_snd != static_cast<int>((*setreq).length())) {
		std::cout << "ERR: not send all buff:" << (*setreq) << std::endl;
	}

    int BUFF_SZ {4096};
	char buf[BUFF_SZ];
    int ret_data = recv(m_connFd,buf,BUFF_SZ,flags);
	if (ret_data <= 0) return false;
	std::optional<std::string> ok=m_CMD_MGR.process_response(buf,BUFF_SZ);
	if (!ok) return false;
	std::cout << *ok << std::endl;
	return true;
}
bool RedisClnt::activate_and_process_get(std::string key)
{
	std::optional<std::string> setreq=m_CMD_MGR.build_get_request(key);
	int flags{};
	errno = 0;
    int ret_snd=send(m_connFd
                     ,(*setreq).c_str()
                     ,(*setreq).length()
                     ,flags);
	if (ret_snd != static_cast<int>((*setreq).length())) {
		std::cout << "ERR: not send all buff:" << (*setreq) << std::endl;
	}

    int BUFF_SZ {4096};
	char buf[BUFF_SZ];
    int ret_data = recv(m_connFd,buf,BUFF_SZ,flags);
	if (ret_data <= 0) return false;
	std::optional<std::string> ok=m_CMD_MGR.process_response(buf,BUFF_SZ);
	if (!ok) return false;
	std::cout << *ok << std::endl;
	return true;
}

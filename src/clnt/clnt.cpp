
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>

#include "../../include/clnt.h"

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


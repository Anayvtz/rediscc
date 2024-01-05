
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <optional>
#include <thread>

#include "commands.h"
#include "rediscc.h"

class Srvr
{
public:

	explicit Srvr(int backlog) : m_listenFd{},m_backlog{backlog} {}
	bool activate();

protected:

	virtual int open_socket() = 0 ;
	void close_socket(int fd) { close(fd); }
	virtual bool assign_protocol_address() = 0 ;
	virtual bool make_socket_passive_listen() = 0 ;
	virtual bool acceptor();
	void conn_mgr(int connfd,CmdMgr& cmdMgr);
	void recv_and_respond(int connfd,CmdMgr& cmdMgr);

	int m_listenFd;
	int m_backlog;

	static CmdMgr	m_CMD_MGR;
	std::vector<std::thread*>	m_conn_thrds;

private:

};

class TcpSrvr : public Srvr
{
public:
	
	enum { BACKLOG=7/*128*/ };

	TcpSrvr() : Srvr(TcpSrvr::BACKLOG) {} 

protected:

	virtual int open_socket();
	virtual bool make_socket_passive_listen();

private:
};

class RedisSrvr : public TcpSrvr
{
public:

	enum Ports : int { REDIS_PORT=REDIS_SRVR_PORT };

protected:

	virtual bool assign_protocol_address();

private:
};

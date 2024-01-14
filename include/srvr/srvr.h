
#ifndef _SRVR_H
#define _SRVR_H

#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <optional>
#include <thread>
#include <memory>

#include "memdb.h"
#include "rediscc.h"
#include "logger.h"

class SrvrCmdMgr;

class Srvr
{
public:

	explicit Srvr(int backlog) : m_listenFd{},m_backlog{backlog} {}
	virtual ~Srvr() { Logger::instance().stop();}

	bool activate();

protected:

	virtual int open_socket() = 0 ;
	void close_socket(int fd) { close(fd); }
	virtual bool assign_protocol_address() = 0 ;
	virtual bool make_socket_passive_listen() = 0 ;
	virtual bool acceptor() = 0;

	int m_listenFd;
	int m_backlog;

	static std::unique_ptr<SrvrCmdMgr>	m_CMD_MGR;
	std::vector<std::thread*>	m_conn_thrds;

private:

};

class TcpSrvr : public Srvr
{
public:
	
	enum { BACKLOG=128 };

	TcpSrvr() : Srvr(TcpSrvr::BACKLOG) {} 
	virtual ~TcpSrvr() {}

protected:

	virtual int open_socket();
	virtual bool make_socket_passive_listen();

private:
};

class RedisSrvr : public TcpSrvr
{
public:

	enum Ports : int { REDIS_PORT=REDIS_SRVR_PORT };

	virtual ~RedisSrvr() {}

	bool insert2memdb(std::string key,std::string val);
	bool insert2memdb(std::string key,std::string val,int seconds);
	std::optional<MemDB::ValueVariant_t> retrieve_from_memdb(std::string key);
protected:

	virtual bool assign_protocol_address();
	virtual bool acceptor();
	void conn_mgr(int connfd,SrvrCmdMgr& cmdMgr);
	bool recv_and_respond(int connfd,SrvrCmdMgr& cmdMgr);

	MemDB	m_memDB;

private:
};
#endif

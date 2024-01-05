
#include <unistd.h>
#include "rediscc.h"

/*
--
*/
class Clnt
{
public:

	bool activate();

protected:

	virtual bool open_socket() = 0;
	void close_socket(int fd) { close(fd); }

	virtual bool connect_to_server() = 0;
	
	int m_connFd;

private:

};

class TcpClnt : public Clnt
{
public:
protected:

	virtual bool open_socket(); 

private:
};

class RedisClnt : public TcpClnt
{
public:

	enum Ports : int { REDIS_PORT=REDIS_SRVR_PORT };

protected:

	virtual bool connect_to_server();

private:
};

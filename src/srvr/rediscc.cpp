
#include <cstdio>
#include <string>
#include <iostream>

#include "rediscc.h"
#include "resp.h"
#include "srvr.h"
#include "logger.h"

int main()
{

	RedisSrvr srvr;
	srvr.activate();

	return 0;
}


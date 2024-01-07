
#include "../../include/clnt.h"

int main()
{
	RedisClnt clnt;
	clnt.activate();
	clnt.activate_and_process_ping();
	return 0;
}


#include "clnt.h"

int main()
{
	RedisClnt clnt;
	clnt.activate();
	clnt.activate_and_process_ping();
	clnt.activate_and_process_set("Name","John");
	clnt.activate_and_process_set("food","falafel");
	clnt.activate_and_process_get("Name");
	clnt.activate_and_process_get("food");
	return 0;
}

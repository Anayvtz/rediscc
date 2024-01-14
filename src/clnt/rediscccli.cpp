
#include "clnt.h"
#include "logger.h"
#include "resp.h"

int main(int argc, char** argv)
{
	Logger::instance().start("rediscc_clnt");
	RedisClnt clnt;
	clnt.activate();
	switch (argc) {
		case 1:
		clnt.activate_and_process_ping();
		clnt.activate_and_process_set("Name","John");
		clnt.activate_and_process_set("food","falafel");
		clnt.activate_and_process_get("Name");
		clnt.activate_and_process_get("food");
		break;

		case 2:
		{
		std::optional<std::string> srlzd=Resp::serialize(Resp::SIMPLE_STR,std::string(argv[1]));
		if (!srlzd) return -1;
		clnt.activate_and_process_cmd(*srlzd);
		}
		break;

		case 3:
		{
		std::list<std::string> lst{argv[1],argv[2]};	
		std::optional<std::string> srlzd=Resp::serialize(Resp::ARR,lst);
		if (!srlzd) return -1;
		clnt.activate_and_process_cmd(*srlzd);
		}
		break;
	
		case 4:
		{
		std::list<std::string> lst{argv[1],argv[2],argv[3]};	
		std::optional<std::string> srlzd=Resp::serialize(Resp::ARR,lst);
		if (!srlzd) return -1;
		clnt.activate_and_process_cmd(*srlzd);
		}
		break;

		case 5:
		{
		std::list<std::string> lst{argv[1],argv[2],argv[3],argv[4]};	
		std::optional<std::string> srlzd=Resp::serialize(Resp::ARR,lst);
		if (!srlzd) return -1;
		clnt.activate_and_process_cmd(*srlzd);
		}
		break;
	}
		
	return 0;
}

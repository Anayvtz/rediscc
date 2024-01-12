
#ifndef _COMMANDS_SRVR_H
#define _COMMANDS_SRVR_H

#include <string>
#include <array>
#include <optional>
#include <iostream>
#include <vector>


#include "commands.h"

class RedisSrvr;

class SrvrCmdMgr : public CmdMgr
{
public:

	std::optional<std::string> process_and_respond(char* buf,int sz,RedisSrvr&);

	std::string build_ok_response();
	std::string build_pong_response();
	std::string build_version_response();
	std::optional<std::string> build_err_response(std::string);
	std::optional<std::string> build_echo_response(char* str,int sz);
	std::optional<std::string> build_set_response(char* str,int sz,RedisSrvr&);
	std::optional<std::string> build_setex_response(char* str,int sz,RedisSrvr& rediss);
	std::optional<std::string> build_get_response(char* str,int sz,RedisSrvr&);
	
protected:

private:
};

#endif

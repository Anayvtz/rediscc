
#ifndef _COMMANDS_CLNT_H
#define _COMMANDS_CLNT_H

#include <string>
#include <array>
#include <optional>
#include <iostream>
#include <vector>
#include <utility>

#include "commands.h"

class ClntCmdMgr : public CmdMgr
{
public:


	std::optional<std::string> build_ping_request();
	std::optional<std::string> build_hello_request();
	std::optional<std::string> build_echo_request(std::string );
	std::optional<std::string> build_set_request(std::pair<std::string,std::string>& );
	std::optional<std::string> build_get_request(std::string );

	std::optional<std::string> process_response(char*,int);
	//std::optional<std::string> process_set_response(char* str,int sz);

protected:

private:
};

#endif

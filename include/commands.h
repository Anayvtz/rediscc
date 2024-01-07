
#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <string>
#include <array>
#include <optional>
#include <iostream>
#include <vector>



class CmdMgr
{
public:


	enum CmdType : int { PING=0, HELLO, ECHO, NOT_SUPPORTED};

	std::optional<std::string> process_and_respond(char* buf,int sz);

	CmdType find_cmd(char* str,int sz);
	CmdType find_cmd_bulk(char* str,int sz);
	CmdType find_cmd_arr(char* str,int sz);

	std::string build_err_response();
	std::string build_pong_response();
	std::string build_version_response();
	std::optional<std::string> build_echo_response(char* str,int sz);
	
	std::optional<std::string> build_ping_request();
	void build_hello_request();
	void build_echo_request(std::string echostr);

	std::optional<std::string> process_pong_response(char*,int);

protected:

	static std::vector<std::string> m_CMD_TYP;

private:
};

#endif

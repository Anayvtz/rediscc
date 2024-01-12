
#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <string>
#include <array>
#include <optional>
#include <iostream>
#include <vector>


class RedisSrvr;

class CmdMgr
{
public:


	enum CmdType : int { PING=0, HELLO, ECHO, SET, GET, SETEX, NOT_SUPPORTED};

	std::optional<std::string> process_and_respond(char* buf,int sz,RedisSrvr&);

	CmdType find_cmd(char* str,int sz);
	CmdType find_cmd_bulk(char* str,int sz);
	CmdType find_cmd_arr(char* str,int sz);

protected:

	static std::vector<std::string> m_CMD_TYP;

private:
};

#endif

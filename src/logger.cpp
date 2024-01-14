
#include <fstream>
#include <mutex>
#include <cstring>
#include <string>
#include <optional>
#include <source_location>

#include	"logger.h" 

bool Logger::start(std::string fileName)
{
	m_logFullName="/tmp/";
	m_logFullName+=fileName;
	m_logFullName+="_";
	m_logFullName+=get_date();
	
	m_logOfstream.open(m_logFullName.c_str(), std::ios_base::out | std::ios_base::app );	
	return true;
}
std::string Logger::get_date()
{
	time_t now{time(0)};
	struct tm tmstruct{*localtime(&now)};
	char buff[256];
	strftime(buff,sizeof(buff),"%Y-%m-%d",&tmstruct);
	return std::string(buff,strlen(buff));
}
std::string Logger::get_date_and_time()
{
	time_t now{time(0)};
	struct tm tmstruct{*localtime(&now)};
	char buff[256];
	strftime(buff,sizeof(buff),"%Y-%m-%d %X",&tmstruct);
	return std::string(buff,strlen(buff));
}
std::ofstream& Logger::log_error(std::string msg1,std::optional<std::string> msg2,std::optional<std::string> msg3,std::optional<std::string> msg4,std::source_location loc)
{
	std::lock_guard<std::mutex> lg(m_mtx);
	m_logOfstream << get_date_and_time() << " ERR: " << loc.function_name() << " linu-num:"<< loc.line() << " : " << msg1;
	if (msg2) m_logOfstream << *msg2;
	if (msg3) m_logOfstream << *msg3;
	if (msg4) m_logOfstream << *msg4;
	m_logOfstream << "\r\n";
	return m_logOfstream;
}

std::ofstream& Logger::log_info(std::string msg1, std::optional<std::string> msg2, std::optional<std::string> msg3, std::optional<std::string> msg4,std::source_location loc)
{
	std::lock_guard<std::mutex> lg(m_mtx);
	m_logOfstream << get_date_and_time() << " INFO: " << loc.function_name() << " line-num:" << loc.line() << " : " << msg1;
	if (msg2) m_logOfstream << *msg2;
	if (msg3) m_logOfstream << *msg3;
	if (msg4) m_logOfstream << *msg4;
	m_logOfstream << "\r\n";
	return m_logOfstream;
}

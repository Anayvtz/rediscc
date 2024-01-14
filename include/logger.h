#ifndef _LOGGER_H
#define _LOGGER_H

#include <iostream>
#include <mutex>
#include <fstream>
#include <string>
#include <optional>
#include <source_location>

class Logger
{
public:
	static Logger& instance()
	{
		static Logger logger;
		return logger;
	}

	~Logger() {
		stop();
	}

	bool start(std::string filename);
	void stop() { m_logOfstream.close(); }

	std::ofstream& log_error(std::string,std::optional<std::string> msg2=std::nullopt,std::optional<std::string> msg3=std::nullopt,std::optional<std::string> msg4=std::nullopt,std::source_location loc=std::source_location::current());
	std::ofstream& log_info(std::string,std::optional<std::string> msg2=std::nullopt,std::optional<std::string> msg3=std::nullopt,std::optional<std::string> msg4=std::nullopt,std::source_location loc=std::source_location::current());
	

protected:

	Logger() {}

	std::string get_date();
	std::string get_date_and_time();

	std::mutex	m_mtx;
	std::string	m_logFullName;
	std::ofstream m_logOfstream;

private:
};

#endif

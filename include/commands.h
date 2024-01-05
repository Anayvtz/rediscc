
#include <string>
#include <array>
#include <optional>



class CmdMgr
{
public:

	std::optional<std::string> process_response(char* buf,int sz);

protected:

	static std::array<std::string,3> m_CMD_TYP;

private:
};

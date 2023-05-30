# include "utils.hpp"
# include "ANSI.hpp"
# include <string>
# include "../parsers/Message.hpp"

int	error(const std::string &str, const int ret)
{
	std::cerr << ANSI::red << ANSI::bold << "ERROR:\t" << ANSI::reset << ANSI::red
		<< str << ANSI::reset << std::endl;
	return(ret);
}

void	error(const std::string &str)
{
	std::cerr << ANSI::red << ANSI::bold << "ERROR:\t" << ANSI::reset << ANSI::red
		<< str << ANSI::reset << std::endl;
}

const std::string	irc_tolower(const std::string &str)
{
	std::string	res(str);

	for (size_t i = 0; i < str.size(); ++i)
	{
		if (res[i] >= 'A' && res[i] <= ']')
			res[i] = str[i] + 32;
	}
	return (res);
}

static int	ft_nbrlen(const int nb)
{
	unsigned int	n;
	size_t			i;

	if (nb == 0)
		return (1);
	i = 0;
	if (nb < 0)
	{
		n = -nb;
		i++;
	}
	else
		n = nb;
	while (n)
	{
		n /= 10;
		i++;
	}
	return (i);
}

void	time(const int time)
{
	int	indent;
	int	size;

	size = ft_nbrlen(time);
	indent = (INDENT_TIME - size) / 2;
	if (indent < 0)
		indent = 0;

	std::cout << ANSI::time << ANSI::bold << ANSI::italic <<
		'<' << std::string(indent, ' ') << time
		<< std::string((size % 2 == 0 ? indent + 1 : indent), ' ') << '>'
		<< ANSI::reset << std::endl;
}

std::string	s_time(const int time)
{
	std::stringstream	o;
	int					indent;
	int					size;

	size = ft_nbrlen(time);
	indent = (INDENT_TIME - size) / 2;
	if (indent < 0)
		indent = 0;

	o << ANSI::time << ANSI::bold << ANSI::italic <<
		'<' << std::string(indent, ' ') << time
		<< std::string((size % 2 == 0 ? indent + 1 : indent), ' ') << '>'
		<< ANSI::reset;

	return (o.str());
}

std::vector<std::string>	ft_split(std::string str, std::string delimiter)
{
	std::vector<std::string> values = std::vector<std::string>();

	size_t position;
	while ((position = str.find(delimiter)) != std::string::npos)
	{
		values.push_back(str.substr(0, position));
		str.erase(0, position + delimiter.length());
	}
	values.push_back(str);

	return values;
}

const std::string	itostring(int i, bool code)
{
	std::stringstream	o;

	o << i;
    std::string str = o.str();
    if (code)
    {
        if (str.size() == 1)
            return "00" + str;
	    else if (str.size() == 2)
            return "0" + str;
    }
    return str;
}

int ft_stoi(const std::string & s)
{
    int i;
    std::istringstream(s) >> i;
    return i;
}

// namespace Debug {
// 	MsgType::MsgType(){
// 		add("BLANK");
// 		add("IRC_INVALID");
// 		add("IRC_PASS");
// 		add("IRC_NICK");
// 		add("IRC_USER");
// 		add("IRC_OPER");
// 		add("IRC_MODE");
// 		add("IRC_SERVICE");
// 		add("IRC_QUIT");
// 		add("IRC_SQUIT");

// 		add("IRC_JOIN");
// 		add("IRC_PART");
// 		add("IRC_CMODE");
// 		add("IRC_TOPIC");
// 		add("IRC_NAMES");
// 		add("IRC_LIST");
// 		add("IRC_INVITE");
// 		add("IRC_KICK");
// 	}

// 	MsgType::~MsgType() {}

// 	void MsgType::add(const std::string& str)
// 	{
// 		__msgVec.push_back(str);
// 	}

// 	std::string& MsgType::toStr(int type)
// 	{
// 		return __msgVec.at(type);
// 	}
// }

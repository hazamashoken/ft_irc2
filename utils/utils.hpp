# pragma once

# ifndef DEBUG
#  define DEBUG	0
# endif

# define INDENT_TIME	3
# define INDENT_SRC		2

# include "ANSI.hpp"

# include <iostream>
# include <sstream>
# include <fstream>
# include <string>
# include <vector>
# include <ctime>
# include <vector>


std::vector<std::string>	ft_split(std::string str, std::string delimiter);

int					error(const std::string &str, const int ret);
void				error(const std::string &str);
void				time(const int time);
std::string			s_time(const int time);
const std::string	irc_tolower(const std::string& str);
const std::string	itostring(int i, bool code);
int					ft_stoi(const std::string & s);

namespace Debug {

	// std::vector<std::string> msgType;
	// class MsgType{
	// 	public:
	// 		MsgType();
	// 		~MsgType();

	// 		std::string& toStr(int type);
	// 		void add(const std::string& str);

	// 	private:
	// 		std::vector<std::string> __msgVec;
	// };

	void				debug(const std::string &str);
	void				debug(const std::string &src, const std::string &str);
	void				debug(const std::ostream& str);
	std::string			s_debug(const std::string &str);
	std::string			s_debug(const std::string &src, const std::string &str);
}

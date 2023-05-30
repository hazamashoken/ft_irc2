#include "utils.hpp"

namespace Debug {
	void	debug(const std::string &str)
	{
		if (!DEBUG)
			return ;

		std::cout << ANSI::red << ANSI::bold << "[ DEBUG ]: " << ANSI::reset << ANSI::red
			<< str << ANSI::reset << std::endl;
	}

	void	debug(const std::string &src, const std::string &str)
	{
		if (!DEBUG)
			return ;

		std::cout << ANSI::red << ANSI::bold << "[ DEBUG ]: "
			<< ANSI::reset << ANSI::src << ANSI::bold << '[' << src << "]:"
			<< std::string(((long)(INDENT_SRC - src.length()) >= 0 ?
						INDENT_SRC - src.length() : 0), ' ')
			<< ANSI::reset << ANSI::red << "\t" << str
			<< ANSI::reset << std::endl;
	}

	void	debug(const std::ostream& str)
	{
		if (!DEBUG)
			return ;

		std::cout << ANSI::red << ANSI::bold << "[ DEBUG ]:\t" << ANSI::reset << ANSI::red
			<< str.rdbuf() << ANSI::reset << std::endl;
	}

	std::string	s_debug(const std::string &str)
	{
		std::stringstream	o;

		o << ANSI::red << ANSI::bold << "[ DEBUG ]:\t" << ANSI::reset << ANSI::red << str;

		return (o.str());
	}

	std::string	s_debug(const std::string &src, const std::string &str)
	{
		std::stringstream	o;

		o << ANSI::red << ANSI::bold << "[ DEBUG ]:\t"
			<< ANSI::reset << ANSI::src << ANSI::bold << '[' << src << "]:"
			<< std::string(((long)(INDENT_SRC - src.length()) >= 0 ?
						INDENT_SRC - src.length() : 0), ' ')
			<< ANSI::reset << ANSI::red << "\t" << str;

		return (o.str());
	}

}

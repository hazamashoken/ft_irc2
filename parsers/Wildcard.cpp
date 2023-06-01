/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Wildcard.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/17 21:44:14 by abossel           #+#    #+#             */
/*   Updated: 2023/06/01 09:59:37 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Wildcard.hpp"

Wildcard::Wildcard()
{
}

Wildcard::Wildcard(const std::string &pattern)
{
	_pattern.assign(pattern);
}

Wildcard::Wildcard(const Wildcard &other)
{
	_pattern.assign(other._pattern);
}

Wildcard::~Wildcard()
{
}

Wildcard &Wildcard::operator=(const Wildcard &other)
{
	if (this != &other)
		_pattern.assign(other._pattern);
	return (*this);
}

void Wildcard::set(const std::string &pattern)
{
	_pattern.assign(pattern);
}

bool Wildcard::match(const std::string &filename) const
{
	return (match_wildcard(filename, _pattern));
}

/*
** returns true if filename matches the wildcard pattern or false otherwise
*/
bool Wildcard::match_wildcard(const std::string &filename, const std::string &pattern) const
{
	if (filename.empty() || pattern.empty())
		return (false);
	return (match_before(filename, pattern));
}

/*
** matches a string with an escape character
** will call match_before if the filename matches the escape character
*/
bool Wildcard::match_escape(const std::string &filename, const std::string &pattern) const
{
	if (filename.empty())
		return (false);
	if (pattern.size() >= 2 && filename[0] == pattern[1]
		&& pattern[0] == '\\' && (pattern[1] == '*' || pattern[1] == '?'))
	{
		return (match_before(filename.substr(1), pattern.substr(2)));
	}
	else if (pattern.size() >= 1 && filename[0] == pattern[0])
	{
		return (match_before(filename.substr(1), pattern.substr(1)));
	}
	return (false);
}

/*
** matches a string before the wildcard
** will call match_after if the filename matches the first part of the pattern
*/
bool Wildcard::match_before(const std::string &filename, const std::string &pattern) const
{
	size_t next;

	if (filename.empty() && pattern.empty())
		return (true);
	if (pattern.empty())
		return (false);
	next = pattern.find_first_of("*?\\");
	if (next == 0)
		return (match_after(filename, pattern));
	if (next == std::string::npos)
		next = pattern.size();
	if (pattern.compare(0, next, filename, 0, next) == 0)
	{
		if (next == pattern.size())
		{
			if (next == filename.size())
				return (true);
			return (false);
		}
		return (match_after(filename.substr(next), pattern.substr(next)));
	}
	return (false);
}

/*
** matches a string after the wildcard
** will call match_before to match the next part of the pattern
*/
bool Wildcard::match_after(const std::string &filename, const std::string &pattern) const
{
	std::string after;
	size_t next;

	if (filename.empty() && pattern.empty())
		return (true);
	if (pattern.empty())
		return (false);
	if (pattern[0] == '\\')
		return (match_escape(filename, pattern));
	if (pattern[0] == '?')
		return (match_before(filename.substr(1), pattern.substr(1)));
	next = pattern.find_first_not_of("*");
	if (next == std::string::npos)
		return (true);
	after = pattern.substr(next);
	for (next = 0; next < filename.size(); next++)
	{
		if (match_before(filename.substr(next), after))
			return (true);
	}
	return (false);
}

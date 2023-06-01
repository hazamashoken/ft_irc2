/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Split.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/16 09:40:19 by abossel           #+#    #+#             */
/*   Updated: 2023/05/30 15:59:34 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Split.hpp"

Split::Split()
{
}

Split::Split(const std::string &string, char delimiter)
{
	split(string, delimiter);
}

Split::Split(const std::string &string, const std::string &delimiters)
{
	split(string, delimiters);
}

Split::Split(const Split &other)
{
	*this = other;
}

Split &Split::operator=(const Split &other)
{
	if (this != &other)
		_words.assign(other._words.begin(), other._words.end());
	return (*this);
}

Split::~Split()
{
}

size_t Split::size()
{
	return (_words.size());
}

void Split::clear()
{
	_words.clear();
}

std::string &Split::operator[](size_t index)
{
	return (_words[index]);
}

Split::iterator Split::begin()
{
	return (_words.begin());
}

Split::iterator Split::end()
{
	return (_words.end());
}

void Split::split(const std::string &string, char delimiter)
{
	std::string delimiters;

	delimiters.append(1, delimiter);
	split(string, delimiters);
}

void Split::split(const std::string &string, const std::string &delimiters)
{
	size_t pos;
	size_t end;

	_words.clear();
	end = 0;
	while (end != string.size())
	{
		pos = string.find_first_not_of(delimiters, end);
		if (pos == std::string::npos)
			break ;
		end = string.find_first_of(delimiters, pos);
		if (end == std::string::npos)
			end = string.size();
		_words.push_back(string.substr(pos, end - pos));
	}
}

std::string Split::join(char delimiter)
{
	std::string output;
	iterator it;

	for (it = begin(); it != end(); it++)
	{
		if (it != begin())
			output.append(1, delimiter);
		output.append(*it);
	}
	return (output);
}

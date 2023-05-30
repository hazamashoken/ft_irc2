/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Expression.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 10:19:58 by abossel           #+#    #+#             */
/*   Updated: 2023/04/17 20:37:01 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Expression.hpp"
#include <algorithm>

#include <iostream>

#define EXPRESSION_ANY 1
#define EXPRESSION_ALL 2
#define EXPRESSION_EXP 3
#define EXPRESSION_JMP 4
#define EXPRESSION_JMPE 5
#define EXPRESSION_CON 6
#define EXPRESSION_END 7
#define EXPRESSION_TAG 8

Expression::Expression()
{
	_extra_matched_string = NULL;
}

Expression::Expression(std::string &matched)
{
	_extra_matched_string = &matched;
}

Expression::Expression(Expression const &other)
{
	*this = other;
}

Expression::Expression(Expression const &other, std::string &matched)
{
	*this = other;
	_extra_matched_string = &matched;
}

Expression::~Expression()
{
	clear();
}

Expression &Expression::operator=(Expression const &other)
{
	struct Pattern p;
	size_t i;

	if (this != &other)
	{
		clear();
		for (i = 0; i < other._pattern_list.size(); i++)
			clone_pattern(other._pattern_list[i]);
		_matched_string = other._matched_string;
		_remainder_string = other._remainder_string;
		_extra_matched_string = other._extra_matched_string;
	}
	return (*this);
}

/*
 * match the string pattern min to max times
 */
Expression &Expression::all(std::string const pattern, size_t min, size_t max)
{
	struct Pattern p;

	p.expression = NULL;
	p.pattern = pattern;
	p.min = min;
	p.max = max;
	p.type = EXPRESSION_ALL;
	p.tag = 0;
	_pattern_list.push_back(p);
	return (*this);
}

/*
 * match the string pattern 1 time
 */
Expression &Expression::all(std::string const pattern)
{
	return (all(pattern, 1, 1));
}

/*
 * match any char in the string pattern min to max times
 */
Expression &Expression::any(std::string const pattern, size_t min, size_t max)
{
	struct Pattern p;

	p.expression = NULL;
	p.pattern = pattern;
	p.min = min;
	p.max = max;
	p.type = EXPRESSION_ANY;
	p.tag = 0;
	_pattern_list.push_back(p);
	return (*this);
}

/*
 * match any char in the string pattern 1 time
 */
Expression &Expression::any(std::string const pattern)
{
	return (any(pattern, 1, 1));
}

/*
 * adds the characters in string pattern to the last added pattern
 * only works if the last added is an ANY type pattern
 */
Expression &Expression::add(std::string const pattern)
{
	iterator_type it;

	if (!_pattern_list.empty()
		&& _pattern_list.back().type == EXPRESSION_ANY)
	{
		_pattern_list.back().pattern.append(pattern);
	}
	return (*this);
}

/*
 * match any char not in the string pattern min to max times.
 */
Expression &Expression::inv(std::string const pattern, size_t min, size_t max)
{
	std::string str;
	char c;
	int i;

	for (i = 0; i < 256; i++)
	{
		if (i > 127)
			c = static_cast<char>(i - 256);
		else
			c = static_cast<char>(i);
		if (pattern.find_first_of(c) == std::string::npos)
			str.push_back(c);
	}
	return (any(str, min, max));
}

/*
 * match any char not in the string pattern 1 time
 */
Expression &Expression::inv(std::string const pattern)
{
	return (inv(pattern, 1, 1));
}

/*
 * match the expression min to max times
 */
Expression &Expression::exp(Expression const &expression, size_t min, size_t max)
{
	struct Pattern p;

	p.expression = new Expression(expression);
	p.expression->end();
	p.min = min;
	p.max = max;
	p.type = EXPRESSION_EXP;
	p.tag = 0;
	_pattern_list.push_back(p);
	return (*this);	
}

/*
 * match the expression 1 time
 */
Expression &Expression::exp(Expression const &expression)
{
	return (exp(expression, 1, 1));
}

/*
 * match the expression min to max times with a different matched string
 */
Expression &Expression::exp(Expression const &expression, std::string &matched, size_t min, size_t max)
{
	struct Pattern p;

	p.expression = new Expression(expression, matched);
	p.expression->end();
	p.min = min;
	p.max = max;
	p.type = EXPRESSION_EXP;
	p.tag = 0;
	_pattern_list.push_back(p);
	return (*this);	
}

/*
 * match the expression 1 time with a different matched string
 */
Expression &Expression::exp(Expression const &expression, std::string &matched)
{
	return (exp(expression, matched, 1, 1));
}

/*
 * on success of the previous match jump to the next con
 */
Expression &Expression::jmp()
{
	struct Pattern p;

	p.expression = NULL;
	p.min = 0;
	p.max = 0;
	p.type = EXPRESSION_JMP;
	p.tag = 0;
	_pattern_list.push_back(p);
	return (*this);	
}

/*
 * on success of the previous match and input string is empty then jump
 */
Expression &Expression::jmpe()
{
	struct Pattern p;

	p.expression = NULL;
	p.min = 0;
	p.max = 0;
	p.type = EXPRESSION_JMPE;
	p.tag = 0;
	_pattern_list.push_back(p);
	return (*this);	
}

/*
 * continue processing from the previous jump
 */
Expression &Expression::con()
{
	struct Pattern p;

	p.expression = NULL;
	p.min = 0;
	p.max = 0;
	p.type = EXPRESSION_CON;
	p.tag = 0;
	_pattern_list.push_back(p);
	return (*this);	
}

/*
 * stop processing the expression and return the remainder
 */
Expression &Expression::end()
{
	struct Pattern p;

	if (_pattern_list.size() != 0 && _pattern_list.back().type == EXPRESSION_END)
		return (*this);
	p.expression = NULL;
	p.min = 0;
	p.max = 0;
	p.type = EXPRESSION_END;
	p.tag = 0;
	_pattern_list.push_back(p);
	return (*this);	
}

/*
 * add a tag to this expression
 */
Expression &Expression::tag(int num)
{
	struct Pattern p;

	p.expression = NULL;
	p.min = 0;
	p.max = 0;
	p.type = EXPRESSION_TAG;
	p.tag = num;
	_pattern_list.push_back(p);
	return (*this);
}

Expression &Expression::digit(size_t min, size_t max)
{
	return (any("0123456789", min, max));
}

Expression &Expression::digit()
{
	return (digit(1, 1));
}

Expression &Expression::xdigit(size_t min, size_t max)
{
	return (any("0123456789abcdefABCDEF", min, max));
}

Expression &Expression::xdigit()
{
	return (xdigit(1, 1));
}

Expression &Expression::upper(size_t min, size_t max)
{
	return (any("ABCDEFGHIJKLMNOPQRSTUVWXYZ", min, max));
}

Expression &Expression::upper()
{
	return (upper(1, 1));
}

Expression &Expression::lower(size_t min, size_t max)
{
	return (any("abcdefghijklmnopqrstuvwxyz", min, max));
}

Expression &Expression::lower()
{
	return (lower(1, 1));
}

Expression &Expression::alpha(size_t min, size_t max)
{
	return (any("abcdefghijklmnopqrstuvwxyz"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ", min, max));
}

Expression &Expression::alpha()
{
	return (alpha(1, 1));
}

Expression &Expression::alnum(size_t min, size_t max)
{
	return (any("0123456789"
				"abcdefghijklmnopqrstuvwxyz"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ", min, max));
}

Expression &Expression::alnum()
{
	return (alnum(1, 1));
}

/*
 * try to match the expression to string
 * returns true if the expression matches the string
 * the matched string and remainder are only stored on success
 */
bool Expression::match(std::string string)
{
	iterator_type it;
	std::string previous;
	std::string current;
	size_t count;
	bool jump;
	int next;

	// set all matched strings to empty
	_matched_string.clear();
	_remainder_string.clear();
	_tag_list.clear();
	if (_extra_matched_string != NULL)
		_extra_matched_string->clear();
	jump = false;
	previous = string;
	current = string;
	for (it = _pattern_list.begin(); it != _pattern_list.end(); it++)
	{
		count = 0;
		if (it->type == EXPRESSION_ALL && !jump)
		{
			while (count < it->max && current.find(it->pattern) == 0)
			{
				current = current.substr(it->pattern.size());
				count++;
			}
		}
		else if (it->type == EXPRESSION_ANY && !jump)
		{
			if (current.size() != 0)
				count = current.find_first_not_of(it->pattern);
			if (count > it->max || count == std::string::npos)
				count = std::min(it->max, current.size());
			current = current.substr(count);
		}
		else if (it->type == EXPRESSION_EXP && !jump)
		{
			while (count < it->max && it->expression->match(current))
			{
				current = it->expression->get_remainder();
				count++;
			}
		}
		else if (it->type == EXPRESSION_TAG && !jump)
		{
			_tag_list.push_back(it->tag);
		}
		else if (it->type == EXPRESSION_JMP)
		{
			jump = true;
		}
		else if (it->type == EXPRESSION_JMPE)
		{
			jump = true;
		}
		else if (it->type == EXPRESSION_CON)
		{
			jump = false;
		}
		else if (it->type == EXPRESSION_END)
		{
			_remainder_string = current;
			_matched_string = string.substr(0, string.size() - current.size());
			if (_extra_matched_string != NULL)
				*_extra_matched_string = _matched_string;
			return (true);
		}

		if (!jump)
		{
			next = next_type(it);
			// minimum patterns not read so the pattern failed
			if (count < it->min)
			{
				// fail if there is no jump
				if (next != EXPRESSION_JMP && next != EXPRESSION_JMPE)
					return (false);
				// next pattern is jmp then roll back changes and skip
				current = previous;
				it++;
			}
			else
			{
				// if read successful and next is jmpe check if string is empty
				if (next == EXPRESSION_JMPE && !current.empty())
				{
					// next pattern is jmpe then roll back changes and skip
					current = previous;
					it++;
				}
			}
		}
		// reading this pattern is done so commit changes
		previous = current;
	}
	if (!current.empty())
		return (false);
	_remainder_string.clear();
	_matched_string = string;
	if (_extra_matched_string != NULL)
		*_extra_matched_string = _matched_string;
	return (true);
}

std::string Expression::get_matched() const
{
	return (_matched_string);
}

std::string Expression::get_remainder() const
{
	return (_remainder_string);
}

/*
 * returns true if the tag num is in the expression
 */
bool Expression::tagged(int num) const
{
	size_t i;

	for (i = 0; i < _tag_list.size(); i++)
		if (_tag_list[i] == num)
			return (true);
	return (false);
}

/*
 * deletes all data in the expression
 */
void Expression::clear()
{
	iterator_type it;

	for (it = _pattern_list.begin(); it != _pattern_list.end(); it++)
		if (it->expression != NULL)
			delete it->expression;
	_pattern_list.clear();
	_tag_list.clear();
	_matched_string.clear();
	_remainder_string.clear();
	_extra_matched_string = NULL;
}

/*
 * clone a pattern and push it onto _pattern_list
 */
void Expression::clone_pattern(struct Pattern const &other)
{
	struct Pattern p;

	p.expression = NULL;
	if (other.expression != NULL)
		p.expression = new Expression(*(other.expression));
	p.pattern = other.pattern;
	p.min = other.min;
	p.max = other.max;
	p.type = other.type;
	_pattern_list.push_back(p);
}

/*
 * returns the type of pattern after the iterator
 */
int Expression::next_type(iterator_type it)
{
	if (it != _pattern_list.end() && (it + 1) != _pattern_list.end())
		return ((it + 1)->type);
	return (EXPRESSION_END);
}

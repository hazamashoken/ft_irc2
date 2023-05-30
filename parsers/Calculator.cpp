/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Calculator.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 14:16:31 by abossel           #+#    #+#             */
/*   Updated: 2023/05/29 20:55:28 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Calculator.hpp"
#include <sstream>

Calculator::Calculator()
{
}

Calculator::Calculator(const std::string &expression)
{
	set(expression);
}

Calculator::~Calculator()
{
}

Calculator::Calculator(const Calculator &other)
{
	_expression = other._expression;
}

Calculator &Calculator::operator=(const Calculator &other)
{
	if (this != &other)
		_expression = other._expression;
	return (*this);
}

bool Calculator::set(const std::string &expression)
{
	size_t pos;
	double number;
	size_t length;
	int brackets;
	bool fail;
	char curr;
	char prev;

	clear();
	pos = 0;
	prev = ' ';
	fail = false;
	brackets = 0;
	while (!fail && pos < expression.size())
	{
		pos = expression.find_first_not_of(' ', pos);
		if (pos == std::string::npos)
			break ;

		if (!is_number(prev) && !is_close(prev))
		{
			if (read_number(expression.substr(pos), &number, &length))
			{
				curr = 'n';
				add_number(number);
				pos += length;
			}
			else if (is_open(expression[pos]))
			{
				curr = expression[pos];
				add_operation(curr);
				brackets++;
				pos++;
			}
			else
				fail = true;
		}
		else if (is_number(prev) || is_close(prev))
		{
			if (is_operation(expression[pos]))
			{
				curr = expression[pos];
				add_operation(curr);
				pos++;
			}
			else if (is_close(expression[pos]))
			{
				curr = expression[pos];
				add_operation(curr);
				brackets--;
				pos++;
			}
			else
				fail = true;
		}
		else
			fail = true;

		if (brackets < 0)
			fail = true;

		prev = curr;
	}
	if (fail || brackets != 0 || is_operation(prev) || is_open(prev))
	{
		clear();
		return (false);
	}
	fix_precedence();
	return (true);
}

void Calculator::clear()
{
	_expression.clear();
}

std::string Calculator::str() const
{
	std::string output;
	bool first;
	char curr;
	size_t i;

	first = true;
	for (i = 0; i < _expression.size(); i++)
	{
		curr = _expression[i].operation;
		if (!is_popen(curr) && !is_pclose(curr))
		{
			if (!first)
				output.push_back(' ');
			first = false;
			if (is_number(curr))
				output.append(ntos(_expression[i].number));
			else
				output.push_back(curr);
		}
	}
	return (output);
}

double Calculator::calculate() const
{
	return (calc_bracket(0, NULL));
}

double Calculator::calc_bracket(size_t pos, size_t *next) const
{
	double operand;
	double total;
	char oper;
	char curr;

	oper = '+';
	total = 0.0;
	while (pos < _expression.size())
	{
		curr = _expression[pos].operation;
		if (is_number(curr))
		{
			operand = _expression[pos].number;
			switch (oper)
			{
				case '+': total += operand; break;
				case '-': total -= operand; break;
				case '*': total *= operand; break;
				case '/': total /= operand; break;
			}
			pos++;
		}
		else if (is_operation(curr))
		{
			oper = curr;
			pos++;
		}
		else if (is_open(curr) || is_popen(curr))
		{
			operand = calc_bracket(pos + 1, &pos);
			switch (oper)
			{
				case '+': total += operand; break;
				case '-': total -= operand; break;
				case '*': total *= operand; break;
				case '/': total /= operand; break;
			}
		}
		else if (is_close(curr) || is_pclose(curr))
		{
			if (next != NULL)
				*next = pos + 1;
			return (total);
		}
	}
	return (total);
}

void Calculator::add_number(double number)
{
	Element e;

	e.number = number;
	e.operation = 'n';
	_expression.push_back(e);
}

void Calculator::add_operation(char operation)
{
	Element e;

	e.number = 0.0;
	e.operation = operation;
	_expression.push_back(e);
}

void Calculator::insert_operation(size_t pos, char operation)
{
	Element e;

	e.number = 0.0;
	e.operation = operation;
	_expression.insert(_expression.begin() + pos, e);
}

bool Calculator::read_number(const std::string &str, double *num, size_t *len)
{
	std::stringstream ss;
	double number;
	size_t length;
	bool result;

	ss << str;
	number = 0.0;
	ss >> number;

	result = true;
	if (ss.fail())
		result = false;

	length = 0;
	if (ss.fail())
		length = 0;
	else if (ss.eof())
		length = str.size();
	else
		length = static_cast<size_t>(ss.tellg());

	*num = number;
	*len = length;
	return (result);
}

void Calculator::fix_reverse(size_t pos)
{
	int bracket;
	char curr;

	bracket = 0;
	while (pos > 0)
	{
		pos--;
		curr = _expression[pos].operation;
		if (pos == 0)
		{
			insert_operation(pos, '<');
			break ;
		}
		if ((is_low(curr) || is_open(curr)) && bracket == 0)
		{
			insert_operation(pos + 1, '<');
			break ;
		}
		if (is_open(curr))
			bracket++;
		if (is_close(curr))
			bracket--;
	}
}

void Calculator::fix_forward(size_t pos)
{
	int bracket;
	char curr;

	bracket = 0;
	while (pos < _expression.size())
	{
		curr = _expression[pos].operation;
		if (pos == _expression.size() - 1)
		{
			insert_operation(pos + 1, '>');
			break;
		}
		if ((is_low(curr) || is_close(curr)) && bracket == 0)
		{
			insert_operation(pos, '>');
			break;
		}
		if (is_open(curr))
			bracket++;
		if (is_close(curr))
			bracket--;
		pos++;
	}
}

void Calculator::fix_precedence()
{
	size_t pos;
	char curr;

	pos = 0;
	while (pos < _expression.size())
	{
		curr = _expression[pos].operation;
		if (is_high(curr))
		{
			fix_reverse(pos);
			pos++;
			fix_forward(pos);
		}
		pos++;
	}
}

bool Calculator::is_number(char op) const
{
	return (op == 'n');
}

bool Calculator::is_open(char op) const
{
	return (op == '(');
}

bool Calculator::is_close(char op) const
{
	return (op == ')');
}

bool Calculator::is_popen(char op) const
{
	return (op == '<');
}

bool Calculator::is_pclose(char op) const
{
	return (op == '>');
}

bool Calculator::is_operation(char op) const
{
	return (op == '+' || op == '-' || op == '*' || op == '/');
}

bool Calculator::is_high(char op) const
{
	return (op == '*' || op == '/');	
}

bool Calculator::is_low(char op) const
{
	return (op == '+' || op == '-');	
}

std::string Calculator::ntos(double num) const
{
	std::stringstream ss;

	ss << num;
	return (ss.str());
}

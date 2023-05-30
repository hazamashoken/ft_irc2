/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Calculator.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 14:16:24 by abossel           #+#    #+#             */
/*   Updated: 2023/05/29 20:55:33 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CALCULATOR_HPP
# define CALCULATOR_HPP

# include <string>
# include <vector>

class Calculator
{
	public:
		Calculator();
		Calculator(const std::string &expression);
		~Calculator();
		Calculator(const Calculator &other);
		Calculator &operator=(const Calculator &other);
		bool set(const std::string &expression);
		void clear();
		std::string str() const;
		double calculate() const;

	private:
		struct Element
		{
			double number;
			char operation;
		};

		double calc_bracket(size_t pos, size_t *next) const;
		void add_number(double number);
		void add_operation(char operation);
		void insert_operation(size_t pos, char operation);
		bool read_number(const std::string &str, double *num, size_t *len);
		void fix_reverse(size_t pos);
		void fix_forward(size_t pos);
		void fix_precedence();
		bool is_number(char op) const;
		bool is_open(char op) const;
		bool is_close(char op) const;
		bool is_popen(char op) const;
		bool is_pclose(char op) const;
		bool is_operation(char op) const;
		bool is_high(char op) const;
		bool is_low(char op) const;
		std::string ntos(double num) const;

		std::vector<struct Element> _expression;
};

#endif

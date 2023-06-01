/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Split.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/16 09:26:37 by abossel           #+#    #+#             */
/*   Updated: 2023/05/30 15:56:49 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPLIT_HPP
# define SPLIT_HPP

# include <string>
# include <vector>

class Split
{
	public:
		typedef std::vector<std::string>::iterator iterator;

		Split();
		Split(const std::string &string, char delimiter);
		Split(const std::string &string, const std::string &delimiters);
		Split(const Split &other);
		Split &operator=(const Split &other);
		virtual ~Split();
		size_t size();
		void clear();
		std::string &operator[](size_t index);
		iterator begin();
		iterator end();
		void split(const std::string &string, char delimiter);
		void split(const std::string &string, const std::string &delimiters);
		std::string join(char delimiter);

	private:
		std::vector<std::string> _words;
};

#endif

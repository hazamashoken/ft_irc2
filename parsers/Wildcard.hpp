/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Wildcard.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/17 21:40:31 by abossel           #+#    #+#             */
/*   Updated: 2023/04/18 08:14:00 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WILDCARD_HPP
# define WILDCARD_HPP

# include <string>

class Wildcard
{
	public:
		Wildcard();
		Wildcard(const std::string &pattern);
		Wildcard(const Wildcard &other);
		virtual ~Wildcard();
		Wildcard &operator=(const Wildcard &other);
		void set(const std::string &pattern);
		bool match(const std::string &filename) const;

	private:
		bool match_wildcard(const std::string &filename, const std::string &pattern) const;
		bool match_escape(const std::string &filename, const std::string &pattern) const;
		bool match_before(const std::string &filename, const std::string &pattern) const;
		bool match_after(const std::string &filename, const std::string &pattern) const;

		std::string _pattern;
};

#endif

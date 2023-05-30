/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MessageParam.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/23 09:58:07 by abossel           #+#    #+#             */
/*   Updated: 2023/05/29 12:11:57 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGEPARAM_HPP
# define MESSAGEPARAM_HPP

# include <string>
# include "Message.hpp"

class Message;

class MessageParam
{
	public:
		MessageParam(Message &m);
		~MessageParam();
		const std::string &param(int n) const;
		int size() const;

	private:
		const std::string &passParam(int n) const;
		const std::string &nickParam(int n) const;
		const std::string &userParam(int n) const;
		const std::string &operParam(int n) const;
		const std::string &modeParam(int n) const;
		const std::string &serviceParam(int n) const;
		const std::string &quitParam(int n) const;
		const std::string &squitParam(int n) const;
		const std::string &joinParam(int n) const;
		const std::string &partParam(int n) const;
		const std::string &cmodeParam(int n) const;
		const std::string &topicParam(int n) const;
		const std::string &namesParam(int n) const;
		const std::string &listParam(int n) const;
		const std::string &inviteParam(int n) const;
		const std::string &kickParam(int n) const;
		const std::string &privmsgParam(int n) const;
		const std::string &noticeParam(int n) const;
		const std::string &motdParam(int n) const;
		const std::string &lusersParam(int n) const;
		const std::string &versionParam(int n) const;
		const std::string &statsParam(int n) const;
		const std::string &linksParam(int n) const;
		const std::string &timeParam(int n) const;
		const std::string &connectParam(int n) const;
		const std::string &traceParam(int n) const;
		const std::string &adminParam(int n) const;
		const std::string &infoParam(int n) const;
		const std::string &servlistParam(int n) const;
		const std::string &squeryParam(int n) const;
		const std::string &whoParam(int n) const;
		const std::string &whoisParam(int n) const;
		const std::string &whowasParam(int n) const;
		const std::string &killParam(int n) const;
		const std::string &pingParam(int n) const;
		const std::string &pongParam(int n) const;
		const std::string &errorParam(int n) const;
		const std::string &awayParam(int n) const;
		const std::string &unsupportedParam(int n) const;

		Message &_message;
		std::string _error;
};

#endif

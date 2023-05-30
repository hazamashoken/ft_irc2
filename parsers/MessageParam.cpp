/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MessageParam.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/23 10:03:12 by abossel           #+#    #+#             */
/*   Updated: 2023/05/29 12:28:40 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MessageParam.hpp"

MessageParam::MessageParam(Message &m)
: _message(m)
{
	_error.clear();
}

MessageParam::~MessageParam()
{
}

const std::string &MessageParam::param(int n) const
{
	int type;

	if (n == 0)
		return (_message.getMsgTypeCmdStr());
	type = _message.getMsgType();
    switch (type)
    {
        case IRC_PASS: return passParam(n);
        case IRC_NICK: return nickParam(n);
        case IRC_USER: return userParam(n);
        case IRC_OPER: return operParam(n);
        case IRC_MODE: return modeParam(n);
        case IRC_SERVICE: return serviceParam(n);
        case IRC_QUIT: return quitParam(n);
        case IRC_SQUIT: return squitParam(n);
        case IRC_JOIN: return joinParam(n);
        case IRC_PART: return partParam(n);
        case IRC_CMODE: return cmodeParam(n);
        case IRC_TOPIC: return topicParam(n);
        case IRC_NAMES: return namesParam(n);
        case IRC_LIST: return listParam(n);
        case IRC_INVITE: return inviteParam(n);
        case IRC_KICK: return kickParam(n);
        case IRC_PRIVMSG: return privmsgParam(n);
        case IRC_NOTICE: return noticeParam(n);
        case IRC_MOTD: return motdParam(n);
        case IRC_LUSERS: return lusersParam(n);
        case IRC_VERSION: return versionParam(n);
        case IRC_STATS: return statsParam(n);
        case IRC_LINKS: return linksParam(n);
        case IRC_TIME: return timeParam(n);
        case IRC_CONNECT: return connectParam(n);
        case IRC_TRACE: return traceParam(n);
        case IRC_ADMIN: return adminParam(n);
        case IRC_INFO: return infoParam(n);
		case IRC_SERVLIST: return servlistParam(n);
		case IRC_SQUERY: return squeryParam(n);
		case IRC_WHO: return whoParam(n);
		case IRC_WHOIS: return whoisParam(n);
		case IRC_WHOWAS: return whowasParam(n);
		case IRC_KILL: return killParam(n);
		case IRC_PING: return pingParam(n);
		case IRC_PONG: return pongParam(n);
		case IRC_ERROR: return errorParam(n);
		case IRC_AWAY: return awayParam(n);
		case IRC_UNSUPPORTED: return unsupportedParam(n);
	}
	return (_error);
}

int MessageParam::size() const
{
	int type;
	int size;

	type = _message.getMsgType();
	if (type == IRC_INVALID && _message.getMsgTypeCmd() != IRC_INVALID)
		return (1);
	size = 0;
    switch (type)
    {
        case IRC_PASS: size = 2; break;
        case IRC_NICK: size = 2; break;
        case IRC_USER: size = 5; break;
        case IRC_OPER: size = 3; break;
        case IRC_MODE: size = 3; break;
        case IRC_SERVICE: size = 7; break;
        case IRC_QUIT: size = 2; break;
        case IRC_SQUIT: size = 3; break;
        case IRC_JOIN: size = 3; break;
        case IRC_PART: size = 3; break;
        case IRC_CMODE: size = 3; break;
        case IRC_TOPIC: size = 3; break;
        case IRC_NAMES: size = 3; break;
        case IRC_LIST: size = 3; break;
        case IRC_INVITE: size = 3; break;
        case IRC_KICK: size = 4; break;
        case IRC_PRIVMSG: size = 3; break;
        case IRC_NOTICE: size = 3; break;
        case IRC_MOTD: size = 2; break;
        case IRC_LUSERS: size = 3; break;
        case IRC_VERSION: size = 2; break;
        case IRC_STATS: size = 3; break;
        case IRC_LINKS: size = 3; break;
        case IRC_TIME: size = 2; break;
        case IRC_CONNECT: size = 4; break;
        case IRC_TRACE: size = 2; break;
        case IRC_ADMIN: size = 2; break;
        case IRC_INFO: size = 2; break;
		case IRC_SERVLIST: size = 3; break;
		case IRC_SQUERY: size = 3; break;
		case IRC_WHO: size = 3; break;
		case IRC_WHOIS: size = 3; break;
		case IRC_WHOWAS: size = 4; break;
		case IRC_KILL: size = 3; break;
		case IRC_PING: size = 4; break;
		case IRC_PONG: size = 3; break;
		case IRC_ERROR: size = 2; break;
		case IRC_AWAY: size = 2; break;
		case IRC_UNSUPPORTED: size = 1; break;
	}
	while (size > 0 && param(size - 1).empty())
		size--;
	return (size);
}

const std::string &MessageParam::passParam(int n) const
{
	if (n == 1)
		return (_message.getPassword());
	return (_error);
}

const std::string &MessageParam::nickParam(int n) const
{
	if (n == 1)
		return (_message.getNickname());
	return (_error);
}

const std::string &MessageParam::userParam(int n) const
{
	if (n == 1)
		return (_message.getUser());
	if (!_message.getBitmode().empty())
	{
		if (n == 2)
			return (_message.getBitmode());
		if (n == 3)
			return (_message.getUnused());
	}
	else
	{
		if (n == 2)
			return (_message.getHostname());
		if (n == 3)
			return (_message.getServername());
	}
	if (n == 4)
		return (_message.getRealname());
	return (_error);
}

const std::string &MessageParam::operParam(int n) const
{
	if (n == 1)
		return (_message.getUser());
	if (n == 2)
		return (_message.getPassword());
	return (_error);
}

const std::string &MessageParam::modeParam(int n) const
{
	if (n == 1)
		return (_message.getNickname());
	if (n == 2)
		return (_message.getMode());
	return (_error);
}

const std::string &MessageParam::serviceParam(int n) const
{
	if (n == 1)
		return (_message.getNickname());
	if (n == 2)
		return (_message.getUnused());
	if (n == 3)
		return (_message.getTarget());
	if (n == 4)
		return (_message.getUnused());
	if (n == 5)
		return (_message.getUnused());
	if (n == 6)
		return (_message.getInfo());
	return (_error);
}

const std::string &MessageParam::quitParam(int n) const
{
	if (n == 1)
		return (_message.getInfo());
	return (_error);
}

const std::string &MessageParam::squitParam(int n) const
{
	if (n == 1)
		return (_message.getServername());
	if (n == 2)
		return (_message.getInfo());
	return (_error);
}

const std::string &MessageParam::joinParam(int n) const
{
	if (n == 1)
		return (_message.getChannelList());
	if (n == 2)
		return (_message.getKeyList());
	return (_error);
}

const std::string &MessageParam::partParam(int n) const
{
	if (n == 1)
		return (_message.getChannelList());
	if (n == 2)
		return (_message.getInfo());
	return (_error);
}

const std::string &MessageParam::cmodeParam(int n) const
{
	if (n == 1)
		return (_message.getChannel());
	if (n == 2)
		return (_message.getChannelModeList());
	return (_error);
}

const std::string &MessageParam::topicParam(int n) const
{
	if (n == 1)
		return (_message.getChannel());
	if (n == 2)
		return (_message.getTopic());
	return (_error);
}

const std::string &MessageParam::namesParam(int n) const
{
	if (n == 1)
		return (_message.getChannelList());
	if (n == 2)
		return (_message.getTarget());
	return (_error);
}

const std::string &MessageParam::listParam(int n) const
{
	if (n == 1)
		return (_message.getChannelList());
	if (n == 2)
		return (_message.getTarget());
	return (_error);
}

const std::string &MessageParam::inviteParam(int n) const
{
	if (n == 1)
		return (_message.getNickname());
	if (n == 2)
		return (_message.getChannel());
	return (_error);
}

const std::string &MessageParam::kickParam(int n) const
{
	if (n == 1)
		return (_message.getChannelList());
	if (n == 2)
		return (_message.getUserList());
	if (n == 3)
		return (_message.getInfo());
	return (_error);
}

const std::string &MessageParam::privmsgParam(int n) const
{
	if (n == 1)
		return (_message.getMsgto());
	if (n == 2)
		return (_message.getInfo());
	return (_error);
}

const std::string &MessageParam::noticeParam(int n) const
{
	if (n == 1)
		return (_message.getMsgto());
	if (n == 2)
		return (_message.getInfo());
	return (_error);
}

const std::string &MessageParam::motdParam(int n) const
{
	if (n == 1)
		return (_message.getTarget());
	return (_error);
}

const std::string &MessageParam::lusersParam(int n) const
{
	if (n == 1)
		return (_message.getTarget());
	if (n == 2)
		return (_message.getServername());
	return (_error);
}

const std::string &MessageParam::versionParam(int n) const
{
	if (n == 1)
		return (_message.getTarget());
	return (_error);
}

const std::string &MessageParam::statsParam(int n) const
{
	if (n == 1)
		return (_message.getStatQuery());
	if (n == 2)
		return (_message.getTarget());
	return (_error);
}

const std::string &MessageParam::linksParam(int n) const
{
	if (n == 1)
		return (_message.getServername());
	if (n == 2)
		return (_message.getTarget());
	return (_error);
}

const std::string &MessageParam::timeParam(int n) const
{
	if (n == 1)
		return (_message.getTarget());
	return (_error);
}

const std::string &MessageParam::connectParam(int n) const
{
	if (n == 1)
		return (_message.getServername());
	if (n == 2)
		return (_message.getPort());
	if (n == 3)
		return (_message.getTarget());
	return (_error);
}

const std::string &MessageParam::traceParam(int n) const
{
	if (n == 1)
		return (_message.getTarget());
	return (_error);
}

const std::string &MessageParam::adminParam(int n) const
{
	if (n == 1)
		return (_message.getTarget());
	return (_error);
}

const std::string &MessageParam::infoParam(int n) const
{
	if (n == 1)
		return (_message.getTarget());
	return (_error);
}

const std::string &MessageParam::servlistParam(int n) const
{
	if (n == 1)
		return (_message.getTarget());
	if (n == 2)
		return (_message.getInfo());
	return (_error);
}

const std::string &MessageParam::squeryParam(int n) const
{
	if (n == 1)
		return (_message.getMsgto());
	if (n == 2)
		return (_message.getInfo());
	return (_error);
}

const std::string &MessageParam::whoParam(int n) const
{
	if (n == 1)
		return (_message.getTarget());
	if (n == 2)
		return (_message.getWhoOpt());
	return (_error);
}

const std::string &MessageParam::whoisParam(int n) const
{
	if (n == 1)
		return (_message.getServername());
	if (n == 2)
		return (_message.getTargetList());
	return (_error);
}

const std::string &MessageParam::whowasParam(int n) const
{
	if (n == 1)
		return (_message.getNicknameList());
	if (n == 2)
		return (_message.getNumber());
	if (n == 3)
		return (_message.getTarget());
	return (_error);
}

const std::string &MessageParam::killParam(int n) const
{
	if (n == 1)
		return (_message.getNickname());
	if (n == 2)
		return (_message.getInfo());
	return (_error);
}

const std::string &MessageParam::pingParam(int n) const
{
	if (n == 1)
		return (_message.getTarget());
	if (n == 2)
		return (_message.getServername());
	if (n == 3)
		return (_message.getInfo());
	return (_error);
}

const std::string &MessageParam::pongParam(int n) const
{
	if (n == 1)
		return (_message.getTarget());
	if (n == 2)
		return (_message.getServername());
	return (_error);
}

const std::string &MessageParam::errorParam(int n) const
{
	if (n == 1)
		return (_message.getInfo());
	return (_error);
}

const std::string &MessageParam::awayParam(int n) const
{
	if (n == 1)
		return (_message.getInfo());
	return (_error);
}

const std::string &MessageParam::unsupportedParam(int n) const
{
	(void)n;
	return (_error);
}

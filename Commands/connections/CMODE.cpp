/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CMODE.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 12:46:56 by abossel           #+#    #+#             */
/*   Updated: 2023/05/31 12:14:08 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Command.hpp"
#include "../../Client.hpp"
#include "../../Server.hpp"
#include "../RPL.hpp"
#include "../../parsers/Message.hpp"
#include "../../utils/utils.hpp"
#include "../../Status.hpp"
#include "../../Channel.hpp"
#include "../../parsers/Split.hpp"
namespace Commands
{
	static void unknown_mode(Command *command, char mode)
	{
		command->replyClient(IRC::ERR_UNKNOWNMODE, std::string(1, mode) + " :is unknown mode char to me");
	}

	static void channel_change(Command *command, Channel *channel, const std::string &mode, const std::string &target)
	{
		if (target.empty())
		{
			command->replyChannel(*channel, "MODE " + channel->getName() + " :" + mode);
			command->replyClient("MODE " + channel->getName() + " :" + mode);
		}
		else
		{
			command->replyChannel(*channel, "MODE " + channel->getName() + " " + mode + " :" + target);
			command->replyClient("MODE " + channel->getName() + " " + mode + " :" + target);
		}
	}

	static std::string mode_string(Channel *channel, Client *client)
	{
		std::string mode = "+n";

		if (channel->getInviteOnly())
			mode += "i";
		if (!channel->getKey().empty())
			mode += "k";
		if (channel->hasLimit())
			mode += "l";
		if (channel->isOperator(client))
			mode += "o";
		if (channel->getTopicChangeAllowed())
			mode += "t";

		if (!channel->getKey().empty())
			mode += " :" + channel->getKey();
		if (channel->hasLimit())
			mode += " :" + itostring(channel->getLimit(), false);

		return (mode);
	}

	void CMODE(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();

		Split modes;
		bool plus;
		bool info;
		size_t i;

		s->incCommandStats("MODE");
		Debug::debug("CMODE");
		if (m->getMsgType() != IRC_CMODE)
		{
			command->replyNeedMoreParams("MODE");
			return;
		}

		Channel *channel = s->getChannelByName(m->param(1));
		Client *client;

		if (channel == NULL)
		{
			command->replyClient(IRC::ERR_NOTONCHANNEL, m->param(1) + " :You're not on that channel");
			return;
		}

		if (m->param(2).empty())
		{
			command->replyClient(IRC::RPL_CHANNELMODEIS, channel->getName() + " :" + mode_string(channel, c));
			return ;
		}

		modes.split(m->param(2), ' ');
		for (i = 0; i < modes.size(); i++)
		{
			// special case for b add a space in front
			if (modes[i] == "b")
				modes[i].insert(0, 1, ' ');
			info = modes[i][0] == ' ';
			plus = modes[i][0] == '+';
			switch (modes[i][1])
			{
			case 'o': // give or take channel operator privilege
				if (channel->isOperator(c))
				{
					client = s->getClientByNickname(modes[i + 1]);
					if (client != NULL)
					{
						channel->addOperator(client);
						channel_change(command, channel, modes[i], modes[i + 1]);
					}
					else
						command->replyClient(IRC::ERR_NOSUCHNICK, modes[i + 1] + " :No such nick/channel");
				}
				else
					command->replyClient(IRC::ERR_CHANOPRIVSNEEDED, channel->getName() + " :You're not channel operator");
				i++;
				break;
			case 'p': // toggle private channel
				unknown_mode(command, modes[i][1]);
				break;
			case 's': // toggle secret channel
				unknown_mode(command, modes[i][1]);
				break;
			case 'i': // toggle invite only channel
				if (channel->isOperator(c))
				{
					if (!plus)
					{
						channel->clearInvite();
						channel_change(command, channel, "-i", "");
					}
					else
						channel_change(command, channel, "+i", "");
					channel->setInviteOnly(plus);
				}
				else
					command->replyClient(IRC::ERR_CHANOPRIVSNEEDED, channel->getName() + " :You're not channel operator");
				break;
			case 't': // toggle topic set by operator only
				if (channel->isOperator(c))
				{
					if (!plus)
						channel_change(command, channel, "-t", "");
					else
						channel_change(command, channel, "+t", "");
					channel->setTopicChangeAllowed(plus);
				}
				else
					command->replyClient(IRC::ERR_CHANOPRIVSNEEDED, channel->getName() + " :You're not channel operator");
				break;
				break;
			case 'n': // toggle no messages from clients outside
				break;
			case 'm': // toggle moderated channel
				unknown_mode(command, modes[i][1]);
				break;
			case 'l': // set or remove channel user limit
				if (channel->isOperator(c))
				{
					if (!plus)
					{
						channel->removeLimit();
						channel_change(command, channel, "-l", "");
					}
					else
					{
						int limit = ft_stoi(modes[i + 1]);
						if (limit > 0)
							channel->setLimit(limit);
						channel_change(command, channel, "+l", itostring(channel->getLimit(), false));
					}
				}
				else
					command->replyClient(IRC::ERR_CHANOPRIVSNEEDED, channel->getName() + " :You're not channel operator");
				if (plus)
					i++;
				break;
			case 'b': // set or remove user ban mask
				if (info)
					command->replyClient(IRC::RPL_ENDOFBANLIST, channel->getName() + " :End of channel ban list");
				else
					unknown_mode(command, modes[i][1]);
				if (plus)
					i++;
				break;
			case 'v': // give or take voice privilege
				unknown_mode(command, modes[i][1]);
				i++;
				break;
			case 'k': // set or remove channel key
				if (channel->isOperator(c))
				{
					if (plus)
						channel->setKey(modes[i + 1]);
					else
						channel->setKey("");
					channel_change(command, channel, modes[i], channel->getKey());
				}
				else
					command->replyClient(IRC::ERR_CHANOPRIVSNEEDED, channel->getName() + " :You're not channel operator");
				if (plus)
					i++;
				break;
			default:
				unknown_mode(command, modes[i][1]);
				break;
			}
		}
		command->replyClient(IRC::RPL_CHANNELMODEIS, channel->getName() + " :" + mode_string(channel, c));
	}
}

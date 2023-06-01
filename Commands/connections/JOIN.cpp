/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 10:02:43 by abossel           #+#    #+#             */
/*   Updated: 2023/06/01 10:26:48 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../Channel.hpp"
# include "../../parsers/Split.hpp"
namespace Commands {
	void JOIN(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("JOIN");
		Debug::debug("JOIN");
		if (m->getMsgType() != IRC_JOIN)
		{
			command->replyNeedMoreParams("JOIN");
			return ;
		}

		std::map<std::string, Channel *>::const_iterator ch_it;
		std::string msg;

		// JOIN 0 ;remove client from all channels
		if (m->size() == 2 && m->param(1) == "0")
		{
			Debug::debug("JOIN 0");
			ch_it = s->getChannels().begin();
			while (ch_it != s->getChannels().end())
			{
				if (ch_it->second->hasClient(c))
				{
					msg = "PART " + ch_it->first + "\r\n";
					// increment iterator before possible delete
					ch_it++;
					s->executeCommand(c, msg);
				}
				else
					ch_it++;
			}
			return ;
		}

		Split channels;
		Split keys;
		Channel *channel;
		size_t i;

		channels.split(m->param(1), ',');
		keys.split(m->param(2), ',');
		for (i = 0; i < channels.size(); i++)
		{
			// find or create the channel
			channel = s->getChannelByName(channels[i]);
			if (channel == NULL)
			{
				// channel not found so create it
				c->joinChannel(channels[i]);
				channel = s->getChannelByName(channels[i]);
				channel->addOperator(c);
				command->replyClient("MODE " + channel->getName() + " o");
				if (i < keys.size())
				{
					msg = "MODE " + channels[i] + " +k " + keys[i] + "\r\n";
					s->executeCommand(c, msg);
				}
			}
			else
			{
				if (channel->getInviteOnly() && !channel->hasInvite(c->getNickname()))
				{
					command->replyClient(IRC::ERR_INVITEONLYCHAN, channel->getName() + " :Cannot join channel (+i)");
					return ;
				}
				if (channel->hasLimit() && channel->getLimit() <= channel->nrClients())
				{
					command->replyClient(IRC::ERR_CHANNELISFULL, channel->getName() + " :Cannot join channel (+l)");
					return ;
				}
				if (channel->getKey() != "")
				{
					if (i < keys.size() && channel->getKey() == keys[i])
					{
						c->joinChannel(channels[i]);
						Debug::debug("JOIN channel size: " + itostring(channel->getClients().size(), false));
					}
					else
					{
						command->replyClient(IRC::ERR_BADCHANNELKEY, channel->getName() + " :Cannot join channel (+k)");
						continue ;
					}
				}
				else
				{
					c->joinChannel(channels[i]);
					channel->removeInvite(c->getNickname());
				}
			}
			// success message
			command->replyClient("JOIN :" + channels[i]);
			command->replyChannel(*channel, "JOIN :" + channels[i]);
			msg = "NAMES " + channels[i] + "\r\n";
			s->executeCommand(c, msg);
		}
	}
}

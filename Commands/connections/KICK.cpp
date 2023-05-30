/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 21:30:56 by abossel           #+#    #+#             */
/*   Updated: 2023/05/26 09:44:10 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../Channel.hpp"
# include "../../parsers/Wildcard.hpp"
# include "../../parsers/Split.hpp"
namespace Commands {
	void KICK(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("KICK");
		Debug::debug("KICK");
		if (m->getMsgType() != IRC_KICK)
		{
			command->replyNeedMoreParams("KICK");
			return ;
		}

		Channel *channel;
		Client *client;
		Split chans;
		Split users;
		size_t i;

		chans.split(m->param(1), ',');
		users.split(m->param(2), ',');
		i = 0;
		for (i = 0; i < chans.size() && i < users.size(); i++)
		{
			channel = s->getChannelByName(chans[i]);
			if (channel == NULL)
			{
				command->replyClient(IRC::ERR_NOSUCHCHANNEL, chans[i] + " :No such channel");
				continue ;
			}
			client = s->getClientByUsername(users[i]);
			if (client == NULL)
			{
				command->replyClient(IRC::ERR_NOSUCHNICK, users[i] + " :No such nick/channel");
				continue ;
			}
			if (!channel->hasClient(client))
			{
				command->replyClient(IRC::ERR_NOTONCHANNEL, chans[i] + " :You're not on that channel");
				continue ;
			}
			if (!channel->isOperator(c))
			{
				command->replyClient(IRC::ERR_CHANOPRIVSNEEDED, chans[i] + " :You're not channel operator");
				continue ;
			}
			if (m->param(3).empty())
			{
				command->replyChannel(*channel, "KICK " + channel->getName() + " " + client->getNickname());
				command->replyClient("KICK " + channel->getName() + " " + client->getNickname());
			}
			else
			{
				command->replyChannel(*channel, "KICK " + channel->getName() + " " + client->getNickname() + " :" + m->param(3));
				command->replyClient("KICK " + channel->getName() + " " + client->getNickname() + " :" + m->param(3));
			}
			channel->removeOperator(client);
			client->partChannel(channel->getName());
		}
	}
}

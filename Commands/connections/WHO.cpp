/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WHO.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 09:01:58 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 20:21:26 by abossel          ###   ########.fr       */
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
namespace Commands {
	static void who_reply(Command *command, Channel *channel, Client *client)
	{
		std::string message;

		message = channel->getName() + " " + client->getUsername() + " "
					+ client->getHostname() + " " + client->getServername() + " "
					+ client->getNickname();

		if (channel->isOperator(client))
			message += " H@ :0 ";
		else
			message += " H :0 ";

		message += client->getRealname();

		command->replyClient(IRC::RPL_WHOREPLY, message);
	}

	static Channel *common_channel(Command *command, Client *c1, Client *c2)
	{
		std::map<std::string, Channel *>::const_iterator it;
		Channel *channel;
		Server *s;

		if (c1 == c2)
			return (NULL);
		s = command->getServer();
		for (it = s->getChannels().begin(); it != s->getChannels().end(); it++)
		{
			channel = it->second;
			if (channel->hasClient(c1) && channel->hasClient(c2))
				return (channel);
		}
		return (NULL);
	}

	static bool mask_match(Client *client, const std::string &mask)
	{
		Wildcard w;

		w.set(mask);
		if (w.match(client->getHostname()) || w.match(client->getServername())
			|| w.match(client->getRealname()) || w.match(client->getNickname()))
		{
			return (true);
		}
		return (false);
	}

	void WHO(Command *command)
	{
		Server *s = command->getServer();
		Client *c = command->getClient();
		Message *m = command->getMessage();

		std::map<int, Client *>::const_iterator it;
		bool ops_only;
		Channel *channel;
		Client *client;

		s->incCommandStats("WHO");
		Debug::debug("WHO");
		if (m->getMsgType() != IRC_WHO)
		{
			command->replyNeedMoreParams("WHO");
			return ;
		}
		if (m->size() == 1 || (m->size() == 2 && m->param(1) == "0"))
		{
			// No mask so list all visible clients who don't have a common channel
			for (it = s->getClients().begin(); it != s->getClients().end(); it++)
			{
				client = it->second;
				channel = common_channel(command, c, client);
				if (channel != NULL)
					who_reply(command, channel, client);
			}
			command->replyClient(IRC::RPL_ENDOFWHO, "* :End of /WHO list.");
		}
		else
		{
			ops_only = m->size() == 3;
			channel = s->getChannelByName(m->param(1));
			if (channel != NULL)
			{
				// The mask matches a channel name so list all visible clients in that channel
				for (it = s->getClients().begin(); it != s->getClients().end(); it++)
				{
					client = it->second;
					if (channel->hasClient(client))
						if (!ops_only || (ops_only && client->isOperator()))
							who_reply(command, channel, client);
				}
			}
			else
			{
				// The mask doesn't match a channel name so try to match any visible client
				for (it = s->getClients().begin(); it != s->getClients().end(); it++)
				{
					client = it->second;
					if (mask_match(client, m->param(1)))
						if (!ops_only || (ops_only && client->isOperator()))
							who_reply(command, channel, client);
				}
			}
			command->replyClient(IRC::RPL_ENDOFWHO, m->param(1) + " :End of /WHO list.");
		}
	}
}

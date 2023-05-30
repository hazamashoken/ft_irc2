/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NAMES.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 11:35:34 by abossel           #+#    #+#             */
/*   Updated: 2023/05/30 10:56:59 by abossel          ###   ########.fr       */
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
# include "../../parsers/Wildcard.hpp"
namespace Commands {
	static bool client_has_channel(Command *command, Client *client)
	{
		Server *s = command->getServer();
		std::map<std::string, Channel *>::const_iterator ch_it;

		for (ch_it = s->getChannels().begin(); ch_it != s->getChannels().end(); ch_it++)
			if (ch_it->second->hasClient(client))
				return (true);

		return (false);
	}

	static void channel_names_orphans(Command *command)
	{
		std::map<int, Client *>::const_iterator it;
		Server *s = command->getServer();
		std::string reply;
		Client *client;

		for (it = s->getClients().begin(); it != s->getClients().end(); it++)
		{
			client = it->second;
			if (!client->hasMode('i') && !client_has_channel(command, client))
			{
				if (client->isOperator())
					reply = "* :@" + client->getNickname();
				else
					reply = "* :" + client->getNickname();
				command->replyClient(IRC::RPL_NAMREPLY, reply);
			}
		}
		command->replyClient(IRC::RPL_ENDOFNAMES, "* :End of /NAMES list");
	}

	static void channel_names(Command *command, Channel *channel)
	{
		std::map<std::string, Client *>::const_iterator it;
		std::string reply;
		Client *client;
		bool first;

		first = true;
		reply = "= " + channel->getName() + " :";
		for (it = channel->getClients().begin(); it != channel->getClients().end(); it++)
		{
			if (!first)
				reply += " ";
			first = false;
			client = it->second;
			if (channel->isOperator(client))
				reply += "@";
			reply += client->getNickname();
		}
		Debug::debug("NAMES name list " + reply);
		command->replyClient(IRC::RPL_NAMREPLY, reply);
		command->replyClient(IRC::RPL_ENDOFNAMES, channel->getName() + " :End of /NAMES list");
	}

	void NAMES(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("NAMES");
		Debug::debug("NAMES");
		if (m->getMsgType() != IRC_NAMES)
		{
			command->replyNeedMoreParams("NAMES");
			return ;
		}

		// special case test if target matches hostname
		if (!m->getTarget().empty() && !Wildcard(m->getTarget()).match(s->getHostname()))
		{
			command->replyClient(IRC::ERR_NOSUCHSERVER, ":Server to server not supported");
			return ;
		}

		std::map<std::string, Channel *>::const_iterator ch_it;
		Channel *channel;
		Split channels;
		size_t i;

		channels.split(m->param(1), ',');
		if (channels.size() == 0)
		{
			for (ch_it = s->getChannels().begin(); ch_it != s->getChannels().end(); ch_it++)
				channel_names(command, ch_it->second);
			channel_names_orphans(command);
		}
		else
		{
			for (i = 0; i < channels.size(); i++)
			{
				channel = s->getChannelByName(channels[i]);
				if (channel != NULL)
					channel_names(command, channel);
			}
		}
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LUSERS.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abossel <abossel@student.42bangkok.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/30 12:47:45 by abossel           #+#    #+#             */
/*   Updated: 2023/05/25 19:54:22 by abossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"
# include "../../parsers/Wildcard.hpp"
namespace Commands {
	static int nr_clients(Command *command)
	{
		return (static_cast<int>(command->getServer()->getClients().size()));
	}

	static int nr_channels(Command *command)
	{
		return (static_cast<int>(command->getServer()->getChannels().size()));
	}

	static int nr_invisible(Command *command)
	{
		std::map<int, Client *>::const_iterator it;
		Server *server;
		int count;

		count = 0;
		server = command->getServer();
		for (it = server->getClients().begin(); it != server->getClients().end(); it++)
			if (it->second->hasMode('i'))
				count++;
		return (count);
	}

	static int nr_unregistered(Command *command)
	{
		std::map<int, Client *>::const_iterator it;
		Server *server;
		int count;

		count = 0;
		server = command->getServer();
		for (it = server->getClients().begin(); it != server->getClients().end(); it++)
			if (!it->second->isRegistered())
				count++;
		return (count);
	}

	static int nr_operators(Command *command)
	{
		return (static_cast<int>(command->getServer()->getOperators().size()));
	}

	void LUSERS(Command *command)
	{
		Server *s = command->getServer();
		//Client *c = command->getClient();
		Message *m = command->getMessage();

		s->incCommandStats("LUSERS");
		Debug::debug("LUSERS");
		if (m->getMsgType() != IRC_LUSERS)
		{
			command->replyNeedMoreParams("LUSERS");
			return ;
		}

		if (m->size() > 1)
		{
			command->reply(IRC::ERR_NOSUCHSERVER, "Server to server not supported");
			return ;
		}

		command->replyClient(IRC::RPL_LUSERCLIENT, ":There are " + itostring(nr_clients(command), false)
												+ " users and " + itostring(nr_invisible(command), false)
												+ " invisible on 1 servers");
		if (nr_operators(command) != 0)
			command->replyClient(IRC::RPL_LUSEROP, itostring(nr_operators(command), false) + " :operators online");
		if (nr_unregistered(command) != 0)
			command->replyClient(IRC::RPL_LUSERUNKNOWN, itostring(nr_operators(command), false) + " :unknown connections");
		if (nr_channels(command) != 0)
			command->replyClient(IRC::RPL_LUSERCHANNELS, itostring(nr_channels(command), false) + " :channels formed");
		command->replyClient(IRC::RPL_LUSERME, ":I have " + itostring(nr_clients(command), false) + " clients and 1 servers");
	}
}

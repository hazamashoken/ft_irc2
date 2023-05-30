# include "../Command.hpp"
# include "../../Client.hpp"
# include "../../Server.hpp"
# include "../RPL.hpp"
# include "../../parsers/Message.hpp"
# include "../../Status.hpp"

namespace Commands{
	void PASS(Command *command)
	{
		command->getServer()->incCommandStats("PASS");
		Debug::debug("PASS: " + command->getMessage()->getPassword() + " =?= " + command->getServer()->getPass());
		if (command->getMessage()->getPassword() == command->getServer()->getPass())
		{
			command->getClient()->setStatus(Status::REGISTER);
			Debug::debug("REGISTER", "");
			return ;
		}
		command->getClient()->appendSendBuffer(":" + command->getServer()->getHostname() + " " + itostring(IRC::ERR_PASSWDMISMATCH, true) + " " + command->getClient()->getNickname() + " :Password incorrect");
		command->getClient()->appendSendBuffer("ERROR :Closing Link: " + command->getClient()->getHostname() + " (Invalid password)");
	}
}

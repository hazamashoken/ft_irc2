# include "Server.hpp"
# include "parsers/Expression.hpp"

bool validPort(const std::string &port)
{
	Expression e;

	e.exp(Expression().all("6553").any("012345")).jmp();
	e.exp(Expression().all("655").any("012").digit()).jmp();
	e.exp(Expression().all("65").any("01234").digit(2, 2)).jmp();
	e.exp(Expression().all("6").any("01234").digit(3, 3)).jmp();
	e.exp(Expression().any("12345").digit(4, 4)).jmp();
	e.digit(1, 4);
	return (e.match(port));
}

int main(int argc, char **argv)
{
	/// @brief check if the number of arguments is correct ( port and password )
	/// @todo check if the port is a number
	if (argc != 3 && argc != 4)
		return (error("Usage: ./ircserv <port> <password> [operator password]", 1));
	if (!validPort(argv[1]))
		return (error("Port must be 1-65535", 1));
	if (std::string(argv[2]).size() < 5)
		return (error("Password must be 5 or more characters", 1));
	if (argc == 4 && std::string(argv[3]).size() < 5)
		return (error("Operator password must be 5 or more characters", 1));

	/// @brief create a server object and start the server
	Server server(argv[1], argv[2]);
	if (argc == 4)
		server.setOperatorPass(argv[3]);
	while (1)
	{
		/// @brief check if there is a new client to accept
		server.processClients();
	}
	return (0);
}

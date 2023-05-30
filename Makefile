NAME=ircserv

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -g -std=c++98

OBJS = $(shell find . -name "*.cpp" -type f | sed 's/\.cpp/\.o/g')

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

run:	$(NAME)
	./$(NAME) 6667 password

.PHONY: all clean fclean re

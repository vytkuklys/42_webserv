CC= c++
NAME = webserv
FLAGS = -Wall -Wextra -Werror -std=c++98 -g
SRC = ./src/*.cpp ./src/config/*.cpp ./src/parsing/*.cpp ./src/socket/*.cpp ./src/server/*.cpp
HEADERS = ./inc/*.hpp

all: $(NAME)

$(NAME): $(SRC) $(HEADERS)
	@$(CC) $(FLAGS) $(SRC) -o $(NAME)

clean:
	@rm -f *.o

fclean: clean
	@rm -f $(NAME)

re: fclean all

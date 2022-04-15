CC= c++
NAME = webserv
FLAGS = -Wall -Wextra -Werror -std=c++98
SRC = ./src/*.cpp ./src/config/*.cpp ./src/parsing/*.cpp ./src/socket/*.cpp ./src/server/*.cpp ./src/helper/*.cpp ./src/http_header/*.cpp
HEADERS = ./inc/*.hpp ./src/config/*.hpp ./src/http_header/*.hpp ./src/parsing/*.hpp ./src/socket/*.hpp ./src/server/*.hpp ./src/helper/*.hpp

all: $(NAME)

$(NAME): $(SRC) $(HEADERS)
	@$(CC) $(FLAGS) $(SRC) -o $(NAME)

clean:
	@rm -f *.o

fclean: clean
	@rm -f $(NAME)

re: fclean all

install:
	@rm -rf $(HOME)/.brew && rm -rf $(HOME)/goinfre/.brew && git clone --depth=1 https://github.com/Homebrew/brew $(HOME)/goinfre/.brew && echo \
	'export PATH=$(HOME)/goinfre/.brew/bin:$(PATH)' >> $(HOME)/.zshrc && source $(HOME)/.zshrc && brew update
	@exec $(SHELL)
	@brew install siege
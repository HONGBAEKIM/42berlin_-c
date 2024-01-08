
NAME = btc
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address -pedantic

SRCS = BitcoinExchange.cpp main.cpp

#SRCS = $(shell find -iname "*.cpp")

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

#to complie with c++ instead of g++
%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

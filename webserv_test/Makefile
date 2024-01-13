# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: agrun <agrun@student.42berlin.de>         +#+  +:+       +#+          #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/21 12:24:05 by agrun             #+#    #+#              #
#    Updated: 2023/12/21 12:24:08 by agrun            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME 		=	webserv

SRC		=	main ConfigFile Location Server Client Request Response

CC		=	c++
RM		=	rm -f
CFLAGS		=	-Wall -Wextra -Werror -g3 -std=c++98

SRCS		=	$(addprefix ${DIRSRC}, $(addsuffix .cpp, ${SRC}))
OBJS		=	$(SRCS:.cpp=.o)

DIRSRC		=	./src/
DIRINC		=	./inc/

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o:	%.cpp
			$(CC) $(CFLAGS) -c -I${DIRINC} $^ -o $@

clean:
			$(RM) $(OBJS)

fclean: 	clean
			$(RM) $(NAME)

re: 		fclean $(NAME)

.PHONY: 	all clean fclean re

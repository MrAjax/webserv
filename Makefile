# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bahommer <bahommer@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/12 13:52:28 by bahommer          #+#    #+#              #
#    Updated: 2024/01/16 13:21:32 by bahommer         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

S = ./
O = obj/
I = ./
D = dep/

SRC		= $(S)main.cpp \
			$(S)Server.cpp 

OBJ		= $(SRC:$(S)%=$(O)%.o)
DEP		= $(SRC:$(S)%=$(D)%.d)

CC 		= c++

RM		= rm -f
RMDIR	= rmdir

CFLAGS		= -Wall -Wextra -Werror -g3 -I$(I) -std=c++98 
LDLFLAGS	= 

NAME	= webserv 

all:	$(NAME)

$(O):	
	@mkdir -p $@

$(OBJ):	| $(O)

$(O)%.o:	$(S)%
	    $(CC) $(CFLAGS) -c $< -o $@

$(D):	
	@mkdir -p $@

$(DEP):	| $(D)

$(D)%.d:	$(S)%
	    $(CC) $(CFLAGS) -MM -MF $@ -MT $(O)$*.o $<

$(NAME):	$(OBJ)
	    $(CC) $(LDFLAGS) $^ -o $@

clean:		
		$(RM) $(wildcard $(OBJ)) $(wildcard $(DEP))
		-$(RMDIR) $(O) $(D)

fclean:	clean
		${RM} ${NAME}

re:		fclean all

-include $(DEP)

.PHONY:	clean fclean re norme all

CC = c++
CXXFLAGS = -Iinc -MMD -g3 -Wall -Wextra -Werror -std=c++98
NAME = webserv

SRC = main.cpp

OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)

GREEN1=\033[0;38;5;118m
GREEN2=\033[0;38;5;36m
YELLOW=\033[0;93m
RED=\033[0;38;5;167m
END=\033[0m

$(NAME) : $(OBJ)
	@$(CC) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "$(RED) Exec ready $(END)"

-include $(DEP)

all:	$(NAME)

clean :
	@rm $(OBJ) $(DEP)
	@echo "$(GREEN2)🦕 All clean! 🤠$(END)"

fclean : clean
	@rm ${NAME}
	@echo "$(GREEN1)🌈 Full clean done!🌻$(END)"

re : fclean all

.PHONY: all clean fclean re
.SILENT:
CC = c++
CXXFLAGS = -MMD -g3 -Wall -Wextra -Werror -std=c++98
NAME = webserv 

LOG_FILE = server/server.log

SRC = 	main.cpp \
		request/HttpRequest.cpp \
		request/HttpRequestAllow.cpp \
		request/HttpRequestBody.cpp \
		request/HttpRequestChecking.cpp \
		request/HttpRequestHeader.cpp \
		method/Delete.cpp \
		method/Get.cpp \
		method/Method.cpp \
		method/Post.cpp \
		response/HttpResponse.cpp \
		response/ResponseSender.cpp \
		response/StatusSender.cpp \
		utils/ContentTypeFinder.cpp \
		utils/HttpStatusCode.cpp \
		utils/LoopUtils.cpp \
		utils/utils.cpp \
		clear/clear.cpp \
		parsing/allocateSTL.cpp \
		parsing/readConfigFile.cpp \
		parsing/Server.cpp \
		parsing/Location.cpp \
		parsing/handlePoll.cpp \
		signal/signal.cpp \
		cgi/Cgi.cpp

OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)

GREEN1=\033[0;38;5;118m
GREEN2=\033[0;38;5;36m
YELLOW=\033[0;93m
RED=\033[0;38;5;167m
END=\033[0m

#valgrind = valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --trace-children=yes ./$(NAME) config
valgrind = valgrind --leak-check=full ./$(NAME) config

$(NAME) : $(OBJ)
	@chmod 400 server/MIME_types
	@$(CC) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "$(YELLOW) Exec ready $(END)"

-include $(DEP)

all:	$(NAME)

clean :
	rm $(OBJ) $(DEP)
	@echo "$(GREEN2)🦕 All clean! 🤠$(END)"

fclean : clean
	@rm ${NAME}
	@if [ -e $(LOG_FILE) ]; then \
		rm $(LOG_FILE); \
	fi
	@echo "$(GREEN1)🌈 Full clean done!🌻$(END)"

re : fclean all

leaks : all
	@ $(valgrind)

display :
	@if [ -e $(LOG_FILE) ]; then \
		more $(LOG_FILE); \
	fi

.PHONY: all clean fclean re leaks display
.SILENT:


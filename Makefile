NAME    = webserv

SRC     = main.cpp tools.cpp print.cpp \
		  cpp/Server.cpp cpp/Socket.cpp cpp/ConfArgs.cpp cpp/Client.cpp cpp/Request.cpp \
		  cpp/Resp_DELETE.cpp cpp/Resp_GET.cpp cpp/Resp_POST.cpp cpp/Resp_tools.cpp

CC      = c++
FLAGS   = -Wall -Werror -Wextra -std=c++98 -g

OBJ_DIR = obj
OBJS    = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

all : $(NAME)

$(OBJ_DIR)/%.o: %.cpp
		@mkdir -p $(dir $@)
		$(CC) $(CFLAGS) -c $< -o $@

$(NAME) : $(OBJS)
				@echo Compiling...
				$(CC) $(OBJS) $(FLAGS) -o $(NAME)

clean :
				@echo Cleaning...
				rm -f $(OBJS)

fclean : clean
				@echo fCleaning...
				rm -f $(NAME)

val :
				valgrind --leak-check=full --track-fds=all --show-leak-kinds=all --trace-children=yes --track-origins=yes ./webserv conf/test.conf

re : fclean all

.PHONY: all clean fclean re val
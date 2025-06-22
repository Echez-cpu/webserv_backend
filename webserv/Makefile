NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC_DIRS = main.cpp \
	config/ConfigParser.cpp config/LocationBlock.cpp config/ServerConfiguration.cpp \
	server/Server.cpp server/Socket.cpp \
	http/ClientRequest.cpp http/ReplyClient.cpp \
	http/UR_Locator.cpp http/ClientSession.cpp \
	utils/Helper.cpp \
	httpResponse/DeleteActionResponse.cpp httpResponse/FailActionResponse.cpp \
	httpResponse/GetActionResponse.cpp httpResponse/PostActionResponse.cpp

OBJS = $(SRC_DIRS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

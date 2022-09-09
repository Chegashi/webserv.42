FLAGS = -Wall -Wextra -Werror -std=c++98 -g
FS = -fsanitize=address -g 

NAME = webserv

SRC =  ./config/AllowedComponent.cpp \
	./config/cgi.cpp \
	./config/Component.cpp \
	./config/LexicalError.cpp \
	./config/location.cpp \
	./config/LogicalError.cpp \
	./config/parse_confile.cpp \
	./config/parse_main.cpp \
	./config/ParsingError.cpp \
	./config/server.cpp \
	./config/utils.cpp \
	./networking/cgi/cgi.cpp \
	./networking/elements/elements.cpp \
	./networking/elements/elements2.cpp \
	./networking/request/Request.cpp \
	./networking/response/Response.cpp \
	./networking/response/Response_utiles.cpp \
	./server.cpp

OBJ = $(SRC:.cpp=.o)

HDR = ./config/AllowedComponent.hpp \
./config/cgi.hpp \
./config/colors.hpp \
./config/Component.hpp \
./config/Context.hpp \
./config/error.hpp \
./config/LexicalError.hpp \
./config/list.hpp \
./config/location.hpp \
./config/LogicalError.hpp \
./config/parse_confile.hpp \
./config/ParsingError.hpp \
./config/print.hpp \
./config/server.hpp \
./config/throwed.hpp \
./config/utils.hpp \
./networking/elements.hpp \
./networking/request/Request.hpp \
./networking/response/Response.hpp \
./networking/response/Response_utiles.hpp \
./webserve.hpp
#SRC =	./config/AllowedComponent.cpp \
		./config/Component.cpp \
		./config/LexicalError.cpp \
		./config/LogicalError.cpp \
		./config/ParsingError.cpp \
		./config/cgi.cpp \
		./config/location.cpp \
		./config/parse_confile.cpp \
		./config/parse_main.cpp \
		./config/server.cpp \
		./config/utils.cpp \
		./networking/elements.cpp \
		./networking/request/Request.cpp \
		./networking/request/RequestParser.cpp \
		./networking/response/Response.cpp \
		./networking/response/Response_utiles.cpp \
		server.cpp

		
all:	$(NAME)



%.o: %.cpp $(HDR)
	c++ -g $(FLAGS)  -c -o $@ $<
$(NAME) : $(OBJ)
	c++  -g $(OBJ) -o $(NAME) $(FLAGS) 

clean:
	@rm -rf $(NAME)
	@rm -rf $(NAME).dSYM
	@rm -rf bodyfile
	@rm -rf log.txt
	rm -rf $(OBJ)
		
fclean:	clean

re:	fclean all

san:
	c++  -g $(SRC) -o $(NAME) $(FLAGS) $(FS)

sanserv:
	c++  -g $(SRC) -o $(NAME) $(FLAGS) $(FS) -DPARSESERV

sre: fclean san

serv: fclean sanserv

.PHONY: all clean fclean re
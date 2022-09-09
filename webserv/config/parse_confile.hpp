#ifndef PARSE_CONFILE_HPP
#define PARSE_CONFILE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "location.hpp"
#include "cgi.hpp"

class Cgi;
class Location;
class Server;

class ParseConfig
{
    protected:
        std::vector<std::string> _lines;
        std::vector<std::string> _words;
        std::vector<Server> _servers;
		char **env;
    public:
        ParseConfig():_lines(), _words(), _servers(){};
        ~ParseConfig(){};
        void                start_parsing();
        void                split_by_space();
        void                accolade_error();
        void                syntax_error();
        void                check_host_server_names_error();
        void                specified_words(std::string&);
        unsigned int        server_parsing(unsigned int&);
        size_t              get_lines_size() const;
        const std::vector<Server> &get_server_vect() const;
        void				set_server_vect(std::vector<Server>);
        void                set_lines(std::vector<std::string> lines);
        void                read_lines();
        void                read_server();
        std::string         get_lines(int i)const;
        int                 parsing_conf(int ac, char **av, std::vector<std::string> lines);
        int                 basic_error(std::string , char const *, std::string);
        unsigned int        fill_index(std::vector<std::string>, unsigned int);
		void				setEnv(char **_env);
		char				**getEnv();
};
int parse_main(int argc, char **argv, char **ep, ParseConfig &conf);


#endif
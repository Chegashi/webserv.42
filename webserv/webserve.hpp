#ifndef WEBSERVE_HPP
#define WEBSERVE_HPP

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <signal.h>


#include "./config/parse_confile.hpp"
#include "./config/parse_confile.hpp"



namespace Color
{
    enum Code
    {
        FG_RED = 31,
        FG_GREEN = 32,
        FG_BLUE = 34,
        FG_DEFAULT = 39,
        BG_RED = 41,
        BG_GREEN = 42,
        BG_BLUE = 44,
        BG_DEFAULT = 49
    };
    class Modifier
    {
        Code code;

    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream &
        operator<<(std::ostream &os, const Modifier &mod)
        {
            return os << "\033[" << mod.code << "m";
        }
    };
}
    static Color::Modifier green(Color::FG_GREEN);
    static Color::Modifier red(Color::FG_RED);
    static Color::Modifier blue(Color::FG_BLUE);
    static Color::Modifier def(Color::FG_DEFAULT);
    static Color::Modifier B_red(Color::BG_RED);
    static Color::Modifier B_green(Color::BG_GREEN);
    static Color::Modifier B_blue(Color::BG_BLUE);
    static Color::Modifier B_def(Color::BG_DEFAULT);

typedef struct s_socket
{
    int server_fd;
    int new_socket;
    struct sockaddr_in address;
    int addrlen;
    long valread;
	std::string host;
    int port;
    int index_server;
} t_socket;

class Config
{
private:
    int port;
    std::string Defaultpath;

public:
    Config() : port(8080), Defaultpath("/www"){};
    ~Config(){};
    int getPort() { return port; };
    std::string getDefaultpath() { return Defaultpath; };
};

void LaunchServer(ParseConfig *c);

#endif
#pragma once

#ifndef ELEMENTS_HPP
#define ELEMENTS_HPP

#include "../webserve.hpp"
#include "./request/Request.hpp"
#include "./response/Response.hpp"
#include "../config/parse_confile.hpp"
#include "../config/print.hpp"
#include "../config/utils.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <cstring>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

#define DELIMITER "\r\n"
#define MAX_URL_LENGTH 2048

#define MAX_CLIENTS 100000
#define NO_SOCKET -1
#define FALSE 0
#define TRUE 1

size_t getFileSize(const char *fileName);
t_socket accepteConnection(t_socket *_socket);
void close_fds(t_socket *_socket_server, int nServers, std::map<int, t_socket> clients);
void init_socket(t_socket *_socket);
void LaunchServer(ParseConfig *config);
int get_buffer(int index_clients);

#endif
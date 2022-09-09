#include "../elements.hpp"

int get_buffer(int index_clients)
{
    int i;
    int unit = 1024;
    if (index_clients <= 10)
        i = unit * 16;
    else if (index_clients <= 100)
        i = unit * 12;
    else if (index_clients <= 1000)
        i = unit * 8;
    else if (index_clients <= 10000)
        i = unit * 6;
    else if (index_clients <= 100000)
        i = unit * 4;
    else 
        i = unit * 2;
    return i;
}

void close_fds(t_socket *_socket_server, int nServers, std::map<int, t_socket> clients)
{
    for (int i = 0; i < nServers; i++)
        close(_socket_server[i].server_fd);

    for (size_t index_client = 0; index_client < clients.size(); index_client++)
    {
        if (clients[index_client].server_fd)
            close(clients[index_client].server_fd);
        if (clients[index_client].new_socket)
            close(clients[index_client].new_socket);
    }
}

t_socket accepteConnection(t_socket *_socket)
{
    t_socket __socket;
    if ((__socket.server_fd = accept(_socket->server_fd, (struct sockaddr *)&_socket->address, (socklen_t *)&_socket->addrlen)) < 0)
        throw std::runtime_error("Error accepting connection");
    return __socket;
}

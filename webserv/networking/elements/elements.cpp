#include "../elements.hpp"

void init_socket(t_socket *_socket, std::vector <int> &ports)
{
    int on = 1;

    _socket->server_fd = 0;
    _socket->new_socket = -1;
    _socket->address.sin_family = AF_INET;
    _socket->address.sin_addr.s_addr = inet_addr(_socket->host.c_str());
	_socket->address.sin_port = htons(_socket->port);

    memset(_socket->address.sin_zero, '\0', sizeof(_socket->address.sin_zero));
    _socket->addrlen = sizeof(_socket->address);
    if ((_socket->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        throw std::runtime_error("socket failed");
    if ((setsockopt(_socket->server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
    {
        close(_socket->server_fd);
        throw std::runtime_error("setsockopt failed");
    }
    if ((fcntl(_socket->server_fd, F_SETFL, O_NONBLOCK)) < 0)
    {
        close(_socket->server_fd);
        throw std::runtime_error("fcntl failed");
    }


	if (bind(_socket->server_fd, (struct sockaddr *)&_socket->address, sizeof(_socket->address)) < 0)
    {
		if (std::find(ports.begin(), ports.end(), _socket->port) == ports.end()) 
        {
			throw std::runtime_error("bind failed");
		}
		else 
        {
			ports.push_back(_socket->port);
		}
	}
	else
    {
		ports.push_back(_socket->port);
	}

    if (listen(_socket->server_fd, 128) < 0)
        throw std::runtime_error("listen failed");
}

void LaunchServer(ParseConfig *config)
{
    // --- //
    std::map<int, Request> requests;
    std::map<int, t_socket> clients;
    std::map<int, Response> responses;
    //
    int nServers = config->get_server_vect().size();
    t_socket _socket_server[nServers];
    //
    fd_set working_rd_set, working_wr_set;
    fd_set backup_rd_set, backup_wr_set;
    //
    int *serv_response = new int[MAX_CLIENTS];
    bool *first = new bool[MAX_CLIENTS];
    //
    int rc, max_sd, index_client;
    struct timeval timeout;
    // --- //
    timeout.tv_sec  = 60 * 60 * 24; // 1 day timeout
    timeout.tv_usec = rc = max_sd = index_client = 0;
    //
    FD_ZERO(&working_rd_set);
    FD_ZERO(&working_wr_set);

    FD_ZERO(&backup_rd_set);
    FD_ZERO(&backup_wr_set);
    //

	std::vector <int> ports;

    std::cout << "Launching " << nServers << " server..." << std::endl;
    for (int  i = 0; i < nServers; ++i)
    {
        std::cout << green << "init server " << config->get_server_vect()[i].get_name(0) << " on port: " << config->get_server_vect()[i].get_listen_port() << " path root :" << config->get_server_vect()[i].get_root() << def << std::endl;
        serv_response[i] = 1;
        first[i] = true;

		_socket_server[i].host = config->get_server_vect()[i].get_listen_host();
        _socket_server[i].port = config->get_server_vect()[i].get_listen_port();
		
		init_socket(&_socket_server[i], ports);
        
		FD_SET(_socket_server[i].server_fd, &backup_rd_set);
        if (_socket_server[i].server_fd > max_sd)
            max_sd = _socket_server[i].server_fd;
    }
    //
    ssize_t size_send = 0;
    std::string str_to_send;
    size_t   *_send_size = new size_t[MAX_CLIENTS];
    _BUFFER_SIZE = 1024 * 16;
    // --- //
    for (;;)
    {
        // initialise the working sets by the backup sets
        memcpy(&working_rd_set, &backup_rd_set, sizeof(backup_rd_set));
        memcpy(&working_wr_set, &backup_wr_set, sizeof(backup_wr_set));
        // selecting for available sockets
        std::cout << green << "Waiting (..)"  << def << std::endl;
        rc = select(max_sd + 1, &working_rd_set, &working_wr_set, NULL, &timeout);
        //
        if (rc < 0)
            throw std::runtime_error("select() failed");
        else if (rc == 0)
            throw std::runtime_error("Timeout");
        else
        {
            // only for servers to accepte new connections
            for (int i = 0; i < nServers; i++)
                if (FD_ISSET(_socket_server[i].server_fd, &working_rd_set))
                {
                    serv_response[index_client] = 1;
                    // accepte new connections
                    t_socket _server = accepteConnection(&_socket_server[i]);
                    clients[index_client] = _server;
                    clients[index_client].new_socket = _socket_server[i].server_fd;
                    clients[index_client].index_server = i;
                    first[index_client] = true;
                    // add new client to working set and backup set
                    FD_SET(clients[index_client].server_fd, &working_rd_set);
                    FD_SET(clients[index_client].server_fd, &backup_rd_set);
                    // set flags
                    if (clients[index_client].server_fd > max_sd)
                        max_sd = clients[index_client].server_fd;
                    serv_response[index_client]++;
                    index_client++;
                }
            
			// try to assigne a dynamically value to buffer_size for I/O depends on clients number
            // _BUFFER_SIZE = get_buffer(index_client);
            // only for clients to handle reading requests and sending responses
            for (int i = 0; i < index_client; i++)
            {
                // request // client socket is ready to read
                if (FD_ISSET(clients[i].server_fd, &working_rd_set) && serv_response[i] == 2)
                {
                    //
                    int bytes = -1;
                    int fd = clients[i].server_fd;
                    char buffer[_BUFFER_SIZE + 1];
                    //
                    if ((bytes = read(fd, buffer, _BUFFER_SIZE)) <= 0) // while nothing is readed move on to the next client
                        continue;
                    buffer[bytes] = '\0';
                    //
                    if (first[i]) // first thing create a request object
                    {
                        Request request((buffer), bytes, clients[i].server_fd);
                        request.set_client_addr(clients[i].address);
                        requests.insert(std::pair<int, Request>(clients[i].server_fd, request));
                        first[i] = false;
                    }
                    else
                        requests.find(clients[i].server_fd)->second.fill_body(buffer, bytes); // fill the rest of request headers otherwise fill body
                    //
                    if (requests.find(clients[i].server_fd)->second.getIsComplete()) // if request is complete, set the fd socket to write set and buckup set
                    {
                        serv_response[i]++;
                        FD_CLR(requests.find(clients[i].server_fd)->first, &working_rd_set);
                        FD_CLR(requests.find(clients[i].server_fd)->first, &backup_rd_set);
                        FD_SET(requests.find(clients[i].server_fd)->first, &working_wr_set);
                        FD_SET(requests.find(clients[i].server_fd)->first, &backup_wr_set);
                        // requests.find(clients[i].server_fd)->second.show(); // show the request
                    }
                }
                // response // client socket is ready to write
                if (FD_ISSET(clients[i].server_fd, &working_wr_set) && serv_response[i] >= 3)
                {
                    // serv response is a flag to ensure the sequence steps of the response
                    if (serv_response[i] == 3) 
                    {
                        responses.insert(std::pair<int, Response>(i,response(&requests.find(clients[i].server_fd)->second, config, clients[i].index_server, ports))); // create a response object
                        // get the headers to be sent first // BUFFER_SIZE 
                        std::string header = responses[i].getHeader();
						
                        // show headers 
                        responses[i].show();
                        // send the headers to the client 
                        if ((size_send = write(requests.find(clients[i].server_fd)->first, header.c_str() , header.size())) <= 0)
                            continue; // if error, move on to the next client
                        _send_size[i] = size_send; // save the size that been sent to the client
                        serv_response[i]++; // move to the next step                           
                    }
                    else if (serv_response[i] == 4) 
                    {
                        // check if path exist and response and it wasn't been readed and sended all of it yet
                        if (responses[i].getpath().size() > 0  && !responses[i].get_finish())
                        {
                            std::vector<char> buffer = responses[i].get_buffer(); // get the buffer from path to be sent
                            if((size_send = write(requests.find(clients[i].server_fd)->first, buffer.data(), buffer.size())) <= 0 ) // send the buffer to the client
                                continue; // if error, move on to the next client
                            _send_size[i] += size_send; // save the size that been sent to the client
                        }
                        else if (responses[i].get_finish()) // all response path was readed and sent
                        {
                            serv_response[i]++; // move on to next step
                            // std::cout << green << " **********        RESPONSIYA      ****************** " << def << std::endl;
                            // responses[i].show();
							if (responses[i].get_autoindex()) {
								remove(responses[i].getpath().c_str());
							}
                            // std::cout << B_def <<  red << " [HEADER_SIZE] : " << blue  << (responses[i].get_header().size()) << def << std::endl;
                            // std::cout << B_def <<  red << " [BODY_SIZE] : " << blue  << getFileSize(responses[i].getpath().c_str()) << def << std::endl;
                            // std::cout << B_def <<  red << " [FULL SIZE OF RESPONSE] : " << blue  << _send_size[i] << def << std::endl;
                            // std::cout << green <<" **********    END RESPONSIYA      ****************** " <<  def << std::endl;
                        }
                        else{ continue; }
                             // if error it means the path_file_body_rasponse doesn't exist, move on to the next client 
                    }
                    if (serv_response[i] == 5) 
                    {
                        // clear the socket from working set and backup set and close it
                        FD_CLR(requests.find(clients[i].server_fd)->first, &working_wr_set);
                        FD_CLR(requests.find(clients[i].server_fd)->first, &backup_wr_set);
                        close(requests.find(clients[i].server_fd)->first);
                        // erase the request and response from the map
                        requests.erase(requests.find(clients[i].server_fd)->first);
                        responses.erase(responses.find(i));
                        // init flags 
                        serv_response[i] = 1;
                        first[i] = true;
                    }
                }
            }
        }
    }
    close_fds(_socket_server, nServers, clients); // close all the sockets
    std::cout << green << "Shutdown Server Properly." << def << std::endl;
}
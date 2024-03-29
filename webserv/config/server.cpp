

#include "server.hpp"
#include <iostream>  
#include <string>  
#include <vector>  
#include <sstream>  
#include "location.hpp"
#include "cgi.hpp"
#include <cstring>
#include "utils.hpp"

//-------------constructor--------------
Server::Server():
    _name(),
    _listen_port(-1),
    _listen_host(),
    _allowed_methods(),
    _index(),
    _error_pages(),
    _redirections(),
    _root(""),
    _client_max_body_size(-1),
    _autoindex(false)
{

}
    // _location(),
    // _cgi()
std::string server_names[] = {"listen","root","allow_methods", "upload_path", "index", "error_page", "autoindex", "redirection"};

//-------------destructor--------------
Server::~Server()
{
}


//-------------seters------------------

void	Server::set_listen_port(int listenp) {
	_listen_port = listenp;
}

void	Server::set_name_vect(std::vector<std::string> names) {
	_name = names;
}

void    Server::set_name(std::string name)
{
    _name.push_back(name);
}

Server::Server(const Server &src)
{
    // if (this != &src)
    // {
    //     *this = src;
    // }
	_name = src._name;
	_listen_port = src._listen_port;
	_listen_host = src._listen_host;
	_allowed_methods = src._allowed_methods;
	_index = src._index;
	_upload_path = src._upload_path;
	_error_pages = src._error_pages;
	_redirections = src._redirections;
	_root = src._root;
	_location = src._location;
	_cgi = src._cgi;
	_client_max_body_size = src._client_max_body_size;
	_autoindex = src._autoindex;
}

void    Server::set_upload_path(std::string upload_path)
{
    if (not_predefined(upload_path))
        _upload_path = upload_path;
    else
    {
        std::cout << "Error: upload_path in server not well defined" << std::endl;
        exit(1);
    }
}

void    Server::set_to_default()
{
    if (_allowed_methods.empty() || _root.empty())
        throw std::runtime_error("Error: server need more info!!");
    if (_listen_host.empty() || _listen_port == -1)
    {
        _listen_host = "0.0.0.0" ;
        _listen_port = 80;
    }
}

void    Server::check_host(std::string listen_host)
{
    if (listen_host != "localhost")
    {
        int i = 0;
        int t = 0;
        while (listen_host[i])
        {
            if (listen_host[i] == '.')
                t++;
            i++;
        }
        if (t != 3)
        {
            std::cout << "Error: host not valid" << std::endl;
            exit(1);
        }
        i = 0;
        char * cstr = new char [listen_host.length()+1];
        std::strcpy (cstr, listen_host.c_str());
        char *p = std::strtok (cstr,".");
        while (p!=0)
        {
            if (!is_number(p))
            {
                delete[] cstr;
                std::cout << "Error: host not valid" << std::endl;
                exit(1);
            }
            int tmp = std::atoi(p);
            if (tmp < 0 || tmp > 255)
            {
                delete[] cstr;
                std::cout << "Error: host not valid" << std::endl;
                exit(1);
            }
            i++;
            p = std::strtok (NULL,".");
        }
        delete[] cstr;
        if (i != 4)
        {
            std::cout << "Error: host not valid" << std::endl;
            exit(1);
        }
    }
}

void    Server::set_listen(std::string listen)
{
	if (listen.find(':') == (size_t)-1) {
		_listen_host = "0.0.0.0";
		_listen_port = to_int(listen);
	}
	else {
		_listen_host = listen.substr(0, listen.find(':'));
		if (_listen_host == "localhost") {
			_listen_host = "127.0.0.1";
		}
		_listen_port = to_int(listen.substr(listen.find(':') + 1, listen.length()));
	}
    // if (!_listen_host.empty() || _listen_port != -1)
    // {
    //     std::cout << "Error: listen already set" << std::endl;
    //     exit(1);
    // }
    // std::size_t found=listen.find(':');
    // if (found != std::string::npos)
    // {
    //     if (found == 0 && (listen.size() - found != 1))
    //     {
    //         std::string tmp;
    //         _listen_host = "0.0.0.0";
    //         tmp = listen.substr(1, listen.size());
    //         if (is_number(tmp))
    //             _listen_port = std::stoi(tmp);
    //         else
    //         {
    //             std::cout << "Error: port should be a number" << std::endl;
    //             exit(1);
    //         }
    //     }
    //     else if (found == 0 && (listen.size() - found == 1))
    //     {
    //         _listen_host = "0.0.0.0" ;
    //         _listen_port = 80;
    //     }
    //     else
    //     {
    //         // check_host(listen.substr(0, found));
    //         _listen_host = listen.substr(0, found);
    //         std::string tmp;
    //         _listen_host = listen.substr(0, found);
    //         tmp = listen.substr(found+1, listen.size());
    //         if (is_number(tmp))
    //             _listen_port = std::stoi(tmp);
    //         else
    //             std::cout << "Error: port should be a number" << std::endl;
    //     }
    // }
    // else
    // {
    //     // check_host(listen);
    //     _listen_host = listen;
    //     _listen_port = 80;
    // }
}

void    Server::set_allowed_methods_vect(std::vector<std::string> allowed_methods)
{
	_allowed_methods = allowed_methods;
}

void    Server::set_allowed_methods(std::string allowed_methods)
{
    if (allowed_methods == "POST" || allowed_methods == "GET" || allowed_methods == "DELETE")
        _allowed_methods.push_back(allowed_methods);
    else
    {
        std::cout << "Error: allowed methods not well defined" << std::endl;
        exit(1);
    }
}

void    Server::set_index_vect(std::vector<std::string> index)
{
    _index = index;
}

void    Server::set_index(std::string index)
{
    _index.push_back(index);
}

void    Server::set_error_pages(std::string error_pages, std::string number_error)
{
    if (!not_predefined(error_pages) || !not_predefined(number_error))
    {
        std::cout << "Error: error_pages or number_of_error not well defined" << std::endl;
        exit(1);
    }
    else
    {
        std::vector<std::string>    tmp;
        tmp.push_back(number_error);
        tmp.push_back(error_pages);
        _error_pages.push_back(tmp);
    }
}

void    Server::set_redirections(std::string redirection_from, std::string redirection_to, std::string code)
{
    if (!not_predefined(redirection_from) || !not_predefined(redirection_to))
    {
        std::cout << "Error: redirections not well defined" << std::endl;
        exit(1);
    }
    else
    {
        std::vector<std::string>    tmp;
        tmp.push_back(redirection_from);
        tmp.push_back(redirection_to);
        tmp.push_back(code);
        _redirections.push_back(tmp);
    }
}

void    Server::set_root(std::string root)
{
    if (not_predefined(root))
        _root = root;
    else
    {
        std::cout << "Error: root not well defined" << std::endl;
        exit (1);
    }
}

bool Server::is_number(const std::string& str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		char current = str[i];
		if (current >= '0' && current <= '9')
			continue;
		return false;
	}
	return true;
}

int getMul(char c);

void    Server::set_client_max_body_size(std::string client_max_body_size)
{
    if (not_predefined(client_max_body_size))
        _client_max_body_size =std::stoi(client_max_body_size) * getMul(client_max_body_size.back());
    else
    {
        std::cout << "Error: client_max_body_size not well defined" << std::endl;
        exit(1);
    }
}

void    Server::set_client_max_body_size(long long int client_max_body_size)
{
    _client_max_body_size = client_max_body_size;
}

void    Server::set_autoindex(bool autoindex)
{
    _autoindex = autoindex;
}


void   Server::set_location_vect(std::vector<Location> location)
{
    _location = location;
}

void   Server::set_location(Location location)
{
    _location.push_back(location);
}

void    Server::set_cgi_vect(std::vector<Cgi> cgi)
{
    _cgi = cgi;
}

void    Server::set_cgi(Cgi cgi)
{
    _cgi.push_back(cgi);
}

//-------------geters------------------
std::string    Server::get_name(unsigned int i) const
{
    // alchemist tag
	if (i < _name.size())
		return _name[i];
	return "OUT OF BOUND INDEX";
}

std::vector<std::string>   Server::get_name() const
{
	return _name;
}

std::string    Server::get_upload_path() const
{
    return _upload_path;
}

std::string   Server::get_listen_host() const
{
    return _listen_host;
}

int   Server::get_listen_port() const
{
    return _listen_port;
}

std::string  Server::get_allowed_methods(int i) const
{
    return _allowed_methods[i];
}

std::vector<std::string>  Server::get_allowed_methods() const
{
    return _allowed_methods;
}

std::vector<std::string> Server::get_error_pages(int i) const
{
    return _error_pages[i];
}

unsigned int Server::get_error_pages_size() const
{
    return _error_pages.size();
}

unsigned int Server::get_redirections_size() const
{
    return _redirections.size();
}

std::vector<std::string> Server::get_redirections(int i) const
{
    return _redirections[i];
}

std::string Server::get_root() const
{
    return _root;
}

bool   Server::get_autoindex() const
{
    return _autoindex;
}


long long int    Server::get_client_max_body_size() const
{
    return _client_max_body_size;
}

Location   Server::get_location(int i) const
{
    return _location[i];
}

Cgi     Server::get_cgi(int i) const
{
    return _cgi[i];
}

unsigned int    Server::get_allowed_methods_size() const
{
    return _allowed_methods.size();
}

std::vector<std::vector<std::string> > Server::get_redirections() const
{
    return this->_redirections;
}

//----------methods

unsigned int Server::fill_allowed_methods(std::vector<std::string> words, unsigned int i)
{
    i++;
    while (i < words.size() && not_predefined(words[i]))
    {
        set_allowed_methods(words[i]);
        i++;
    }
    if (get_allowed_methods_size() == 0)
    {
        std::cout << "Error: allowed methods not well defined" << std::endl;
        exit(1);            
    }
    i--;
    return i;
}

unsigned int Server::fill_name(std::vector<std::string> words, unsigned int i)
{
    i++;
    while (i < words.size() && words[i] != "}" && words[i] != "server" && words[i] != "{" && words[i] != "listen" 
            && words[i] != "root" && words[i] != "allow_methods" && words[i] != "server_names"
            && words[i] != "upload_path" && words[i] != "index"
            && words[i] != "error_page" && words[i] != "autoindex"
            && words[i] != "redirection" && words[i] != "client_max_body_size"
            && words[i] != "location" && words[i] != "cgi")
    {
        set_name(words[i]);
        i++;
    }
    if (get_name_size() == 0)
    {
        std::cout << "Error: name not well defined" << std::endl;
        exit(1);
    }
    i--;
    return i;
}

unsigned int Server::fill_listen(std::vector<std::string> words, unsigned int i)
{
    i++;
    while (i < words.size() && words[i] != "}" && words[i] != "server" && words[i] != "{" && words[i] != "listen" 
            && words[i] != "root" && words[i] != "allow_methods"
            && words[i] != "upload_path" && words[i] != "index" && words[i] != "server_names"
            && words[i] != "error_page" && words[i] != "autoindex"
            && words[i] != "redirection" && words[i] != "client_max_body_size"
            && words[i] != "location" && words[i] != "cgi")
    {
        set_listen(words[i]);
        i++;
    }
    i--;
    return i;
}

unsigned int Server::fill_index(std::vector<std::string> words, unsigned int i)
{
    i++;
    while (i < words.size() && words[i] != "}" && words[i] != "server" && words[i] != "{" && words[i] != "listen" 
            && words[i] != "root" && words[i] != "allow_methods"
            && words[i] != "upload_path" && words[i] != "index" && words[i] != "server_names"
            && words[i] != "error_page" && words[i] != "autoindex"
            && words[i] != "redirection" && words[i] != "client_max_body_size"
            && words[i] != "location" && words[i] != "cgi")
    {
        set_index(words[i]);
        i++;
    }
    if (get_index_size() == 0)
    {
        std::cout << "Error: index not well defined" << std::endl;
        exit(1);
    }
    i--;
    return i;
}

unsigned int Server::fill_autoindex(std::vector<std::string> words, unsigned int i)
{
    if (words[i + 1] == "on")
        set_autoindex(true);
    else
        set_autoindex(false);
    return i;
}

unsigned int Server::fill_location(std::vector<std::string> words, unsigned int i, bool &location_flag)
{
    location_flag = true;
    // bool cgi_flag = false;
    Location l;
    l.set_locations_path(words[i + 1]);
    while (1)
    {
        if (i >= words.size() || (words[i] == "}" && location_flag))
            break ;
        if (words[i] == "root")
            l.set_root(words[i + 1]);
        else if (words[i] == "allow_methods")
            i = l.fill_allowed_methods(words, i);
        else if (words[i] == "index")
            i = l.fill_index(words, i);
        else if (words[i] == "autoindex")
            i = l.fill_autoindex(words, i);
        else if (words[i] == "client_max_body_size")
            l.set_client_max_body_size(words[i + 1]);
        else if (words[i] == "upload_path")
            l.set_upload_path(words[i + 1]);
        else if (words[i] == "cgi")
        {
            std::cout << "Error: no cgi inside Location" << std::endl;
            exit(1);
        }
        i++;
    }
    location_flag = false;
    set_location(l);
    if (l.get_methods().empty() || l.get_root().empty())
    {
        std::cout << "Error: location not well defined" << std::endl;
        exit(1);
    }
    return i;
}

unsigned int Server::get_index_size() const
{
    return (_index.size());
}

unsigned int Server::get_location_size() const
{
    return _location.size();
}

unsigned int Server::get_cgi_size() const
{
    return _cgi.size();
}

unsigned int Server::get_name_size() const
{
    return _name.size();
}

std::string                  Server::get_index(int i) const
{
    return  (_index[i]);
}


unsigned int Server::fill_cgi(std::vector<std::string> words, unsigned int i, bool &cgi_flag)
{
    if (_cgi.size())
    {
        std::cout << "Error: cgi already defined" << std::endl;
        exit(1);
    }
    cgi_flag = true;
    Cgi c;
    c.set_cgi_name(words[i + 1]);
    while (1)
    {
        if (i >= words.size() || (words[i] == "}" && cgi_flag))
            break ;
        if (words[i] == "cgi_path")
            c.set_cgi_path(words[i + 1]);
        else if (words[i] == "allow_methods")
        {
            i++;
            while (i < words.size() && not_predefined(words[i]))
            {
                c.set_cgi_methods(words[i]);
                i++;
            }
            i--;
        }
        i++;
    }
    cgi_flag = false;
    set_cgi(c);
    return i;
}

/*
    operator
*/

Server    &Server::operator=(Server const &rhs)
{
    if (this != &rhs)
    {
        _name = rhs._name;
        _listen_host = rhs._listen_host;
        _listen_port = rhs._listen_port;
        _index = rhs._index;
        _autoindex = rhs._autoindex;
        _client_max_body_size = rhs._client_max_body_size;
        _allowed_methods = rhs._allowed_methods;
        _location = rhs._location;
        _cgi = rhs._cgi;
        _redirections = rhs._redirections;
        _upload_path = rhs._upload_path;
        _root = rhs._root;
        _error_pages = rhs._error_pages;
    }
    return *this;
}

bool Server::not_predefined(std::string &word) const
{
    if (word != "}" && word != "server" && word != "{" && word != "listen" 
            && word != "root" && word != "allow_methods" && word != "server_names"
            && word != "upload_path" && word != "index"
            && word != "error_page" && word != "autoindex"
            && word != "redirection" && word != "client_max_body_size"
            && word != "location" && word != "cgi" && word != "cgi_path")
            return (1);
    return (0);
}

std::vector<Location>    Server::get_location() const
{
    return _location;
}

std::vector<std::string>     Server::get_index() const
{
    return _index;
}

std::vector<Cgi>    Server::get_cgi() const
{
    return _cgi;
}
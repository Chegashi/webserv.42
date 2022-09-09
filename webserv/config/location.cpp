#include "location.hpp"

Location::Location() : _cgi(),
					   _locations_path(""),
                       _allow_methods(std::vector<std::string>()),
                       _root(""),
                       _client_max_body_size(-1),
                       _index(std::vector<std::string>()),
                       _autoindex(false),
                       _upload_path("")
{
}
Location::~Location() {}

// copy constructor
Location::Location(const Location &obj)
{
    // if (this != &obj)
    // {
    //     *this = obj;
    // }
	_cgi = obj._cgi;
	_name = obj._name;
	_locations_path = obj._locations_path;
	_allow_methods = obj._allow_methods;
	_root = obj._root;
	_client_max_body_size = obj._client_max_body_size;
	_index = obj._index;
	_autoindex = obj._autoindex;
	_upload_path = obj._upload_path;
}

/*
 * GETTERS
 */
std::string Location::get_locations_path() const { return this->_locations_path; }
std::vector<std::string> Location::get_methods() const { return this->_allow_methods; }
std::vector<std::string> Location::get_index() const { return this->_index; }
std::string Location::get_root() const { return this->_root; }
bool Location::get_autoindex() const { return this->_autoindex; }
long long int Location::get_client_max_body_size() const { return this->_client_max_body_size; }
std::string Location::get_upload_path() const { return _upload_path; }
std::vector<Cgi> &Location::get_cgi() { return _cgi; }

/*
 * SETTERS
 */
void Location::set_cgi(std::vector<Cgi> cgi) { _cgi = cgi; }
void Location::set_locations_path(std::string locations_path) { this->_locations_path = locations_path; }
void Location::set_methods_vect(std::vector<std::string> methods) { _allow_methods = methods;}
void Location::set_methods(std::string methods)
{
    if (methods == "POST" || methods == "GET" || methods == "DELETE")
        _allow_methods.push_back(methods);
    else
    {
        std::cout << "Error: allowed methods in location not well defined" << std::endl;
        exit(1);
    }
}
void Location::set_root(std::string root)
{
    if (not_predefined(root))
        this->_root = root;
    else
    {
        std::cout << "Error: root in location not well defined" << std::endl;
        exit(1);
    }
}
void Location::set_autoindex(bool autoindex) { this->_autoindex = autoindex; }
void Location::set_index(std::string index) { this->_index.push_back(index); }
bool Location::is_number(const std::string &str)
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

int getMul(char c) {
	switch (c) {
		case 'K':
			return 1024;
		case 'M':
			return 1024 * 1024;
		case 'G':
			return 1024 * 1024 * 1024;
		default:
			return 1;
	}
}

void Location::set_client_max_body_size(std::string client_max_body_size)
{
    // std::cout << "inside location " << client_max_body_size << std::endl;
    if (not_predefined(client_max_body_size))
        this->_client_max_body_size = std::stoi(client_max_body_size) * getMul(client_max_body_size.back());
    else
    {
        std::cout << "Error: client max body size should be number" << std::endl;
        exit(1);
    }
}

void Location::set_upload_path(std::string upload_path)
{
    if (not_predefined(upload_path))
        _upload_path = upload_path;
    else
    {
        std::cout << "Error: upload_path is empty" << std::endl;
        exit(1);
    }
}

/*
 * methods
 */

unsigned int Location::fill_allowed_methods(std::vector<std::string> words, unsigned int i)
{
    i++;
    while (i < words.size() && not_predefined(words[i]))
    {
        set_methods(words[i]);
        i++;
    }
    if (get_methods_size() == 0)
    {
        std::cout << "Error: location allow_method is empty" << std::endl;
        exit(1);
    }
    i--;
    return i;
}

unsigned int Location::fill_index(std::vector<std::string> words, unsigned int i)
{
    i++;
    while (i < words.size() && words[i] != "}" && words[i] != "server" && words[i] != "{" && words[i] != "listen" && words[i] != "root" && words[i] != "allow_methods" && words[i] != "upload_path" && words[i] != "index" && words[i] != "error_page" && words[i] != "autoindex" && words[i] != "redirection" && words[i] != "client_max_body_size" && words[i] != "location" && words[i] != "cgi")
    {
        set_index(words[i]);
        i++;
    }
    if (get_index_size() == 0)
    {
        std::cout << "Error: location index is empty" << std::endl;
        exit(1);
    }
    i--;
    return i;
}

unsigned int Location::fill_autoindex(std::vector<std::string> words, unsigned int i)
{
    if (words[i + 1] == "on")
        set_autoindex(true);
    else
        set_autoindex(false);
    return i;
}

unsigned int Location::get_index_size() const
{
    return (_index.size());
}

std::string Location::get_index(unsigned int i) const
{
    return (this->_index[i]);
}

unsigned int Location::get_methods_size() const
{
    return (_allow_methods.size());
}

std::string Location::get_methods(unsigned int i) const
{
    return (this->_allow_methods[i]);
}

void Location::set_client_max_body_size(long long int client_max_body_size)
{
    // std::cout << "inside location 55" << client_max_body_size << std::endl;
    _client_max_body_size = client_max_body_size;
}

/*
    operator
*/

Location &Location::operator=(Location const &rhs)
{
    this->_locations_path = rhs._locations_path;
    this->_allow_methods = rhs._allow_methods;
    this->_root = rhs._root;
    this->_autoindex = rhs._autoindex;
    this->_client_max_body_size = rhs._client_max_body_size;
    this->_index = rhs._index;
    this->_upload_path = rhs._upload_path;
    return *this;
}

bool Location::not_predefined(std::string &word) const
{
    if (word != "}" && word != "server" && word != "{" && word != "listen" && word != "root" && word != "allow_methods" && word != "server_names" && word != "upload_path" && word != "index" && word != "error_page" && word != "autoindex" && word != "redirection" && word != "client_max_body_size" && word != "location" && word != "cgi" && word != "cgi_path")
        return (1);
    return (0);
}

std::vector<std::string>    Location::get_allow_methods() const
{
    return (_allow_methods);
}
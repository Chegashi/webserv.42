


#ifndef CGI_HPP
#define CGI_HPP

#include "server.hpp"
#include <iostream>
#include <vector>


class Cgi
{
protected:
    std::string           		_name;
    std::string                 _cgi_path;
    std::vector<std::string>    _allow_methods;
public:
    Cgi();
    ~Cgi();

    std::string                 get_cgi_path() const;
    std::string                 get_cgi_methods(int i) const;
    std::string                 get_cgi_name() const;
    std::vector<std::string>    get_cgi_methods() const;
    void                        set_cgi_path(std::string cgi_path);
    void                        set_cgi_methods(std::string methods);
	void						set_cgi_methods_vect(std::vector<std::string> methods);
    void                        set_cgi_name(std::string name);
    unsigned int                get_cgi_methods_size();
    bool                        not_predefined(std::string &word) const;
    Cgi                         &operator=(Cgi const & rhs);
};


#endif
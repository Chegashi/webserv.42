#pragma once

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../elements.hpp"

static int  _BUFFER_SIZE = 1024 * 16;         //

#define min(x, y) (x < y ? x : y)

class Request;

size_t getFileSize(const char *fileName);

class Request;

class Response
{
    private:
        std::string version;
        std::string status;
        std::string header;
        std::string body;
        std::string response;
        std::string contentType;
        //
        size_t      body_file_size;
        bool        is_complete;
        std::string body_file_path;
        size_t      maxBufferLenght;
        //
        std::vector<std::string> status_vector;
        std::string requested_path;
        std::string redirection_path;
        std::string upload_path;
        std::string index;
        // 
        bool is_cgi;
		bool is_autoindex;
		std::vector<std::pair<std::string, std::string> > resHeaders;
    public:
        Response() : version("HTTP/1.1"), status(""), header(""), body(""), response(""), contentType(""),
        body_file_size(0),  is_complete(false), body_file_path(""), maxBufferLenght(0), is_autoindex(false), resHeaders(std::vector<std::pair<std::string, std::string> >()) {};

		void setCgiHeaders(std::vector<std::pair<std::string, std::string> > );
        void setVersion(std::string version);
        void setStatus(std::string status);
        void setHeader(char *_header);
        void setHeader(std::string header);
        void setBody(char *body);
        void setBody(std::vector<char> _body);
        void setpath (std::string path);
        std::string getpath ();
        void setResponseHeader();
        std::string get_header();
        std::string getHeader();
        std::string getBody();
        void    setbody_file_size(int size);
		void	set_autoindex(bool ai);
		bool	get_autoindex() const ;
        int     getbody_file_size();
        void    set_finish(bool i);
        void    setContentType(std::string type);
        bool    get_finish();
        std::vector<char> get_buffer();
        //
        void set_maxBufferLenght(size_t size);
        size_t get_maxBufferLenght();
        //
        std::string setStatus(Request *request, Server server);
        void show();
        std::string get_location();
        std::string get_redirection();
        void init_location(std::string url, Server server);
        void set_location(std::string url, Server server);
        void init_redirection(std::string url, Server server, std::string &ref);
        void set_redirection(std::string url);
        std::string get_version();
        std::string get_status();
        std::string get_content_type();
        void set_index(std::string str);
        std::string get_index();
        std::string get_index(std::string url, Server server);
        std::string get_upload_path();
        void set_upload_path(std::string path);
        void init_location_for_upload(std::string url, Server server);
};

std::string ERRORresponse(Request *request, Response *response, ParseConfig *config, int server_index);
std::string GETresponse(Request *request, Response *response, ParseConfig *config, int server_index);
std::string DELETEresponse(Request *request, Response *response, ParseConfig *config, int server_index);
std::string POSTresponse(Request *request, Response *response, ParseConfig *config, int server_index);
Response response( Request *request, ParseConfig *config, int fd_server, std::vector<int > ports);
std::string URLgetFileName(std::string url);



#endif
#pragma once

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../elements.hpp"


class Response;

size_t getFileSize(const char *fileName);
bool extensionCgi(std::string path);

class Request
{
private:
    std::string _method;
    std::string _path;
    std::string _version;
    std::string _host;
    std::string _connection;
    std::string _accept;
    std::string _accept_encoding;
    std::string _content_type;
    size_t      _content_length;
    std::string _accept_language;
    std::map<std::string, std::string> _headers;
    std::string bodyFileName;
    int client_fd;
    int _fdBodyFile;
    bool _isGoodRequest;
    bool _is_complete;
    int requestStatus;
    std::string status_message;
    ssize_t bodyFileSize;
    // attributes to reviewed
    bool        is_formated;
    std::string transfer_encoding;
    int         _port;
	struct sockaddr_in _client_addr;
    size_t bodyBytesWritten;
    bool    is_cgi_request;

public:
    Request() : _method(""), _path(""), _version(""), _host(""), _connection(""), _accept(""), _accept_encoding(""),  _content_type(""), _content_length(-1), _headers(std::map<std::string, std::string>()),
                bodyFileName(""), client_fd(-1), _fdBodyFile(-1), _is_complete(false), requestStatus(0), status_message(""), bodyFileSize(0), is_formated(false), transfer_encoding(""), _port(0), bodyBytesWritten(0) {};
    Request(char *buffer, size_t bytes, int fd, size_t cl = 0);
    ~Request(){};

    std::string getHost() {return _host;};
   
    // void isCgiRequest(std::string path) { is_cgi_request = extensionCgi(path);}
    bool isCgiRequest(Request *req, ParseConfig *conf, int serv_index, Response *res, std::string query);
    size_t get_body_length(){  return getFileSize(bodyFileName.c_str());  }
    std::string getMethod() const { return _method; };
    std::string getPath() const { return _path; };
    std::string getVersion() const { return _version; };
    bool        getIsComplete() const { return _is_complete; };
    std::string getConnection() const { return _connection; };
    int         getRequestStatus() const { return requestStatus; };
	std::string getBodyFileName() const { return bodyFileName; };
    const std::map<std::string, std::string> &getHeaders() const;
    void fill_body(char *buffer, size_t bytes);
    int getcontent_length() const { return _content_length; };
    int get_port() const { return _port; };
    void parse(char *buffer);
    //char *readFile(const char *fileName);
    void checkRequest();
    void badRequest() { requestStatus = 400; status_message = "Bad Request";_isGoodRequest = false; };
    void httpVersionNotSupported() { requestStatus = 505; status_message = "HTTP Version Not Supported";_isGoodRequest = false; };
    void goodRequest() { requestStatus = 0; status_message = "Good Request";_isGoodRequest = true; };
	std::string getStatusMessage() const { return status_message; };
    bool isGoodrequest() { return (true); };
    void show();
    // Here to add methods to be reviewed
    bool get_is_formated() { return is_formated; }
    std::string get_transfer_encoding() { return transfer_encoding; }
    std::string geturl() const { return _path; };
	const struct sockaddr_in &getRefClientAddr() const;
    void set_client_addr(struct sockaddr_in client_addr) { _client_addr = client_addr; };
    void set_path(std::string);
    
};

//char *readFile(const char *fileName);
long readRequest(int new_socket, Request *request);
std::vector<std::string> split(const std::string &s, char delim);
std::string to_Lower_case(std::string str);
std::string URLgetFileName(std::string url);

#endif // REQUEST_HPP

    
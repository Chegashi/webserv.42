#include "../elements.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include "Response_utiles.hpp"
#include "../../config/print.hpp"
#include "../../config/utils.hpp"
#include <cstring>
#include <limits.h>
#include <stdlib.h>

#define DELIMITER "\r\n"

bool isDuplicate(std::vector<int> ports, int port)
{
    int compteur = 0;

    for (size_t i = 0; i < ports.size(); i++)
    {
        if (ports[i] == port)
            compteur++;
        if (compteur >= 2)
            return true;
    }
    return false;

}

void getServerIndexByServerName(int *index_server, Request *request, ParseConfig *config, std::vector<Server> &servers) {
	std::string hostname = request->getHost().substr(0, request->getHost().find(':'));
	// std::cout << "-->Hostname: " << hostname << std::endl;

	for (size_t i = 0; i < servers.size(); i++) {
		// std::cout << "x = " << config->get_server_vect()[i].get_name(0) << std::endl;
		if (servers[i].get_listen_port() == servers[*index_server].get_listen_port()) {
			std::vector<std::string> names = config->get_server_vect()[i].get_name();
			for (size_t j = 0; j < names.size(); j++) {

				while (std::iswspace(hostname.back())) {
					hostname = hostname.substr(0, hostname.length() - 1);
				}
				if (strcmp(names[j].c_str(), hostname.c_str()) == 0) {
					*index_server = i; // switch index to the specific server
					return;
				}
			}
		}
	}
	// check if the choosen server have a friend shared port with other servers
}

Response  response(Request *request, ParseConfig *config, int index_server, std::vector<int> ports)
{
    std::vector<Server> servers = config->get_server_vect();
    
    // if there is multiple servers that share same ports, we need to chose the specific server
    // depend on hostname == server.name .
    bool flag = isDuplicate(ports, servers[index_server].get_listen_port());
    if (servers.size() >= 2 && flag)
    {
        getServerIndexByServerName(&index_server, request, config, servers);
    }

    std::string header_str = "";
    Response response;
    response.setpath("empty");
    // std::string s;
    std::string path = request->getPath();
    

	if (request->getRequestStatus() != 0) { // request error
		response.setStatus(std::string(" ") + to_string(request->getRequestStatus()) + " " + request->getStatusMessage() + "\r\n");
		response.setpath(get_error_page(request->getRequestStatus() , config->get_server_vect()[index_server]));
        return response;
	}

    if (!isValidURLPath(path) || (request->getHeaders().find("host") == request->getHeaders().end())) {
        response.setStatus(" 400 BAD REQUEST\r\n");
        response.setpath(get_error_page(400 , config->get_server_vect()[index_server]));
        return response;
    }
	std::string queryParams = URLgetQueryParams(path);
    request->set_path(URLdecode(URLremoveQueryParams(path)));

	if (request->isCgiRequest(request, config, index_server, &response, queryParams)) {
		return response;
	}

	println("url decoded path: ", request->getPath());
    response.setpath(response.setStatus(request, config->get_server_vect()[index_server]));
    if (!response.get_status().empty())
        return response;
    else if (!(request->getMethod().compare("GET")))
        response.setpath(GETresponse(request, &response, config, index_server));
    else if (request->getMethod().compare("DELETE") == 0)
        response.setpath(DELETEresponse(request, &response, config, index_server));
    else if (request->getMethod().compare("POST") == 0)
        response.setpath(POSTresponse(request, &response, config, index_server));
    std::cout << "Requested path : " << response.get_location() << std::endl;
    std::cout << "Redirection path : " << response.get_redirection() << std::endl;
    if (!response.get_redirection().empty())
        response.setContentType(response.get_redirection());
    else
        response.setContentType(response.get_location());
    std::cout << header_str << std::endl;
    // response.setpath(s);
    std::cout << "body_path : " << response.getpath() << std::endl;
    std::cout << "body_path : " << response.getpath() << std::endl;

    println("file = ", response.getpath());
    return response;
}

std::string capitalize(std::string status);

std::string Response::setStatus(Request *request, Server server)
{
    std::string code_redirection;
    // std::string code_status_file;

    size_t max_body_size = get_max_body_size(request->getPath(), server);
	// PRINT_LINE_VALUE(max_body_size);
    this->init_location(request->geturl(),server);
    this->init_redirection(request->geturl(), server, code_redirection);
    init_location_for_upload(request->geturl(), server);
	// PRINT_LINE_VALUE(!request->get_is_formated());
	if (!request->get_is_formated())
	{
		if (request->get_transfer_encoding().size() > 0 && request->get_transfer_encoding() != "chunked")
        {
			this->status = capitalize(" 501 NOT IMPLEMENTED\r\n");
            return get_error_page(501, server);
        }
		else if (!request->get_transfer_encoding().size() && request->getcontent_length() <= 0 &&  request->getMethod() == "POST")
        {
            this->status = capitalize(" 400 BAD REQUEST\r\n");
            return get_error_page(400 , server);
        }
		// else if (!url_is_formated(request->geturl()))
        // {
        //     this->status = capitalize(" 400 BAD REQUEST\r\n");
        //     return get_error_page(400 , server);
        // }
        else if (request->geturl().length() > MAX_URL_LENGTH)
		{
        	this->status = capitalize(" 414 URI TOO LONG\r\n");
            return get_error_page(414, server);
        }
        else 
        {
			// PRINT_LINE_VALUE(max_body_size);
			// PRINT_LINE_VALUE(request->get_body_length());
			if (max_body_size != (size_t)-1 && request->get_body_length() != (size_t)-1 && request->get_body_length() > max_body_size) {
				this->status = capitalize(" 413 REQUEST ENTITY TOO LARGE\r\n");
				return get_error_page(413, server);
			}
        }
    }
    if (!method_is_allowed(request->getMethod(), request->geturl(),  server))
    {
        this->status = capitalize(" 405 METHOD NOT ALLOWED\r\n");
        return get_error_page(405, server);
    }
    else if (!get_redirection().empty())
    {
        this->status = capitalize(code_redirection);
        return get_error_page(std::atoi(code_redirection.c_str()), server);
    }
    else if (!file_exist(get_location()) && request->getMethod() != "POST")
    {
        this->status = capitalize(" 404 NOT FOUND\r\n");
        return get_error_page(404, server);
    }
    else if (request->getMethod() == "POST" && !file_exist(get_upload_path()))
    {
        this->status = capitalize(" 404 NOT FOUND\r\n");
        return get_error_page(404, server);
    }
    return "";
}

void Response::init_location(std::string url, Server server)
{
	std::vector<Location> location = server.get_location();
	std::vector<Location>::const_iterator it_loc = location.begin();
	std::string location_path = "";
	std::string location_str;
	int location_path_matched = 0;
	// Location location_matched;
	this->requested_path = "";

	for (; it_loc != location.end(); it_loc++)
	{
		location_str = it_loc->get_locations_path();
		if (location_str.back() != '/')
			location_str += '/';
		if (!std::strncmp(url.c_str(), location_str.c_str(), location_str.size()))
		{
			std::string url_copy;
			if (str_matched(location_str, location_path) > location_path_matched)
			{
				url_copy = url.substr(location_str.size());
				// PRINT_LINE_VALUE(url_copy);
				location_path = location_str;
				location_path_matched = str_matched(location_str, location_path);
				// location_matched = *it_loc;
				this->requested_path = remove_duplicate_slash(it_loc->get_root() + "/" + url_copy);
			}
		}
	}
	if (this->requested_path.empty())
		this->requested_path = remove_duplicate_slash(server.get_root() + url);
}

std::string get_status_code(int n)
{
    std::string code = "";
    switch (n)
    {
        case 300:
            code = " 300 MULTIPLE CHOICES\r\n";
            break;
        case 302:
            code = " 302 FOUND\r\n";
            break;
        case 303:
            code = " 303 SEE OTHER\r\n";
            break;
        case 304:
            code = " 304 NOT MODIFIED\r\n";
            break;
        case 305:
            code = " 305 USE PROXY\r\n";
            break;
        case 307:
            code = " 307 TEMPORARY REDIRECT\r\n";
            break;
        case 308:
            code = " 308 PERMANENT REDIRECT\r\n";
            break;
        default :
            code = " 301 MOVED PERMANENTLY\r\n";
    }
    return code;
}

void	Response::init_redirection(std::string url, Server server, std::string &statusLine)
{
	std::vector<std::vector<std::string> > red = server.get_redirections();
	std::string redirection_str;
	std::string _redirection_path = "";
	// int redirection_path_matched = 0;
    std::string path_absol = "";

	for(std::vector<std::vector<std::string> >::iterator it = red.begin(); it != red.end(); ++it) {
		redirection_str = it->at(0);
		if (redirection_str.front() != '/')
			redirection_str = "/" + redirection_str;
		if (redirection_str == url){
			this->redirection_path = it->at(1);
			if (std::isdigit(it[0][2][0])) {
				statusLine = get_status_code(to_int(it[0][2]));
			} else if (it[0][2] == "temporary") {
				statusLine = get_status_code(307);
			} else {
				statusLine = get_status_code(308);
			}
		}
	}
	if (url.back() != '/') {
		std::vector<Location> locs = server.get_location();
		std::string urlplusslash = url + "/";
		for (std::vector<Location>::iterator lit = locs.begin(); lit != locs.end(); lit++) {
			std::string loc_path = lit->get_locations_path();
			if (loc_path.back() != '/') {
				loc_path += '/';
			}
			if (urlplusslash == loc_path) {
				this->redirection_path = loc_path;
				statusLine = get_status_code(301);
				break;
			}
		}
	}
	// PRINT_LINE_VALUE(this->redirection_path);
	// if (redirection_path_matched)
    // {
    //     if (path_absol.length() > 0)
    //         this->redirection_path = path_absol;
    //     else
    //         this->redirection_path = url.replace(0, _redirection_path.size(), _redirection_path);
    // }
    // else
    //     this->redirection_path = "";

		// if (redirection_str.back() != '/')
		// 		redirection_str += "/";
	// 	if (url.substr(0, redirection_str.size()) == redirection_str) {
	// 		if (str_matched(redirection_str, _redirection_path) > redirection_path_matched) {
    //             println("it02 = ", it[0][2]);
    //             if (std::isdigit(it[0][2][0])) {
    //                 statusLine = get_status_code(to_int(it[0][2]));
    //             }
    //             else if (it[0][2] == "temporary") {
    //                 statusLine = get_status_code(307);
    //             }
    //             else {
    //                 statusLine = get_status_code(308);
    //             }
    //             println("status = ", statusLine);
    //             _redirection_path = (*it)[1];
	// 			redirection_path_matched = str_matched(redirection_str, _redirection_path);
    //             if (std::strncmp(_redirection_path.c_str(), "http", 4) == 0)
    //                 path_absol = _redirection_path;

	// 		}
	// 	}
}

void Response::setContentType(std:: string s)
{
    std::string s1 = s.substr(s.find_last_of(".") + 1);
    if (s1 == "html" || s1 == "htm")
        this->contentType = "text/html";
    else if (s1 == "css")
        this->contentType = "text/css";
    else if (s1 == "js")
        this->contentType = "application/javascript";
    else if (s1 == "jpg")
        this->contentType = "image/jpeg";
    else if (s1 == "png")
        this->contentType = "image/png";
    else if (s1 == "gif")
        this->contentType = "image/gif";
    else if (s1 == "ico")
        this->contentType = "image/x-icon";
    else if (s1 == "svg")
        this->contentType = "image/svg+xml";
    else if (s1 == "mp3")
        this->contentType = "audio/mpeg";
    else if (s1 == "mp4")
        this->contentType = "video/mp4";
    else if (s1 == "ogg")
        this->contentType = "audio/ogg";
    else if (s1 == "ogv")
        this->contentType = "video/ogg";
    else if (s1 == "wav")
        this->contentType = "audio/wav";
    else if (s1 == "webm")
        this->contentType = "video/webm";
    else if (s1 == "txt")
        this->contentType = "text/plain";
    std::cout << red << "- Set Content-Type : " << s1 << green << " " << s1 << def << std::endl;
};

std::vector<char> Response::get_buffer()
{
    std::vector <char> buffer;
    std::cout << red << "read buffer dfrom : "  << body_file_path << std::endl;
    int fd = open(this->body_file_path.c_str(), O_RDONLY);
    if (fd < 0)
        std::cout << "open file error";
    int size = lseek(fd, maxBufferLenght, SEEK_SET);
    if (size < 0)
        std::cout << "lseek error";
    char *buf = (char *)malloc((_BUFFER_SIZE) + 1);
    int read_size = read(fd, buf, (_BUFFER_SIZE));
    if (read_size < 0)
        std::cout << "read error";
    maxBufferLenght += read_size;
    for (int i = 0; i < read_size; i++)
        buffer.push_back(buf[i]);
    free(buf);
    close(fd);
    if (maxBufferLenght >= getFileSize(body_file_path.c_str()))
        this->is_complete = true;
    else 
        this->is_complete = false;
    std::cout << red << "- Read_size(" << _BUFFER_SIZE << ")  : " << read_size << " maxBuffer : " << maxBufferLenght << " file path size : " << getFileSize(body_file_path.c_str()) << def << std::endl;
    return buffer;
};

void Response::setCgiHeaders(std::vector<std::pair<std::string, std::string> > __headers__) {
	resHeaders = __headers__;
}

std::string Response::getHeader() 
{
    std::string res;
    
    is_cgi = false;
    if (is_cgi)
    {
        // TODO: check if path is set and headedrs exist
    }; 
    // status line
    res.append(version);
    res.append(status);
    // HEADERS : 
    if (this->contentType.length())
    {
        res.append("Content-Type: ");
        res.append(this->contentType);
        res.append("\r\n");
    }
    // LOCATION
    if (!get_redirection().empty())
        res.append("Location: " + get_redirection() + "\r\n");
    size_t tt = getFileSize(body_file_path.c_str());
    //C_Lenght
    if (tt && tt != (size_t)-1)
    {
        res.append("Content-Length: ");
        res.append(std::to_string(tt));
        res.append("\r\n");
    }
    res.append((is_cgi) ? "server: alchemist\r\n":"server: alchemist_CGI\r\n");
	
	ITERATE(SELF(std::vector<std::pair<std::string, std::string> >), resHeaders, it) {
		res.append(it->first);
		res.append(": ");
		res.append(it->second);
		res.append("\r\n");
	}
    res.append("\r\n");
    setHeader(res);
    //
    return res;
};

// std::string ERRORresponse(Request *request, Response *response, ParseConfig *config, int server_index)
// {
//     std::string body_f = "";
//     body_f = get_error_page(std::atoi(response->get_status().c_str()),  config->get_server_vect()[server_index]);
//     response->setpath(body_f);
//     return body_f;
// }

std::string DELETEresponse(Request *request, Response *response, ParseConfig *config,  int index_server)
{
    std::string body_f = "empty";
    if(requested_file_in_root(response->get_location()))
    {
        if (is_file(response->get_location()))
        {
				PRINT_LINE_VALUE(response->get_location().c_str());
				PRINT_LINE_VALUE(access(response->get_location().c_str(), W_OK));
				if (!access(response->get_location().c_str(), W_OK))
				{
                	remove(response->get_location().c_str());
                	response->setStatus(" 204 No Content\r\n"); 
                	get_error_page(204, config->get_server_vect()[index_server]);
				}
				else
				{
					response->setStatus(" 403 Forbidden\r\n");
                    get_error_page(403, config->get_server_vect()[index_server]);
				}
        }
        else
        {
            if (response->get_location().back() == '/')
            {
                std::string index_path = response->get_index(request->getPath(), config->get_server_vect()[index_server]);
                    if(remove_all_folder_content(response->get_location()))
                    {
                        response->setStatus(" 204 No Content\r\n");
                        get_error_page(204, config->get_server_vect()[index_server]);
                    }
                    else
                    {
                        if (have_write_access_on_folder(response->get_location()))
                        {
                            response->setStatus(" 500 Internal Server Error\r\n");
                            get_error_page(500, config->get_server_vect()[index_server]);
                        }
                        else
                        {
                            response->setStatus(" 403 Forbidden\r\n");
                            get_error_page(403, config->get_server_vect()[index_server]);
                        }
                    }
            }
            else
            {
                response->setStatus(" 409 Conflict\r\n");
                get_error_page(409, config->get_server_vect()[index_server]);
            }
        }
    }
    else
    {
        response->setStatus(" 403 Forbidden\r\n");
        get_error_page(403, config->get_server_vect()[index_server]);
    }
    return "empty";
}

std::string  POSTresponse(Request *request, Response *response, ParseConfig *config,  int index_server)
{
    (void)request;
    (void)config;
    std::string body_f = "empty";
    std::string path_upload_file;

    if(!response->get_upload_path().empty())
    {
        if (upload_post(request, response, config->get_server_vect()[index_server].get_upload_path()))
        {
            body_f = get_error_page(201, config->get_server_vect()[index_server]);
            response->setStatus(" 201 Created\r\n");
        }
        else
        {
			body_f = get_error_page(500, config->get_server_vect()[index_server]);
			response->setStatus(" 500 Internal Server Error\r\n");
		}
		body_f = get_error_page(403, config->get_server_vect()[index_server]);
    }
    else
    {
        body_f = get_error_page(403, config->get_server_vect()[index_server]);
        response->setStatus(" 403 Forbidden\r\n");
    }
    return body_f;
    std::cout << "im doing post response\n";
}

std::string  GETresponse(Request *request, Response *response, ParseConfig *config, int index_server)
{
    (void)request;
    /////// main process to set a good response : set mandatory headers + set path of file to send
    std::string body_f = "empty";
    if(!response->get_status().empty())
        return body_f;
	// PRINT_LINE_VALUE(response->get_location());
    if(requested_file_in_root(response->get_location()))
    {
        if (is_file(response->get_location()))
        {
			response->setpath(response->get_location());
			response->setStatus(" 200 OK\r\n");
			body_f = response->getpath();
            
        }
        else // if you request a directory
        {

            if (response->get_location().back() == '/') // uri have / at the end
            {
                std::string index_path = response->get_index(request->getPath(), config->get_server_vect()[index_server]);
                if (file_exist(index_path)) //have index file
                {
					response->set_index(index_path);
					response->setpath(response->get_index());
					response->setStatus(" 200 OK\r\n");
					body_f = index_path;
                }
                else
                {
					if (check_auto_index(request->getPath(), config->get_server_vect()[index_server]))
                    {
						response->set_autoindex(true);
                        body_f = generate_auto_index(response->get_location());
                        response->setpath(body_f);
                        response->setStatus(" 200 OK\r\n");
                    }
                    else
                    {
                        response->setStatus(" 403 FORBIDDEN\r\n");
                        body_f = get_error_page(403,  config->get_server_vect()[index_server]);
                        response->setpath(body_f);
                    }
                }
            }
            else //uri have not a / at the end
            {
                response->set_redirection(request->getPath() + "/");
                std::cout << "redirection to " << response->get_redirection() << std::endl;
                response->setStatus(" 301 MOVED PERMANENTLY\r\n");
                body_f = get_error_page(301,  config->get_server_vect()[index_server]);
                response->setpath(body_f);
            }
        }
    }
    else // requested resource not in root
    {
        response->setStatus(" 404 NOT FOUND\r\n");  //requested resource not found
        body_f = get_error_page(404,  config->get_server_vect()[index_server]);
        response->setpath(body_f);
    }

    return body_f;
}

std::string Response::get_index(std::string url, Server server)
{
	std::vector<Location> location = server.get_location();

	std::vector<Location>::const_iterator it_loc = location.begin();
    std::vector<std::string> index_file = server.get_index();
	std::string location_path = "";
	std::string location_str;
	int location_path_matched = 0;
	Location location_matched;

	for (; it_loc != location.end(); it_loc++)
	{
		location_str = it_loc->get_locations_path();
		if (location_str.back() != '/')
			location_str += '/';
		if (!std::strncmp(url.c_str(), location_str.c_str(), location_str.size()))
		{
			if (str_matched(location_str, location_path) > location_path_matched)
			{
				std::vector<std::string> indexVec = it_loc->get_index();
				for (std::vector<std::string>::iterator it = indexVec.begin();it != indexVec.end(); ++it)
				{
					std::string url_copy;
					url_copy = url.substr(location_str.size());
					if (file_exist(remove_duplicate_slash(it_loc->get_root() + "/" + url_copy + "/" + *it)))
					{
						return (remove_duplicate_slash(it_loc->get_root() + "/" + url_copy + "/" + *it));
					}
				}
				location_path = location_str;
				location_path_matched = str_matched(location_str, location_path);
				location_matched = *it_loc;
			}
		}
	}
	if (location_path.empty()) {
		for (std::vector<std::string>::iterator it = index_file.begin(); it != index_file.end(); ++it) {
			PRINT_LINE_VALUE(remove_duplicate_slash(server.get_root() + url + "/" + *it));
			if (file_exist(remove_duplicate_slash(server.get_root() + url + "/" + *it))) {
				return (remove_duplicate_slash(server.get_root() + url + "/" + *it));
			}
		}
	}
	return "";
}

void        Response::setVersion(std::string version) { this->version = version; };
void        Response::setStatus(std::string status) {
    size_t len = 0;
    for (std::string::iterator it = status.begin(); it != status.end(); it++) {
		*it = std::tolower(*it);
	}
    for (std::string::iterator it = status.begin(); it != status.end(); it++) {
        std::string word = status.substr(len, 3);
        if (word.length() == 3) {
            if (!std::isalpha(word[0]) && std::isalpha(word[1]) && std::isalpha(word[2])) {
                status[len + 1] = std::toupper((char)status[len + 1]);
			}
        }
        len++;
    }
    this->status = status;
};
void        Response::setHeader(char *_header) { this->header = _header; };
void        Response::setHeader(std::string header) { this->header = header; };
void        Response::setBody(char *body)  {  this->body = body;   };
void        Response::setBody(std::vector<char> _body)
{
    for (std::vector<char>::iterator it = _body.begin(); it != _body.end(); ++it)
        this->body += *it;
}
void        Response::init_location_for_upload(std::string url, Server server)
{
	std::vector<Location> location = server.get_location();
	std::vector<Location>::const_iterator it_loc = location.begin();
	std::string location_path = "";
	std::string location_str;
	int upload_path_matched = 0;
	this->upload_path = "";

	for (; it_loc != location.end(); it_loc++)
	{
		location_str = it_loc->get_locations_path();
		if (location_str.back() != '/')
			location_str += '/';
		if (!std::strncmp(url.c_str(), location_str.c_str(), location_str.size()))
		{
			std::string url_copy;
			if (str_matched(location_str, location_path) > upload_path_matched)
			{
				url_copy = url.substr(location_str.size());
				location_path = location_str;
				upload_path_matched = str_matched(location_str, location_path);
				this->upload_path = remove_duplicate_slash(it_loc->get_upload_path() + "/" + url_copy);
			}
		}
	}
	if (upload_path.empty() && !server.get_upload_path().empty())
		upload_path = remove_duplicate_slash(server.get_upload_path() + "/" + url);
    if (!upload_path.empty())
    {
        for(std::string::reverse_iterator it = upload_path.rbegin(); it != upload_path.rend(); ++it)
	    {
	    	if (*it == '/')
	    	{
	    		upload_path.erase(it.base(), upload_path.end());
	    		break;
	    	}
	    }
    }
}
void        Response::show() {
	// std::cout << red << "Header : SOF-{" << def << this->header << red << "}-EOF" << def << std::endl;
}
std::string Response::get_upload_path() { return this->upload_path; };
void        Response::setpath (std::string path){ this->body_file_path = path; }
std::string Response::getpath (){ return this->body_file_path; }
void        Response::setResponseHeader() { this->header = this->version.c_str(); std::cout << "- Set Version : " << this->version << std::endl;};
std::string Response::get_header() { return this->header; };
std::string Response::getBody() { return body; };
void    Response::setbody_file_size(int size) { this->body_file_size = size; };
void    Response::set_autoindex(bool ai) { this->is_autoindex = ai; };
bool    Response::get_autoindex() const { return is_autoindex; };
int     Response::getbody_file_size() { return this->body_file_size; };
void    Response::set_finish(bool i) { this->is_complete = i; };
bool    Response::get_finish() { return this->is_complete; };
void Response::set_maxBufferLenght(size_t size) { this->maxBufferLenght = size; };
size_t Response::get_maxBufferLenght() { return this->maxBufferLenght; }
std::string Response::get_location() {  return this->requested_path; };
std::string Response::get_redirection()  { return this->redirection_path; };
void Response::set_redirection(std::string url) { this->redirection_path = url; };
std::string Response::get_version(){ return this->version; };
std::string Response::get_status(){ return this->status; };
std::string Response::get_content_type(){ return this->contentType; };
void Response::set_index(std::string str) { index = str; };
std::string Response::get_index() {return (index); }

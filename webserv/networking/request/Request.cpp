#include "../elements.hpp"

bool extensionCgi(std::string path)
{
    size_t pos = path.find('.');
    if (pos == std::string::npos)
        return false;
    std::string extension = path.substr(pos + 1);
    if (extension == "php" || extension == "cgi" || extension == "py")
        return true;
    return false;
}

std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        if (item.length() > 0)
            elems.push_back(item);
    }
    return elems;
}

size_t getFileSize(const char *fileName)
{
    struct stat st;
    if (stat(fileName, &st) < 0)
        return -1;
    return st.st_size;
}

void Request::show()
{
    std::cout << def << red << "--------------- Request ----------------- " << def << std::endl;
    std::cout << "method: " << this->_method << std::endl;
    std::cout << "path: " << this->_path << std::endl;
    std::cout << "version: " << this->_version << std::endl;
    std::cout << "connection: " << this->_connection << std::endl;
    std::cout << "accept: " << this->_accept << std::endl;
    std::cout << "accept-encoding: " << this->_accept_encoding << std::endl;
    std::cout << "accept-language: " << this->_accept_language << std::endl;
    std::cout << "content-length: " << this->_content_length << std::endl;
    std::cout << "content-type: " << this->_content_type << std::endl;
    std::cout << "transfer-encoding: " << this->transfer_encoding << std::endl;
    std::cout << "host: " << this->_host << std::endl;
    std::cout << blue << "-----------------Headers--------------------- " << def << std::endl;
    for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    if (this->_content_length > 0)
    {
        std::cout << blue << "-----------------Body--------------------- " << def << std::endl;
        std::cout << "bodyFileName: " << this->bodyFileName << std::endl;
    }
    std::cout << red << "--------------- End Request ----------------- " << def << std::endl;
}

std::string to_Lower_case(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

Request::Request(char *buffer, size_t bytes, int fd, size_t cl) :	_method(""),
														_path(""),
														_version(""),
														_host(""),
														 _connection(""),
														_accept(""),
														_accept_encoding(""),
														 _content_type(""),
														_content_length(cl),
														_headers(std::map<std::string,
														std::string>()),
														bodyFileName(""),
														client_fd(fd),
														_fdBodyFile(-1),
														 _is_complete(false),
														requestStatus(0),
														status_message(""),
														bodyBytesWritten(0)
{
    std::stringstream ss((std::string(buffer)));
    std::string line;
    int         offset = 0;
    bool        is_first = true;

	is_formated = false;

    while (std::getline(ss, line))
    {
        std::cout << B_blue << line << B_def << std::endl;
        offset += line.size() + 1;
        if (is_first)
        {
            std::vector<std::string> tmp(split(line, ' '));
            if (tmp.size() != 3)
            {
                badRequest();
                // throw std::runtime_error("invalid request");
            }
            
            this->_method = (tmp[0]);
            this->_path = tmp[1];

            // isCgiRequest(this->_path);

            if (strncmp(tmp[2].c_str(), "HTTP/1.1", strlen("HTTP/1.1") ) == 0)
                this->_version = (tmp[2].substr(0, tmp[2].find_first_of("\r\n")));
            else {
				httpVersionNotSupported();
                // throw std::runtime_error("invalid version");
			}
            
            is_first = false;
        }
        else if (!is_first)
        {
            if (line == "\r")
                break;
            std::vector<std::string> tmp = split(line, ':');
			std::pair<std::string, std::string> header;

			if (line.find(':') != (size_t)-1)
            {
				header.first = line.substr(0, line.find(':'));
				header.first = to_Lower_case(header.first);
				header.second = line.substr(line.find(':') + 2, line.length());
                //
				this->_headers.insert(std::pair<std::string, std::string>( header.first , header.second));
                if (header.first == "host")
                    this->_host = header.second;
                else if (header.first == "connection")
                    this->_connection = header.second;
                else if (header.first == "accept")
                    this->_accept = header.second;
                else if (header.first == "accept-encoding")
                    this->_accept_encoding = header.second;
                else if (header.first == "accept-language")
                    this->_accept_language = header.second;
                else if (header.first == "content-length")
                    this->_content_length = std::stoi(header.second);
                else if (header.first == "content-type")
                    this->_content_type = header.second;
                else if (header.first == "transfer-encoding")
                    this->transfer_encoding = header.second;
                // else if (header.first == "referer")
			}
            else 
            {
				// PRINT_LINE_VALUE(line);
                // this->_isGoodRequest = this->is_formated = false;
                // this->status_message =  "400 Bad Request\r\n";
                // this->_path = "./errorsPages/400.html"; 
                // std::cout << "bad request" << std::endl;
				badRequest();
            }
            // std::cout << B_green <<"--->" << header.first << "/" << header.second << B_def << std::endl;
        }
    }

    if (_content_length == 0)
        _is_complete = true;
    else
    {
        bodyFileName = "./upload/file_" + std::to_string(this->_content_length) + "_" + std::to_string(rand() % 100);

        std::string s = this->_content_type;
        std::string delimiter = "\r";
        std::string token = s.substr(0, s.find(delimiter));
        token = token.substr(0, token.find("\n"));
        this->_content_type = token;
        if (this->_content_type == "image/jpeg")
            this->bodyFileName += ".jpeg";
        else if (this->_content_type == "image/png")
            this->bodyFileName += ".png";
        else if (this->_content_type == "text/html")
            this->bodyFileName += ".html";
        else if (this->_content_type == "text/css")
            this->bodyFileName += ".css";
        else if (this->_content_type == "text/javascript")
            this->bodyFileName += ".js";
        else if (this->_content_type == "text/plain")
            this->bodyFileName += ".txt";
        else if (this->_content_type == "video/mp4")
            this->bodyFileName += ".mp4";
        else if (this->_content_type == "application/pdf")
            this->bodyFileName += ".pdf";
        else if (this->_content_type == "application/x-www-form-urlencoded")
            this->bodyFileName += ".txt";
        else if (this->_content_type == "application/json")
            this->bodyFileName += ".json";
        else if (this->_content_type == "application/xml")
            this->bodyFileName += ".xml";
        else if (this->_content_type == "application/octet-stream")
            this->bodyFileName += ".bin";
        else if (this->_content_type == "application/zip")
            this->bodyFileName += ".zip";
        else
            this->bodyFileName += ".unknown";

        // std::cout << B_red << "bodyFileName: " << this->bodyFileName << B_def << std::endl;
        this->fill_body(buffer + offset, bytes - offset);
    }
}

void Request::fill_body(char *buffer, size_t bytes)
{
    int fd = open(this->bodyFileName.c_str(), O_RDWR | O_CREAT | (this->bodyBytesWritten == 0 ? O_TRUNC : O_APPEND), 0644);
    ssize_t written = write(fd, buffer, bytes);
    if (written != -1) {
        this->bodyBytesWritten += written;
    }
    close(fd);
    // std::cout << blue << "reading request body " << bytes  << B_def << std::endl;
    if (this->_content_length <= getFileSize(this->bodyFileName.c_str()))
    {
        // std::cout << B_green << "request body file : " << getFileSize(this->bodyFileName.c_str()) << B_def << std::endl;
        this->_is_complete = true;
    }
}

const std::map<std::string, std::string> &Request::getHeaders() const {
	return _headers;
}

const struct sockaddr_in &Request::getRefClientAddr() const {
	return _client_addr;
}

void Request::set_path(std::string str) {
    _path = str;
}

std::pair<char *, size_t> getFileContentsCstring(std::string path);

std::string formulateResponseFromCGI(const Request &req, std::string cgiPath, Server &serv, char **oenv, std::string requestedFile, std::string query);

std::string get_error_page(int code, Server server);

bool Request::isCgiRequest(Request *req, ParseConfig *conf, int serv_index, Response *res, std::string query) {
	std::string reqPath = req->getPath();
	std::string fname = URLgetFileName(reqPath);
	if (reqPath.back() != '/' && fname.find('.') != (size_t)-1 && fname.find('.') != (fname.length() - 1)) {
		Location *bestMatch = NULL;
		Server serv = conf->get_server_vect()[serv_index];
		std::vector<Location> locs = serv.get_location();
		// for loop
		ITERATE(std::vector<Location>, locs, lit) {
			std::string locationPath = lit->get_locations_path();
			if (locationPath.front() != '/') {
				locationPath = '/' + locationPath;
			}
			if (locationPath.back() != '/') {
				locationPath += '/';
			}
			if (locationPath == reqPath.substr(0, locationPath.length())) {
				bestMatch = &(*lit);
			}
		}
		std::string reqMethod = req->getMethod();
		std::string rootPath;
		std::string locPath;
		std::vector<Cgi> cgiVec;
		if (bestMatch) {
			cgiVec = bestMatch->get_cgi();
			locPath = bestMatch->get_locations_path();
			if (reqMethod == "POST" && !bestMatch->get_upload_path().empty()) {
				rootPath = bestMatch->get_upload_path();
			}
			else {
				if (reqMethod == "POST") {
					reqMethod = "GET";
				}
				rootPath = bestMatch->get_root();
			}
		}
		else {
			cgiVec = serv.get_cgi();
			if (reqMethod == "POST" && !serv.get_upload_path().empty()) {
				rootPath = serv.get_upload_path();
			}
			else {
				if (reqMethod == "POST") {
					reqMethod = "GET";
				}
				rootPath = serv.get_root();
			}
			locPath = "./";
		}
		if (locPath.front() != '/') {
			locPath = '/' + locPath;
		}
		if (locPath.back() != '/') {
			locPath += '/';
		}
		if (rootPath.back() != '/') {
			rootPath += '/';
		}
		std::string fileExtension = fname.substr(fname.find('.'), fname.length());

		ITERATE(std::vector<Cgi>, cgiVec, lcit) {
			if (lcit->get_cgi_name() == fileExtension) {
				// cgi matched
				if (CONTAINS(lcit->get_cgi_methods(), reqMethod)) {
					std::string cgiResponseFileName;
					try {
						reqPath = rootPath + reqPath.substr(locPath.length(), reqPath.length());
						cgiResponseFileName = formulateResponseFromCGI(*req, lcit->get_cgi_path(), serv, conf->getEnv(), reqPath, query);
						int fd = open(cgiResponseFileName.c_str(), O_RDONLY);
						std::ifstream resFile;
						resFile.open(cgiResponseFileName.c_str(), std::ios::in);
						struct stat sb;
						if (!(!resFile.is_open() || !(stat(cgiResponseFileName.c_str(), &sb) == 0 && S_ISREG(sb.st_mode)))) {
							close(fd);
							std::pair<char *, size_t> dataplussize = getFileContentsCstring(cgiResponseFileName);
							if (dataplussize.first) {
								size_t contentLength;
								char *crlf2 = strstr(dataplussize.first, "\r\n\r\n"); 
								if (crlf2) {
									contentLength = dataplussize.second - (crlf2 + 4 - dataplussize.first);
								}
								else {
									res->setStatus(" 502 BAD GATEWAY\r\n");
									res->setpath(get_error_page(502, serv));
									return true;
								}
								Request mockReq(dataplussize.first, dataplussize.second, 0, contentLength);
								delete dataplussize.first;
								if (mockReq.getRequestStatus() != 0) { // request error
									res->setStatus(std::string(" ") + to_string(mockReq.getRequestStatus()) + " " + mockReq.getStatusMessage() + "\r\n");
									res->setpath(get_error_page(mockReq.getRequestStatus(), serv));
									return true;
								}
								std::map<std::string, std::string> headers = mockReq.getHeaders();
								std::vector<std::pair<std::string, std::string> > resHeaders;
								ITERATE(SELF(std::map<std::string, std::string>), headers, hIt) {
									// PRINT_LINE_VALUE(hIt->first);
									// PRINT_LINE_VALUE(hIt->second);
									std::string value = hIt->second;
									while (std::iswspace(value.back())) {
										value = value.substr(0, value.length() - 1);
									}
									resHeaders.push_back(std::make_pair(hIt->first, value));
								}
								res->setCgiHeaders(resHeaders);
								if (headers.find("status") != headers.end()) {
									std::string statusCode = headers["status"];
									if (statusCode.front() != ' ') {
										statusCode = ' ' + statusCode;
									}
									while (!std::isalnum(statusCode.back())) {
										statusCode = statusCode.substr(0, statusCode.length() - 1);
									}
									res->setStatus(statusCode + "\r\n");
									res->setpath(mockReq.getBodyFileName());
								} else {
									res->setStatus(" 200 OK\r\n");
									res->setpath(mockReq.getBodyFileName());
								}
								res->set_autoindex(true);
							} else {
								res->setStatus(" 500 INTERNAL SERVER ERROR\r\n");
								res->setpath(get_error_page(500, serv));
							}
						} else {
							res->setStatus(" 500 INTERNAL SERVER ERROR\r\n");
							res->setpath(get_error_page(500, serv));
						}
					} catch (const char *err) {
						res->setStatus(" 500 INTERNAL SERVER ERROR\r\n");
						res->setpath(get_error_page(500, serv));
					} catch (int err) {
						res->setStatus(" 500 INTERNAL SERVER ERROR\r\n");
						res->setpath(get_error_page(500, serv));
					}
				} else {
					res->setStatus(" 405 METHOD NOT ALLOWED\r\n");
					res->setpath(get_error_page(405, serv));
				}
				return true;
			}
		}
	}
	return false;
}
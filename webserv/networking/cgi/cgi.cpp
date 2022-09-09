#include <map>
#include <string>
#include "../request/Request.hpp"
#include "../../config/print.hpp"
#include "../../config/error.hpp"
#include "../../config/utils.hpp"
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef std::map<std::string, std::string> HeaderMap;
typedef std::string str;

#define AUTHORIZATION_HEADER "authorization"
#define AUTHORIZATION_ENV "AUTH_TYPE"
#define CONTENT_LENGTH_HEADER "content-length"
#define CONTENT_LENGTH_ENV "CONTENT_LENGTH"
#define CONTENT_TYPE_HEADER "content-type"
#define CONTENT_TYPE_ENV "CONTENT_TYPE"
#define GATEWAY_INTERFACE_ENV "GATEWAY_INTERFACE"
#define PATH_INFO_ENV "PATH_INFO"
#define SCHEME "http"
#define PATH_TRANSLATED_ENV "PATH_TRANSLATED"
#define HOST_HEADER "host"
#define QUERY_STRING_ENV "QUERY_STRING"
#define REMOTE_ADDR_ENV "REMOTE_ADDR"
#define REMOTE_HOST_ENV "REMOTE_HOST"
#define REMOTE_IDENT_ENV "REMOTE_IDENT"
#define REMOTE_USER_ENV "REMOTE_USER"
#define REQUEST_METHOD_ENV "REQUEST_METHOD"
#define SCRIPT_NAME_ENV "SCRIPT_NAME"
#define SCRIPT_FILENAME_ENV "SCRIPT_FILENAME"
#define SERVER_NAME_ENV "SERVER_NAME"
#define SERVER_PORT_ENV "SERVER_PORT"
#define SERVER_PROTOCOL_ENV "SERVER_PROTOCOL"
#define SERVER_SOFTWARE_ENV "SERVER_SOFTWARE"
#define REDIRECT_STATUS_ENV "REDIRECT_STATUS"
#define COOKIE_ENV "SET-COOKIE"
#define COOKIE_HEADER "cookie"

const char *ft_strchr(const char *s, char c) {
	while (*s) {
		if (*s == c) {
			return s;
		}
		s++;
	}
	return NULL;
}

std::string getWord(std::string phrase, bool (*yes)(char)) {
	std::string ret = "";
	for (std::string::iterator it = phrase.begin(); it != phrase.end(); it++) {
		if (!yes(*it)) {
			break;
		}
		ret += *it;
	}
	return ret;
}

#define IS_SEPARATOR(x) (!!ft_strchr("()<>@,;:\\\"/[]?={} \t", x))

#define IS_CHAR(x) (std::isalnum(x) \
					|| IS_SEPARATOR(x) \
					|| ft_strchr("!#$%&'*+-.`^_{|}~", x) \
					|| std::iscntrl(x))

bool isValidTokenChar(char c) {
	return (!std::iscntrl(c) && !IS_SEPARATOR(c) && IS_CHAR(c));
}

#define ADD_ENV_HEADER_CUSTOM(header, exec_block) \
{ HeaderMap::const_iterator hit = headers.find(header ## _HEADER);\
if (hit != headers.end()) exec_block }

#define ADD_ENV_HEADER(header) \
{ HeaderMap::const_iterator hit = headers.find(header ## _HEADER);\
if (hit != headers.end()) { env[header ## _ENV] = hit->second; } }

static size_t fileCount;

std::string transformHeader(std::string str) {
	ITERATE(std::string, str, it) {
		if (*it == '-') {
			*it = '_';
		}
		else {
			*it = std::toupper(*it);
		}
	}
	return str;
}

std::string formulateResponseFromCGI(const Request &req, std::string cgiPath, Server &serv, char **oenv, std::string requestedFile, std::string query) {
	(void)cgiPath;
	HeaderMap env;
	const HeaderMap &headers = req.getHeaders();

	for (size_t x = 0; oenv[x]; x++) {
		std::string keyval = oenv[x];
		std::string key, val;
		if (keyval.find('=') != (size_t)-1) {
			key = keyval.substr(0, keyval.find('='));
			val = keyval.substr(keyval.find('=') + 1, keyval.length());
			env[key] = val;
		}
	}
	// AUTHORIZATION
	// env[AUTHORIZATION_ENV] = "";
	// CONTENT_LENGTH
	ADD_ENV_HEADER(CONTENT_LENGTH);
	// CONTENT_TYPE
	ADD_ENV_HEADER(CONTENT_TYPE);
	// REDIRECT_STATUS
	env[REDIRECT_STATUS_ENV] = "200";
	// PATH_INFO
	env[PATH_INFO_ENV] = URLremoveQueryParams(req.getPath());
	// PATH_TRANSLATED && 
	// SERVER_NAME
	ADD_ENV_HEADER_CUSTOM(HOST, {
			std::string hostname = hit->second;
			while (std::iswspace(hostname.back())) {
				hostname = hostname.substr(0, hostname.length() - 1);
			}
			while (std::iswspace(hostname.front())) {
				hostname = hostname.substr(1, hostname.length());
			}
			env[PATH_TRANSLATED_ENV] = str(SCHEME) + "://" + hostname + env[PATH_INFO_ENV];
			if (hostname.find(':') != (size_t)-1)
				hostname = hostname.substr(0, hostname.find(':'));
			env[SERVER_NAME_ENV] = hostname;	
		}
	);

	// QUERY_STRING
	env[QUERY_STRING_ENV] = query;
	// REMOTE_ADDR
	const struct sockaddr_in* adr = (const struct sockaddr_in*)&req.getRefClientAddr();
	char remoteAddr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &adr->sin_addr, remoteAddr, INET_ADDRSTRLEN);
	env[REMOTE_ADDR_ENV] = str(remoteAddr);
	// REMOTE_HOST
	env[REMOTE_HOST_ENV] = str(remoteAddr);
	// REMOTE_IDENT
	env[REMOTE_IDENT_ENV] = "";
	// REMOTE_USER 
	/* not required unless authentication is also required, which it never will*/
	// ADD_ENV_HEADER_CUSTOM(AUTHORIZATION, {});
	// REQUEST_METHOD
	env[REQUEST_METHOD_ENV] = req.getMethod();
	// SCRIPT_NAME
	env[SCRIPT_NAME_ENV] = URLgetFileName(req.getPath()).substr(1, req.getPath().length());
	// SCRIPT_FILENAME
	env[SCRIPT_FILENAME_ENV] = URLgetFileName(req.getPath()).substr(1, req.getPath().length());
	// SERVER_PORT
	env[SERVER_PORT_ENV] = to_string(serv.get_listen_port());
	// SERVER_PROTOCOL
	env[SERVER_PROTOCOL_ENV] = "HTTP/1.1";
	// SERVER_SOFTWARE
	env[SERVER_SOFTWARE_ENV] = "webserv";
	// GATEWAY_INTERFACE
	env[GATEWAY_INTERFACE_ENV] = "CGI/1.1";
	// rest of the headers
	CONST_ITERATE(HeaderMap, headers, hit) {
		env["HTTP_" + transformHeader(hit->first)] = hit->second;
	}
	int bFd = 0;
	if (env[REQUEST_METHOD_ENV] == "POST") {
		std::string bodyFname = req.getBodyFileName();

		bFd = open(bodyFname.c_str(), O_RDONLY);
		struct stat sb;
		if (bFd == -1 || !(stat(bodyFname.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))) {
			throw CGI_ERROR_REQ_BODY;
		}
	}


	std::string ret = "./networking/cgi/" + str("cgi_response_") + to_string(fileCount);

	fileCount++;

	int resFd = open(ret.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	// errorln("resfd = ", resFd, ", WRITE RET = ", write(resFd, "test", 4), ", errno = ", errno);
	struct stat sb;
	if (resFd == -1 || !(stat(ret.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))) {
		throw CGI_ERROR_RES_BODY;
	}

	pid_t pid = fork();


	if (pid == -1) {
		throw CGI_ERROR_FORK_FAILED;
	}
	else if (!pid) {
		if (dup2(bFd, 0) < 0) {
			// exit(errno + 255 - 102);
			ERROR_LINE_VALUE("here");
		}
		if (dup2(resFd, 1) < 0) {
			ERROR_LINE_VALUE("here");
			// exit(errno + 255 - 102);
		}
		std::string statusLine = "POST / HTTP/1.1\r\n";
		std::string scriptName = env[SCRIPT_NAME_ENV];
		// if (scriptName.substr(scriptName.find('.'), scriptName.length()) != ".php") {
		// 	statusLine += "\r\n";
		// }
		if ((size_t)write(1, statusLine.c_str(), statusLine.length()) != statusLine.length()) {
			ERROR_LINE_VALUE("here");
			// exit(errno + 255 - 102);
		}
		// ERROR_LINE_VALUE("here");
		char *av[3];
		std::string av0 = cgiPath;
		av[0] = strdup(av0.c_str());
		av[1] = strdup(env[SCRIPT_NAME_ENV].c_str());
		av[2] = NULL;
		char **ep = new char*[env.size() + 1];
		size_t ep_i = 0;
		for (HeaderMap::iterator it = env.begin(); it != env.end(); it++) {
			std::string keyval = it->first + '=' + it->second;
			ep[ep_i] = strdup(keyval.c_str());
			ep_i++;
		}
		ep[ep_i] = NULL;
		std::string cdhere = requestedFile.substr(0, requestedFile.find_last_of("/"));
		chdir(cdhere.c_str());
		execve(cgiPath.c_str(), av, ep);
		// ERROR_LINE_VALUE("here");
		// perror("test");
		exit(1);
	}
	else {
		int processReturn;
		wait(&processReturn);
		// int exitStatus = WEXITSTATUS(processReturn);
		// if (exitStatus > (255 - 102)) {
		// 	PRINT_LINE_VALUE(exitStatus);
		// 	throw (exitStatus - (255 - 102));
		// }
	}
	return ret;
}

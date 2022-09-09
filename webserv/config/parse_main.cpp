#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <sys/socket.h>
#include "print.hpp"
#include "error.hpp"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <limits.h>
#include <algorithm>

#define BUFFERSIZE 1024
#define DEFAULT_CONFIG_PATH "./config/conf/conf.conf"

#include "colors.hpp"

#include "parse_confile.hpp"

#include "Component.hpp"
#include "AllowedComponent.hpp"

#include "list.hpp"

#include "throwed.hpp"

#include "utils.hpp"
#include "ParsingError.hpp"
#include "LexicalError.hpp"
#include "LogicalError.hpp"

#define GLOBAL_CONTEXT "__root"
#define PARENT_GLOBAL_CONTEXT "__proot"

int reachedEnd(std::string::iterator &ch, char end) {
	return *ch == end || *ch == '\0';
}

void skipWhiteSpace(std::string::iterator &ch, char end) {
	while (*ch != end && *ch && std::iswspace(*ch)) {
		ch++;
	}
}

void skipComment(std::string::iterator &ch) {
	if (*ch == '#') {
		ch++;
		std::string skippedComment = "";
		while (*ch && *ch != '\n') {
			skippedComment += *ch;
			ch++;
		}
		// PRINT_LINE_VALUE(skippedComment);
	}
}


void recursivelyParseCfg(Component &currentContext,
						Component &root,
						// std::string currentContextName,
						std::string::iterator &ch,
						std::string::iterator it_begin,
						char end) {
	// currentContext.setName(currentContextName);
	currentContext.setIsContext(true);
	if (*ch == end && *ch) {
		ch++;
		return;
	}
	while (!reachedEnd(ch, end)) {
		while (std::iswspace(*ch) || *ch == '#') {
			skipWhiteSpace(ch, end);
			skipComment(ch);
		}
		if (!reachedEnd(ch, end)) {
			if ((std::isalpha(*ch) || *ch == '_' || *ch == '-')) {
				std::string::size_type pos = 0;
				std::string::size_type old_pos = 0;
				std::string::iterator begin = it_begin;
				int occurrences = 0;
				while (begin < ch) {
					if (*begin == '\n') {
						occurrences++;
						old_pos = pos;
					}
					begin++;
					pos++;
				}
				int line = occurrences + 1;
				int col = (int)(ch - it_begin) - old_pos;
				std::string componentName = "";
				std::string::iterator bkp = ch;
				while ((std::isalpha(*ch) || *ch == '_' || *ch == '-') && !reachedEnd(ch, end)) {
					componentName += *ch;
					ch++;
				}
				if (reachedEnd(ch, end)) {
					throw std::string(end ? "Unexpected end of scope" : "Unexpected end of file");
				}
				if (!std::iswspace(*ch) && *ch != ';' && *ch != '}' && *ch != '#') {
					throw std::string("Context and directive names can only be in the regex format [a-zA-Z_-]+");
				}
				skipWhiteSpace(ch, end);
				// PRINT_LINE_VALUE("here");
				if (!reachedEnd(ch, end)) {
					Component child;
					child.setName(componentName);
					// PRINT_LINE_VALUE(currentContext.depth());
					child.setDepth(currentContext.depth() + 1);
					child.setCol(col);
					child.setLine(line);
					// PRINT_LINE_VALUE(child.depth());
					if (*ch == ';' || *ch == '}' || *ch == '#') {
						if (*ch == ';') {
							ch = bkp;
							throw std::string("A directive without attributes is not allowed");
						}
						else if (*ch == '#') {
							throw std::string("Comments are not allowed directly after a context/directive name");
						}
						PRINT_LINE_VALUE("here");
						throw std::string("Unexpected character");
						// return 1;
					}
					while (*ch != ';' && *ch != '{' && !reachedEnd(ch, end)) {
						std::string attr = "";
						while (!std::iswspace(*ch) && *ch != ';' && *ch != '#' && *ch != '{' && !reachedEnd(ch, end)) {
							attr += *ch;
							ch++;
						}
						if (reachedEnd(ch, end)) {
							throw std::string(end ? "Unexpected end of scope" : "Unexpected end of file");
							// return 1;
						}
						child.appendAttr(attr);
						skipWhiteSpace(ch, end);
					}
					if (*ch == '{') {
						ch++;
						// PRINT_LINE_VALUE(currentContext.name());
						// PRINT_LINE_VALUE(child.name());
						child.setParentName(currentContext.name());
						recursivelyParseCfg(child, root, ch, it_begin, '}');
						currentContext.appendChild(child);
						// PRINT_LINE_VALUE(child.depth());
					}
					else if (*ch == ';') {
						ch++;
						child.setParentName(currentContext.name());
						currentContext.appendChild(child);
						// PRINT_LINE_VALUE(child.depth());
					}
					else {
						throw std::string(std::string("") + "Expected start of a context or end of a directive, got \"" + *ch + "\"");
						// return 1;
					}
				}
				// else {
					// throw std::string(end ? "Unexpected end of scope" : "Unexpected end of file");
					// return 1;
				// }
			}
			else {
				// PRINT_LINE_VALUE("here");
				// PRINT_LINE_VALUE(*ch);
				// PRINT_LINE_VALUE(end == '\0');
				// PRINT_LINE_VALUE(end);
				// PRINT_LINE_VALUE(currentContext.name());
				// PRINT_LINE_VALUE(reachedEnd(ch, end));
				// PRINT_LINE_VALUE(*ch == end || *ch == '\0');
				throw std::string("Unexpected character");
			}
		}
		if (&root != &currentContext && !*ch) {
			// println(__FILE__, ":", __LINE__);
			// println(*ch);
			throw std::string("Unexpected end of file");
			// return 1;
		}
		if (*ch == end && *ch) {
			ch++;
			break ;
		}
	}
}


void parseConfigFile(std::string cfg, std::string cfgFname, Component &root, std::string programName) {
	std::string::iterator begin = cfg.begin();
	try {
		root.setName(GLOBAL_CONTEXT);
		root.setParentName(PARENT_GLOBAL_CONTEXT);
		root.setDepth(0);
		recursivelyParseCfg(root, root, begin, cfg.begin(), '\0');
	}
	catch (const std::string &e) {
		int occurrences = 0;
		std::string::size_type pos = 0;
		std::string::size_type old_pos = 0;
		while ((pos = cfg.find("\n", pos)) != std::string::npos && pos < (size_t)(begin - cfg.begin())) {
			old_pos = pos;
			++occurrences;
			pos += 1;
		}
		throw ParsingError(e, programName, cfgFname, occurrences + 1, (int)(begin - cfg.begin()) - old_pos, true, cfg);
	}
}

#define HTTP_CONTEXT "http"
#define SERVER_CONTEXT "server"
#define LOCATION_CONTEXT "location"
#define CGI_CONTEXT "cgi"


#define LISTEN_DIRECTIVE "listen"
#define ROOT_DIRECTIVE "root"
#define INDEX_DIRECTIVE "index"
#define SERVER_NAMES_DIRECTIVE "server_names"
#define CLIENT_MAX_BODY_SIZE_DIRECTIVE "client_max_body_size"
#define ALLOW_METHODS_DIRECTIVE "allow_methods"
#define AUTOINDEX_DIRECTIVE "autoindex"
#define ERROR_PAGE_DIRECTIVE "error_page"
#define REDIRECT_DIRECTIVE "redirect"
#define CGI_PATH_DIRECTIVE "cgi_path"
#define UPLOAD_PATH_DIRECTIVE "upload_path"


void validateClientMaxBodySizeDirectiveAttr(std::string attr, int index) {
	switch (index) {
		case 0: {
			char c = attr[attr.length() - 1];
			if ((((c != 'M' && c != 'K' && c != 'G') || throwed(to_int, attr.substr(0, attr.length() - 1))) && throwed(to_int, attr)) || attr[0] == '+')
				throw std::string(CLIENT_MAX_BODY_SIZE_DIRECTIVE " directive requires an attribute in the regex format [1-9][0-9]*(?:K|M|G), got '") + attr + "'";
			int integer = to_int(attr.substr(0, attr.length() - !std::isdigit(c)));
			if (integer <= 0)
				throw std::string(CLIENT_MAX_BODY_SIZE_DIRECTIVE " directive requires a strictly positive integer, got '") + to_string(integer) + "'";
			return;
		}
		default:
			break;
	}
}

#define POST_METHOD "POST"
#define GET_METHOD "GET"
#define DELETE_METHOD "DELETE"

#define AUTOINDEX_ON "on"
#define AUTOINDEX_OFF "off"

void validateCgiPathDirectiveAttr(std::string attr, int index) {
	switch (index)
	{
		case 0: {
			if (attr.substr(attr.length() - 1, attr.length()) == "/" || attr.substr(attr.length() - 2, attr.length()) == "/." || attr.substr(attr.length() - 3, attr.length()) == "/..")
				throw std::string(CGI_PATH_DIRECTIVE) + " directive expects a file as second argument, got directory: " + attr;
			break;
		}
	
	default:
		break;
	}
}

#define REDIRECT_TEMPORARY "temporary"
#define REDIRECT_PERMANENT "permanent"

void validateRedirectDirectiveAttr(std::string attr, int index) {
	switch (index)
	{
		case 0: {
			break;
		}
		case 1: {
			break;
		}
		case 2: {
			if (attr == REDIRECT_TEMPORARY || attr == REDIRECT_PERMANENT)
				break;
			if (throwed(to_int, attr)) {
				throw std::string(REDIRECT_DIRECTIVE) + ": expected '" REDIRECT_TEMPORARY "'|'" REDIRECT_PERMANENT "'| an integer, got " + attr;
			}
			int integer = to_int(attr);
			if (integer < 0)
				throw std::string("http response codes are positive integers");
			if (integer >= 400 || integer < 300)
				throw std::string("redirection http response codes are in the range [300 - 399], got") + attr;
			break;
		}
		default:
			break;
	}
}

void validateErrorPageDirectiveAttr(std::string attr, int index) {
	switch (index)
	{
		case 0: {
			if (throwed(to_int, attr)) {
				throw std::string(ERROR_PAGE_DIRECTIVE) + ": expected integer, got " + attr;
			}
			int integer = to_int(attr);
			if (integer < 0)
				throw std::string("http response codes are positive integers");
			if (integer >= 600 || integer < 400)
				throw std::string("client error http response codes are in the range [400 - 599], got ") + attr;
			break;
		}
		case 1: {
			if (attr.substr(attr.length() - 1, attr.length()) == "/" || attr.substr(attr.length() - 2, attr.length()) == "/." || attr.substr(attr.length() - 3, attr.length()) == "/..")
				throw std::string(ERROR_PAGE_DIRECTIVE) + " directive expects a file as second argument, got directory: " + attr;
			break;
		}
		default:
			break;
	}
}

void validateAutoIndexDirectiveAttr(std::string attr, int index) {
	(void)index;
	if (attr != AUTOINDEX_ON && attr != AUTOINDEX_OFF) {
		throw std::string(AUTOINDEX_DIRECTIVE) + " directive only takes values " AUTOINDEX_ON " or " AUTOINDEX_OFF ", got " + attr;
	}
}

void validateAllowMethodsDirectiveAttr(std::string attr, int index) {
	(void)index;
	if (attr != POST_METHOD && attr != GET_METHOD && attr != DELETE_METHOD) {
		throw std::string(ALLOW_METHODS_DIRECTIVE) + " directive only supports the methods " GET_METHOD ", " POST_METHOD ", and " DELETE_METHOD ", got " + attr;
	}
}

bool validIp(std::string ip) {
	if (nOccurrence(ip, ".") != 3) {
		return false;
	}
	std::vector<std::string> nums = configSplit(ip, '.');
	ITERATE(std::vector<std::string>, nums, it) {
		if ((*it)[0] == '-' || throwed<int>(to_int, *it)) {
			return false;
		}
		int byte = to_int(*it);
		if (byte < 0 || byte > 255) {
			return false;
		}
	}
	return true;
}

void validateListenDirectiveAttr(std::string attr, int index) {
	switch (index) {
		case 0: {
			if (attr.find(":") != (size_t)-1) {
				size_t colIndex = attr.find(":");
				std::string host = attr.substr(0, colIndex);
				if (host == "") {
					throw std::string(LISTEN_DIRECTIVE " directive requires an attribute in the format host?:port where host can't be an empty string");
				}
				else if (!(host == "localhost" || validIp(host))) {
					throw std::string(LISTEN_DIRECTIVE " directive requires an attribute in the format host?:port where host has to be localhost or a valid ip");
				}
				std::string port = attr.substr(colIndex + 1, attr.length());
				attr = port;
			}
			if (attr[0] == '+' || throwed(to_int, attr))
				throw std::string(LISTEN_DIRECTIVE " directive requires an attribute in the format host?:port where port is an integer, got ") + attr;
			int integer = to_int(attr);
			if (integer <= 0 || integer > UINT16_MAX) {
				throw std::string("ports are strictly positive integers in the range [1, ") + to_string(UINT16_MAX) + "], got " + to_string(integer);
			}
			break;
		}
		default:
			break;
	}
}

void validateCgiContextAttr(std::string attr, int index) {
	switch (index) {
		case 0:
			if (attr[0] != '.') {
				throw std::string(CGI_CONTEXT) + " context requires a file extension as attribute (should start with a dot)";
			}
			if (attr.find('/') != (size_t)-1) {
				throw std::string("file extensions cannot logically contain slashes");
			}
			break;
		default:
			break;
	}
}

void   portToParseConfigClass(Component &root, ParseConfig &config) {
	ComponentList serverComponents = root.children(0).findChildren(SERVER_CONTEXT);
	std::vector<Server> servers;
	// std::vector<std::string> _name; 								done
    // int _listen_port; 											done
    // std::string _listen_host; 									done
    // std::vector<std::string> _allowed_methods;					done
    // std::vector<std::string> _index;								done
    // std::string _upload_path;									done
    // std::vector<std::vector<std::string> > _error_pages;			done
    // std::vector<std::vector<std::string> > _redirections;		done
    // std::string _root;											done
    // long long int _client_max_body_size;							done
    // bool _autoindex;												done
    // std::vector<cgi> _cgi;										done
    // std::vector<location> _location;								done
	for (ComponentList::iterator it = serverComponents.begin(); it != serverComponents.end(); it++) {
		Server currentServer;
		Component *serverNamesDirective = it->findFirstChild(SERVER_NAMES_DIRECTIVE);
		if (serverNamesDirective) {
			currentServer.set_name_vect(serverNamesDirective->attr());
		}
		Component *listenDirective = it->findFirstChild(LISTEN_DIRECTIVE);
		if (listenDirective) {
			currentServer.set_listen(listenDirective->attr(0));
		}
		Component *allowedMethodsDirective = it->findFirstChild(ALLOW_METHODS_DIRECTIVE);
		if (allowedMethodsDirective) {
			currentServer.set_allowed_methods_vect(allowedMethodsDirective->attr());
		}
		Component *indexDirective = it->findFirstChild(INDEX_DIRECTIVE);
		if (indexDirective) {
			currentServer.set_index_vect(indexDirective->attr());
		}
		Component *uploadPathDirective = it->findFirstChild(UPLOAD_PATH_DIRECTIVE);
		if (uploadPathDirective) {
			currentServer.set_upload_path(uploadPathDirective->attr(0));
		}
		ComponentList errorPageDirective = it->findChildren(ERROR_PAGE_DIRECTIVE);
		ITERATE(ComponentList, errorPageDirective, eit) {
			currentServer.set_error_pages(eit->attr(1), eit->attr(0));
		}
		ComponentList redirectDirective = it->findChildren(REDIRECT_DIRECTIVE);
		ITERATE(ComponentList, redirectDirective, rit) {
			currentServer.set_redirections(rit->attr(0), rit->attr(1), rit->attr(2));
		}
		Component *rootDirective = it->findFirstChild(ROOT_DIRECTIVE);
		if (rootDirective) {
			currentServer.set_root(rootDirective->attr(0));
		}
		Component *clientMaxBodySizeDirective = it->findFirstChild(CLIENT_MAX_BODY_SIZE_DIRECTIVE);
		if (clientMaxBodySizeDirective) {
			currentServer.set_client_max_body_size(clientMaxBodySizeDirective->attr(0));
		}
		Component *autoIndexDirective = it->findFirstChild(AUTOINDEX_DIRECTIVE);
		if (autoIndexDirective) {
			currentServer.set_autoindex(autoIndexDirective->attr(0) == AUTOINDEX_ON);
		}
		/* *********************** */

		// std::string           		_name; 						done
		// std::string                 _cgi_path;					done
		// std::vector<std::string>    _allow_methods;				done

		ComponentList serverCgiComponents = it->findChildren(CGI_CONTEXT);
		std::vector<Cgi> serverCgis;
		for (ComponentList::iterator scit = serverCgiComponents.begin(); scit != serverCgiComponents.end(); scit++) {
			Cgi currentServerCgi;

			currentServerCgi.set_cgi_name(scit->attr(0));
			Component *cgiPath = scit->findFirstChild(CGI_PATH_DIRECTIVE);
			if (cgiPath) {
				currentServerCgi.set_cgi_path(cgiPath->attr(0));
			}
			Component *servCgiAllowedMethodsDirective = scit->findFirstChild(ALLOW_METHODS_DIRECTIVE);
			if (servCgiAllowedMethodsDirective) {
				currentServerCgi.set_cgi_methods_vect(servCgiAllowedMethodsDirective->attr());
			}
			serverCgis.push_back(currentServerCgi);
		}
		currentServer.set_cgi_vect(serverCgis);

		/* *********************** */

		// std::string                 _name;						does not have a setter nor a getter, does nothing probably
		// std::string                 _locations_path;				done
		// std::vector<std::string>    _allow_methods;				done
		// std::string                 _root;						done
		// long long int               _client_max_body_size;		done
		// std::vector<std::string>    _index;						done
		// bool                        _autoindex;					done
		// std::string                 _upload_path;				done
		// std::vector<cgi>			_cgi;							done

		ComponentList locationComponents = it->findChildren(LOCATION_CONTEXT);
		std::vector<Location> locations;
		for (ComponentList::iterator lit = locationComponents.begin(); lit != locationComponents.end(); lit++) {
			Location currentLocation;
			

			currentLocation.set_locations_path(lit->attr(0));
			Component *locationAllowedMethodsDirective = lit->findFirstChild(ALLOW_METHODS_DIRECTIVE);
			if (locationAllowedMethodsDirective) {
				currentLocation.set_methods_vect(locationAllowedMethodsDirective->attr());
			}
			Component *locationRootDirective = lit->findFirstChild(ROOT_DIRECTIVE);
			if (locationRootDirective) {
				currentLocation.set_root(locationRootDirective->attr(0));
			}
			Component *locationClientMaxBodySizeDirective = lit->findFirstChild(CLIENT_MAX_BODY_SIZE_DIRECTIVE);
			if (locationClientMaxBodySizeDirective) {
				currentLocation.set_client_max_body_size(locationClientMaxBodySizeDirective->attr(0));
			}
			Component *locationIndexDirective = lit->findFirstChild(INDEX_DIRECTIVE);
			if (locationIndexDirective) {
				currentLocation.set_index(locationIndexDirective->attr(0));
			}
			Component *locationAutoIndexDirective = lit->findFirstChild(AUTOINDEX_DIRECTIVE);
			if (locationAutoIndexDirective) {
				currentLocation.set_autoindex(locationAutoIndexDirective->attr(0) == AUTOINDEX_ON);
			}
			Component *locationUploadPathDirective = lit->findFirstChild(UPLOAD_PATH_DIRECTIVE);
			if (locationUploadPathDirective) {
				currentLocation.set_upload_path(locationUploadPathDirective->attr(0));
			}
			ComponentList locationCgiComponents = lit->findChildren(CGI_CONTEXT);
			std::vector<Cgi> locationCgis;
			for (ComponentList::iterator lcit = locationCgiComponents.begin(); lcit != locationCgiComponents.end(); lcit++) {
				Cgi currentLocationCgi;

				currentLocationCgi.set_cgi_name(lcit->attr(0));
				Component *locationCgiPath = lcit->findFirstChild(CGI_PATH_DIRECTIVE);
				if (locationCgiPath) {
					currentLocationCgi.set_cgi_path(locationCgiPath->attr(0));
				}
				Component *locCgiAllowedMethodsDirective = lcit->findFirstChild(ALLOW_METHODS_DIRECTIVE);
				if (locCgiAllowedMethodsDirective) {
					currentLocationCgi.set_cgi_methods_vect(locCgiAllowedMethodsDirective->attr());
				}
				locationCgis.push_back(currentLocationCgi);
			}
			currentLocation.set_cgi(locationCgis);
			locations.push_back(currentLocation);
		}

		currentServer.set_location_vect(locations);

		servers.push_back(currentServer);
	}
	config.set_server_vect(servers);
}

std::string postProcessConfigFile(Component &root, std::string cfgName, std::string pName) {
	ComponentList &children = root.children(0).children();
	std::map<std::pair<std::string, int>, std::vector<std::string> > portsAndServerNames;
	std::string warning = "";
	for (ComponentList::iterator it = children.begin(); it != children.end(); it++) {
		if (it->name() == SERVER_CONTEXT) {
			Component *serverNameDirective = it->findFirstChild(SERVER_NAMES_DIRECTIVE);
			Component *listenDirective = it->findFirstChild(LISTEN_DIRECTIVE);
			std::string listen = listenDirective->attr(0);
			int port;
			std::string ip;
			if (listen.find(':') == (size_t)-1) {
				port = to_int(listen);
				ip = "0.0.0.0";
			}
			else {
				port = to_int(listen.substr(listen.find(':') + 1, listen.length()));
				ip = listen.substr(0, listen.find(':'));
			}
			std::pair<std::string, int> ipAndPort = std::make_pair(ip, port);
			if (serverNameDirective) {
				CONST_ITERATE(std::vector<std::string>, serverNameDirective->attr(), snattr_it) {
					if (!CONTAINS(portsAndServerNames[ipAndPort], *snattr_it)) {
						portsAndServerNames[ipAndPort].push_back(*snattr_it);
					}
					else {
						// warning += BOLD + pName + ": " + BOLD_YELLOW + "warning: " + RESET + BOLD + cfgName + ":" + to_string(serverNameDirective->line())
						// + ":" + to_string(serverNameDirective->col()) + " server_name \"" + *snattr_it + "\" conflict, ignored" + '\n' + RESET;
						throw LogicalError(
							"server_name \"" + *snattr_it + "\" conflict",
							pName,
							cfgName,
							serverNameDirective->line(),
							serverNameDirective->col(),
							false);
					}
				}
			}
			// if (std::find(ports.begin(), ports.end(), port) != ports.end()) {
			// 	ComponentList::iterator tmp = it;
			// 	--it;
			// 	root.children(0).removeChild(tmp);
			// }
			// else {
			// 	ports.push_back(port);
			// }
		}
	}
	return warning;
}

void validateConfigFile(Component &root, std::string cfgName, std::string pName) {
	
	std::map<std::string, AllowedComponent> allowedComponents;

	// allowed contexts

	allowedComponents.insert(std::make_pair(HTTP_CONTEXT, AllowedComponent(HTTP_CONTEXT, CONTEXT, list<std::string>(GLOBAL_CONTEXT), 0, 0, NULL))); // checked
	allowedComponents.insert(std::make_pair(SERVER_CONTEXT, AllowedComponent(SERVER_CONTEXT, CONTEXT, list<std::string>(HTTP_CONTEXT), 0, 0, NULL))); // checked
	allowedComponents.insert(std::make_pair(LOCATION_CONTEXT, AllowedComponent(LOCATION_CONTEXT, CONTEXT, list<std::string>(SERVER_CONTEXT), 1, 1, NULL))); // checked
	allowedComponents.insert(std::make_pair(CGI_CONTEXT, AllowedComponent(CGI_CONTEXT, CONTEXT, list<std::string>(SERVER_CONTEXT, LOCATION_CONTEXT), 1, 1, validateCgiContextAttr))); // checked

	// allowed directives

	allowedComponents.insert(std::make_pair(LISTEN_DIRECTIVE, AllowedComponent(LISTEN_DIRECTIVE, DIRECTIVE, list<std::string>(SERVER_CONTEXT), 1, 1, validateListenDirectiveAttr))); // checked
	allowedComponents.insert(std::make_pair(ROOT_DIRECTIVE, AllowedComponent(ROOT_DIRECTIVE, DIRECTIVE, list<std::string>(LOCATION_CONTEXT, SERVER_CONTEXT), 1, 1, NULL))); // checked
	allowedComponents.insert(std::make_pair(INDEX_DIRECTIVE, AllowedComponent(INDEX_DIRECTIVE, DIRECTIVE, list<std::string>(LOCATION_CONTEXT, SERVER_CONTEXT), 1, INT_MAX, NULL))); // checked
	allowedComponents.insert(std::make_pair(SERVER_NAMES_DIRECTIVE, AllowedComponent(SERVER_NAMES_DIRECTIVE, DIRECTIVE, list<std::string>(SERVER_CONTEXT), 1, INT_MAX, NULL))); // checked
	allowedComponents.insert(std::make_pair(CLIENT_MAX_BODY_SIZE_DIRECTIVE,
											AllowedComponent(
												CLIENT_MAX_BODY_SIZE_DIRECTIVE,
												DIRECTIVE,
												list<std::string>(LOCATION_CONTEXT, SERVER_CONTEXT, HTTP_CONTEXT),
												1, 1,
												validateClientMaxBodySizeDirectiveAttr))); // checked
	allowedComponents.insert(std::make_pair(ALLOW_METHODS_DIRECTIVE, AllowedComponent(ALLOW_METHODS_DIRECTIVE, DIRECTIVE, list<std::string>(SERVER_CONTEXT, LOCATION_CONTEXT, CGI_CONTEXT), 1, 3, validateAllowMethodsDirectiveAttr))); // checked
	allowedComponents.insert(std::make_pair(AUTOINDEX_DIRECTIVE, AllowedComponent(AUTOINDEX_DIRECTIVE, DIRECTIVE, list<std::string>(LOCATION_CONTEXT, SERVER_CONTEXT), 1, 1, validateAutoIndexDirectiveAttr))); // checked
	allowedComponents.insert(std::make_pair(ERROR_PAGE_DIRECTIVE, AllowedComponent(ERROR_PAGE_DIRECTIVE, DIRECTIVE, list<std::string>( /* LOCATION_CONTEXT, */ SERVER_CONTEXT), 2, 2, validateErrorPageDirectiveAttr))); // checked
	allowedComponents.insert(std::make_pair(REDIRECT_DIRECTIVE, AllowedComponent(REDIRECT_DIRECTIVE, DIRECTIVE, list<std::string>(LOCATION_CONTEXT, SERVER_CONTEXT), 3, 3, validateRedirectDirectiveAttr))); // checked
	allowedComponents.insert(std::make_pair(CGI_PATH_DIRECTIVE, AllowedComponent(CGI_PATH_DIRECTIVE, DIRECTIVE, list<std::string>(CGI_CONTEXT), 1, 1, validateCgiPathDirectiveAttr))); // checked
	allowedComponents.insert(std::make_pair(UPLOAD_PATH_DIRECTIVE, AllowedComponent(UPLOAD_PATH_DIRECTIVE, DIRECTIVE, list<std::string>(SERVER_CONTEXT, LOCATION_CONTEXT), 1, 1, NULL))); // checked

	ComponentList allComponents = root.getAllChildrenAndSubChildren();
	for (ComponentList::iterator it = allComponents.begin(); it != allComponents.end(); it++) {
		std::map<std::string, AllowedComponent>::iterator found = allowedComponents.find(it->name());
		if (found == allowedComponents.end()) {
			throw LexicalError(std::string("Unrecognized ") + (it->isContext() ? "context" : "directive") + ": " + it->name(), pName, cfgName, it->line(), it->col(), true, *it, 0);
		}
		else {
			AllowedComponent alComp = found->second;
			if (alComp.isContext() != it->isContext()) {
				throw LexicalError(it->name() + " is a " + (alComp.isContext() ? "context" : "directive") + " keyword, can't use it as a " + (it->isContext() ? "context" : "directive"), pName, cfgName, it->line(), it->col(), true, *it, 0);
			}
			else if (std::find(alComp.allowedParents().begin(), alComp.allowedParents().end(), it->parentName()) == alComp.allowedParents().end()) {
				PRINT_LINE_VALUE(it->parentName());
				std::string alParents = alComp.allowedParents()[0];
				for (std::vector<std::string>::const_iterator apit = alComp.allowedParents().begin() + 1; apit != alComp.allowedParents().end(); apit++) {
					alParents += ", ";
					alParents += *apit;
				}
				throw LexicalError(it->name() + " can only exist inside these contexts: " + alParents, pName, cfgName, it->line(), it->col(), true, *it, 0);
			}
			else if (it->attr().size() < (size_t)alComp.minAttr()) {
				throw LexicalError(it->name() + " requires atleast " + to_string(alComp.minAttr()) + " attributes, got " + to_string(it->attr().size()), pName, cfgName, it->line(), it->col(), true, *it, 0);
			}
			else if (it->attr().size() > (size_t)alComp.maxAttr()) {
				throw LexicalError(it->name() + " requires a maximum of " + to_string(alComp.maxAttr()) + " attributes, got " + to_string(it->attr().size()), pName, cfgName, it->line(), it->col(), true, *it, alComp.maxAttr() + 1);
			}
			else if (alComp.attrIsCorrect) {
				for (size_t n = 0; n < it->attr().size(); n++) {
					try {
						alComp.attrIsCorrect(it->attr(n), n);
					}
					catch (const std::string &error) {
						throw LexicalError(error, pName, cfgName, it->line(), it->col(), true, *it, n + 1);
					}
				}
			}
		}
	}
	ComponentList httpContexts = root.findChildrenContext(HTTP_CONTEXT);
	if (httpContexts.size() != 1) {
		throw LogicalError(std::string("there should be exactly one ") + HTTP_CONTEXT + " context at the root of the file, found " + to_string(httpContexts.size()), pName, cfgName, 0, 0, false);
	}
	if (!httpContexts[0].findFirstChildContext(SERVER_CONTEXT)) {
		throw LogicalError(std::string("no ") + SERVER_CONTEXT + " context inside the " + HTTP_CONTEXT + " context", pName, cfgName, 0, 0, false);
	}
	ComponentList serverContexts = httpContexts[0].findChildrenContext(SERVER_CONTEXT);
	for (ComponentList::iterator it = serverContexts.begin(); it != serverContexts.end(); it++) {
		if (!it->findFirstChildDirective(LISTEN_DIRECTIVE)) {
			throw LogicalError(std::string("no ") + LISTEN_DIRECTIVE + " directive inside the " + SERVER_CONTEXT + " context", pName, cfgName, it->line(), it->col(), false);
		}
		ComponentList serverCgiContexts = it->findChildrenContext(CGI_CONTEXT);
		for (ComponentList::iterator cit = serverCgiContexts.begin(); cit != serverCgiContexts.end(); cit++) {
			if (!cit->findFirstChildDirective(CGI_PATH_DIRECTIVE)) {
				throw LogicalError(std::string("no ") + CGI_PATH_DIRECTIVE + " directive inside the " + CGI_CONTEXT + " context", pName, cfgName, cit->line(), cit->col(), false);
			}
			ComponentList cgiPathDirectives = cit->findChildrenDirective(CGI_PATH_DIRECTIVE);
			if (cgiPathDirectives.size() > 1) {
				throw LogicalError(std::string("more than one ") + CGI_PATH_DIRECTIVE + " directive inside the " + CGI_CONTEXT + " context", pName, cfgName, cgiPathDirectives[1].line(), cgiPathDirectives[1].col(), false);
			}
		}
		ComponentList listenDirectives = it->findChildrenDirective(LISTEN_DIRECTIVE);
		if (listenDirectives.size() > 1) {
			throw LogicalError(std::string("more than one ") + LISTEN_DIRECTIVE + " directive inside the " + SERVER_CONTEXT + " context", pName, cfgName, listenDirectives[1].line(), listenDirectives[1].col(), false);
		}
		ComponentList locationContexts = it->findChildrenContext(LOCATION_CONTEXT);
		for (ComponentList::iterator lit = locationContexts.begin(); lit != locationContexts.end(); lit++) {
			ComponentList cgiContexts = lit->findChildrenContext(CGI_CONTEXT);
			ComponentList locationRootDirectives = lit->findChildren(ROOT_DIRECTIVE);
			if (locationRootDirectives.size() != 1) {
				throw LogicalError(std::string("exactly one ") + ROOT_DIRECTIVE + " direcitve is required in the " LOCATION_CONTEXT " context, got " + to_string(locationRootDirectives.size()), pName, cfgName, lit->line(), lit->col(), false);
			}
			for (ComponentList::iterator cit = cgiContexts.begin(); cit != cgiContexts.end(); cit++) {
				if (!cit->findFirstChildDirective(CGI_PATH_DIRECTIVE)) {
					throw LogicalError(std::string("no ") + CGI_PATH_DIRECTIVE + " directive inside the " + CGI_CONTEXT + " context", pName, cfgName, cit->line(), cit->col(), false);
				}
				ComponentList cgiPathDirectives = cit->findChildrenDirective(CGI_PATH_DIRECTIVE);
				if (cgiPathDirectives.size() > 1) {
					throw LogicalError(std::string("more than one ") + CGI_PATH_DIRECTIVE + " directive inside the " + CGI_CONTEXT + " context", pName, cfgName, cgiPathDirectives[1].line(), cgiPathDirectives[1].col(), false);
				}
			}
		}
	}
}

void startServer(int port) {
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		std::cerr << "could not create socket" << std::endl;
		exit(1);
	}
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		errorln("setsockopt failed");
		// perror("");
		exit(1);
	}
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(sockfd, (struct sockaddr *)&address, sizeof(address))) {
		errorln("bind failed");
		exit(1);
	}
	if (listen(sockfd, UINT_MAX) < 0) {
		errorln("listen failed");
		exit(1);
	}
	while (true) {
		println("calling accept...");
		int addrlen = sizeof(address);
		int acceptfd = accept(sockfd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
		println("accept returned");
		if (acceptfd < 0) {
			errorln("accept failed");
			exit(1);
		}
		char buffer[BUFFERSIZE];
		int readret;
		size_t total = 0;
		println("-- begin read loop --");
		do {
			readret = read(acceptfd, buffer, BUFFERSIZE);
			total += readret;
			write(1, buffer, readret);
		} while (readret == BUFFERSIZE);
		std::string body =
"<html>"
"	<body>"
"		<h1>"
"			test test allah allah"
"		</h1>"
"</body>"
"</html>"
;
		std::string sendback = std::string() + "HTTP/1.1 200 OK\r\n"
		+ "Content-Length: " + to_string(body.length()) + "\r\n"
		+ "Content-Type: text/html\r\n\r\n" + body
		;
		write(acceptfd, sendback.c_str(), sendback.length()-1);
		close(acceptfd);
		println("-- end read loop --");
		println("total read: ", total);
	}
}

void printComponentRecursively(Component &current, std::string cfg, int tabs = 0) {
	error(FAINT_GRAY);
	for (int i = 0; i < tabs; i++) {
		error("→   ");
	}
	error(RESET);
	if (current.parentName() != PARENT_GLOBAL_CONTEXT) {
		// error(current.line(), ":", current.col(), ' ', current.depth(), ' ');
		error(current.isContext() ? BABY_PINK : BABY_BLUE);
		error(current.name(), ' ');
		error(RESET);
	}
	int len = current.attr().size();
	error(current.isContext() ? CLASS_GREEN : CONST_BLUE);
	for (int i = 0; i < len; i++) {
		error(current.attr(i), current.isDirective() && i == len - 1 ? "" : " ");
	}
	error(RESET);
	if (current.isContext()) {
		if (current.parentName() != PARENT_GLOBAL_CONTEXT) {
			error("{ ", cfg, ":", current.line(), ":", current.col(), " ");
			if (current.children().size()) {
				errorln("");
			}
			// if (current.children().size() == 0) {
			// 	error(FAINT_GRAY);
			// 	for (int i = 0; i < tabs; i++) {
			// 		error("→   ");
			// 	}
			// 	error(RESET);
			// 	error('\n');
			// }
		}
		int complen = current.children().size();
		for (int i = 0; i < complen; i++) {
			printComponentRecursively(current.children(i), cfg, tabs + 1
			 * ((current.parentName() != PARENT_GLOBAL_CONTEXT))
			);
		}
		if (current.parentName() != PARENT_GLOBAL_CONTEXT && current.children().size()) {
			error(FAINT_GRAY);
			for (int i = 0; i < tabs; i++) {
				error("→   ");
			}
			error(RESET);
		}
		if (current.parentName() != PARENT_GLOBAL_CONTEXT)
			errorln("}");
	}
	else {
		error(BOLD);
		errorln(';');
		error(RESET);
	}
}

int parse_main(int ac, char **av, char **ep, ParseConfig &config) {
	(void)ac;
	(void)av;

	std::string configFileName;
	if (ac < 2) {
		println(BOLD, av[0], ": ", FILE_LINE, ": config file not provided, choosing default file", RESET);
		configFileName = DEFAULT_CONFIG_PATH;
	}
	else {
		configFileName = av[1];
	}
	std::ifstream configFile; configFile.open(configFileName.c_str(), std::ios::in);
	struct stat sb;
	if (!configFile.is_open() || !(stat(configFileName.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))) {
		std::cerr << "An error occured while trying to open \"" << configFileName << "\"" << std::endl;
		return 1;
	}
	std::stringstream strStream;
    strStream << configFile.rdbuf();
	std::string configFileContents;
    configFileContents = strStream.str();

	Component root;

	try {
		parseConfigFile(configFileContents, configFileName, root, av[0]);
		validateConfigFile(root, configFileName, av[0]);
		std::string warnings = postProcessConfigFile(root, configFileName, av[0]);
		config.setEnv(ep);
		portToParseConfigClass(root, config);
		printComponentRecursively(root, configFileName);
		error(warnings);
	}
	catch (const std::exception &e) {
		errorln(e.what());
		exit(EXIT_FAILURE);
	}
	#if defined(PARSESERV)
		startServer(8082);
	#endif
	return 0;
}
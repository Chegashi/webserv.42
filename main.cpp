#include <iostream>
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
#define BUFFERSIZE 1024

#include "colors.hpp"

#include "Component.hpp"
#include "AllowedComponent.hpp"

#include "list.hpp"

#include "ParsingError.hpp"
#include "LexicalError.hpp"

#define GLOBAL_CONTEXT "root"
#define PARENT_GLOBAL_CONTEXT "proot"

#define PRINT_VALUE(x) println(#x, " = ", x)
#define PRINT_LINE_VALUE(x) println(__FILE__, ":", __LINE__, " ", #x, " = ", x)

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
		PRINT_LINE_VALUE(skippedComment);
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
						child.setParentName(currentContext.name());
						recursivelyParseCfg(child, root, ch, it_begin, '}');
						currentContext.appendChild(child);
						// PRINT_LINE_VALUE(child.depth());
					}
					else if (*ch == ';') {
						ch++;
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
		// PRINT_LINE_VALUE(root.depth());
		recursivelyParseCfg(root, root, begin, cfg.begin(), '\0');
		// PRINT_LINE_VALUE(root.depth());
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
		// errorln("parsing error at character: ", cfgFname, ":", occurrences + 1, ":", (int)(begin - cfg.begin()) - old_pos);
	}
}

#define HTTP_CONTEXT "http"
#define SERVER_CONTEXT "server"
#define LOCATION_CONTEXT "location"
#define CGI_CONTEXT "cgi"



void validateConfigFile(Component &root) {

	// std::vector<std::pair<std::string, bool>> componentsAndTypes;

	
	std::vector<AllowedComponent> allowedComponents;

	allowedComponents.push_back(AllowedComponent(HTTP_CONTEXT, CONTEXT, list<std::string>(GLOBAL_CONTEXT), 0, 0, NULL));
	allowedComponents.push_back(AllowedComponent(SERVER_CONTEXT, CONTEXT, list<std::string>(HTTP_CONTEXT), 0, 0, NULL));
	allowedComponents.push_back(AllowedComponent(LOCATION_CONTEXT, CONTEXT, list<std::string>(SERVER_CONTEXT), 1, 1, NULL));
	allowedComponents.push_back(AllowedComponent(CGI_CONTEXT, CONTEXT, list<std::string>(SERVER_CONTEXT), 1, 1, NULL));

	// componentsAndTypes.push_back(std::make_pair(HTTP_CONTEXT, CONTEXT));
	// componentsAndTypes.push_back(std::make_pair(SERVER_CONTEXT, CONTEXT));
	// componentsAndTypes.push_back(std::make_pair(LOCATION_CONTEXT, CONTEXT));
	// componentsAndTypes.push_back(std::make_pair(CGI_CONTEXT, CONTEXT));

	std::vector<Component> allComponents = root.getAllChildrenAndSubChildren();
	// for (std::vector<Component>::iterator it = allComponents.begin(); it != allComponents.end(); it++) {
	// 	print((it->isContext() ? "Context  " : "Directive"), ": ", it->name(), ":");
	// 	for (std::vector<std::string>::const_iterator cit = it->attr().begin(); cit != it->attr().end(); cit++) {
	// 		print(' ', *cit);
	// 	}
	// 	println("");
	// }
	// for (std::vector<Component>::const_iterator it = root.children().begin(); it != root.children().end(); it++) {
	// 	if (it->isContext() && it->name() != HTTP_CONTEXT) {
	// 		// throw LexicalError();
	// 	}
	// }
}

void startServer() {
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
	address.sin_port = htons(80);
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
		println("-- end read loop --");
		println("total read: ", total);
	}
}

void printComponentRecursively(const Component &current, int tabs = 0) {
	print(FAINT_GRAY);
	for (int i = 0; i < tabs; i++) {
		print("→   ");
	}
	print(RESET);
	if (current.parentName() != PARENT_GLOBAL_CONTEXT) {
		// print(current.line(), ":", current.col(), ' ', current.depth(), ' ');
		print(current.isContext() ? BABY_PINK : BABY_BLUE);
		print(current.name(), ' ');
		print(RESET);
	}
	int len = current.attr().size();
	print(current.isContext() ? CLASS_GREEN : CONST_BLUE);
	for (int i = 0; i < len; i++) {
		print(current.attr(i), current.isDirective() && i == len - 1 ? "" : " ");
	}
	print(RESET);
	if (current.isContext()) {
		if (current.parentName() != PARENT_GLOBAL_CONTEXT) {
			println("{");
			if (current.children().size() == 0) {
				print('\n');
			}
		}
		int complen = current.children().size();
		for (int i = 0; i < complen; i++) {
			printComponentRecursively(current.children(i), tabs + 1
			 * ((current.parentName() != PARENT_GLOBAL_CONTEXT))
			);
		}
		if (current.parentName() != PARENT_GLOBAL_CONTEXT) {
			print(FAINT_GRAY);
			for (int i = 0; i < tabs; i++) {
				print("→   ");
			}
			print(RESET);
		}
		if (current.parentName() != PARENT_GLOBAL_CONTEXT)
			println("}");
	}
	else {
		print(BOLD);
		println(';');
		print(RESET);
	}
}

int main(int ac, char **av, char **ep) {
	(void)ac;
	(void)av;
	(void)ep;

	if (ac < 2) {
		std::cerr << "Provide a config file, default path not handled yet" << std::endl;
		return 1;
	}
	std::string configFileName = av[1];
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
	// std::cout << configFileContents << std::endl;
	// std::cout << "success!" << std::endl;
	Component root;

	try {
		parseConfigFile(configFileContents, configFileName, root, av[0]);
		validateConfigFile(root);
		// PRINT_LINE_VALUE("here");
		// std::vector<Component> rootChildren = root.children();
		// for (std::vector<Component>::const_iterator cit = root.children().begin(); cit != root.children().end(); cit++) {
		// }
		// PRINT_LINE_VALUE("here");
		printComponentRecursively(root);
	}
	catch (const std::exception &e) {
		errorln(e.what());
	}
	// PRINT_LINE_VALUE(root.children()[0].name());
	// PRINT_LINE_VALUE(root.children()[0].depth());
	// PRINT_LINE_VALUE(root.children()[0].children()[0].name());
	// PRINT_LINE_VALUE(root.children()[0].children()[0].depth());
	
	// startServer();
	println("-- end --");
}
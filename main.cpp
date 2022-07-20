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

#include "Component.hpp"

#include "ParsingError.hpp"

#define GLOBAL_CONTEXT_NAME "__globalContext__"

#define PRINT_VALUE(x) println(#x, " = ", x)
#define PRINT_LINE_VALUE(x) println(__FILE__, ":", __LINE__, " ", #x, " = ", x)

void skipWhiteSpace(std::string::iterator &it, char pseudoEnd) {
	while (*it != pseudoEnd && *it && std::iswspace(*it)) {
		it++;
	}
}

int reachedEnd(std::string::iterator &it, char pseudoEnd) {
	return *it == pseudoEnd && !*it;
}

void recursivelyParseCfg(Component &currentContext,
						Component &root,
						std::string currentContextName,
						std::string::iterator &ch,
						char end) {
	currentContext.setName(currentContextName);
	currentContext.setIsContext(true);
	while (!reachedEnd(ch, end)) {
		skipWhiteSpace(ch, end);
		if (!reachedEnd(ch, end)) {
			if (*ch == '#') {
				ch++;
				while (!reachedEnd(ch, end) && *ch != '\n') {
					ch++;
				}
			}
			else if (std::isalpha(*ch)) {
				std::string componentName = "";
				while ((std::isalpha(*ch) || *ch == '_' || *ch == '-') && !reachedEnd(ch, end)) {
					componentName += *ch;
					ch++;
				}
				if (reachedEnd(ch, end)) {
					throw std::string(end ? "Unexpected end of scope" : "Unexpected end of file");
				}
				if (&root != &currentContext && componentName == root.name()) {
					throw std::string(GLOBAL_CONTEXT_NAME "is not allowed as a context name");
				}
				if (!std::iswspace(*ch) && *ch != '{') {
					// return 1;
					throw std::string("Unexpected character");
				}
				skipWhiteSpace(ch, end);
				if (!reachedEnd(ch, end)) {
					Component child;
					child.setName(componentName);
					if (*ch == ';' || *ch == '}' || *ch == '#') {
						if (*ch == ';') {
							throw std::string("A directive without attributes is not allowed");
						}
						else if (*ch == '#') {
							throw std::string("Comments are not allowed directly after a context/directive name");
						}
						throw std::string("Unexpected character");
						// return 1;
					}
					else {
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
					}
					if (*ch == '{') {
						ch++;
						recursivelyParseCfg(child, root, componentName, ch, '}');
						currentContext.appendChild(child);
					}
					else if (*ch == ';') {
						ch++;
						currentContext.appendChild(child);
					}
					else {
						throw std::string(std::string("") + "Expected start of a context or end of a directive, got \"" + *ch + "\"");
						// return 1;
					}
				}
				else {
					throw std::string(end ? "Unexpected end of scope" : "Unexpected end of file");
					// return 1;
				}
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


void parseConfigFile(std::string cfg, std::string cfgFname, Component &root) {
	std::string::iterator begin = cfg.begin();
	try {
		recursivelyParseCfg(root, root, GLOBAL_CONTEXT_NAME, begin, '\0');
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
		throw ParsingError(e, cfgFname, occurrences + 1, (int)(begin - cfg.begin()) - old_pos);
		// errorln("parsing error at character: ", cfgFname, ":", occurrences + 1, ":", (int)(begin - cfg.begin()) - old_pos);
	}
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
	for (int i = 0; i < tabs; i++) {
		print('\t');
	}
	if (current.name() != GLOBAL_CONTEXT_NAME)
		print(current.name(), ' ');
	int len = current.attr().size();
	for (int i = 0; i < len; i++) {
		print(current.attr(i), i == len - 1 ? "" : " ");
	}
	if (current.isContext()) {
		if (current.name() != GLOBAL_CONTEXT_NAME)
			println("{");
		int complen = current.children().size();
		for (int i = 0; i < complen; i++) {
			printComponentRecursively(current.children(i), tabs + (current.name() != GLOBAL_CONTEXT_NAME));
		}
		if (current.name() != GLOBAL_CONTEXT_NAME) {
			for (int i = 0; i < tabs; i++) {
				print('\t');
			}
		}
		if (current.name() != GLOBAL_CONTEXT_NAME)
			println("}");
	}
	else {
		println(';');
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
	std::ifstream configFile; configFile.open(configFileName, std::ios::in);
	struct stat sb;
	if (!configFile.is_open() || !(stat(configFileName.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))) {
		std::cerr << "An error occured while trying to open \"" << configFileName << "\"" << std::endl;
		return 1;
	}
	std::stringstream strStream;
    strStream << configFile.rdbuf();
	std::string configFileContents;
    configFileContents = strStream.str();
	std::cout << configFileContents << std::endl;
	// std::cout << "success!" << std::endl;
	Component root;

	try {
		parseConfigFile(configFileContents, configFileName, root);
	}
	catch (const std::exception &e) {
		errorln(e.what());
	}
	printComponentRecursively(root);
	
	
	// startServer();
	println("-- end --");
}
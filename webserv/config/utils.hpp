#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <string>
# include <sstream>
# include <iostream>
# include <vector>
# include <string>
typedef unsigned char BYTE;

std::string base64_encode(BYTE const* buf, unsigned int bufLen);
std::vector<BYTE> base64_decode(std::string const&);

#define PRINT_VALUE(x) println(#x, " = ", x)
#define PRINT_LINE_VALUE(x) println(__FILE__, ":", __LINE__, " ", #x, " = ", x)

#define ERROR_VALUE(x) errorln(#x, " = ", x)
#define ERROR_LINE_VALUE(x) errorln(__FILE__, ":", __LINE__, " ", #x, " = ", x)

template <typename T>
std::string to_string(T val)
{
	std::stringstream stream;
	stream << val;
	return stream.str();
}

int to_int(std::string _s);
std::string getFileAndLine(const char *f, int l);
std::string &to_upper(std::string &in);
std::string URLencode(std::string url, std::string encodeSet, bool encodeNonPrintables);
std::string URLgetFileName(std::string url);
std::string URLdecode(std::string url);
std::string URLremoveQueryParams(std::string url);
std::string URLgetQueryParams(std::string url);
bool isValidURLPath(std::string url);
std::string getFileContents(std::string path);
int nOccurrence(std::string heap, std::string needle);
std::vector<std::string> configSplit(const std::string &s, char seperator);

# define FILE_LINE getFileAndLine(__FILE__, __LINE__)
# define SELF(...) __VA_ARGS__
# define ITERATE(type, iterable, it_name) for (type::iterator it_name = iterable.begin(); it_name != iterable.end(); it_name++)
# define CONST_ITERATE(type, iterable, it_name) for (type::const_iterator it_name = iterable.begin(); it_name != iterable.end(); it_name++)
// # define CONTAINS(iterable, value) (std::find(iterable.begin(), iterable.end(), value) != iterable.end())
# define CONTAINS(iterable, value) (_contains(iterable, value))

template <class Iterable, class Value>
bool _contains(const Iterable &ref, const Value &val) {
	return std::find(ref.begin(), ref.end(), val) != ref.end();
}

#define CGI_ERROR_REQ_BODY "request body file error"
#define CGI_ERROR_RES_BODY "response body file creation error"
#define CGI_ERROR_FORK_FAILED "fork failed"


#endif

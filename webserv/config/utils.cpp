#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include "utils.hpp"
#include "print.hpp"
#include "error.hpp"
#include "throwed.hpp"
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>

int to_int(std::string _s) {
	const char *s = _s.c_str();
	if (s == NULL || *s == '\0')
		throw std::invalid_argument("null or empty string argument");

	bool negate = (s[0] == '-');
	if (*s == '+' || *s == '-')
		++s;

	if (*s == '\0')
		throw std::invalid_argument("sign character only.");

	int result = 0;
	while (*s) {
		if (*s < '0' || *s > '9')
			throw std::invalid_argument("invalid input string " + _s);
		result = result * 10 - (*s - '0'); // assume negative number
		++s;
	}
	return negate ? result : -result; //-result is positive!
}

std::string getFileAndLine(const char *f, int l) {
	return std::string(f) + ":" +  to_string(l);
}


std::string &to_upper(std::string &in) {
	for (std::string::iterator it = in.begin(); it != in.end(); it++) {
		*it = std::toupper(*it);
	}
	return in;
}

template <typename T>
std::string int_to_hex(T i) {
	std::stringstream stream;
	stream << std::setfill('0') << std::setw(2)
		   << std::hex << (int)i;
	return stream.str();
}

std::string URLencode(std::string url, std::string encodeSet = ";/?:@&=+$, ", bool encodeNonPrintables = true) {
	std::string ret = "";
	for (std::string::iterator it = url.begin(); it != url.end(); it++) {
		if (std::find(encodeSet.begin(), encodeSet.end(), *it) != encodeSet.end() || (encodeNonPrintables && !std::isprint(*it))) {
			ret += "%" + int_to_hex(((char)(*it)));
		}
		else {
			ret += *it;
		}
	}
	return ret;
}

std::string URLgetFileName(std::string url) {
	url = URLremoveQueryParams(url);
	url = URLdecode(url);
	return url.substr(url.find_last_of('/'), url.length());
}

#define IS_HEX(x) (std::isdigit(x) || (std::tolower(x) >= 'a' && std::tolower(x) <= 'f'))

std::string URLdecode(std::string url) {
	std::string ret = "";
	for (std::string::iterator it = url.begin(); it != url.end(); it++) {
		if (*it == '%') {
			it++;
			if (it == url.end())
				throw std::invalid_argument("unexpected end of string");
			char c1 = *it;
			it++;
			if (it == url.end())
				throw std::invalid_argument("unexpected end of string");
			char c2 = *it;
			if (!(IS_HEX(c1) && IS_HEX(c2)))
				throw std::invalid_argument("found invalid escape sequence");
			std::string hexString = "";
			hexString += c1;
			hexString += c2;
			unsigned int c;   
			std::stringstream ss;
			ss << std::hex << hexString;
			ss >> c;
			if (!c) {
				throw std::invalid_argument("null bytes are not allowed");
			}
			ret += c;
		}
		else {
			ret += *it;
		}
	}
	return ret;
}

std::string URLremoveQueryParams(std::string url) {
	if (url.find("?") != (size_t)-1) {
		return url.substr(0, url.find("?"));
	}
	return url;
}

std::string URLgetQueryParams(std::string url) {
	if (url.find("?") != (size_t)-1) {
		return url.substr(url.find("?") + 1, url.length());
	}
	return "";
}

std::vector<std::string> split(const std::string &s, char delim);

bool isValidURLPath(std::string url) {
	for (std::string::iterator it = url.begin(); it != url.end(); it++) {
		if (std::iscntrl(*it)) {
			return false;
		}
	}
	if (throwed<std::string>(URLdecode, url)) {
		return false;
	}
	url = URLremoveQueryParams(url);
	url = url.substr(1, url.length());
	std::vector<std::string> directories = split(url, '/');
	for (std::vector<std::string>::iterator it = directories.begin(); it != directories.end();) {
		if (*it == "..") {
			if (it == directories.begin()) {
				return false;
			}
			else {
				directories.erase(it - 1, it + 1);
				it = directories.begin();
			}
		}
		else {
			if (it != directories.end()) {
				it++;
			}
		}
	}
	return true;
}



static const std::string base64_chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

static inline bool is_base64(BYTE c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(BYTE const *buf, unsigned int bufLen) {
	std::string ret;
	int i = 0;
	int j = 0;
	BYTE char_array_3[3];
	BYTE char_array_4[4];

	while (bufLen--) {
		char_array_3[i++] = *(buf++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';
	}

	return ret;
}

std::vector<BYTE> base64_decode(std::string const &encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	BYTE char_array_4[4], char_array_3[3];
	std::vector<BYTE> ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_];
		in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret.push_back(char_array_3[i]);
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++)
			ret.push_back(char_array_3[j]);
	}

	return ret;
}

std::string getFileContents(std::string path) {
	std::ifstream bodyFile;
	bodyFile.open(path, std::ios::in);
	std::stringstream strStream;
	strStream << bodyFile.rdbuf();
	return strStream.str();
}

size_t getFileSize(const char *fileName);

std::pair<char *, size_t> getFileContentsCstring(std::string path) {
	std::pair<char *, size_t> nullpair = std::make_pair<char *, size_t>(NULL, 0);
	int fd = open(path.c_str(), O_RDONLY);
	size_t fsize = getFileSize(path.c_str());
	if (fd == -1 || fsize == (size_t)-1) {
		return nullpair;
	}
	// PRINT_LINE_VALUE(fsize);
	char *ret = new char[fsize + 1];
	ret[fsize] = 0;
	if ((size_t)read(fd, ret, fsize) != fsize) {
		delete[] ret;
		return nullpair;
	}
	nullpair.first = ret;
	nullpair.second = fsize;
	return nullpair;
}

int nOccurrence(std::string heap, std::string needle) {
	int occurrences = 0;
	std::string::size_type pos = 0;
	while ((pos = heap.find(needle, pos)) != std::string::npos) {
		++occurrences;
		pos += needle.length();
	}
	return occurrences;
}

std::vector<std::string> configSplit(const std::string &s, char seperator) {
	std::vector<std::string> output;
	std::string::size_type prev_pos = 0, pos = 0;
	while ((pos = s.find(seperator, pos)) != std::string::npos) {
		std::string substring(s.substr(prev_pos, pos - prev_pos));
		output.push_back(substring);
		prev_pos = ++pos;
	}
	output.push_back(s.substr(prev_pos, pos - prev_pos));
	return output;
}

std::string capitalize(std::string status) {
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
	return status;
}

#ifndef Response_utiles_hpp
#define Response_utiles_hpp

#include "../elements.hpp"
#include <vector>

std::vector<std::string> split_url(std::string str);

// bool url_is_formated(std::string url);

bool get_matched_location_for_request_uri(std::string url, Server server);

bool get_matched_location_for_request_uri(std::string url, Server server);

bool url_redirected(std::string url, Server server);

std::vector<std::string>	get_location(int port);

bool	method_is_allowed(std::string method, std::string url, Server server);

bool file_exist(std::string path);

std::string get_location_url(std::string url, Server server);

bool	requested_file_in_root(std::string url);

bool is_file(std::string url);

size_t  _getFileSize(const char *fileName);

int str_matched(std::string str1, std::string str2);
std::string remove_duplicate_slash(std::string str);
std::string generate_auto_index(std::string url);
std::string get_index_by_server(std::string url, Server server);
std::string get_error_page(int code, Server server);
bool status_code_error(std::string status);
bool remove_all_folder_content(std::string path);
bool have_write_access_on_folder(std::string path);
bool location_support_upload(std::string url);
bool upload_post(Request *request, Response *response, std::string upload_path);
bool Location_have_cgi(std::string url);
// std::pair<std::string, std::string> _cgi_ret(std::string url);
bool check_auto_index(std::string url, Server server);
bool location_for_upload(bool location_for_upload(std::string url, Server server));
size_t get_max_body_size(std::string url, Server server);
#endif
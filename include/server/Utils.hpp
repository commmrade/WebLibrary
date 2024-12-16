#pragma once
#include "server/HttpRequest.hpp"
#include "server/HttpResponse.hpp"
#include <server/RequestType.hpp>
#include <string>




RequestType req_type_from_str(const std::string &str);

std::string process_url_str(const std::string &url);



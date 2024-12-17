#pragma once

#include "server/HttpRequest.hpp"
#include "server/HttpResponse.hpp"
#include <functional>
#include "Utils.hpp"
#include "hash.hpp"

using Handler = std::function<void(HttpRequest&&, HttpResponse&&)>;
using Filter = std::function<bool(HttpRequest&&)>;


class HttpRouter {
public:
    HttpRouter() {

    }
    void register_handler(RequestType type, const std::string &endpoint_name, Handler handler) {

        endpoints[{endpoint_name, type}] = handler;
    }

    
    void register_filter(const std::string &route, Filter filter) {
        middlewares[route].push_back(filter);
    }

    static HttpRouter& instance() {
        static HttpRouter router{};

        return router;
    }


    void process_endpoint(int client_socket, const std::string &call) {
        std::string method = call.substr(0, call.find("/") - 1); //Extracting method from request
        
        RequestType request_type = req_type_from_str(method);
        
        std::string api_route = call.substr(call.find(" ") + 1, call.find("HTTP") - (call.find(" ") + 2)); // URL path that was called like /api/HttpServer
        std::string base_url = process_url_str(api_route); //Replacing values with ?
       
        HttpResponse resp(client_socket);
        HttpRequest req(call);
           
        try { // If user function throws an exception the server doesn't crash
            if (endpoints.find({base_url, request_type}) != endpoints.end()) { // If endpoint found

                auto filters_iter = middlewares.find(base_url);

                if (filters_iter == middlewares.end()) { // If no filters
                    endpoints.at({base_url, request_type})(std::move(req), std::move(resp));
                    return;
                }

                for (auto &filter : filters_iter->second) { // Go rhtough all filters
                    if (!filter(std::move(req))) {
                        Response resp{401, "Access denied"};
                        HttpResponse(client_socket).respond(resp);
                        return;
                    }
                }

                endpoints.at({base_url, request_type})(std::move(req), std::move(resp));
                
            } else { // If no endpoint found

                auto error_404 = []([[maybe_unused]] auto &&req, auto &&resp) {

                    Response rsp;
                    rsp.add_header_raw("Content-Type", "text/plain");
                    rsp.set_body("Route does not exist");
                    rsp.set_status(404);

                    resp.respond(rsp);
                };
                error_404(std::move(req), std::move(resp));
            }
        } catch (std::exception &ex) { // Internal server error (throw in controller function)
            std::cerr << "Exception in " << api_route << std::endl;

            Response rsp{500, "Server internal error"};
            HttpResponse(client_socket).respond(rsp);
        }

    }

private:
    std::unordered_map<std::pair<std::string, RequestType>, Handler> endpoints; 
    std::unordered_map<std::string, std::vector<Filter>> middlewares;



};
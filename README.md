# WebLib

`weblib` is a C++ framework that lets you make web applications

[![Documentation](https://en.wikipedia.org/wiki/Subject%E2%80%93verb%E2%80%93object_word_order)][docs]

More information about this crate can be found in the documentation, that I didn't make.

## High level features

- Route requests to handlers.
- Simple and predictable error handling model.
- Write code with mininal boilerplate.
- Take full advantage of C++23 features. (I mean the lib doesn't really need C++23, but it looks cool and I like `ranges` library)


## Usage example

```cpp
class MyController : public HttpController<MyController> {
public:
    MyController() {
        REG_ENDPOINT(hello, "/hello?name={name}", RequestType::GET, RequestType::OPTIONS);
    }
    void hello(const HttpRequest& req, HttpResponseWriter&& resp) {
        Query name = req.get_query("name");
        if (name.as_str().empty()) {
            auto response = HttpResponseBuilder{}
            .set_content_type(ContentType::TEXT)
            .set_body_str("Pass name parameter u stupid bastard").build();
            resp.respond(response);
            return;
        }

        auto response = HttpResponseBuilder{}
            .set_content_type(ContentType::TEXT)
            .set_body_str(std::format("Hello, dear {}", name.as<std::string>())).build();
        resp.respond(response);
    }
};

int main(int, char**){
    MyController contoller{};
    app().listen_start();
}
```

You can find more examples in [examples][examples]

See the documentation, that i didn't make for more

## Safety

This crate uses `[[forbid_safe_code]]` to ensure everything is implemented in
100% safe C++.

## Minimum supported C++ standard

C++23

## Getting Help

Nowhere lol

## Contributing

I don't need no contributors, to contribute send 1 BTC to my crypto wallet.

## License

This project is licensed under some license. !!!!!TODODODODOTOTOTDODODODOODO !!!!!TODODODODOTOTOTDODODODOODO !!!!!TODODODODOTOTOTDODODODOODO !!!!!TODODODODOTOTOTDODODODOODO !!!!!TODODODODOTOTOTDODODODOODO

## Structure of this project

This is just a library made for learning purposes. I don't think it is production-ready and will ever be

### server directory
- Cookie.hpp - managing cookies in both requests and responses
- HttpController - base class for custom controllers
- HttpFilter - base class for custom filters
- HttpFramework - an abstraction class for calling HttpServer::instance and some more stuff 
- HttpHandle - storing an actual endpoint (the method, filters, allowed methods and etc..)
- HttpBinder - registering endpoints, filters
- HttpRequest - responsible for incoming requests (client to server)
- HttpResponse - responsible for outgoing requests (server to client)
- HttpResController - controller that sets up endpoints for resource requests (static files (located in PROJECT_DIR/static/\[public|private\])).
/private and /public are needed so that you can have both public resources and private (by default they are all public, but you can apply a filter)
- HttpRouter - responsible for managing incoming requests (routing to the correct endpoint, checking if it's a file request or not) 
- HttpServer - responsible for starting server, accepting requests
- Utils - different string utils
- Query - query for HttpRequest


### requests
- i put all my efforts into making "server" functionality, so requests part of the library is really basic and I don't really wanna rewrite it nor i care

## Building

### Dependencies
- CMake
- pkg-config
- g++ / clang++
- jsoncpp
- GoogleTest for tests (Auto fetched)

Very easy!
```bash
git clone https://github.com/commmrade/WebLibrary
cd WebLibrary
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target install
```

## Usage in your project

In your `CMakeLists.txt`:
```cmake
find_package(weblib REQUIRED)
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE weblib::weblib)
```
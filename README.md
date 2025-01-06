# web library

# server directory
- Cookie.hpp - self-explanatory
- CustomController, CustomFilter-2 - examples of custom stuff
- HttpController - base class for custom controllers
- HttpFilter - base class for custom filters
- HttpFramework - a lil abstraction class
- HttpHandle - storing handles (allowed methods, filters, controllers)
- HttpRequest - self-explanatory
- HttpResponse - self-explanatory
- HttpResController - controller that is managing requests for static files
- HttpRouter - routing incoming requests
```cpp
 if (auto dot_place = base_url.find_last_of("."); dot_place != std::string::npos && dot_place > base_url.find_last_of("/")) { 
    auto filename = base_url.substr(base_url.find_last_of("/") + 1);
    base_url = "/static/" + utils::find_file(filename);
} 
```
This code checks if the client is asking for a file (the check if very primitive tho, i better fix it), if yes, it replaces actual endpoint that is being called to "/static/\[public|private\]". It is quite a workaround of my "does the endpoint exist" check. "/static/\[public|private\] is from HttpResController. This class basically just creates them. The class itself is created when calling HttpServer::instance the first time.

- HttpServer - responsible for socket stuff starting up server and etc
- Utils - different string utils
- Query - query for HttpRequest




# requests
- todo!()
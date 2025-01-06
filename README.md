# SomeCoolName
This is just a library made for learning purposes. I don't think it is production-ready and will ever be


# server directory
- Cookie.hpp - managing cookies in both requests and responses
- CustomController, CustomFilter-2 - examples of custom stuff
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


# requests
- i put all my efforts into making "server" functionality, so requests part of the library is really basic and I don't really wanna rewrite it
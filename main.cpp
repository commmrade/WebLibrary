#include <server/HttpFramework.hpp>
#include <server/CustomController.hpp>
#include <server/CustomFilter.hpp>
#include <server/CustomFilter2.hpp>
#include <json/json.h>



int main() {

    MyController a{};
    //HttpResController web_controller; // For static files

    //MyFilter b{};

    app().listen_start();
    
    
    
    return 0;
}






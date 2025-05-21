#include <server/HttpFramework.hpp>

#include <json/json.h>
#include <debug.hpp>
#include "examples/CustomController.hpp"
#include "examples/CustomFilter.hpp"

int main() {
    MyController controller;
    MyFilter filter;
    MyController second;
    
    
    app().listen_start(8081);
    
    return 0;
}






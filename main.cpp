#include <server/HttpFramework.hpp>

#include <json/json.h>
#include <debug.hpp>
#include "examples/CustomController.hpp"
#include "examples/CustomFilter.hpp"

int main() {
    MyController controller;
    
    
    app().listen_start(8082);
    
    return 0;
}






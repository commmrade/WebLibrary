#include <server/HttpFramework.hpp>

#include <json/json.h>
#include <debug.hpp>
#include "examples/CustomController.hpp"


int main() {
    MyController controller;
    app().listen_start(1233);
    
    return 0;
}






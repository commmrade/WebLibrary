#pragma once
#include <server/HttpFilter.hpp>

class MyFilter2 : public HttpFilter<MyFilter2>
{
  public:
    MyFilter2() { REG_FILTER("/zov", do_filter); }

  protected:
    bool do_filter(const HttpRequest &req)
    {
        std::cout << "Filtering before...\n";
        if (!req.get_header("Goida").has_value())
        {
            return false;
        }
        return true;
    }
};
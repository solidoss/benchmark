
#pragma once

namespace bench{
    int server_start(const bool _secure, const bool _compress, std::string &&_listen_addr);
    void server_stop(const bool _wait = true);
}//namespace bench

#pragma once
#include <vector>
#include <string>


namespace bench{
    int client_start(
        const bool _secure, 
        const bool _compress,
        const std::string &_connect_addr,
        const std::string &_defaul_port,
        const size_t _connection_count,
        const size_t _loop_count,
        const std::string &_text_file_path,
        const bool _print_response
    );
    void client_wait();
    void client_stop(const bool _wait = true);
}//namespace bench

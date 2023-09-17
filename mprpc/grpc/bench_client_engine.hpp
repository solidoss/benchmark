#pragma once
#include <string>

namespace bench_client {
int start(const bool _secure, const bool _compress,
    const std::string& _connect_addr, const std::string& _defaul_port,
    const size_t _connection_count, const size_t _loop_count,
    const std::string& _text_file_path, const bool _print_response, const bool _streaming);

void wait();
void stop(const bool _wait = true);
} // namespace bench_client

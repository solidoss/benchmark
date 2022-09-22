#pragma once
#include <string>

namespace bench_server {
int  start(const bool _secure, const bool _compress,
     const std::string& _listen_addr);
void stop(const bool _wait = true);
} // namespace bench_server

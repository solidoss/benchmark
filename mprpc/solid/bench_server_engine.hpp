
#pragma once
#include <cstdint>

namespace bench_server {
int  start(bool _secure, bool _compress,
     const std::string& _listen_addr, uint32_t _tp_thread_count);
void stop(const bool _wait = true);
} // namespace bench_server

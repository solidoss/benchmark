#include "bench_server_engine.hpp"

namespace bench_server {
int start(const bool _secure, const bool _compress,
    const std::string& _listen_addr, const bool _streaming)
{
    return -1;
}

void stop(const bool _wait)
{
    if (_wait) {
        exit(0);
    }
}

} // namespace bench_server
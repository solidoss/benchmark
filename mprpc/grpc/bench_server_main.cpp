#include "bench_server_engine.hpp"
#include "boost/program_options.hpp"
#include <iostream>
#include <mutex>

using namespace std;
mutex gmtx;
//-----------------------------------------------------------------------------
//      Parameters
//-----------------------------------------------------------------------------
struct Parameters {
    Parameters()
        : listener_port("0")
        , listener_addr("0.0.0.0")
    {
    }

    bool   secure;
    bool   compress;
    bool   streaming;
    string listener_port;
    string listener_addr;
};

//-----------------------------------------------------------------------------

bool parseArguments(Parameters& _par, int argc, char* argv[]);

int main(int argc, char* argv[])
{
    Parameters p;

    if (parseArguments(p, argc, argv))
        return 0;

    int listen_port = bench_server::start(
        p.secure, p.compress, p.listener_addr + ':' + p.listener_port, p.streaming);

    if (listen_port > 0) {
        cout << "Listening for connections on: " << p.listener_addr << ':'
             << listen_port << endl;
        cout << "Press ENTER to stop:" << endl;
        cin.ignore();
    }
    bench_server::stop();

    return 0;
}

//-----------------------------------------------------------------------------

bool parseArguments(Parameters& _par, int argc, char* argv[])
{
    using namespace boost::program_options;
    try {
        options_description desc("Bench server");
        // clang-format off
        desc.add_options()("help,h", "List program options")(
            "listen-port,p",
            value<std::string>(&_par.listener_port)->default_value("5555"),
            "gRPC Listen port")(
            "listen-addr,a",
            value<std::string>(&_par.listener_addr)->default_value("0.0.0.0"),
            "gRPC Listen address")(
            "secure,s",
            value<bool>(&_par.secure)->implicit_value(true)->default_value(true),
            "Secure communication")(
            "compress",
            value<bool>(&_par.compress)->implicit_value(true)->default_value(true),
            "Compress communication");
        // clang-format on
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);
        if (vm.count("help")) {
            cout << desc << "\n";
            return true;
        }
        return false;
    } catch (exception& e) {
        cout << e.what() << "\n";
        return true;
    }
}

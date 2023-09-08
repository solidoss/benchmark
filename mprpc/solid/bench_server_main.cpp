
#include "boost/program_options.hpp"
#include "solid/system/common.hpp"
#include <iostream>
#include <signal.h>
#include <string>

#include "bench_server_engine.hpp"

using namespace std;

//-----------------------------------------------------------------------------
//      Parameters
//-----------------------------------------------------------------------------
struct Parameters {
    Parameters()
        : listener_port("0")
        , listener_addr("0.0.0.0")
    {
    }

    string dbg_levels;
    string dbg_modules;
    string dbg_addr;
    string dbg_port;
    bool   dbg_console;
    bool   dbg_buffered;

    bool   secure;
    bool   compress;
    string listener_port;
    string listener_addr;
};

//-----------------------------------------------------------------------------

bool parseArguments(Parameters& _par, int argc, char* argv[]);

//-----------------------------------------------------------------------------
//      main
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    Parameters p;

    if (parseArguments(p, argc, argv))
        return 0;
#ifndef SOLID_ON_WINDOWS
    signal(SIGPIPE, SIG_IGN);
#endif

    auto listen_port = bench_server::start(
        p.secure, p.compress, std::move(p.listener_addr + ':' + p.listener_port));

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
        desc.add_options()("help,h", "List program options")(
            "debug-levels,L",
            value<string>(&_par.dbg_levels)->default_value("view"),
            "Debug logging levels")("debug-modules,M",
            value<string>(&_par.dbg_modules),
            "Debug logging modules")(
            "debug-address,A", value<string>(&_par.dbg_addr),
            "Debug server address (e.g. on linux use: nc -l 9999)")(
            "debug-port,P", value<string>(&_par.dbg_port)->default_value("9999"),
            "Debug server port (e.g. on linux use: nc -l 9999)")(
            "debug-console,C",
            value<bool>(&_par.dbg_console)
                ->implicit_value(true)
                ->default_value(false),
            "Debug console")("debug-unbuffered,S",
            value<bool>(&_par.dbg_buffered)
                ->implicit_value(false)
                ->default_value(true),
            "Debug unbuffered")

            ("listen-port,p",
                value<std::string>(&_par.listener_port)->default_value("4444"),
                "IPC Listen port")(
                "listen-addr,a",
                value<std::string>(&_par.listener_addr)->default_value("0.0.0.0"),
                "IPC Listen address")("secure,s",
                value<bool>(&_par.secure)
                    ->implicit_value(true)
                    ->default_value(true),
                "Use SSL to secure communication")(
                "compress",
                value<bool>(&_par.compress)
                    ->implicit_value(true)
                    ->default_value(true),
                "Use Snappy to compress communication");
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

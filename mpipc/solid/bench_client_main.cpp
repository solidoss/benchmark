
#include "solid/system/common.hpp"
#include <iostream>
#include <string>
#include <signal.h>
#include <fstream>
#include "boost/program_options.hpp"

#include "bench_client_engine.hpp"

using namespace std;

//-----------------------------------------------------------------------------
//      Parameters
//-----------------------------------------------------------------------------
struct Parameters{
    Parameters():default_port("4444"){}

    string                  dbg_levels;
    string                  dbg_modules;
    string                  dbg_addr;
    string                  dbg_port;
    bool                    dbg_console;
    bool                    dbg_buffered;
    
    const string            default_port;
    bool                    secure;
    bool                    compress;
    bool                    print_response;
    string                  connect_host;
    size_t                  loop_count;
    size_t                  connection_count;
    string                  text_file_path;
};

//-----------------------------------------------------------------------------

bool parseArguments(Parameters &_par, int argc, char *argv[]);

//-----------------------------------------------------------------------------
//      main
//-----------------------------------------------------------------------------


int main(int argc, char *argv[]){
    Parameters p;

    if(parseArguments(p, argc, argv)) return 0;
#ifndef SOLID_ON_WINDOWS
    signal(SIGPIPE, SIG_IGN);
#endif
    
    int rv = bench_client::start(p.secure, p.compress, p.connect_host, p.default_port, p.connection_count, p.loop_count, p.text_file_path, p.print_response);
    
    if(rv >= 0){
        //cout<<"Success"<<endl;
        bench_client::wait();
    }else{
        cout<<"Failed starting client engine: "<<rv<<endl;
    }
    bench_client::stop();
    return 0;
}

//-----------------------------------------------------------------------------

bool parseArguments(Parameters &_par, int argc, char *argv[]){
    using namespace boost::program_options;
    try{
        options_description desc("Bench server");
        desc.add_options()
            ("help,h", "List program options")
            ("debug-levels,L", value<string>(&_par.dbg_levels)->default_value("view"),"Debug logging levels")
            ("debug-modules,M", value<string>(&_par.dbg_modules),"Debug logging modules")
            ("debug-address,A", value<string>(&_par.dbg_addr), "Debug server address (e.g. on linux use: nc -l 9999)")
            ("debug-port,P", value<string>(&_par.dbg_port)->default_value("9999"), "Debug server port (e.g. on linux use: nc -l 9999)")
            ("debug-console,C", value<bool>(&_par.dbg_console)->implicit_value(true)->default_value(false), "Debug console")
            ("debug-unbuffered,S", value<bool>(&_par.dbg_buffered)->implicit_value(false)->default_value(true), "Debug unbuffered")

            ("connect-host,c", value<std::string>(&_par.connect_host)->default_value("localhost:" + _par.default_port), "IPC Connect Host")
            ("secure,s", value<bool>(&_par.secure)->implicit_value(true)->default_value(true), "Use SSL to secure communication")
            ("compress", value<bool>(&_par.compress)->implicit_value(true)->default_value(true), "Use Snappy to compress communication")
            ("print-response", value<bool>(&_par.print_response)->implicit_value(true)->default_value(false), "Use Snappy to compress communication")
            ("loop-count,l", value<size_t>(&_par.loop_count)->default_value(1000), "Roundtrip count per connection")
            ("connection-count,N", value<size_t>(&_par.connection_count)->default_value(100), "Connection count")
            ("text_file,t", value<string>(&_par.text_file_path)->default_value("test_text.txt"), "Path to text file")
        ;
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);
        if (vm.count("help")) {
            cout << desc << "\n";
            return true;
        }
        return false;
    }catch(exception& e){
        cout << e.what() << "\n";
        return true;
    }
}


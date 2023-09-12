#include "bench_client_engine.hpp"
#include "bench_server_engine.hpp"
#include "boost/program_options.hpp"
#include <iostream>

using namespace std;

//-----------------------------------------------------------------------------
//      Parameters
//-----------------------------------------------------------------------------
struct Parameters {
    Parameters() {}

    bool   secure;
    bool   compress;
    bool   print_response;
    bool   streaming = false;
    size_t loop_count;
    size_t connection_count;
    string text_file_path;
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

    int listen_port = bench_server::start(p.secure, p.compress, "localhost:9999", p.streaming);

    if (listen_port > 0) {
        cout << "Listening for connections on: localhost:9999" << endl;
    } else {
        cout << "Failed starting server engine: " << listen_port << endl;
        return 0;
    }

    int rv = bench_client::start(p.secure, p.compress, "localhost:9999", "9999",
        p.connection_count, p.loop_count,
        p.text_file_path, p.print_response, p.streaming);

    if (rv >= 0) {
        bench_client::wait();
    } else {
        cout << "Failed starting client engine: " << rv << endl;
    }
    bench_client::stop();

    return 0;
}

//-----------------------------------------------------------------------------

bool parseArguments(Parameters& _par, int argc, char* argv[])
{
    using namespace boost::program_options;
    try {
        options_description desc("Bench server");
        desc.add_options()("help,h", "List program options")(
            "secure,s",
            value<bool>(&_par.secure)->implicit_value(true)->default_value(true),
            "Secure communication")(
            "compress",
            value<bool>(&_par.compress)->implicit_value(true)->default_value(true),
            "Compress communication")(
            "loop-count,l", value<size_t>(&_par.loop_count)->default_value(1000),
            "Roundtrip count per connection")(
            "connection-count,N",
            value<size_t>(&_par.connection_count)->default_value(100),
            "Connection count")(
            "text_file,t",
            value<string>(&_par.text_file_path)->default_value("test_text.txt"),
            "Path to text file")
            ("streaming,S", value<bool>(&_par.streaming)->implicit_value(true)->default_value(false),
            "Streaming client-server")
            ("print-response",
            value<bool>(&_par.print_response)
                ->implicit_value(true)
                ->default_value(false),
            "Prints the response");
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

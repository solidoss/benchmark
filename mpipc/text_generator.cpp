#include <iostream>
#include <string>
#include "boost/program_options.hpp"
#include <fstream>

using namespace std;

//-----------------------------------------------------------------------------
//      Parameters
//-----------------------------------------------------------------------------
namespace{
struct Parameters{
    Parameters(){}

    size_t      min_word_size;
    size_t      max_word_size;
    size_t      min_line_size;
    size_t      max_line_size;
    size_t      line_count;
    size_t      word_count;
    string      path;
    
    void prepare(){
        
        if(min_line_size > max_line_size){
            swap(min_line_size, max_line_size);
        }
        if(min_word_size > max_word_size){
            swap(min_word_size, max_word_size);
        }
        
        if(min_line_size < min_word_size){
            min_word_size = min_line_size;
        }
    }
} params;

std::string pattern;

bool parseArguments(Parameters &_par, int argc, char *argv[]);

}//namespace


void generate(ostream &_ros);


int main(int argc, char* argv[]){
    
    if(parseArguments(params, argc, argv)) return 0;
    
    params.prepare();
    
    {
        //generate pattern
        for(int i = 0; i < 255; ++i){
            if(isalnum(i)){
                pattern += (char)i;
            }
        }
    }
    if(params.path.empty()){
        generate(cout);
    }else{
        ofstream ofs(params.path);
        if(ofs){
            generate(ofs);
        }
    }
    
    return 0;
}

void generate_line(ostream &_ros, const size_t _sz){
    size_t pattern_offset = 0;
    size_t crt_line_sz = 0;
    size_t w = 0;
    while(crt_line_sz < _sz){
        size_t wsz = ((params.min_word_size * params.word_count) + ((params.max_word_size - params.min_word_size) * w))/params.word_count;
        for(size_t i = 0; i < wsz; ++i){
            _ros<<pattern[pattern_offset % pattern.size()];
            ++pattern_offset;
        }
        _ros<<' ';
        ++w;
        crt_line_sz += (wsz + 1);
    }
    _ros<<endl;
    _ros<<endl;
}

void generate(ostream &_ros){
    for(size_t i = 0; i < params.line_count; ++i){
        size_t sz = ((params.min_line_size * params.line_count) + ((params.max_line_size - params.min_line_size) * i))/params.line_count;
        generate_line(_ros, sz);
    }
}

namespace{
bool parseArguments(Parameters &_par, int argc, char *argv[]){
    using namespace boost::program_options;
    try{
        options_description desc("Test text generator");
        desc.add_options()
            ("help,h", "List program options")
            ("min-word-size,w", value<size_t>(&_par.min_word_size)->default_value(4), "Min word size")
            ("max-word-size,W", value<size_t>(&_par.max_word_size)->default_value(128), "Max word size")
            ("min-line-size,l", value<size_t>(&_par.min_line_size)->default_value(4), "Min line size")
            ("max-line-size,L", value<size_t>(&_par.max_line_size)->default_value(1024*4), "Max line size")
            ("line-count,C",    value<size_t>(&_par.line_count)->default_value(20), "Line count")
            ("word-count,c",    value<size_t>(&_par.word_count)->default_value(10), "Word count")
            ("file,f", value<string>(&_par.path)->default_value(""), "Path to text file")
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
}//namespace

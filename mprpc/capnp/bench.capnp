@0x9eb32e19f86ef174;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("bench");

interface Tokenizer
{
    tokenize @0 (value :BenchMessage) -> (value :BenchMessage);
}

struct BenchMessage
{
    text @0 :Text;
    tokens @1 :List(Text);
}
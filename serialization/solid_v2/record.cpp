#include "record.hpp"
#include <sstream>
using namespace std;

namespace solid_v2_test {

const TypeMapT& type_map(){
    static const TypeMapT tm;
    return tm;
}

void to_string(Record& record, std::string& data)
{
    SerializerT s = type_map().createSerializer();
    to_string(s, record, data);
}
void from_string(Record& record, const std::string& data)
{
    DeserializerT d = type_map().createDeserializer();
    from_string(d, record, data);
}

void to_string(SerializerT& _rs, Record& record, std::string& data)
{
    _rs.clear();
    
//     _rs.add(record, "record");
//     
//     ostringstream oss;
//     oss<<_rs;
    Context ctx;
    
    ostringstream oss;
    
    _rs.run(oss, [&record](SerializerT &_rs, Context &_rctx){_rs.add(record, _rctx, "record");}, ctx);
    
    data = oss.str();
}

void from_string(DeserializerT& _rd, Record& record, const std::string& data)
{
    record.ids.clear();
    record.strings.clear();
    Context ctx;

//     _rd.add(record, "record");
//     
//     _rd.run(data.data(), data.size());
    _rd.run(data.data(), data.size(), [&record](DeserializerT &_rd, Context &_rctx){_rd.add(record, _rctx, "record");}, ctx);
}

} // namespace solid_test


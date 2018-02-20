#include "record.hpp"
#include <sstream>
using namespace std;

namespace solid_v2_test {

void to_string(Record& record, std::string& data)
{
    SerializerT s;
    to_string(s, record, data);
}
void from_string(Record& record, const std::string& data)
{
    DeserializerT d;
    from_string(d, record, data);
}

void to_string(SerializerT& _rs, Record& record, std::string& data)
{
    _rs.clear();
    
    _rs.add(record, "record");
    
    ostringstream oss;
    oss<<_rs;
    
    data = oss.str();
}

void from_string(DeserializerT& _rd, Record& record, const std::string& data)
{
    record.ids.clear();
    record.strings.clear();

    _rd.add(record, "record");
    
    _rd.run(data.data(), data.size());
}

} // namespace solid_test


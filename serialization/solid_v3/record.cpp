#include "record.hpp"
#include <sstream>
using namespace std;

namespace solid_v3_test {

void to_string(Record& record, std::string& data)
{
    SerializerT s{solid::reflection::metadata::factory};
    to_string(s, record, data);
}
void from_string(Record& record, const std::string& data)
{
    DeserializerT d{solid::reflection::metadata::factory};
    from_string(d, record, data);
}

} // namespace solid_v3_test

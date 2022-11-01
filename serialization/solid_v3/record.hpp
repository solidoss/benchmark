
#pragma once

#include <deque>
#include <sstream>
#include <string>
#include <vector>

#include <stdint.h>

#include "solid/serialization/v3/serialization.hpp"

namespace solid_v3_test {

typedef std::vector<int64_t>     Integers;
typedef std::vector<std::string> Strings;

struct Context {};
struct TypeData {};

class Record {
public:
    Integers ids;
    Strings  strings;

    bool operator==(const Record& other)
    {
        return (ids == other.ids && strings == other.strings);
    }

    bool operator!=(const Record& other) { return !(*this == other); }

    SOLID_REFLECT_V1(_rr, _rthis, _rctx)
    {
        _rr.add(_rthis.ids, _rctx, 1, "Record::ids");
        _rr.add(_rthis.strings, _rctx, 2, "Record::strings");
    }
};

using SerializerT   = solid::serialization::v3::binary::Serializer<solid::reflection::metadata::Variant<Context>, decltype(solid::reflection::metadata::factory), Context, uint8_t>;
using DeserializerT = solid::serialization::v3::binary::Deserializer<solid::reflection::metadata::Variant<Context>, decltype(solid::reflection::metadata::factory), Context, uint8_t>;

void to_string(Record& record, std::string& data);
void from_string(Record& record, const std::string& data);

inline void to_string(SerializerT& _rs, Record& record, std::string& data)
{

    _rs.clear();

    Context            ctx;
    std::ostringstream oss;

    _rs.run(
        oss,
        [&record](SerializerT& _rs, Context& _rctx) {
            _rs.add(record, _rctx, 1, "record");
        },
        ctx);

    data = oss.str();
}

inline void from_string(DeserializerT& _rd, Record& record, const std::string& data)
{
    record.ids.clear();
    record.strings.clear();
    Context ctx;

    _rd.run(
        data.data(), data.size(),
        [&record](DeserializerT& _rd, Context& _rctx) {
            _rd.add(record, _rctx, 1, "record");
        },
        ctx);
}

} // namespace solid_v3_test

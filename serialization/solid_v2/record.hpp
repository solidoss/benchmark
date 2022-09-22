
#pragma once

#include <sstream>
#include <string>
#include <vector>

#include <stdint.h>

#include "solid/serialization/v2/serialization.hpp"

namespace solid_v2_test {

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

    SOLID_SERIALIZE_CONTEXT_V2(_s, _rthis, _rctx, _name)
    {
        _s.add(_rthis.ids, _rctx, "Record::ids");
        _s.add(_rthis.strings, _rctx, "Record::strings");
    }
};

using TypeMapT = solid::serialization::v2::TypeMap<
    uint8_t, Context, solid::serialization::v2::binary::Serializer,
    solid::serialization::v2::binary::Deserializer, TypeData>;
using SerializerT   = TypeMapT::SerializerT;
using DeserializerT = TypeMapT::DeserializerT;

void to_string(Record& record, std::string& data);
void from_string(Record& record, const std::string& data);

void to_string(SerializerT& _rs, Record& record, std::string& data);
void from_string(DeserializerT& _rd, Record& record, const std::string& data);

const TypeMapT& type_map();

} // namespace solid_v2_test

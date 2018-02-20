
#pragma once

#include <sstream>
#include <string>
#include <vector>

#include <stdint.h>

#include "solid/serialization/serialization.hpp"

namespace solid_v2_test {

typedef std::vector<int64_t>     Integers;
typedef std::vector<std::string> Strings;

class Record {
public:
    Integers ids;
    Strings  strings;

    bool operator==(const Record& other)
    {
        return (ids == other.ids && strings == other.strings);
    }

    bool operator!=(const Record& other)
    {
        return !(*this == other);
    }

    template <class S>
    void solidSerializeV2(S& _s, const char *_name)
    {
        _s.add(ids, "Record::ids");
        _s.add(strings, "Record::strings");
    }
};

using SerializerT   = solid::serialization::v2::binary::Serializer<void>;
using DeserializerT = solid::serialization::v2::binary::Deserializer<void>;
//using TypeIdMapT    = solid::serialization::TypeIdMap<SerializerT, DeserializerT>;

void to_string(Record& record, std::string& data);
void from_string(Record& record, const std::string& data);

void to_string(SerializerT& _rs, Record& record, std::string& data);
void from_string(DeserializerT& _rd, Record& record, const std::string& data);

} // namespace solid_test

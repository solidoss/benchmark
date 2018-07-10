//courtesy to: https://github.com/thekvs/cpp-serializers
#include "record.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/archives/portable_binary.hpp"

namespace cereal_test {

void to_string(const Record& record, std::string& data, const bool _portable)
{
    std::ostringstream stream;
    if(_portable){
        cereal::PortableBinaryOutputArchive archiver(stream);
        archiver << record;
    }else{
        cereal::BinaryOutputArchive archiver(stream);
        archiver << record;
    }

    data = stream.str();
}

void from_string(Record& record, const std::string& data, const bool _portable)
{
    std::stringstream stream(data);
    
    if(_portable){
        cereal::PortableBinaryInputArchive archiver(stream);
        archiver >> record;
    }else{
        cereal::BinaryInputArchive archiver(stream);
        archiver >> record;
    }
}

} // namespace cereal_test

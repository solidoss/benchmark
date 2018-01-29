
#pragma once

#include "solid/frame/mpipc/mpipccontext.hpp"
#include "solid/frame/mpipc/mpipcmessage.hpp"
#include "solid/frame/mpipc/mpipcprotocol_serialization_v1.hpp"

#include <vector>
#include <string>

namespace bench {

struct Message : solid::frame::mpipc::Message {
    using StringVectorT = std::vector<std::string>;
    std::string     str;
    StringVectorT   vec;

    Message() {}

    Message(std::string&& _ustr)
        : str(std::move(_ustr))
    {
    }
    
    Message(const std::string& _str)
        : str(_str)
    {
    }

    template <class S>
    void solidSerialize(S& _s, solid::frame::mpipc::ConnectionContext& _rctx)
    {
        _s.push(str, "str");
        _s.pushContainer(vec, "vec");
    }
};

using ProtoSpecT = solid::frame::mpipc::serialization_v1::ProtoSpec<0, Message>;

} //namespace ipc_echo



#pragma once

#include "solid/frame/mprpc/mprpccontext.hpp"
#include "solid/frame/mprpc/mprpcmessage.hpp"
#include "solid/frame/mprpc/mprpcprotocol_serialization_v2.hpp"

#include <vector>
#include <string>

namespace bench {

struct Message : solid::frame::mprpc::Message {
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

    SOLID_PROTOCOL_V2(_s, _rthis, _rctx, _name)
    {
        _s.add(_rthis.str, _rctx, "str");
        _s.add(_rthis.vec, _rctx,  "vec");
    }
};

using ProtocolT = solid::frame::mprpc::serialization_v2::Protocol<uint8_t>;

template <class R>
inline void protocol_setup(R _r, ProtocolT& _rproto)
{
    _rproto.null(static_cast<ProtocolT::TypeIdT>(0));

    _r(_rproto, solid::TypeToType<Message>(), 1);
}


} //namespace ipc_echo


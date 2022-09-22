
#pragma once

#include "solid/frame/mprpc/mprpccontext.hpp"
#include "solid/frame/mprpc/mprpcmessage.hpp"
#include "solid/reflection/v1/reflection.hpp"
#include "solid/frame/mprpc/mprpcprotocol_serialization_v3.hpp"

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

    SOLID_REFLECT_V1(_rr, _rthis, _rctx)
    {
        _rr.add(_rthis.str, _rctx, 1, "str");
        _rr.add(_rthis.vec, _rctx, 2, "vec");
    }
};

template <class Reg>
inline void configure_protocol(Reg _rreg)
{
    _rreg(1, "Message", solid::TypeToType<Message>());
}

} //namespace ipc_echo


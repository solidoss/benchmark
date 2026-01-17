
#pragma once

#include "solid/frame/mprpc/mprpccontext.hpp"
#include "solid/frame/mprpc/mprpcmessage.hpp"
#include "solid/frame/mprpc/mprpcprotocol_serialization_v3.hpp"
#include "solid/reflection/v1/reflection.hpp"
#include "solid/utility/poolable.hpp"

#include <string>
#include <vector>

namespace bench {

struct Message final : solid::frame::mprpc::Message, solid::Poolable<Message> {
    using StringVectorT = std::vector<std::string>;
    std::string   str;
    StringVectorT vec;
    uint64_t      microseconds_since_epoch = 0;

    Message()
    {
        message_created();
    }

    Message(std::string&& _ustr, uint64_t _microseconds_since_epoch)
        : str(std::move(_ustr))
        , microseconds_since_epoch(_microseconds_since_epoch)
    {
        message_created();
    }

    Message(const std::string& _str, uint64_t _microseconds_since_epoch)
        : str(_str)
        , microseconds_since_epoch(_microseconds_since_epoch)
    {
        message_created();
    }

    void init()
    {
        str.clear();
        vec.clear();
    }

    SOLID_REFLECT_V1(_rr, _rthis, _rctx)
    {
        _rr.add(_rthis.str, _rctx, 1, "str");
        _rr.add(_rthis.vec, _rctx, 2, "vec");
        _rr.add(_rthis.microseconds_since_epoch, _rctx, 3, "usecs");
    }
};

template <class Reg>
inline void configure_protocol(Reg _rreg)
{
    _rreg(1, "Message", std::type_identity<Message>());
}

} // namespace bench

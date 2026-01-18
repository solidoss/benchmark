
#pragma once

#include "solid/frame/mprpc/mprpccontext.hpp"
#include "solid/frame/mprpc/mprpcmessage.hpp"
#include "solid/frame/mprpc/mprpcprotocol_serialization_v3.hpp"
#include "solid/reflection/v1/reflection.hpp"
#include "solid/utility/poolable.hpp"

#include <string>
#include <vector>

namespace bench {

struct Request final : solid::frame::mprpc::Message, solid::Poolable<Request> {
    std::string str;
    uint64_t    microseconds_since_epoch = 0;

    Request()
    {
        request_created();
    }

    Request(std::string&& _ustr, uint64_t _microseconds_since_epoch)
        : str(std::move(_ustr))
        , microseconds_since_epoch(_microseconds_since_epoch)
    {
        request_created();
    }

    Request(const std::string& _str, uint64_t _microseconds_since_epoch)
        : str(_str)
        , microseconds_since_epoch(_microseconds_since_epoch)
    {
        request_created();
    }

    void init(std::string&& _ustr, uint64_t _microseconds_since_epoch)
    {
        str                      = std::move(_ustr);
        microseconds_since_epoch = _microseconds_since_epoch;
    }

    void init(std::string const& _str, uint64_t _microseconds_since_epoch)
    {
        str                      = _str;
        microseconds_since_epoch = _microseconds_since_epoch;
    }

    void init()
    {
        str.clear();
    }

    SOLID_REFLECT_V1(_rr, _rthis, _rctx)
    {
        _rr.add(_rthis.str, _rctx, 1, "str");
        _rr.add(_rthis.microseconds_since_epoch, _rctx, 3, "usecs");
    }
};

struct Response final : solid::frame::mprpc::Message, solid::Poolable<Response> {
    using StringVectorT = std::vector<std::string>;
    StringVectorT vec;
    uint64_t      microseconds_since_epoch = 0;

    Response()
    {
        response_created();
    }

    explicit Response(
        Request const& _req)
        : solid::frame::mprpc::Message(_req)
        , microseconds_since_epoch(_req.microseconds_since_epoch)
    {
        response_created();
    }

    void init(Request const& _req)
    {
        *static_cast<solid::frame::mprpc::Message*>(this) = _req;
        vec.clear();
    }

    void init()
    {
        vec.clear();
    }

    SOLID_REFLECT_V1(_rr, _rthis, _rctx)
    {
        _rr.add(_rthis.vec, _rctx, 2, "vec");
        _rr.add(_rthis.microseconds_since_epoch, _rctx, 3, "usecs");
    }
};

template <class Reg>
inline void configure_protocol(Reg _rreg)
{
    _rreg(1, "Request", std::type_identity<Request>());
    _rreg(2, "Response", std::type_identity<Response>());
}

} // namespace bench

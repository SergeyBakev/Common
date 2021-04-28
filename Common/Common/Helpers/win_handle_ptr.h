#pragma once
#include <Windows.h>
#include <memory>

using HandlePtr = std::shared_ptr<void>;

template <class Handle, class Deleter >
auto MakeHandlePtr(Handle handle, Deleter del);

template<class Handle, class Deleter>
inline auto MakeHandlePtr(Handle handle, Deleter del)
{
    HandlePtr handle_(handle, del);
    return handle_;
}

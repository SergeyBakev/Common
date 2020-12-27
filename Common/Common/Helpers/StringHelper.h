#pragma once
#include <string_view>
#include <sstream>
struct StringHelper
{
    template<typename... Args>
    static std::wstring Concatenate(std::wstring_view separator, Args&& ...args)
    {
        std::wostringstream oss;
        return Concatenate(separator, oss, args...);
    }

    template<typename... Args>
    static std::string Concatenate(std::string_view separator, Args&& ...args)
    {
        std::ostringstream oss;
        return Concatenate(separator, oss, std::forward<Args>(args)...);
    }
private:

    template<typename T, typename... Args>
    static std::wstring Concatenate(std::wstring_view separator, std::wostringstream& oss, T&& value, Args&& ...args)
    {
        oss << value << separator;
        return Concatenate(separator, oss, args...);
    }

    template<typename T>
    static std::wstring Concatenate(std::wstring_view separator, std::wostringstream& oss, T&& value)
    {
        oss << value;
        return oss.str();
    }

    template<typename T, typename... Args>
    static std::string Concatenate(std::string_view separator, std::ostringstream& oss, T&& value, Args&& ...args)
    {
        oss << value << separator;
        return Concatenate(separator, oss, args...);
    }

    template<typename T>
    static std::string Concatenate(std::string_view separator, std::ostringstream& oss, T&& value)
    {
        oss << value;
        return oss.str();
    }
};

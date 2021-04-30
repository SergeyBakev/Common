#pragma once
#include <string>
#include <ctime>
#include <Windows.h>
std::wstring GetLastStringError();
std::wstring GetMessageString(unsigned long resMessageId, unsigned long  argc, const wchar_t* argv, void* hModule = nullptr);

namespace Kernel
{
	class Converter
	{
	public:
		static std::wstring ToWString(const std::string& str);
		static std::wstring ToWString(const char* str);
		static std::wstring ToWHex(const char* data, size_t count, bool upperCase = true);
		static std::wstring ToWHex(const std::string& data, bool upperCase = true);
		static std::string WHex(const std::wstring& hex);
		static size_t WHex(const std::wstring& str, char* data);

		static std::string ToString(std::wstring_view str);
		static std::string ToString(const wchar_t* str);
		static std::string ToHex(const char* data, size_t count, bool upperCase = true);
	};

	struct DateTimeConverter
	{
		static const inline std::wstring mdyhm = L"%m/%d/%Y/%H:%M";	// format date "month/day/year/hours/minutes"

		static std::wstring ToString(std::time_t time, const std::wstring& format);
		static std::wstring ToString(std::tm time, const std::wstring& format);
		static std::wstring Now(const std::wstring& format);

		static std::tm ToTime(const std::wstring& time, const std::wstring& format);
		static std::tm ToTime_tm(std::time_t time, const std::wstring& format);
		static std::tm Now_tm();
		static std::tm Now_tm(const std::wstring& format);

		static std::time_t ToTime_t(const std::wstring& time, const std::wstring& format);
		static std::time_t ToTime_t(const std::tm& time, const std::wstring& format);
		static std::time_t ToTime_t(const std::tm& time);
		static std::time_t Now_t();
		static std::time_t Now_t(const std::wstring& format);

		static int Compare(std::time_t t1, std::time_t t2);
		static int Compare(std::tm t1, std::tm t2);
	};

	std::wstring KeGetModuleFileName(HMODULE module);
}



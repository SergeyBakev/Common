#include "pch.h"
#include "ke_kernel.h"
#include "..\Algorithm\ke_string.h"
#include <windows.h>
#include <stdio.h>
#include <sstream>
#include <tchar.h>
#include <iomanip>
#include <chrono>
#pragma comment(lib, "Version.lib")

static std::time_t __buildTime;

//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::wstring GetLastStringError()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) 
    {
        return std::wstring(); //No error message has been recorded
    }

    LPWSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);

    //Copy the error message into a std::string.
    std::wstring message(messageBuffer, size);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
}
//--------------------------------------------------------
std::wstring GetMessageString(unsigned long  resMessageId, unsigned long  argc, const wchar_t* argv, void* hModule)
{
	LPWSTR pMessage = NULL;

	DWORD dwFormatFlags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER;
	if (hModule)
		dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
	else
		dwFormatFlags |= FORMAT_MESSAGE_FROM_SYSTEM;

	const wchar_t* pString = argv;
	std::vector<DWORD_PTR> pArgs2;
	for (DWORD i = 0; i < argc; i++)
	{
		pArgs2.push_back((DWORD_PTR)pString);
		pString += wcslen(pString) + 1;
	}

	if (!pArgs2.empty())
		dwFormatFlags |= FORMAT_MESSAGE_ARGUMENT_ARRAY;

	size_t size = FormatMessage(dwFormatFlags,
		hModule,
		resMessageId,
		0,
		(LPWSTR)&pMessage,
		0,
		pArgs2.empty() ? nullptr : (va_list*)pArgs2.data());


	std::wstring ret(pMessage, size);
	ret.erase(std::remove(std::begin(ret), std::end(ret), L'\r'));
	ret.erase(std::remove(std::begin(ret), std::end(ret), L'\n'));

	if (pMessage != nullptr)
		LocalFree(pMessage);
	return ret;
}
//--------------------------------------------------------
//--------------------------------------------------------
std::wstring Kernel::Converter::ToWHex(const char* data, size_t count, bool upperCase/* = true*/)
{
	std::string str = ToHex(data, count, upperCase);
	std::wstring wstr = ToWString(str);
	return wstr;
}
//--------------------------------------------------------
std::wstring Kernel::Converter::ToWHex(const std::string& data, bool upperCase/* = true*/)
{
	return ToWHex(data.data(), data.length(), upperCase);
}
//--------------------------------------------------------
std::string Kernel::Converter::WHex(const std::wstring& hex)
{
	size_t size = WHex(hex.data(), nullptr);
	std::string ret;
	ret.resize(size);
	WHex(hex.data(), ret.data());
	return ret;
}
//--------------------------------------------------------
size_t Kernel::Converter::WHex(const std::wstring& str, char* data)
{
	// define size of encoded data of current HW item
	size_t sizeHW = str.length() / 2;	// note: 2 hexadecimal digits per 1 encoded symbol
	if (data == nullptr)
		return sizeHW;
	unsigned int c;
	for (size_t j = 0; j < sizeHW; j++)
	{
		_stscanf_s( str.c_str() + j * 2, _T("%02X"), &c);
		data[j] += (BYTE)c;
	}
	return sizeHW;
}
//--------------------------------------------------------
std::string Kernel::Converter::ToHex(const char* data, size_t count, bool upperCase /*= true*/)
{
	std::ostringstream retStr;
	for (size_t i = 0; i < count; i++)
	{
		char strTmp[10] = { 0 };
		if(upperCase)
			sprintf_s(strTmp, _countof(strTmp), "%02X", (unsigned char)data[i]);
		else
			sprintf_s(strTmp, _countof(strTmp), "%02x", (unsigned char)data[i]);
		retStr << strTmp;
	}
	return retStr.str();
}
//--------------------------------------------------------
// Convert an UTF8 string to a wide Unicode String
std::wstring Kernel::Converter::ToWString(const std::string& str)
{
	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	std::wstring wstr;
	if (wchars_num == 1)
		return wstr;
	wstr.reserve(wchars_num);
	wstr.resize(wchars_num - 1);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wstr.data(), wchars_num);
	return wstr;
}
//--------------------------------------------------------
std::wstring Kernel::Converter::ToWString(const char* str)
{
	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	std::wstring wstr;
	if (wchars_num == 1)
		return wstr;
	wstr.reserve(wchars_num);
	wstr.resize(wchars_num - 1);
	MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr.data(), wchars_num);
	return wstr;
}
//--------------------------------------------------------
std::string Kernel::Converter::ToString(std::wstring_view str)
{
	return ToString(str.data());
}
//--------------------------------------------------------
// Convert a wide Unicode string to an UTF8 string
std::string Kernel::Converter::ToString(const wchar_t* str)
{
	int wchars_num = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	std::string wstr;
	if (wchars_num == 1)
		return wstr;
	wstr.reserve(wchars_num);
	wstr.resize(wchars_num - 1);
	WideCharToMultiByte(CP_ACP, 0, str, -1, wstr.data(), wchars_num, NULL, NULL);
	return wstr;
}
//--------------------------------------------------------
std::wstring Kernel::DateTimeConverter::ToString(std::time_t time, const std::wstring& format)
{
	std::tm tm;
	gmtime_s(&tm, &time);
	return ToString(tm, format);
}
//--------------------------------------------------------
std::wstring Kernel::DateTimeConverter::ToString(std::tm time, const std::wstring& format)
{
	std::wostringstream ss;
	std::wstring f;
	ss << std::put_time(&time, format.c_str());
	return ss.str();
}
//--------------------------------------------------------
std::tm Kernel::DateTimeConverter::ToTime(const std::wstring& time, const std::wstring& format)
{
	std::wistringstream ss(time);
	std::tm tm = { 0 };
	ss >> std::get_time(&tm, format.c_str());
	return tm;
}
//--------------------------------------------------------
std::time_t Kernel::DateTimeConverter::ToTime_t(const std::wstring& time, const std::wstring& format)
{
	std::tm tm = ToTime(time, format);
	return _mkgmtime(&tm);
}
//--------------------------------------------------------
std::time_t Kernel::DateTimeConverter::ToTime_t(const std::tm& time, const std::wstring& format)
{
	return ToTime_t((ToString(time, format)), format);
}
//--------------------------------------------------------
std::time_t Kernel::DateTimeConverter::ToTime_t(const std::tm& time)
{
	std::tm tm = time;
	return _mkgmtime(&tm);
}
//--------------------------------------------------------
std::tm Kernel::DateTimeConverter::ToTime_tm(std::time_t time, const std::wstring& format)
{
	std::wstring str = ToString(time, format);
	std::tm tm = { 0 };
	std::wistringstream ss(str);
	ss >> std::get_time(&tm, format.c_str());
	return tm;
}
//--------------------------------------------------------
std::time_t Kernel::DateTimeConverter::Now_t()
{
	return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}
//--------------------------------------------------------
std::time_t Kernel::DateTimeConverter::Now_t(const std::wstring& format)
{
	auto tm = Now_tm(format);
	return _mkgmtime(&tm);
}
//--------------------------------------------------------
std::tm Kernel::DateTimeConverter::Now_tm()
{
	auto time = Now_t();
	std::tm tm;
	gmtime_s(&tm, &time);
	return tm;
}
std::tm Kernel::DateTimeConverter::Now_tm(const std::wstring& format)
{
	std::tm tm = ToTime(ToString(Now_tm(),format), format);
	return tm;
}
//--------------------------------------------------------
std::wstring Kernel::DateTimeConverter::Now(const std::wstring& format)
{
	time_t time = Now_t();
	return  ToString(time, format);
}
//--------------------------------------------------------
int Kernel::DateTimeConverter::Compare(std::time_t t1, std::time_t t2)
{
	double diff = std::difftime(t1, t2);
	return diff < 0.0 ? -1 : diff > 0.0 ? 1 : 0;
}
//--------------------------------------------------------
int Kernel::DateTimeConverter::Compare(std::tm t1, std::tm t2)
{
	std::time_t t11 = _mkgmtime(&t1);
	std::time_t t22 = _mkgmtime(&t2);
	return Compare(t11, t22);
}
//--------------------------------------------------------


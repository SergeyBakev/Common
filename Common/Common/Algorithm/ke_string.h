#pragma once

#include <string>
#include <string_view>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <iostream>

// trim from start (in place)
static inline void ltrim_local(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim_local(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim_local(std::string& s) {
	ltrim_local(s);
	rtrim_local(s);
}

// trim from start (copying)
static inline std::string ltrim(std::string s) {
	ltrim_local(s);
	return s;
}

// trim from end (copying)
static inline std::string rtrim(std::string s) {
	rtrim_local(s);
	return s;
}

// trim from both ends (copying)
static inline std::string trim(std::string s) {
	trim_local(s);
	return s;
}
//-----------------------------------------------------------------------
// trim from start (in place)
static inline void lwtrim_local(std::wstring& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rwtrim_local(std::wstring& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static inline void wtrim_local(std::wstring& s) {
	lwtrim_local(s);
	rwtrim_local(s);
}

// trim from start (copying)
static inline std::wstring lwtrim(std::wstring s) {
	lwtrim_local(s);
	return s;
}

// trim from end (copying)
static inline std::wstring rwtrim(std::wstring s) {
	rwtrim_local(s);
	return s;
}

// trim from both ends (copying)
static inline std::wstring wtrim(std::wstring s) {
	wtrim_local(s);
	return s;
}

static inline void remove_local(std::wstring& s) 
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

//-----------------------------------------------------------------------
//std::ifstream& getstr(std::ifstream& in, std::string& str)
//{
//	getline(in, str);
//	trim_local(str);
//	//RemoveDelimeterInString(str);
//	//if (IsTag(str))
//	//	FixTagName(str);
//	return in;
//}

//------------------------------------------------------------------------


static inline bool beginsWith(std::string const& inputString, std::string const& prefix)
{
	if (inputString.length() >= prefix.length())
	{
		return (0 == inputString.compare(0, prefix.length(), prefix));
	}
	return false;
}

static inline bool endsWith(std::string const& inputString, std::string const& ending)
{
	if (inputString.length() >= ending.length())
	{
		const size_t startIndex = inputString.length() - ending.length();
		const size_t numCharactesToCompare = ending.length();
		return (0 == inputString.compare(startIndex, numCharactesToCompare, ending));
	}
	return false;
}

static inline bool has_walpha_or_empty(const std::wstring& s)
{
	if (s.empty())
		return true;

	bool res = false;

	for (auto& c : s)
	{
		if (iswalpha(c) || c == L' ')
		{
			res = true;
			break;
		}
	}
	return res;
}
//Generic split
static inline std::vector<std::wstring> Split(std::wstring_view delim, std::wstring_view str)
{
	std::vector<std::wstring> ret;
	size_t pos = 0;
	std::wstring local = str.data();
	while ((pos = local.find(delim)) != std::wstring::npos)
	{
		std::wstring token = local.substr(0, pos);
		ret.push_back(token);
		local.erase(0, pos + delim.length());
	}
	if (local != L"")
		ret.push_back(local);
	return ret;
}

//Remove empty entires, if string has alphabet symbol return empty vector
static inline std::vector<std::wstring> SplitByNumbers(std::wstring_view delim, std::wstring_view str)
{
	std::vector<std::wstring> ret;
	size_t pos = 0;
	std::wstring local(str);
	while ((pos = local.find(delim)) != std::wstring::npos)
	{
		std::wstring token = local.substr(0, pos);
		if (has_walpha_or_empty(token))
			return {};
		ret.push_back(token);
		local.erase(0, pos + delim.length());
	}
	if (local != L"") 
		ret.push_back(local);
	return ret;
}

static inline bool CaseInsCharCompareA(char a, char b) 
{
	return(std::toupper(a) == std::toupper(b));
}

static inline bool CaseInsCharCompareW(wchar_t a, wchar_t b) 
{
	return(towupper(a) == towupper(b));
}

static inline bool string_equal_nocase(const std::string& s1, const std::string& s2)
{
	return((s1.size() == s2.size()) &&
		equal(s1.begin(), s1.end(), s2.begin(), CaseInsCharCompareA));
}

static inline bool wstring_equal_nocase(const std::wstring& s1, const std::wstring& s2) 
{
	return((s1.size() == s2.size()) &&
		equal(s1.begin(), s1.end(), s2.begin(), CaseInsCharCompareW));
}

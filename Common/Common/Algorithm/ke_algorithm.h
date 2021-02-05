#pragma once
#include <vector>
#include <string>

namespace Common
{
	namespace algorithms
	{
		namespace string
		{
			enum class StringSplitOptinos{eTrimed/*not implemented*/, eRemoveEmpty, eNone};

			std::vector<std::wstring> Split(std::wstring_view delim, std::wstring_view str, StringSplitOptinos options = StringSplitOptinos::eNone);

			// trim from start (in place)
			inline void ltrim_local(std::string& s); 
			inline void ltrim_local(std::wstring& s); 

			// trim from end (in place)
			inline void rtrim_local(std::string& s);
			inline void rtrim_local(std::wstring& s);

			// trim from both ends (in place)
			inline void trim_local(std::string& s);
			inline void trim_local(std::wstring& s);

			// trim from start (copying)
			inline std::string ltrim(std::string s);
			inline std::wstring ltrim(std::wstring s);

			// trim from end (copying)
			inline std::string rtrim(std::string s);
			inline std::wstring rtrim(std::wstring s);

			// trim from both ends (copying)
			inline std::string trim(std::string s);
			inline std::wstring trim(std::wstring s);

			template <class Converter>
			auto Split(std::wstring_view delim, std::wstring_view str, Converter converter, StringSplitOptinos options = StringSplitOptinos::eNone)
			{
				std::vector<decltype(converter(L""))> ret;


				return ret;
			}
		
		}
	}
	
}


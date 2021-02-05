#include "pch.h"
#include <cwctype>

#include "ke_algorithm.h"

namespace Common
{
	namespace algorithms
	{
		namespace string
		{
			std::vector<std::wstring> Split(std::wstring_view delim, std::wstring_view str, StringSplitOptinos options)
			{
				std::vector<std::wstring> ret;
				size_t pos = 0;
				std::wstring local = str.data();
				while ((pos = local.find(delim)) != std::wstring::npos)
				{
					std::wstring token = local.substr(0, pos);
					if (token.empty() && options == StringSplitOptinos::eRemoveEmpty)
						continue;

					ret.push_back(token);
					local.erase(0, pos + delim.length());
				}
				if (local != L"") ret.push_back(local);
				return ret;
			}
			void ltrim_local(std::string& s)
			{
				s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
					return !std::isspace(ch);
					}));
			}

			void ltrim_local(std::wstring& s)
			{
				s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](wint_t ch) {
					return !std::iswspace(ch);
					}));
			}

			void rtrim_local(std::string& s)
			{
				s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
					return !std::isspace(ch);
					}).base(), s.end());
			}

			void rtrim_local(std::wstring& s)
			{
				s.erase(std::find_if(s.rbegin(), s.rend(), [](wint_t ch) {
					return !std::iswspace(ch);
					}).base(), s.end());
			}

			void trim_local(std::string& s)
			{
				ltrim_local(s);
				rtrim_local(s);
			}

			void trim_local(std::wstring& s)
			{
				ltrim_local(s);
				rtrim_local(s);
			}

			std::string ltrim(std::string s)
			{
				ltrim_local(s);
				return s;
			}

			std::wstring ltrim(std::wstring s)
			{
				ltrim_local(s);
				return s;
			}
			std::string rtrim(std::string s)
			{
				rtrim_local(s);
				return s;
			}

			std::wstring rtrim(std::wstring s)
			{
				rtrim_local(s);
				return s;
			}

			std::string trim(std::string s)
			{
				trim_local(s);
				return s;
			}

			std::wstring trim(std::wstring s)
			{
				trim_local(s);
				return s;
			}
		}
	}
}
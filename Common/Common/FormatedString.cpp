#include "pch.h"
#include "FormatedString.h"
#include "Algorithm/ke_string.h"
#include <string>
using namespace  std::string_literals;

namespace Common
{
	std::wstring FormattedString::Str() const
	{
		return _string;
	}

	void FormattedString::Str(std::wstring_view str)
	{
		_string = str;
	}

	FormattedString::FormattedString(std::wstring_view str) noexcept
	{
		_string = str;
	}

	std::wstring FormattedString::GetParameter(std::wstring_view name) const
	{
		std::wstring param;
		std::wstring findName = name.data();
		size_t pos0 = 0;
		size_t pos1 = _string.find(L'=', pos0);
		while (pos1 != std::wstring::npos)
		{
			std::wstring key = wtrim(_string.substr(pos0, pos1 - pos0));
			pos0 = pos1 + 1;
			pos1 = _string.find(L';', pos0);
			if (wstring_equal_nocase(key, findName))
			{
				param = _string.substr(pos0, pos1 - pos0);
				wtrim_local(param);
				break;
			}

			if (pos1 == std::wstring::npos)
				break;
			pos0 = pos1 + 1;
			pos1 = _string.find(L'=', pos0);
		}
		return param;
	}

	void FormattedString::SetParameter(std::wstring tag, std::wstring value)
	{
		wtrim_local(tag);
		wtrim_local(value);
		_string += tag;
		_string += L" = ";
		_string += value;
		_string += delimeter;
	}

	FormattedStringV2::FormattedStringV2(std::wstring_view str) noexcept
		: FormattedString(str)
	{
		
	}





}

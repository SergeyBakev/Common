#pragma once

#include <system_error>
#include <sstream>
#include <type_traits>
#include <optional>

namespace Common
{
	
	class FormattedString
	{
	public:
		inline static std::wstring delimeter = L";";
		FormattedString() = default;

		FormattedString(std::wstring_view str) noexcept;
		virtual ~FormattedString() = default;

		std::wstring GetParameter(std::wstring_view name) const;
		//note: all parameters will be trimmed
		void SetParameter(std::wstring tag, std::wstring value);


		std::wstring Str() const;
		void Str(std::wstring_view str);

	private:
		std::wstring _string;
	};


	//String format: tag = value;
	class FormattedStringV2 final : public FormattedString
	{
	public:
		template <class T>
		struct DefaultConverter
		{
			constexpr DefaultConverter() {}
			T operator()(const std::wstring& param)
			{
				static_assert(std::is_arithmetic<T>::value);
				std::wstringstream ss(param);
				T val{};
				ss >> val;
			/*	if (ss.fail())
					throw std::invalid_argument("DefaultConverter::convert fail");*/

				return val;
			}

			std::wstring operator()(T value)
			{
				std::wstringstream ss;
				ss << value;
			/*	if (ss.fail())
					throw std::invalid_argument("DefaultConverter::convert fail");*/
				return ss.str();
			}
		};

		FormattedStringV2() = default;

		FormattedStringV2(std::wstring_view str) noexcept;
		virtual ~FormattedStringV2() = default;

		using FormattedString::GetParameter;
		using FormattedString::SetParameter;
		using FormattedString::Str;

		template <class T>
		void SetParameter(std::wstring tag, T value)
		{
			SetParameter(std::move(tag), value, DefaultConverter<T>{});
		}

		template <class T>
		T GetParameter(std::wstring_view tag) const
		{
			return GetParameter(tag, DefaultConverter<T>{});
		}

		template <class T>
		std::optional<T> TryGetParameter(std::wstring_view tag)
		{
			return TryGetParameter(tag, DefaultConverter<T>{});
		}
		
		template <class Converter>
		decltype(auto) GetParameter(std::wstring_view tag, Converter cnv) const
		{
			using type	= decltype(cnv(L""));
			static_assert(std::is_arithmetic_v<type> || std::is_convertible_v<type, std::wstring>, "Converter should convert only primitive type or string" );
			std::wstring value = GetParameter(tag);
			if constexpr(std::is_convertible_v<type, std::wstring>)
			{
				return value;
			}
			else
			{
				return cnv(value);
			}
			
		}

		template <class Converter>
		auto TryGetParameter(std::wstring_view tag, Converter cnv) const noexcept
		{
			using type = decltype(cnv(L""));
			static_assert(std::is_arithmetic_v<type> || std::is_convertible_v<type, std::wstring>, "Converter should convert only primitive type or string");
			std::wstring value = GetParameter(tag);
			std::optional<type> opt = std::nullopt;
			try{	opt = cnv(value);	}
			catch (...){}

			return opt;
		}

		template <class T ,class Converter>
		void SetParameter(std::wstring tag, T value, Converter cnv)
		{
			static_assert(std::is_arithmetic_v<T> || std::is_convertible_v<T, std::wstring>, "Converter should convert only primitive type or string");
			if constexpr (std::is_convertible_v<T, std::wstring>)
			{
				SetParameter(tag,value);
			}
			else
			{
				SetParameter(tag, cnv(value));
			}

		}
	};
}


#include "pch.h"
#include "ke_kernel.h"
#include "WinEventRecord.h"
#include "..\FormatedString.h"
WinEventRecord::WinEventRecord(unsigned short cat, unsigned short type, std::wstring_view provider, std::wstring_view message, std::time_t date, int eventID) :
	category_(cat), type_(type), provider_(provider), message_(message), date_(date), eventID_(eventID)
{

}
//------------------------------------------------------------------------------------
int WinEventRecord::GetID() const
{
	return eventID_;
}
//------------------------------------------------------------------------------------
std::wstring WinEventRecord::GetProvider() const
{
	return provider_;
}
//------------------------------------------------------------------------------------
std::wstring WinEventRecord::GetMassage() const
{
	return message_;
}
//------------------------------------------------------------------------------------
std::time_t WinEventRecord::GetTime() const
{
	return date_;
}
//------------------------------------------------------------------------------------
unsigned short WinEventRecord::GetType() const
{
	return type_;
}
//------------------------------------------------------------------------------------
unsigned short WinEventRecord::GetCategory() const
{
	return category_;
}
//------------------------------------------------------------------------------------
void WinEventRecord::Serialize(std::wostream& ar) const
{
	Common::FormattedString fmt;
	fmt.SetParameter(L"Provider", provider_);
	fmt.SetParameter(L"Id", std::to_wstring(eventID_));
	fmt.SetParameter(L"Category", std::to_wstring(category_));
	fmt.SetParameter(L"Type", std::to_wstring(type_));
	fmt.SetParameter(L"Date", std::to_wstring(date_));
	fmt.SetParameter(L"Message", message_);
	ar << fmt.Str() << std::endl;
}
//------------------------------------------------------------------------------------
bool WinEventRecord::Deserialize(std::wistream& ar)
{
	std::wstring str;
	if (!std::getline(ar, str))
		return false;

	std::wstring_view vStr = str;
	size_t i = 0;
	Common::FormattedString fmt(vStr);
	provider_ = fmt.GetParameter(L"Provider");
	message_ = fmt.GetParameter(L"Message");
	eventID_ = std::stoi(fmt.GetParameter(L"Id"));
	category_ = std::stoi(fmt.GetParameter(L"Category"));
	type_ = std::stoi(fmt.GetParameter(L"Type"));
	date_ = std::stoll(fmt.GetParameter(L"Date"));
	return true;
}
//------------------------------------------------------------------------------------

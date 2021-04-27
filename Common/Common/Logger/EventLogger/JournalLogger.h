#pragma once
#include "IJournalLogger.h"
#include "ILogReader.h"
#include <sstream>

class LogJournalProvider final
{
public:
	LogJournalProvider() = default;
	void Initialize(std::wstring_view providerName);
	void Deinitialize();

	template <class ...Args>
	void Write(unsigned short type,
		unsigned short category,
		unsigned long eventId,
		Args&&... args) const 
	{
		StringVector params;
		auto to_str = [&](auto&& param)
		{
			std::wostringstream ss;
			ss << param;
			return ss.str();
		};

		(params.push_back(to_str(std::forward<Args>(args))), ...);

		Instance()->Write(type, category, eventId, params);
	}
	std::wstring Provider() const;
	ILogReaderPtr MakeReader() const;
	IJournalLoggerPtr Instance() const;
private:
	mutable IJournalLoggerPtr instance_;
	std::wstring provider_;
};

//Note: Current implementation use Windows Journal.
//		Method uses for register journal provider in windows registry
//		This method work only if users have administrative previlegies 

bool RegisterWinJournalProvider(std::wstring_view entrie_, std::wstring_view dllPath, size_t categoryCount, size_t countSupportedTypes, bool bUpdateCount);




#pragma once
#include "..\Common\Logger\EventLogger\IJournalLogger.h"
class WinJournalLoggerImplV2 : public IJournalLogger
{
public:
	// Inherited via IJournalLogger
	virtual void Write(unsigned short type, unsigned short category, unsigned long eventId, StringVector& params) const override;
	virtual ILogReaderPtr MakeReader() const override;
	
	static void Register(const std::wstring& dllPath, std::wstring providerName);
};


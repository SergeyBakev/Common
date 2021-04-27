#pragma once
#include <vector>
#include <string_view>
#include <memory>
#include "ILogReader.h"

using StringVector = std::vector<std::wstring>;
class IJournalLogger
{
public:
	virtual ~IJournalLogger() = default;
	virtual void Write(
		unsigned short type,
		unsigned short category,
		unsigned long eventId,
		StringVector& params
	) const = 0;

	virtual ILogReaderPtr MakeReader()  const = 0;
};

using IJournalLoggerPtr = std::shared_ptr<IJournalLogger>;
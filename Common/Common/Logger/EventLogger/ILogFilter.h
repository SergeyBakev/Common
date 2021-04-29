#pragma once
#include "ILogRecord.h"
class ILogFilter
{
public:
	virtual ~ILogFilter() = default;
	virtual std::wstring GetName() const = 0;
	virtual std::wstring Str() const = 0;
	virtual bool IsAvailable(ILogRecordPtr record) const = 0;
};
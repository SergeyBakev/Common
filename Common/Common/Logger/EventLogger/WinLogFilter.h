#pragma once
#include "ILogFilter.h"
#include <numeric>

class WinLogFilter : public ILogFilter
{
public:
	virtual bool IsAvailable(ILogRecordPtr record) const override;
    virtual std::wstring GetName() const override;
    virtual std::wstring Str() const override;
public:
    std::wstring name;
    unsigned short eventCategory = (std::numeric_limits<unsigned short>::max)();
    unsigned short eventType = (std::numeric_limits<unsigned short>::max)();
    std::time_t startTime = (std::numeric_limits<std::time_t>::max)();
    std::time_t endTime = (std::numeric_limits<std::time_t>::max)();
};


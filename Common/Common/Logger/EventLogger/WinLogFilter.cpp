#include "pch.h"
#include "ke_kernel.h"
#include "WinLogFilter.h"


bool WinLogFilter::IsAvailable(ILogRecordPtr record) const
{
    if (name != L"*" && record->GetProvider() != name)
        return false;

    if (eventCategory != (std::numeric_limits<unsigned short>::max)() && (!(record->GetCategory() & eventCategory)))
        return false;

    if (eventType != (std::numeric_limits<unsigned short>::max)() && (!(record->GetType() & eventType)))
        return false;

    if (startTime != (std::numeric_limits<std::time_t>::max)() && Kernel::DateTimeConverter::Compare(startTime, record->GetTime()) == 1)
        return false;

    if (endTime != (std::numeric_limits<std::time_t>::max)() && Kernel::DateTimeConverter::Compare(endTime, record->GetTime()) == -1)
        return false;

    return true;
}

std::wstring WinLogFilter::GetName() const
{
    return name;
}

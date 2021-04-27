#include "pch.h"
#include "ke_kernel.h"
#include "WinLogReader.h"
#include "win_handle_ptr.h"
#include <algorithm>

#pragma comment(lib,"Advapi32.lib")
WinLogReader::WinLogReader(std::wstring_view providerName, HMODULE msgModule) : provider_(providerName), resDll_(msgModule)
{

}
//------------------------------------------------------------------------------------
const LogRecordsArray& WinLogReader::GetRecords() const
{
    return records_;
}
//------------------------------------------------------------------------------------
size_t WinLogReader::CountRecords() const
{
    return records_.size();
}
//------------------------------------------------------------------------------------
ILogReaderPtr WinLogReader::Select(const ILogFilter& filter)
{
    auto this_ = shared_from_this();

    records_.clear();
    if (provider_.empty())
        return this_;

    DWORD dwBytesToRead = 0x10000;
    DWORD dwBytesRead, dwMinimumBytesNeeded;

    ByteBuffer pBuffer(dwBytesToRead, 0);
    if (!TryRead(pBuffer, &dwBytesRead, &dwMinimumBytesNeeded))
        return this_;

    DumpRecordsInBuffer(pBuffer.data(), dwBytesRead, filter);
    isFiltred_ = true;

    return this_;
}
//------------------------------------------------------------------------------------
void WinLogReader::ToStream(std::wostream& ar)
{
    if (!isFiltred_)
        GetRecords();

    std::for_each(std::begin(records_), std::end(records_), [&ar](const ILogRecordPtr record)
        {
            record->Serialize(ar);
        });
}
//------------------------------------------------------------------------------------
DWORD WinLogReader::DumpRecordsInBuffer(PBYTE pBuffer, DWORD dwBytesRead, const ILogFilter& filter)
{
    DWORD status = ERROR_SUCCESS;
    PBYTE pRecord = pBuffer;
    PBYTE pEndOfRecords = pBuffer + dwBytesRead;
    LPWSTR pMessage = NULL;
    LPWSTR pFinalMessage = NULL;
    while (pRecord < pEndOfRecords)
    {
        PEVENTLOGRECORD record = (PEVENTLOGRECORD)pRecord;
        const wchar_t* name = (LPWSTR)(pRecord + sizeof(EVENTLOGRECORD));
        if (name == filter.GetName())
        {
            auto logRecord = ToRecord(record, name);
            if (filter.IsAvailable(logRecord))
            {
                records_.push_back(std::move(logRecord));
            }
        }
        pRecord += record->Length;
    }

    return status;
}
//------------------------------------------------------------------------------------
bool WinLogReader::TryRead(ByteBuffer& pBuffer, DWORD* dwBytesRead, DWORD* dwMinimumBytesNeeded)
{
    HandlePtr eventLog_ = MakeHandlePtr(OpenEventLog(NULL, provider_.data()), [](void* p) {CloseEventLog(p); });
    if (eventLog_ == nullptr)
        return false;

    ReadEventLog(eventLog_.get(), EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ, 0, pBuffer.data(), pBuffer.size(), dwBytesRead, dwMinimumBytesNeeded);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        pBuffer.resize(*dwMinimumBytesNeeded);
        ReadEventLog(eventLog_.get(), EVENTLOG_SEQUENTIAL_READ | EVENTLOG_FORWARDS_READ, 0, pBuffer.data(), pBuffer.size(), dwBytesRead, dwMinimumBytesNeeded);
    }
    auto err = GetLastError();
    return GetLastError() == 0;
}
//------------------------------------------------------------------------------------
ILogRecordPtr WinLogReader::ToRecord(PEVENTLOGRECORD pRecord, std::wstring name)
{
    std::wstring provider = name;
    unsigned short type, category, eventID;
    std::wstring message;
    std::time_t date;

    category = pRecord->EventCategory;
    type =  pRecord->EventType;
    eventID = pRecord->EventID & 0xFFFF;

    std::wstring pMessage = GetMessageString(pRecord->EventID, pRecord->NumStrings, (LPWSTR)((PBYTE)pRecord + pRecord->StringOffset),resDll_);
    if (!pMessage.empty())
        message = std::move(pMessage);

    {
        DWORD Time = pRecord->TimeGenerated;
        ULONGLONG ullTimeStamp = 0;
        ULONGLONG SecsTo1970 = 116444736000000000;
        SYSTEMTIME st;
        FILETIME ft, ftLocal;

        ullTimeStamp = Int32x32To64(Time, 10000000) + SecsTo1970;
        ft.dwHighDateTime = (DWORD)((ullTimeStamp >> 32) & 0xFFFFFFFF);
        ft.dwLowDateTime = (DWORD)(ullTimeStamp & 0xFFFFFFFF);

        FileTimeToLocalFileTime(&ft, &ftLocal);
        FileTimeToSystemTime(&ftLocal, &st);

        std::tm time = { 0 };
        time.tm_year = st.wYear - 1900;
        time.tm_mon = st.wMonth - 1;
        time.tm_mday = st.wDay;
        time.tm_hour = st.wHour;
        time.tm_min = st.wMinute;
        time.tm_isdst = -1;
        date = Kernel::DateTimeConverter::ToTime_t(time);
    }
    
    return std::make_shared<WinEventRecord>(category, type, provider, message, date, eventID);
}
//------------------------------------------------------------------------------------
bool WinLogReader::HasRecord() const
{
    return !records_.empty();
}
//------------------------------------------------------------------------------------
LogRecordType WinLogReader::GetLogRecordType() const
{
    return LogRecordType::WinLogRecord;
}
//------------------------------------------------------------------------------------
bool WinLogReader::IsFiltred() const
{
    return isFiltred_;
}
//------------------------------------------------------------------------------------


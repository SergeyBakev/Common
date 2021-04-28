#include "pch.h"
#include "WinLogReaderV2.h"
#include <sddl.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>
#include "..\Helpers\win_handle_ptr.h"
#include "ke_kernel.h"
#include "WinEventRecord.h"
#pragma comment(lib, "wevtapi.lib")	

//------------------------------------------------------------------------------------
WinLogReaderV2::WinLogReaderV2(std::wstring_view providerName, HMODULE msgModule)
	: provider_(providerName), resDll_(msgModule)
{
}
//------------------------------------------------------------------------------------
const LogRecordsArray& WinLogReaderV2::GetRecords() const
{
	if (!IsFiltred())
	{

	}
	return records_;
}
//------------------------------------------------------------------------------------
size_t WinLogReaderV2::CountRecords() const
{
	return records_.size();
}
//------------------------------------------------------------------------------------
bool WinLogReaderV2::HasRecord() const
{
	return !records_.empty();
}
//------------------------------------------------------------------------------------
LogRecordType WinLogReaderV2::GetLogRecordType() const
{
	return LogRecordType::WinLogRecord;
}
//------------------------------------------------------------------------------------
bool WinLogReaderV2::RenderEvents(const HandlePtr& hResults)
{
    std::vector<EVT_HANDLE> hEvents(10);
    DWORD dwReturned = 0;
    records_.clear();
    while (EvtNext(hResults.get(), 10, hEvents.data(), INFINITE, 0, &dwReturned))
    {
        for (DWORD i = 0; i < dwReturned; i++)
        {
            auto eHandle = MakeHandlePtr(hEvents[i], EventDeleter{});
            auto record = RenderEvent(eHandle);
            if (record)
                records_.push_back(std::move(record));
            
        }
    }
    return HasRecord() && GetLastError() == 0;
}
//------------------------------------------------------------------------------------
std::wstring GetMessageString(EVT_HANDLE hMetadata, EVT_HANDLE hEvent, EVT_FORMAT_MESSAGE_FLAGS FormatId)
{
    DWORD dwBufferSize = 0;
    DWORD dwBufferUsed = 0;
    DWORD status = 0;
    std::vector<wchar_t> buffer;
    if (!EvtFormatMessage(hMetadata, hEvent, 0, 0, NULL, FormatId, dwBufferSize, nullptr, &dwBufferUsed))
    {
        status = GetLastError();
        if (ERROR_INSUFFICIENT_BUFFER == status)
        {
            dwBufferSize = dwBufferUsed;
            buffer.resize(dwBufferUsed, 0);
            EvtFormatMessage(hMetadata, hEvent, 0, 0, NULL, FormatId, dwBufferSize, buffer.data(), &dwBufferUsed);
            status = GetLastError();
        }  
    }
    std::wstring massage;
    if (!buffer.empty())
        massage.assign(buffer.data(), buffer.size());
    return massage;
}
//------------------------------------------------------------------------------------
ILogRecordPtr WinLogReaderV2::RenderEvent(const HandlePtr& hEventPtr)
{
    DWORD status = ERROR_SUCCESS;
    DWORD dwBufferSize = 0;
    DWORD dwBufferUsed = 0;
    DWORD dwPropertyCount = 0;
    ILogRecordPtr record;
    struct MemDeleter
    {
        void operator()(void* variant) const
        {
            if(variant != nullptr)
            {
                free(variant);
                variant = nullptr;
            }
        }

    };

    std::unique_ptr<EVT_VARIANT, MemDeleter> pRenderedValues;
    auto hContext = MakeHandlePtr(EvtCreateRenderContext(0, NULL, EvtRenderContextSystem), EventDeleter{});
    if (hContext == nullptr)
        return record;
    
    if (!EvtRender(hContext.get(), hEventPtr.get(), EvtRenderEventValues, dwBufferSize, nullptr, &dwBufferUsed, &dwPropertyCount))
    {
        dwBufferSize = dwBufferUsed;
        pRenderedValues.reset((PEVT_VARIANT)malloc(dwBufferSize));
        if (pRenderedValues)
            EvtRender(hContext.get(), hEventPtr.get(), EvtRenderEventValues, dwBufferSize, pRenderedValues.get(), &dwBufferUsed, &dwPropertyCount);

        if (ERROR_SUCCESS == (status = GetLastError()))
        {
            PEVT_VARIANT values = (PEVT_VARIANT)pRenderedValues.get();
            record = ToRecord(values, hEventPtr);
        }

    }

    return record;
}
//------------------------------------------------------------------------------------
ILogRecordPtr WinLogReaderV2::ToRecord(const PEVT_VARIANT pRenderedValues, const HandlePtr& hEvent)
{
    std::wstring provider = pRenderedValues[EvtSystemProviderName].StringVal;
    int eventID = -1;
    unsigned short category = -1;
    unsigned short type = -1;
    std::time_t date;
    std::wstring massage;
    if (!provider.empty())
    {
        auto hMethadata = MakeHandlePtr(EvtOpenPublisherMetadata(nullptr, provider.c_str(), nullptr, 0, 0), EventDeleter{});
        massage = GetMessageString(hMethadata.get(), hEvent.get(), EvtFormatMessageEvent);
    }

    type = pRenderedValues[EvtSystemLevel].Type == EvtVarTypeNull ? 0 : pRenderedValues[EvtSystemLevel].ByteVal;
    eventID = pRenderedValues[EvtSystemEventID].Type == EvtVarTypeNull ? 0 : pRenderedValues[EvtSystemEventID].ByteVal;
    category = pRenderedValues[EvtSystemTask].Type == EvtVarTypeNull ? 0 : pRenderedValues[EvtSystemTask].ByteVal;
    {
        ULONGLONG ullTimeStamp = 0;
        ULONGLONG ullNanoseconds = 0;
        FILETIME ft;
        ullTimeStamp = pRenderedValues[EvtSystemTimeCreated].FileTimeVal;
        ft.dwHighDateTime = (DWORD)((ullTimeStamp >> 32) & 0xFFFFFFFF);
        ft.dwLowDateTime = (DWORD)(ullTimeStamp & 0xFFFFFFFF);

        ULARGE_INTEGER ull;
        ull.LowPart = ft.dwLowDateTime;
        ull.HighPart = ft.dwHighDateTime;
        date = (ull.QuadPart / 10000000ULL - 11644473600ULL);
        ULONGLONG QuadPartMod = ull.QuadPart % 10000000ULL;
        if (QuadPartMod >= 5000000ULL)
            date++;
    }

    return std::make_shared<WinEventRecord>(category,type,provider,massage,date,eventID);
}
//------------------------------------------------------------------------------------
ILogReaderPtr WinLogReaderV2::Select(const ILogFilter& filter)
{
    DWORD status = ERROR_SUCCESS;
    std::wstringstream ssquery;
    ssquery << L"<QueryList>" 
            << L"<Query Path='Application'>"
            << L"<Select Path='Application'>"<<L"*[System[Provider[@Name = '"<< filter.GetName() <<"']]]" <<"</Select>"
            << L"</Query>"
            << L"</QueryList>";

    auto this_ = shared_from_this();
    std::wstring query  = ssquery.str();
    auto hResults = MakeHandlePtr(EvtQuery(NULL, NULL, query.c_str(), EvtQueryChannelPath | EvtQueryReverseDirection), EventDeleter{});
    if (hResults == nullptr)
        return this_;
 
    
    RenderEvents(hResults);
    
	return shared_from_this();
}
//------------------------------------------------------------------------------------
void WinLogReaderV2::ToStream(std::wostream& ar)
{
    std::for_each(std::begin(records_), std::end(records_), [&ar](const ILogRecordPtr record)
        {
            record->Serialize(ar);
        });
}
//------------------------------------------------------------------------------------
bool WinLogReaderV2::IsFiltred() const
{
	return false;
}
//------------------------------------------------------------------------------------
void WinLogReaderV2::EventDeleter::operator()(void* hEvent) const
{
    if (hEvent != nullptr)
    {
        EvtClose(hEvent);
        hEvent = nullptr;
    }
}
//------------------------------------------------------------------------------------
#include "pch.h"
#include "WinLogReaderV2.h"
#include <sddl.h>
#include <stdio.h>

#include <sstream>
#include "..\Helpers\win_handle_ptr.h"

#pragma comment(lib, "wevtapi.lib")	
WinLogReaderV2::WinLogReaderV2(std::wstring_view providerName, HMODULE msgModule)
	: provider_(providerName), resDll_(msgModule)
{
}

const LogRecordsArray& WinLogReaderV2::GetRecords() const
{
	if (!IsFiltred())
	{

	}
	return records_;
}

size_t WinLogReaderV2::CountRecords() const
{
	return size_t();
}

bool WinLogReaderV2::HasRecord() const
{
	return false;
}

LogRecordType WinLogReaderV2::GetLogRecordType() const
{
	return LogRecordType::WinLogRecord;
}
DWORD PrintEvent(EVT_HANDLE hEvent)
{
    DWORD status = ERROR_SUCCESS;
    DWORD dwBufferSize = 0;
    DWORD dwBufferUsed = 0;
    DWORD dwPropertyCount = 0;
    LPWSTR pRenderedContent = NULL;

    // The EvtRenderEventXml flag tells EvtRender to render the event as an XML string.
    if (!EvtRender(NULL, hEvent, EvtRenderEventXml, dwBufferSize, pRenderedContent, &dwBufferUsed, &dwPropertyCount))
    {
        if (ERROR_INSUFFICIENT_BUFFER == (status = GetLastError()))
        {
            dwBufferSize = dwBufferUsed;
            pRenderedContent = (LPWSTR)malloc(dwBufferSize);
            if (pRenderedContent)
            {
                EvtRender(NULL, hEvent, EvtRenderEventXml, dwBufferSize, pRenderedContent, &dwBufferUsed, &dwPropertyCount);
            }
            else
            {
                wprintf(L"malloc failed\n");
                status = ERROR_OUTOFMEMORY;
                goto cleanup;
            }
        }

        if (ERROR_SUCCESS != (status = GetLastError()))
        {
            wprintf(L"EvtRender failed with %d\n", GetLastError());
            goto cleanup;
        }
    }

    wprintf(L"\n\n%s", pRenderedContent);

cleanup:

    if (pRenderedContent)
        free(pRenderedContent);

    return status;
}

DWORD PrintResults(EVT_HANDLE hResults)
{
    DWORD status = ERROR_SUCCESS;
    EVT_HANDLE hEvents[10];
    DWORD dwReturned = 0;

    while (true)
    {
        // Get a block of events from the result set.
        if (!EvtNext(hResults, 10, hEvents, INFINITE, 0, &dwReturned))
        {
            if (ERROR_NO_MORE_ITEMS != (status = GetLastError()))
            {
                wprintf(L"EvtNext failed with %lu\n", status);
            }

            goto cleanup;
        }

        // For each event, call the PrintEvent function which renders the
        // event for display. PrintEvent is shown in RenderingEvents.
        for (DWORD i = 0; i < dwReturned; i++)
        {
            if (ERROR_SUCCESS == (status = PrintEvent(hEvents[i])))
            {
                EvtClose(hEvents[i]);
                hEvents[i] = NULL;
            }
            else
            {
                goto cleanup;
            }
        }
    }

cleanup:

    for (DWORD i = 0; i < dwReturned; i++)
    {
        if (NULL != hEvents[i])
            EvtClose(hEvents[i]);
    }

    return status;
}

bool WinLogReaderV2::RenderEvents(HandlePtr hResults)
{
    DWORD status = ERROR_SUCCESS;
    std::vector<EVT_HANDLE> hEvents(10);
    bool rc = false;
    bool bStop = false;
    DWORD dwReturned = 0;

    while (EvtNext(hResults.get(), 10, hEvents.data(), INFINITE, 0, &dwReturned)&& !bStop)
    {
        for (DWORD i = 0; i < dwReturned; i++)
        {
            {
                auto eHandle = MakeHandlePtr(hEvents[i], [](void* p) { EvtClose(p); p = nullptr; });
                RenderEvent(eHandle);
            }
            /*if (ERROR_SUCCESS == (status = PrintEvent(hEvents[i])))
            {
                EvtClose(hEvents[i]);
                hEvents[i] = NULL;
            }
            else
            {
                bStop = true;
                break;
            }*/
           
        }
    }


    
    return rc;
}

bool WinLogReaderV2::RenderEvent(HandlePtr hEvent)
{
    DWORD status = ERROR_SUCCESS;
    DWORD dwBufferSize = 0;
    DWORD dwBufferUsed = 0;
    DWORD dwPropertyCount = 0;
    EVT_HANDLE hContext = NULL;  
    PEVT_VARIANT pRenderedValues = NULL;
   

    hContext = EvtCreateRenderContext(0, NULL, EvtRenderContextSystem);
    if (NULL == hContext)
    {
        wprintf(L"EvtCreateRenderContext failed with %lu\n", status = GetLastError());
    }
    bool rc = false;
    // The EvtRenderEventXml flag tells EvtRender to render the event as an XML string.
    if (!EvtRender(hContext, hEvent.get(), EvtRenderEventValues, dwBufferSize, pRenderedValues, &dwBufferUsed, &dwPropertyCount))
    {
        pRenderedValues = (PEVT_VARIANT)malloc(dwBufferSize);
        if (pRenderedValues)
        {
            dwBufferSize = dwBufferUsed;
            EvtRender(NULL, hEvent.get(), EvtRenderEventValues, dwBufferSize, pRenderedValues, &dwBufferUsed, &dwPropertyCount);
            
        }
        if (ERROR_SUCCESS == (status = GetLastError()))
            rc = true;

    }
    ToRecord(pRenderedValues);

    if (hContext)
        EvtClose(hContext);

    if (pRenderedValues)
        free(pRenderedValues);
    return rc;
}

ILogRecordPtr WinLogReaderV2::ToRecord(PEVT_VARIANT& pRenderedValues)
{
    DWORD dwBufferSize = 0;
    DWORD dwBufferUsed = 0;
    DWORD dwPropertyCount = 0;
    WCHAR wsGuid[50];
    LPWSTR pwsSid = NULL;
    ULONGLONG ullTimeStamp = 0;
    ULONGLONG ullNanoseconds = 0;
    SYSTEMTIME st;
    FILETIME ft;


    wprintf(L"Provider Name: %s\n", pRenderedValues[EvtSystemProviderName].StringVal);
  


    DWORD EventID = pRenderedValues[EvtSystemEventID].UInt16Val;
    if (EvtVarTypeNull != pRenderedValues[EvtSystemQualifiers].Type)
    {
        EventID = MAKELONG(pRenderedValues[EvtSystemEventID].UInt16Val, pRenderedValues[EvtSystemQualifiers].UInt16Val);
    }
    wprintf(L"EventID: %lu\n", EventID);

    wprintf(L"Version: %u\n", (EvtVarTypeNull == pRenderedValues[EvtSystemVersion].Type) ? 0 : pRenderedValues[EvtSystemVersion].ByteVal);
    wprintf(L"Level: %u\n", (EvtVarTypeNull == pRenderedValues[EvtSystemLevel].Type) ? 0 : pRenderedValues[EvtSystemLevel].ByteVal);
    wprintf(L"Task: %hu\n", (EvtVarTypeNull == pRenderedValues[EvtSystemTask].Type) ? 0 : pRenderedValues[EvtSystemTask].UInt16Val);
    wprintf(L"Opcode: %u\n", (EvtVarTypeNull == pRenderedValues[EvtSystemOpcode].Type) ? 0 : pRenderedValues[EvtSystemOpcode].ByteVal);
    wprintf(L"Keywords: 0x%I64x\n", pRenderedValues[EvtSystemKeywords].UInt64Val);

    ullTimeStamp = pRenderedValues[EvtSystemTimeCreated].FileTimeVal;
    ft.dwHighDateTime = (DWORD)((ullTimeStamp >> 32) & 0xFFFFFFFF);
    ft.dwLowDateTime = (DWORD)(ullTimeStamp & 0xFFFFFFFF);

    FileTimeToSystemTime(&ft, &st);
    ullNanoseconds = (ullTimeStamp % 10000000) * 100; // Display nanoseconds instead of milliseconds for higher resolution
    wprintf(L"TimeCreated SystemTime: %02d/%02d/%02d %02d:%02d:%02d.%I64u)\n",
        st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond, ullNanoseconds);

    wprintf(L"EventRecordID: %I64u\n", pRenderedValues[EvtSystemEventRecordId].UInt64Val);

    wprintf(L"Execution ProcessID: %lu\n", pRenderedValues[EvtSystemProcessID].UInt32Val);
    wprintf(L"Execution ThreadID: %lu\n", pRenderedValues[EvtSystemThreadID].UInt32Val);
    wprintf(L"Channel: %s\n", (EvtVarTypeNull == pRenderedValues[EvtSystemChannel].Type) ? L"" : pRenderedValues[EvtSystemChannel].StringVal);
    wprintf(L"Computer: %s\n", pRenderedValues[EvtSystemComputer].StringVal);

    if (EvtVarTypeNull != pRenderedValues[EvtSystemUserID].Type)
    {
        if (ConvertSidToStringSid(pRenderedValues[EvtSystemUserID].SidVal, &pwsSid))
        {
            wprintf(L"Security UserID: %s\n", pwsSid);
            LocalFree(pwsSid);
        }
    }

    return nullptr;
}

ILogReaderPtr WinLogReaderV2::Select(const ILogFilter& filter)
{
    DWORD status = ERROR_SUCCESS;
    std::wstringstream ssquery;
    ssquery 
        << L"<QueryList>" 
        << L"<Query Path='Application'>"
        << L"<Select Path='Application'>"<<L"*[System[Provider[@Name = '"<< filter.GetName() <<"']]]" <<"</Select>"
        << L"</Query>"
        << L"</QueryList>";
    std::wstring query  = ssquery.str();
    auto hResults = MakeHandlePtr(EvtQuery(NULL,NULL, query.c_str(), EvtQueryChannelPath | EvtQueryReverseDirection),
        [](void* handle) {if(handle != nullptr) EvtClose(handle); });
    if (hResults == nullptr)
    {
        status = GetLastError();

        if (ERROR_EVT_CHANNEL_NOT_FOUND == status)
            wprintf(L"The channel was not found.\n");
        else if (ERROR_EVT_INVALID_QUERY == status)
            // You can call the EvtGetExtendedStatus function to try to get 
            // additional information as to what is wrong with the query.
            wprintf(L"The query is not valid.\n");
        else
            wprintf(L"EvtQuery failed with %lu.\n", status);


        return shared_from_this();
    }

    RenderEvents(hResults);
	return shared_from_this();
}

void WinLogReaderV2::ToStream(std::wostream& ar)
{

}

bool WinLogReaderV2::IsFiltred() const
{
	return false;
}


#include "pch.h"
#include "WinLogReaderV2.h"
#include <sddl.h>
#include <stdio.h>
#include <winevt.h>
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
    std::vector<wchar_t> pRenderedContent;
    bool rc = false;
    // The EvtRenderEventXml flag tells EvtRender to render the event as an XML string.
    if (!EvtRender(NULL, hEvent.get(), EvtRenderEventXml, dwBufferSize, NULL, &dwBufferUsed, &dwPropertyCount))
    {
        if (ERROR_INSUFFICIENT_BUFFER == (status = GetLastError()))
        {
            dwBufferSize = dwBufferUsed;
            pRenderedContent.resize(dwBufferSize);
            EvtRender(NULL, hEvent.get(), EvtRenderEventXml, dwBufferSize, pRenderedContent.data(), &dwBufferUsed, &dwPropertyCount);
            
        }
        if (ERROR_SUCCESS == (status = GetLastError()))
            rc = true;

    }
    std::wstring data(pRenderedContent.data());
    std::wcout << data << std::endl;
    return rc;
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


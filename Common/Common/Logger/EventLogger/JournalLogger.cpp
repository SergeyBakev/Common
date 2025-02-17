#include "pch.h"
#include <Windows.h>
#include <filesystem>
#include "JournalLogger.h"
#include "..\..\Helpers\win_handle_ptr.h"
#include "WinLogReaderV2.h"
#include "DummyReader.h"
//------------------------------------------------------------------------------------
//---------------------------JournalLoggerImpl---------------------------------------------------------
class WinJournalLoggerImpl : public IJournalLogger
{
public:

    WinJournalLoggerImpl(std::wstring_view providerName) : provider_(providerName)
	{
        eventLog_ = MakeHandlePtr(RegisterEventSourceW(nullptr, providerName.data()), [](void* p) { DeregisterEventSource(p); });
	}

    virtual ~WinJournalLoggerImpl() = default;
	
    virtual void Write(
        unsigned short type,
        unsigned short category,
        unsigned long eventId,
        StringVector& params
    ) const  override
    {
        size_t argc = 0;
        std::vector<LPWSTR> argv(params.size(),nullptr);
        if (!params.empty())
        {
            for (size_t i = 0; i < params.size(); i++)
                argv[i] = params[i].data();

            argc = argv.size();
        }
       
        BOOL ret = ReportEvent(eventLog_.get(),
            type,
            category,
            eventId,
            NULL,
            argc,
            0,
            (LPCWSTR*)argv.data(),
            NULL);
        _ASSERT(ret != 0);
    }

    virtual ILogReaderPtr MakeReader()  const override
    {
        /*std::wstring_view REGISTRY_FOLDER = L"SYSTEM\\CurrentControlSet\\services\\eventlog\\Application";
        const HKEY hKey = HKEY_LOCAL_MACHINE;
        Kernel::CRegistry reg(hKey, REGISTRY_FOLDER);
        std::wstring massageResource = reg.GetProfileString(provider_, L"EventMessageFile",L"*");
        _ASSERT_EXPR(massageResource != L"*", L"WinJournalLoggerImpl:: fail! massages resources not foud");*/
      /*  TCHAR exePath[_MAX_PATH] = { 0 };
        ::GetModuleFileName(NULL, exePath, _MAX_PATH);
        std::wstring_view  szFilePath = exePath;
        HMODULE module = LoadLibraryEx(szFilePath.data(), NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);*/
        return std::make_shared<WinLogReaderV2>(provider_);
    }
    
private:
    HandlePtr eventLog_;
	std::wstring provider_;
};
//------------------------------Dummy Reader------------------------------------------------------
class DummyLogReader : public ILogReader, std::enable_shared_from_this<DummyLogReader>
{
public:
    // Inherited via ILogReader
    virtual const LogRecordsArray& GetRecords() const override
    {
        static LogRecordsArray arr;
        return arr;
    }
    virtual size_t CountRecords() const override
    {
        return GetRecords().size();
    }
    virtual bool HasRecord() const override
    {
        return false;
    }
    virtual LogRecordType GetLogRecordType() const override
    {
        return LogRecordType::eUndefined;
    }
    virtual ILogReaderPtr Select(const ILogFilter& filter) override
    {
        return shared_from_this();
    }
    virtual void ToStream(std::wostream& ar) override
    {

    }
    virtual bool IsFiltred() const override
    {
        return false;
    }
};
//------------------------------------------------------------------------------------
ILogReaderPtr DummyJournalLogger::MakeReader() const
{
    return std::make_shared<DummyLogReader>();
}
//------------------------------------------------------------------------------------
void LogJournalProvider::Initialize(std::wstring_view providerName)
{
    if (providerName.empty() || providerName == Provider())
        return;

    provider_ = providerName;
}
//------------------------------------------------------------------------------------
void LogJournalProvider::Deinitialize()
{
    instance_ = nullptr;
}
//------------------------------------------------------------------------------------
std::wstring LogJournalProvider::Provider() const
{
    return provider_;
}
//------------------------------------------------------------------------------------
ILogReaderPtr LogJournalProvider::MakeReader() const
{
    return Instance()->MakeReader();
}
//------------------------------------------------------------------------------------
IJournalLoggerPtr LogJournalProvider::Instance() const
{
    if (instance_ == nullptr)
    {
        auto provider = Provider();
        _ASSERT_EXPR(!provider.empty(), L"JournalLogger::Instance cannot create. Provider name is empty");
        if (provider.empty())
            instance_ = std::make_shared<DummyJournalLogger>();
        else
            instance_ = std::make_shared<WinJournalLoggerImpl>(provider);
    }
    return instance_;
}
//------------------------------------------------------------------------------------
bool RegisterWinJournalProvider(std::wstring entrie_, std::wstring dllPath, size_t categoryCount, size_t countSupportedTypes, bool bUpdateCount)
{
    if (!std::filesystem::exists(dllPath))
    {
        _ASSERT_EXPR(0, L"resource path dosent exist");
        return false;
    }
    const HKEY hUseKey = HKEY_LOCAL_MACHINE;
    HKEY hKey = NULL;

    if (::RegCreateKey(hUseKey, entrie_.data(), &hKey) != ERROR_SUCCESS)
        return false;
    
   
    // Add the Event ID message-file name to the 'EventMessageFile' subkey.
    ::RegSetValueEx(hKey,
        L"EventMessageFile",
        0,
        REG_EXPAND_SZ,
        (CONST BYTE*)dllPath.data(),
        dllPath.size() * sizeof(wchar_t));

    ::RegSetValueEx(hKey,
        L"CategoryMessageFile",
        0,
        REG_EXPAND_SZ,
        (CONST BYTE*)dllPath.data(),
        dllPath.size() * sizeof(wchar_t));

    ::RegSetValueEx(hKey,
        L"ParameterMessageFile",
        0,
        REG_EXPAND_SZ,
        (CONST BYTE*)dllPath.data(),
        dllPath.size() * sizeof(wchar_t));

    // Set the supported types flags.
    DWORD dwData = countSupportedTypes;
    ::RegSetValueEx(hKey,
        L"TypesSupported",
        0,
        REG_DWORD,
        (CONST BYTE*)&dwData,
        sizeof(DWORD));

    dwData = categoryCount;
    ::RegSetValueEx(hKey,
        L"CategoryCount",
        0,
        REG_DWORD,
        (CONST BYTE*) & dwData,
        sizeof(DWORD));
    ::RegCloseKey(hKey);
    
    return true;
}

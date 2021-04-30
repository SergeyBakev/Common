#include "WinJournalLoggerImplV2.h"
#include <sstream>
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000
#include <Windows.h>
#include <fstream>
using namespace std::string_literals;

void WinJournalLoggerImplV2::Write(unsigned short type, unsigned short category, unsigned long eventId, StringVector& params) const
{
}

ILogReaderPtr WinJournalLoggerImplV2::MakeReader() const
{
    return ILogReaderPtr();
}

std::wstring MakeAtribute(std::wstring attr, std::wstring value)
{
    std::wstringstream ss;
    ss << attr << LR"( =")"<<value << LR"(")";
    return ss.str();
}

//std::vector<std::pair<std::wstring, std::wstring>> resources;
//void WriteMasage(std::wostringstream& os, std::wstring id, std::wstring value)
//{
//    resources.push_back({ id,value });
//}
void WriteTask(std::wostringstream& ss,std::wstring name, std::wstring symbol,int value, std::wstring msg)
{
    /*ss << L"            <task " << MakeAtribute(L"name", name) << std::endl;
    ss << L"                   " << MakeAtribute(L"symbol",symbol) <<std::endl;
    ss << L"                   " << MakeAtribute(L"value",std::to_wstring(value)) <<std::endl;
    ss << L"                    " << MakeAtribute(L"message", L"$(string.Level." + msg + L")") <<L">" << std::endl;*/

    ss << L"        <task " << MakeAtribute(L"name", name)
        << L" " << MakeAtribute(L"value", std::to_wstring(value))
        << L" " << MakeAtribute(L"symbol", symbol)
        << L" " << MakeAtribute(L"message", L"$(string.Level." + msg + L")")
        << L"></task>" << std::endl;
}
void WriteTasks(std::wostringstream& ss)
{
    ss << L"    <tasks>" << std::endl;
    WriteTask(ss, L"Disconnect", L"TASK_DISCONNECT", 1, L"Disconnect");
    WriteTask(ss, L"Connect", L"TASK_CONNECT", 2, L"Connect");
    ss << L"    </tasks>" << std::endl;
}

void WriteSeverityLevel(std::wostringstream& ss, std::wstring name, int value, std::wstring symbol, std::wstring msg)
{
    ss  << L"        <level " << MakeAtribute(L"name", name) 
        << L" " << MakeAtribute(L"value",std::to_wstring(value))
        << L" " << MakeAtribute(L"symbol", symbol)
        << L" " << MakeAtribute(L"message", L"$(string.Level." + msg + L")")
        << L"/>" << std::endl;
}
void WriteSeverityLevels(std::wostringstream& ss)
{
    ss << L"    <levels>" << std::endl;
    WriteSeverityLevel(ss, L"NotValid", 1, L"LEVEL_SAMPLEPROVIDER_NOTVALID", L"NotValid");
    WriteSeverityLevel(ss, L"Valid", 2, L"LEVEL_SAMPLEPROVIDER_VALID", L"Valid");
    ss << L"    </levels>" << std::endl;
}
void WriteChanel(std::wostringstream& ss, std::wstring providerName, std::wstring chidName)
{
    ss << L"    <channel " << MakeAtribute(L"chid", chidName) 
    << L" " << MakeAtribute(L"name", providerName + L"/" + L"Admin")
    << L" " << MakeAtribute(L"symbol", L"CHANNEL_BASEPROVIDER_ADMIN") 
    << L" " << MakeAtribute(L"type", L"Admin") 
    << "/>" << std::endl;
}
void WriteChannels(std::wostringstream& ss, std::wstring providerName, std::wstring chidName)
{
    ss << L"    <channels>" << std::endl;
    WriteChanel(ss, providerName, chidName);
    ss << L"    </channels>" << std::endl;
}

void WriteEventsData(std::wostringstream& ss, std::wstring providerName, std::wstring uuidStr, std::wstring dllPath)
{
    ss << L"    <events>" << std::endl;
    ss  << L"        <provider " << MakeAtribute(L"name", providerName)
        << L" " << MakeAtribute(L"guid", L"{" + uuidStr + L"}") 
        << L" " << MakeAtribute(L"resourceFileName", dllPath)
        << L" " << MakeAtribute(L"messageFileName", dllPath) 
        << L" " << MakeAtribute(L"message", L"$(string.Provider.Name)") << L">" 
        << std::endl;
    std::wstring chidName = L"c1";
    WriteChannels(ss, providerName, chidName);
    WriteSeverityLevels(ss);
    WriteTasks(ss);
    ss << L"        </provider>" << std::endl;
    ss << L"    </events>" << std::endl;
}

std::wstring FormatStringTag(std::wstring id, std::wstring value)
{
    std::wostringstream ss;
    ss << L"<string " << MakeAtribute(L"id", id) << L" " << MakeAtribute(L"value", value) << L"/>";
    return ss.str();
}
void WriteLocalization(std::wostringstream& ss, std::wstring providerName)
{
    ss << L"    <localization>" << std::endl;
    ss << L"        <resources " << MakeAtribute(L"culture", L"en-US") << L">" << std::endl;
    ss << L"            <stringTable>" << std::endl;
    ss << L"                " << FormatStringTag( L"Provied.Name", providerName) << std::endl;
    ss << L"                " << FormatStringTag( L"Level.Valid", L"Valid") << std::endl;
    ss << L"                " << FormatStringTag( L"Level.NotValid", L"Not Valid") << std::endl;
    ss << L"                " << FormatStringTag( L"Task.Disconnect", L"Disconnect") << std::endl;
    ss << L"                " << FormatStringTag( L"Task.Connect", L"Connect") << std::endl;
    ss << L"            </stringTable>" << std::endl;
    ss << L"        </resources>" << std::endl;
    ss << L"    </localization>" << std::endl;
}



void ManifestBegin(std::wostringstream& ss)
{
    ss << LR"(<instrumentationManifest xmlns="http://schemas.microsoft.com/win/2004/08/events"   xmlns:win="https://manifests.microsoft.com/win/2004/08/windows/events" xmlns:xs="https://www.w3.org/2001/XMLSchema">)" << std::endl;
    ss << L"<instrumentation>" << std::endl;
}

void ManifestEnd(std::wostringstream& ss)
{
    ss << L"</instrumentation>" << std::endl;
    ss << L"</instrumentationManifest>" << std::endl;
}
void WinJournalLoggerImplV2::Register(const std::wstring& dllPath, std::wstring providerName)
{
    
    UUID uuid;
    if (UuidCreate(&uuid) != RPC_S_OK)
        return;

    RPC_WSTR str;
    if (UuidToString(&uuid, (RPC_WSTR*)&str) != RPC_S_OK)
    {
        return;
    }

    std::wstring uuidStr = (wchar_t*)str;
    RpcStringFree(&str);

    std::wostringstream ss;
    ManifestBegin(ss);
    WriteEventsData(ss,providerName,uuidStr,dllPath);
    WriteLocalization(ss,providerName);
    //WriteTasks(ss);
    ManifestEnd(ss);
   

   
    std::wofstream ff(L"eventManifest.xml");
    ff << ss.str();
    ff.close();
    return;
}

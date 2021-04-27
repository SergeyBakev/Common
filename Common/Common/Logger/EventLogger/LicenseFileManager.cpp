#include "stdafx.h"
#include <windows.h>
#include <fileapi.h>
#include <synchapi.h>
#include <handleapi.h>
#include <filesystem>
#include <iostream>
#include "LicenseFileManager.h"
#include "defResourceManager.h"
#include "licResourceManager.h"
#define	TIMER_DELTA		60		// in seconds - so each 60 second TimerProc() should be called

//--------------------------------------------------------------------
//void _CL(LicenseFileManager* pManager//, 
//	//BOOL bMsg, // CollectLicenses
//	//CString* pHLLicText,
//	//CString* pTQText
//	)
//{
//	//if (!pManager->m_bServer && pManager->useSharedLic() && !pManager->userConnected())
//	//	return;		// if AxSTEAM application works & shared license used & user not connected - do nothing
//
//	// Important: should be called via monitoring functions only : TimerWaitingProc(), NLC(), RMON()
//	//
//	if (WaitForSingleObject(pManager->m_evTerminate, 0) == WAIT_OBJECT_0)	// if 'm_evTerminate' is signaled
//		return;
//
//	//pManager->CL(bMsg, TRUE, pHLLicText, pTQText);	// collect licenses (bLoadFeatures=TRUE)
//	pManager->CL();
//
//	pManager->OnCLFinish();
//
//
//	std::wcout << _T("Collect license\n");
//
//}		// ::_CL - CollectLicenses

//----------------------------------------------------------------------------
VOID CALLBACK TimerProc(LPVOID lpArgToCompletionRoutine, DWORD dwTimerLowValue,
	DWORD dwTimerHighValue) // used for AxSTREAM & server license manager (see TimerWaitingProc() below)
{
	////TRACE1(_T("\nTimerProc: %s\n"), CTime::GetCurrentTime().Format( _T("%Y-%b-%d %H:%M:%S")));

	//LicenseFileManager* pManager = (LicenseFileManager*)lpArgToCompletionRoutine;
	//hasp_handle_t sessionHandle = HASP_INVALID_HANDLE_VALUE;
	//CString HLMemText(_T(""));
	//CString TQText(_T(""));
	//CString HLLicText(_T(""));
	//if (HHL() &&												// HasHardLock
	//	(pManager->m_bServer || !pManager->useSharedLic()))	// manager work with HL (it is personal license or server)
	//{// HL works
	//	// Note: we create one HASP session and read HL memory once
	//	// in order to speed up TimerProc() execution (as execution time is more than 1 min)
	//	InHL(sessionHandle);	// login HASP session
	//	// read HL memory
	//	int status = RHLM(HLMemText, sessionHandle);
	//	ASSERT(status == HASP_STATUS_OK);
	//	if (status != HASP_STATUS_OK)
	//	{// something wrong - manager must suspend to give license features to user
	//		OutHL(sessionHandle);	// logout HASP session
	//		CString msg;
	//		msg.Format(_T("HASP read error. Returned status '%s'."), GHLS((hasp_error_codes)status));
	//		pManager->OnCLFailed(&msg);
	//		return;			// -------------- failed ------->
	//	}
	//	// extract license & TQ text
	//	PHLMEM(HLMemText, &HLLicText, &TQText);	// ParseHLMemory - if failed 'TQText' will be empty
	//}

	//if (pManager->m_nTimeServer < 0 || pManager->m_nTimeServer > 60)
	//{	// call Time Server just after runing and than every 60 sec
	//	pManager->m_nTimeServer = 0;
	//	pManager->m_bTimeServerOK = (pManager->GTFS() == 0) ? TRUE : FALSE;	// Get Time From Server
	//	pManager->m_nFilesDateCode = pManager->GTFF();						// Get Time From System Files
	//	pManager->m_bHLTime = (GRTC(pManager->m_HLTime, sessionHandle) == HASP_STATUS_OK) ? TRUE : FALSE;	// Get Time From Hardlock
	//}
	//else
	//	pManager->m_nTimeServer++;

	//if (WaitForSingleObject(pManager->m_petSuspended->m_hObject, 0) != WAIT_OBJECT_0)	// if 'm_petSuspended' is nonsignaled
	//{
	//	// fix TQ usage
	//	CString errMsg(_T(""));
	//	if (!_FTQUs(pManager, sessionHandle, HLMemText, HLLicText, TQText, errMsg))	// FixTimeQuoteUsage
	//	{// _FTQUs() is failed for some reason; so manager must suspend to give license features to user
	//		OutHL(sessionHandle);	// logout HASP session
	//		if (errMsg.IsEmpty())
	//			errMsg = _T("_FTQUs error.");
	//		pManager->OnCLFailed(&errMsg);
	//		return;			// -------------- failed ------->
	//	}
	//	else
	//		// collect licenses
	//		_CL(pManager, FALSE, &HLLicText, &TQText);	// bMsg=FALSE
	//}
	//if (sessionHandle != HASP_INVALID_HANDLE_VALUE)
	//	// logout HASP session
	//	OutHL(sessionHandle);

}		// ::TimerProc

//--------------------------------------------------------------------
//void TimerWaitingProc(LicenseFileManager* pManager) // used for AxSTREAM & server license manager ( see CLicenseManagerBase::m_pTimerThread)
//{
//	LARGE_INTEGER li;
//
//	HANDLE m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
//	const int nTimerUnitsPerSecond = 10000000;
//	li.QuadPart = -(TIMER_DELTA * nTimerUnitsPerSecond);	// negative values indicate relative time; so - each 'TIMER_DELTA' second
//
//	BOOL bRes = TRUE;
//
//	while (bRes)	// while 'm_evTerminate' is nonsignaled (see below)
//	{
//		// call TimerProc() over 'li.QuadPart' milliseconds (see value above)
//		BOOL bRet = SetWaitableTimer(m_hTimer,			// handle to timer
//			&li,						// timer due time 'QuadPart'
//			0,							// timer interval - if zero, the timer is signaled once (not periodic)
//			TimerProc,					// completion routine
//			pManager,						// completion routine parameter (in our case - lic manager pointer)
//			TRUE);
//		if (!bRet)
//		{
//			//TRACE1("\nTimerWaitingProc:: SetWaitableTimer failed (%d)\n", GetLastError());
//			//ASSERT(0);
//		}
//		DWORD res = ::WaitForSingleObjectEx(pManager->m_evTerminate, INFINITE, TRUE);	// TRUE - the function returns when the system queues an I/O completion routine or APC (Asynchronous procedure calls), and the thread runs the routine or function
//		bRes = (res != WAIT_OBJECT_0);	// TRUE - 'm_evTerminate' is nonsignaled (WAIT_OBJECT_0 - the state of the specified object is signaled)
//	}
//	CancelWaitableTimer(m_hTimer);
//	BOOL bRet = CloseHandle(m_hTimer);
//	if (!bRet)
//	{
//		//TRACE1("\nTimerWaitingProc:: CloseHandle failed (%d)\n", GetLastError());
//		//ASSERT(0);
//	}
//	//ASSERT(WaitForSingleObject(pManager->m_hTimer, 0) != WAIT_OBJECT_0);
//}		// ::TimerWaitingProc

//---------------------------------------------------------
// License directory change monitoring (used for AxSTREAM & server license manager - see CLicenseManagerBase::m_pLicenseChangeMonitor)
//
void LicenseFileManager::NLC()	// Notify license change
{
	// prepare monitoring handle
	HANDLE m_hMonitor = FindFirstChangeNotification(
		GetLicenseDirectory().c_str(),		// directory to watch 
		FALSE,						// do not watch the subtree 
		FILE_NOTIFY_CHANGE_FILE_NAME |
		FILE_NOTIFY_CHANGE_SIZE |
		FILE_NOTIFY_CHANGE_LAST_WRITE);
	// Important: if there is no necessity to scan directory (for HASP) let's leave this cycle working
	// to keep synchronization via 'm_evTerminate' event.
	//
	HANDLE handles[2] = { m_evTerminate, m_hMonitor };
	while (TRUE)
	{
		DWORD res = ::WaitForMultipleObjects(2, handles, FALSE, INFINITE);
		if (res == WAIT_OBJECT_0)
			// handles[0] was signaled - 'm_evTerminate'
			break;
		else
			if (res == WAIT_OBJECT_0 + 1)
			{	
				std::wclog << L"NLC signal" << std::endl;

				// handles[1] was signaled - 'pManager->m_hMonitor'

				//YURA TODO
				//BOOL bSkipCL = !pManager->m_bServer && pManager->useSharedLic();	// skip _CL() for shared license usage in AxSTREAM as only server should scan directory in such case
				//// note: HL usage should be skiped below as HL may be connected at any time and will be verified via TimerProc()

				//if (!pManager->HHLL() &&		// skip _CL() as there HL license should be written in license directory
				//	!bSkipCL &&
				//	WaitForSingleObject(pManager->m_petSuspended->m_hObject, 0) != WAIT_OBJECT_0)	// if 'm_petSuspended' is nonsignale

				// collect licenses
				//_CL(pManager/*, FALSE, NULL, NULL*/);	// bMsg=FALSE; pHLLicText=NULL, pTQText=NULL
				CL();

				FindNextChangeNotification(m_hMonitor);
			}
			else
				// unexpected Wait error
				break;	// GetLastError() or what to do ?
	}
	FindCloseChangeNotification(m_hMonitor);
}		// ::NLC


//------------------------------------------------------------------------------------
LicenseFileManager::LicenseFileManager()
	: m_evTerminate(nullptr)
{
}
//------------------------------------------------------------------------------------
LicenseFileManager::LicenseFileManager(std::wstring_view licenseDir)
	: LicenseFileManager()
{
	SetLicenseDirectory(licenseDir);
}
//-------------------------------------------------------------------------
LicenseFileManager::~LicenseFileManager()
{
	Destroy();
}
//-------------------------------------------------------------------------
void LicenseFileManager::Destroy()
{
	Stop();
	m_callback = nullptr;
}
//-------------------------------------------------------------------------
bool LicenseFileManager::IsStarted() const
{
	return m_evTerminate != nullptr;
}
//-------------------------------------------------------------------------
void LicenseFileManager::Start()
{
	if (IsStarted())
		Stop();
	GHA(HWItemsArray, true);
	CL();

	m_evTerminate = CreateEvent(NULL, TRUE, FALSE, _T("Terminate"));

	m_pLicenseChangeMonitor = std::thread(&LicenseFileManager::NLC, this);
}
//-------------------------------------------------------------------------
void LicenseFileManager::Stop()
{
	if (!IsStarted())
		return;

	SetEvent(m_evTerminate);

	//WaitForSingleObject(m_hMonitor, INFINITE);
	m_pLicenseChangeMonitor.join();

	CloseHandle(m_evTerminate);
	m_evTerminate = nullptr;
}
//-------------------------------------------------------------------------
void LicenseFileManager::SetLicenseDirectory(std::wstring_view path)
{
	_licenseDirectory = path;
}
//-------------------------------------------------------------------------
const std::wstring& LicenseFileManager::GetLicenseDirectory() const
{
	return _licenseDirectory;
}
//-------------------------------------------------------------------------
void LicenseFileManager::SetCallback(CallbackFunction f)
{
	m_callback = f;
}
//-------------------------------------------------------------------------
const CLicensesArray& LicenseFileManager::LA() const
{
	return _licenses;
}
//-------------------------------------------------------------------------
void LicenseFileManager::CL()
{
	std::wstring pMsg;
	std::wstring pTQText;
	const std::wstring& folder = GetLicenseDirectory();
	
	/*bool bExist = std::filesystem::exists(GetExecutableAppPath());
	_ASSERT_EXPR(bExist, L"CL, Aplication path not found");
	if (!bExist)
		return;*/


	int ret = GLF(folder,	// get license files from folder
		_licenses,
		pMsg,
		pTQText,
		HWItemsArray);

	OnCLFinish();
}
//-------------------------------------------------------------------------
void LicenseFileManager::OnCLFinish()
{
	if (m_callback)
		m_callback();
}
//-------------------------------------------------------------------------
bool LicenseFileManager::SLF(std::wstring_view fileName, std::wstring_view licContent) const
{
	auto& currentDir = GetLicenseDirectory();
	std::filesystem::path path = currentDir;
	path /= fileName;
	std::wofstream fs(path);
	fs << licContent;
	fs.close();
	return fs.good();
}
//-------------------------------------------------------------------------
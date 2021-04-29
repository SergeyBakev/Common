#include "pch.h"
#include "CppUnitTest.h"
#include <memory>
#include "logMsg.h"
#include <thread>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::string_literals;
using namespace std::chrono_literals;
namespace CommonTests
{

	TEST_CLASS(EventLoggerTests)
	{
		static inline std::wstring providerName = L"CommonTests EventLoggerTests";
		static inline std::wstring REGISTRY_FOLDER = L"SYSTEM\\CurrentControlSet\\services\\eventlog\\Application\\";
		static std::time_t startTime;
		static std::time_t endTime;
		static size_t totalCount;
		static size_t warrningsCount;
		static size_t errorsCount;
		static size_t informationsCount;

		TEST_CLASS_INITIALIZE(Init)
		{
			auto path = std::filesystem::current_path().append(L"CommonTests.dll");
			std::wstring test = LR"(D:\Common\Common\x64\Debug\CommonTests.dll)";
			Assert::AreEqual(true, RegisterWinJournalProvider(REGISTRY_FOLDER + providerName, path.c_str(), 2, 16, false));
			WriteLogs();
		}

	private:

		template <class ...Args>
		static void WriteLog(LogJournalProvider& provider,
			unsigned short type,
			unsigned short category,
			unsigned long eventId,
			Args&&... args)
		{
			provider.Write(type, category, eventId, std::forward<Args>(args)...);
			if (type == EVENTLOG_INFORMATION_TYPE)
			{
				informationsCount++;
			}
			else if (type == EVENTLOG_WARNING_TYPE)
			{
				warrningsCount++;
			}
			else if (type == EVENTLOG_ERROR_TYPE)
			{
				errorsCount++;
			}
			else
			{
				Assert::Fail(L"Log type not implemented");
			}

			totalCount++;
		}


		static LogJournalProvider MakeProvider()
		{
			LogJournalProvider provider;
			provider.Initialize(providerName);
			return provider;
		}
	public:
		TEST_METHOD(Create_Empty_Loger_And_Verify_Is_Dummy_Type)
		{
			//arange
			LogJournalProvider provider;
			
			//act
			auto reader = provider.MakeReader();
			auto providerName = provider.Provider();

			//assert
			
			//Assert::IsTrue()
			Assert::AreEqual(L""s, providerName);
			Assert::IsTrue(reader != nullptr);
		}

		TEST_METHOD(Count_Log_Should_By_Not_Empty)
		{
			//arange
			auto provider = MakeProvider();
			auto reader = provider.MakeReader();

			//act
			WinLogFilter filter;
			filter.name = providerName;
			reader->Select(filter);
			const auto& records = reader->GetRecords();

			//assert
			Assert::AreNotEqual((size_t)0, records.size());
		}

		TEST_METHOD(Select_By_Time)
		{
			//arange
			auto provider = MakeProvider();
			auto reader = provider.MakeReader();

			WinLogFilter filter;
			filter.name = providerName;
			filter.endTime = endTime;
			filter.startTime = startTime;

			//act
			reader->Select(filter);

			//assert
			auto& records = reader->GetRecords();
			Assert::AreEqual(totalCount, records.size());
		}

		TEST_METHOD(Select_By_Time_When_The_Recording_Time_Is_Less_Than_The_Search_Time)
		{
			//arange
			auto provider = MakeProvider();
			auto reader = provider.MakeReader();
			std::this_thread::sleep_for(10s);
			//act
			WinLogFilter filter;
			filter.name = providerName;
			filter.startTime = Kernel::DateTimeConverter::Now_t();
			filter.endTime = Kernel::DateTimeConverter::Now_t();

			reader->Select(filter);
			const auto& records = reader->GetRecords();

			//assert
			Assert::AreEqual((size_t)0, records.size());
		}
		
		template <
					unsigned short Type = (std::numeric_limits<unsigned short>::max)(),
					unsigned short Category = (std::numeric_limits<unsigned short>::max)()
				  >
		void Select(size_t Expected)
		{
			//arange
			auto provider = MakeProvider();
			auto reader = provider.MakeReader();
			
			//act
			WinLogFilter filter;
			filter.name = providerName;
			filter.endTime = endTime;
			filter.startTime = startTime;
			filter.eventType = Type;
			filter.eventCategory = Category;

			//act
			reader->Select(filter);

			//assert
			auto& records = reader->GetRecords();
			Assert::AreEqual(Expected, records.size());
		}

		TEST_METHOD(Select_By_EVENTLOG_ERROR_TYPE)
		{
			Select<EVENTLOG_ERROR_TYPE>(errorsCount);
		}

		TEST_METHOD(Select_By_EVENTLOG_WARNING_TYPE)
		{
			Select<EVENTLOG_WARNING_TYPE>(warrningsCount);
		}

		TEST_METHOD(Select_By_EVENTLOG_INFORMATION_TYPE)
		{
			Select<EVENTLOG_INFORMATION_TYPE>(informationsCount);
		}

		TEST_METHOD(Select_By_EVENTLOG_ERROR_TYPE_And_EVENTLOG_WARNING_TYPE)
		{
			Select<EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE>(errorsCount + warrningsCount);
		}

		TEST_METHOD(Select_By_EVENTLOG_ERROR_TYPE_And_EVENTLOG_INFORMATION_TYPE)
		{
			Select<EVENTLOG_ERROR_TYPE | EVENTLOG_INFORMATION_TYPE>(errorsCount + informationsCount);
		}

		TEST_METHOD(Select_By_EVENTLOG_INFORMATION_TYPE_And_EVENTLOG_WARNING_TYPE)
		{
			Select<EVENTLOG_INFORMATION_TYPE | EVENTLOG_WARNING_TYPE>(informationsCount + warrningsCount);
		}

		TEST_METHOD(Select_By_EVENTLOG_INFORMATION_TYPE_And_RPC_CATEGORY)
		{
			Select<EVENTLOG_INFORMATION_TYPE, RPC_CATEGORY>(2);
		}

		TEST_METHOD(Select_By_EVENTLOG_WARNING_TYPE_TYPE_And_RPC_CATEGORY)
		{
			Select<EVENTLOG_WARNING_TYPE, RPC_CATEGORY>(2);
		}

		TEST_METHOD(Select_By_EVENTLOG_ERROR_TYPE_And_RPC_CATEGORY)
		{
			Select<EVENTLOG_ERROR_TYPE, RPC_CATEGORY>(1);
		}

		TEST_METHOD(Select_All_Type_By_Rpc_Category)
		{
			Select<EVENTLOG_ERROR_TYPE | EVENTLOG_INFORMATION_TYPE | EVENTLOG_ERROR_TYPE, RPC_CATEGORY>(5);
		}


	private:

		static void WriteLogs()
		{
			startTime = Kernel::DateTimeConverter::Now_t();
			std::this_thread::sleep_for(10s);
			auto provider = MakeProvider();
			WriteLog(provider, EVENTLOG_INFORMATION_TYPE, RPC_CATEGORY, USER_CONNECTED, L"Test user");
			WriteLog(provider, EVENTLOG_INFORMATION_TYPE, RPC_CATEGORY, USER_DISCONNECTED, L"Test user");
			WriteLog(provider, EVENTLOG_INFORMATION_TYPE, NT_CATEGORY, RPC_CANNOT_STOPED);

			WriteLog(provider, EVENTLOG_WARNING_TYPE, RPC_CATEGORY, LICENSE_MNGR_DICONNECT_FAIL);
			WriteLog(provider, EVENTLOG_WARNING_TYPE, RPC_CATEGORY, USER_CONNECTED);
			WriteLog(provider, EVENTLOG_WARNING_TYPE, NT_CATEGORY, EVMSG_NOTREMOVED);
			WriteLog(provider, EVENTLOG_WARNING_TYPE, NT_CATEGORY, EVMSG_CTRLHANDLERNOTINSTALLED);

			WriteLog(provider, EVENTLOG_ERROR_TYPE, RPC_CATEGORY, NOT_FOUND_LIC_DIRECTORY, L"test error");
			WriteLog(provider, EVENTLOG_ERROR_TYPE, NT_CATEGORY, EVMSG_BADREQUEST);
			std::this_thread::sleep_for(1s);
			endTime = Kernel::DateTimeConverter::Now_t();
		}
	};

	//#define EVENTLOG_SUCCESS                0x0000
	//#define EVENTLOG_ERROR_TYPE             0x0001
	//#define EVENTLOG_WARNING_TYPE           0x0002
	//#define EVENTLOG_INFORMATION_TYPE       0x0004
	//#define EVENTLOG_AUDIT_SUCCESS          0x0008
	//#define EVENTLOG_AUDIT_FAILURE          0x0010
	INIT_STATIC(EventLoggerTests, startTime);
	INIT_STATIC(EventLoggerTests, endTime);
	INIT_STATIC(EventLoggerTests, totalCount);
	INIT_STATIC(EventLoggerTests, errorsCount);
	INIT_STATIC(EventLoggerTests, warrningsCount);
	INIT_STATIC(EventLoggerTests, informationsCount);
}
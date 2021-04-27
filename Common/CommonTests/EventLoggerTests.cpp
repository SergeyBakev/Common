#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonTests
{
	TEST_CLASS(EventLoggerTests)
	{
		static inline std::wstring providerName = L"AxStream License Service v3.0";
		TEST_METHOD(InitLogger)
		{
			//arange
			LogJournalProvider provider;
			
			//act
			

			//assert
			Assert::AreEqual(providerName, provider.Provider());
			Assert::IsTrue(provider.MakeReader() != nullptr);
		}
		

		TEST_METHOD(MyTest)
		{
			//arange

			//act

			//assert
		}

		TEST_METHOD(ReadLogv2)
		{
			//arange
			WinLogReaderV2 reader(providerName, GetModuleHandle(nullptr));

			//act
			WinLogFilter filter;
			filter.name = providerName;
			reader.Select(filter);
			auto records = reader.GetRecords();

			//assert
			Assert::AreNotEqual((size_t)0, records.size());
		}

	};
}
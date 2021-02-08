#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonTests
{
	TEST_CLASS(String_Alogrithm_Test)
	{
		TEST_METHOD(Split_By_Dot_When_Str_Contains_Only_Integral_Numbers)
		{
			//arrange
			std::vector<std::wstring> expected = {
				L"3",L"4",L"5",L"6",L"7",
			};

			std::wstring str = L"3.4.5.6.7";

			//act
			auto actual = Common::algorithms::string::Split(L".", str);

			//assert
			Assert::AreEqual(expected.size(), actual.size());
			for (size_t i = 0; i < expected.size(); i++)
			{
				bool eq = expected[i] == actual[i];
				Assert::IsTrue(eq);
			}
		}

		TEST_METHOD(Split_By_Dot_When_Str_Contains_Mixed_Walphabet_And_Numbers)
		{
			//arrange
			std::vector<std::wstring> expected = {
				L"3",L"dfgdf",L"5",L"dfgdfg",L"7",
			};

			std::wstring str = L"3.dfgdf.5.dfgdfg.7";

			//act
			auto actual = Common::algorithms::string::Split(L".", str);

			//assert
			Assert::AreEqual(expected.size(), actual.size());
			for (size_t i = 0; i < expected.size(); i++)
			{
				bool eq = expected[i] == actual[i];
				Assert::IsTrue(eq);
			}
		}

		TEST_METHOD(Split_By_Dot_When_Mode_Str_Contains_Empty_Lexems)
		{
			//arrange
			std::vector<std::wstring> expected = {
				L"3",L" ",L"5",L"6",L" ",
			};

			std::wstring str = L"3. .5.6. ";

			//act
			auto actual = Common::algorithms::string::Split(L".", str);

			//assert
			Assert::AreEqual(expected.size(), actual.size());
			for (size_t i = 0; i < expected.size(); i++)
			{
				bool eq = expected[i] == actual[i];
				Assert::IsTrue(eq);
			}
		}
	};
}
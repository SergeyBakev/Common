#include "pch.h"
#include "CppUnitTest.h"
#include <string>
#include <functional>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;
using namespace std::string_literals;

namespace CommonTests
{
	double toDouble(std::wstring v) { return std::stod(v); }

	TEST_CLASS(FormattedStringV2Tests)
	{
		TEST_METHOD(Initialize_From_Constructor)
		{
			//expected
			std::wstring expected = L"Test str";

			//arrange
			FormattedStringV2 str(expected);

			//act
			std::wstring actual = str.Str();

			//assert
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Initialize_From_Str_Method)
		{
			//expected
			std::wstring expected = L"Test str";

			//arrange
			FormattedStringV2 str;

			//act
			str.Str(expected);

			//assert
			std::wstring actual = str.Str();
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Construct_From_String_And_Get_Paramter_That_Does_Not_Contain_White_Space)
		{
			//expected
			std::wstring parameter = L"TestTag = TestStr;";
			std::wstring expected = L"TestStr";

			//arrange
			FormattedStringV2 str(parameter);
			str.GetParameter(L"");
			//act
			std::wstring actual = str.GetParameter(L"TestTag");
			
			//assert	
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Construct_From_String_And_Get_Paramter_That_Contain_White_Space)
		{
			//expected
			std::wstring parameter = L"TestTag = TestStr White Space;";
			std::wstring expected = L"TestStr White Space";

			//arrange
			FormattedStringV2 str(parameter);

			//act
			std::wstring actual = str.GetParameter(L"TestTag");

			//assert	
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Set_Get_Parameter_When_Tag_And_Value_Not_Trimed)
		{
			//expected
			std::wstring tag = L" Test ";
			std::wstring expected = L"   TestStr  ";

			//arrange
			FormattedStringV2 str{};

			//act
			str.SetParameter(tag, expected);
			std::wstring actual = str.GetParameter(L"Test");

			//assert
			Assert::AreEqual(L"TestStr"s, actual);
		}

		TEST_METHOD(Construct_From_Not_Trimed_String_And_Get_Paramter_That_Contain_White_Space)
		{
			//expected
			std::wstring parameter = L"  TestTag   = TestStr White Space   ;";
			std::wstring expected = L"TestStr White Space";

			//arrange
			FormattedStringV2 str(parameter);

			//act
			std::wstring actual = str.GetParameter(L"TestTag");

			//assert	
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Set_Get_Paramter_That_Does_Not_Contain_White_Space)
		{
			//expected
			std::wstring tag = L"Test";
			std::wstring expected = L"TestStr";

			//arrange
			FormattedStringV2 str{};
			
			//act
			str.SetParameter(tag, expected);
			std::wstring actual = str.GetParameter(tag);

			//assert
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Get_Paramter_Equals_No_Case)
		{
			//expected
			std::wstring tag = L"Test";
			std::wstring expected = L"TestStr";

			//arrange
			FormattedStringV2 str;

			//act
			str.SetParameter(tag, expected);
			std::wstring actual1 = str.GetParameter(L"test");
			std::wstring actual2 = str.GetParameter(L"TeSt");

			//assert
			Assert::AreEqual(expected, actual1);
			Assert::AreEqual(expected, actual2);
		}

		TEST_METHOD(Get_Parameter_And_Convert_To_Int)
		{
			//expected
			std::wstring tag = L"Test";
			int expected = 57;

			//arrange
			FormattedStringV2 str;
			std::wstring value = L"57";
			str.SetParameter(tag, value);
			//act

			auto actual = str.GetParameter<int>(tag);

			//assert
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Get_Parameter_And_Convert_To_Double)
		{
			//expected
			std::wstring tag = L"Test";
			double expected = 57.345;

			//arrange
			FormattedStringV2 str;
			std::wstring value = L"57.345";
			str.SetParameter(tag, value);
			//act
			auto actual = str.GetParameter<double>(tag);
			//assert
			Assert::AreEqual(expected, actual, std::numeric_limits<double>::epsilon());
		}
		
		TEST_METHOD(Get_Parameter_Whith_Custom_Converter)
		{
			std::wstring tag = L"Test";
			double expected = 57.345;
			
			//arrange
			FormattedStringV2 str;
			std::wstring value = L"57.345";
			str.SetParameter(tag, value);
			//act

			auto actual = str.GetParameter(tag, toDouble);
			//assert
			Assert::AreEqual(expected, actual, std::numeric_limits<double>::epsilon());
		}

		TEST_METHOD(Get_Paramter_Exception)
		{
			std::wstring tag = L"Test";
			double expected = 57.345;

			//arrange
			FormattedStringV2 str;
			std::wstring value = L"Test";
			str.SetParameter(tag, value);
			//act

			auto callable = [&]() {str.GetParameter<int>(tag); };

			//assert
			Assert::ExpectException<std::invalid_argument>(callable);
		}

		TEST_METHOD(Get_Paramter_Exception_Whith_Custom_Converter)
		{
			std::wstring tag = L"Test";
			double expected = 57.345;

			//arrange
			FormattedStringV2 str;
			std::wstring value = L"Test";
			str.SetParameter(tag, value);
			//act

			auto callable = [&]() {str.GetParameter(tag, toDouble); };

			//assert
			Assert::ExpectException<std::invalid_argument>(callable);
		}

		TEST_METHOD(Set_Int_Parameter_And_Get_Parameter_Converted_To_Double)
		{
			//expected
			std::wstring tag = L"Test";
			double expected = 57.0;

			//arrange
			FormattedStringV2 str;
			std::wstring value = L"57";
			str.SetParameter(tag, value);
			//act

			auto actual = str.GetParameter<double>(tag);

			//assert
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Set_Paramaeter_From_Template_Function)
		{
			//expected
			std::wstring tag = L"Test";
			double expected = 57.345;

			//arrange
			FormattedStringV2 str;
			
			//act
			str.SetParameter(tag, expected);
			auto actual = str.GetParameter<double>(tag);

			Assert::AreEqual(expected, actual,std::numeric_limits<double>::epsilon());
		}

		TEST_METHOD(Try_Get_Parameter_When_String_Contains_Double)
		{
			//expected
			std::wstring tag = L"Test";
			double expected = 57.345;

			//arrange
			FormattedStringV2 str;
			std::wstring value = L"57.345";
			str.SetParameter(tag, value);

			//act
			auto actual = str.TryGetParameter<double>(tag);

			//assert
			Assert::IsTrue(actual != std::nullopt);
			Assert::AreEqual(*actual, expected, std::numeric_limits<double>::epsilon());
		}

		TEST_METHOD(Try_Get_Parameter_When_String_Contains_Int)
		{
			//expected
			std::wstring tag = L"Test";
			int expected = 57;

			//arrange
			FormattedStringV2 str;
			std::wstring value = L"57";
			str.SetParameter(tag, value);

			//act
			auto actual = str.TryGetParameter<int>(tag);

			//assert
			Assert::IsTrue(actual != std::nullopt);
			Assert::AreEqual(*actual, expected);
		}

		TEST_METHOD(Try_Get_When_Parameter_Doesnt_Convertible)
		{
			//expected
			std::wstring tag = L"Test";

			//arrange
			FormattedStringV2 str;
			std::wstring value = L"Test";
			str.SetParameter(tag, value);

			//act
			auto actual = str.TryGetParameter<int>(tag);

			//assert
			Assert::IsTrue(actual == std::nullopt);
		}
	};
}
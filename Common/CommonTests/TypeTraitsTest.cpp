#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::string_literals;
namespace CommonTests
{
	TEST_CLASS(TypeTraits_Test)
	{
		TEST_METHOD(Is_Vector_True)
		{
			//arrange
			std::vector<double> v;

			//act
			bool res = is_vector_default_alloc<std::vector<double>>::value;

			//assert
			Assert::IsTrue(res);
		}

		TEST_METHOD(Is_Vector_False_If_Integral_Value)
		{
			//arrange
			std::vector<double> v;

			//act
			bool res = is_vector_default_alloc<double>::value;

			//assert
			Assert::IsFalse(res);
		}

		TEST_METHOD(Is_Vector_False_If_Struct)
		{
			//arrange
			std::vector<double> v;

			//act
			bool res = is_vector_default_alloc<std::false_type>::value;

			//assert
			Assert::IsFalse(res);
		}

		
	};
}
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonTests
{
	struct Test
	{
		size_t pos;
		
		double _arr[26];
	};

	TEST_CLASS(TypedFArrayTest_Standart_Indexer)
	{
		static std::vector<Test> expected;
		static constexpr size_t SIZE = 50000;

		using Array = FArray<Test>;
		static void GenerateRandomVector(double* vec,size_t size)
		{
			srand((uint32_t)time(nullptr));
			for (size_t i = 0; i < size; i++)
				vec[i] = (double)(rand() % 10000);

		}

		static void AddArrayElementFromExpectedVector(Array& vec)
		{
			for (size_t i = 0; i < expected.size(); i++)
			{
				auto& var = expected[i];
				vec.Add(var);
			}
		}

		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			Test t{};
			for (size_t i = 0; i < SIZE; i++)
			{
				t.pos = i;
				GenerateRandomVector(t._arr, 26);
				expected.push_back(t);
			}
		}

		TEST_METHOD(Add_Data_And_Get_Them)
		{
			//arrange
			Array ar;
		}
	};

	std::vector<Test> TypedFArrayTest_Standart_Indexer::expected;
}
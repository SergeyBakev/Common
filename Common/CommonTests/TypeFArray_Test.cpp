#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonTests
{
	struct Test
	{
		size_t pos;
		
		double _arr[26];
		friend bool operator==(Test& a, Test& b);
	};

	bool operator==(Test& a, Test& b)
	{
		return std::tie(a.pos, a._arr[0], a._arr[1], a._arr[2], a._arr[3]
			, a._arr[4], a._arr[5], a._arr[6], a._arr[7], a._arr[8]
			, a._arr[9], a._arr[11], a._arr[12], a._arr[13], a._arr[14]
			, a._arr[15], a._arr[16], a._arr[17], a._arr[18], a._arr[19]
			, a._arr[20], a._arr[21], a._arr[22], a._arr[23], a._arr[24], a._arr[25])
		==
		std::tie(b.pos, b._arr[0], b._arr[1], b._arr[2], b._arr[3]
			, b._arr[4], b._arr[5], b._arr[6], b._arr[7], b._arr[8]
			, b._arr[9], b._arr[11], b._arr[12], b._arr[13], b._arr[14]
			, b._arr[15], b._arr[16], b._arr[17], b._arr[18], b._arr[19]
			, b._arr[20], b._arr[21], b._arr[22], b._arr[23], b._arr[24], b._arr[25]);
	}
	TEST_CLASS(TypedFArrayTest_Standart_Indexer)
	{
		static std::vector<Test> expected;
		static constexpr size_t SIZE = 50000;

		using Array = FArray<Test,StandartIndexMapper>;
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

		TEST_METHOD(Add_New_Elemnt_And_Get_Them)
		{
			//arrange
			Array ar;
		
			//act
			AddArrayElementFromExpectedVector(ar);

			//assert
			for (size_t i = 0; i < SIZE; i++)
			{
				auto actual = ar.GetAt(i);
				auto& exp = expected[i];
				Assert::IsTrue(actual == exp);
			}
		}

		TEST_METHOD(Get_At_Random_Index)
		{
			//arange
			Array ar;
			AddArrayElementFromExpectedVector(ar);

			//act

			//assert
			srand((uint32_t)time(nullptr));
			for (size_t i = 0; i < SIZE; i++)
			{
				size_t random_index = rand() % SIZE;

				auto actual = ar.GetAt(random_index);
				auto& exp = expected[random_index];
				Assert::IsTrue(actual == exp);
			}
		}

		TEST_METHOD(SetAt_New_Element_At_And_Get_Them)
		{
			//arange
			Array ar;

			//act
			for (size_t i = 0; i < SIZE; i++)
				ar.SetAt(i, expected[i]);
			
			//assert
			for (size_t i = 0; i < SIZE; i++)
			{
				auto actual = ar.GetAt(i);
				auto& exp = expected[i];
				Assert::IsTrue(actual == exp);
			}
		}

		TEST_METHOD(Set_New_Element_If_Index_Exsit)
		{
			//arange
			Array ar;
			AddArrayElementFromExpectedVector(ar);

			//act
			srand((uint32_t)time(nullptr));
			for (size_t i = 0; i < SIZE; i++)
			{
				size_t random_index = rand() % SIZE;
				if (i == random_index)
					continue;

				ar.SetAt(random_index,expected[i]);
				expected[random_index] = expected[i];
			}

			//assert
			for (size_t i = 0; i < SIZE; i++)
			{
				auto actual = ar.GetAt(i);
				auto& exp = expected[i];
				Assert::IsTrue(actual == exp);
			}
		}

		TEST_METHOD(Add_Element_Data_Patrial)
		{
			//arange
			Array ar;

			//act
			Test t;
			for (size_t i = 0; i < SIZE; i++)
			{
				t.pos = i;
				for (size_t j = 0; j < 5; j++)
					t._arr[j] = expected[i]._arr[j];

				ar.SetAt(i, t);
			}

			for (size_t i = 0; i < SIZE; i++)
			{
				t = ar.GetAt(i);
				for (size_t j = 5; j < 10; j++)
					t._arr[j] = expected[i]._arr[j];

				ar.SetAt(i, t);
			}

			for (size_t i = 0; i < SIZE; i++)
			{
				t = ar.GetAt(i);
				for (size_t j = 10; j < 15; j++)
					t._arr[j] = expected[i]._arr[j];

				ar.SetAt(i, t);
			}

			for (size_t i = 0; i < SIZE; i++)
			{
				t = ar.GetAt(i);
				for (size_t j = 15; j < 20; j++)
					t._arr[j] = expected[i]._arr[j];

				ar.SetAt(i, t);
			}

			for (size_t i = 0; i < SIZE; i++)
			{
				t = ar.GetAt(i);
				for (size_t j = 20; j < 26; j++)
					t._arr[j] = expected[i]._arr[j];

				ar.SetAt(i, t);
			}


			//assert
			for (size_t i = 0; i < SIZE; i++)
			{
				auto actual = ar.GetAt(i);
				auto& exp = expected[i];
				Assert::IsTrue(actual == exp);
			}
		}

		TEST_METHOD(Get_Begin_Iterator)
		{
			//arrange
			Array ar;
			ar.Add(expected[0]);

			//act
			auto it = ar.begin();
			

			//assert
			Assert::IsTrue(*it == expected[0]);
		}

		TEST_METHOD(Get_Begin_Iterator_If_Array_Empty_And_Try_Get_Data)
		{
			//arange
			Array ar;

			//act
			auto it = ar.begin();
			Assert::ExpectException<std::out_of_range>([&]() {*it; });
		}

		TEST_METHOD(Get_End_Iterator_And_Try_Get_Data)
		{

			//arrange
			Array ar;
			ar.Add(expected[0]);

			//act
			auto it = ar.end();
			Assert::ExpectException<std::out_of_range>([&]() {*it; });
		}

		TEST_METHOD(End_Equals_Begin_If_Array_Empty)
		{
			//arrange
			Array ar;

			//act
			auto begin = ar.begin();
			auto end = ar.end();
			//assert
			Assert::IsTrue(begin == end);
		}

		TEST_METHOD(End_Not_Equals_Begin_If_Array_Not_Empty)
		{
			//arrange
			Array ar;
			ar.Add(expected[0]);
			//act
			auto begin = ar.begin();
			auto end = ar.end();
			//assert
			Assert::IsTrue(begin != end);
		}

		TEST_METHOD(For_Each_Get)
		{
			//arrange
			Array ar;
			AddArrayElementFromExpectedVector(ar);
			size_t i = 0;

			//act
			//assert
			for (auto& data : ar)
			{
				Assert::IsTrue(data == expected[i]);
				i++;
			}
		}

		TEST_METHOD(For_Each_Get_Set)
		{
			//arrange
			Array ar;
			AddArrayElementFromExpectedVector(ar);
			
			std::vector<Test> actual;
			std::vector<Test> expected_vec;

			//act
			for (auto& it : ar)
			{
				size_t index = rand() % SIZE;
				auto& data = expected[index];
				it = data;
				expected_vec.push_back(data);
			}

			//assert
			size_t i = 0;
			for (auto& it : ar)
			{
				Assert::IsTrue(it == expected_vec[i]);
				i++;
			}
		}

	};

	std::vector<Test> TypedFArrayTest_Standart_Indexer::expected;
}
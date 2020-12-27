#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonTests
{
	TEST_CLASS(StandartIndexerTest)
	{
		TEST_METHOD(If_Empty_Get_Firsty)
		{
			//arrange
			StandartIndexMapper indexer;

			//act
			auto idx = indexer.First();

			//assert
			Assert::AreEqual(StandartIndexMapper::npos, idx);
		}

		TEST_METHOD(If_Empty_Get_Last)
		{
			//arrange
			StandartIndexMapper indexer;

			//act
			auto idx = indexer.Last();

			//assert
			Assert::AreEqual(StandartIndexMapper::npos, idx);
		}

		TEST_METHOD(If_Empty_Get_Prev)
		{
			//arrange
			StandartIndexMapper indexer;

			//act
			auto idx = indexer.Prev(0);

			//assert
			Assert::AreEqual(StandartIndexMapper::npos, idx);
		}

		TEST_METHOD(If_Empty_Get_Next)
		{
			//arrange
			StandartIndexMapper indexer;

			//act
			auto idx = indexer.Next(0);

			//assert
			Assert::AreEqual(StandartIndexMapper::npos, idx);
		}

		TEST_METHOD(If_Empty_Get_Count)
		{
			//arrange
			StandartIndexMapper indexer;

			//act
			auto idx = indexer.Count();

			//assert
			Assert::AreEqual((size_t)0, idx);
		}

		TEST_METHOD(If_Empty_Get_Real_Index)
		{
			//arrange
			StandartIndexMapper indexer;

			//act
			auto idx = indexer.RealIndex(0);

			//assert
			Assert::AreEqual(StandartIndexMapper::npos, idx);
		}

		TEST_METHOD(If_Empty_Create_New_Index)
		{
			//arrange
			StandartIndexMapper indexer;

			//act
			auto idx = indexer.CreateIndex();

			//assert
			Assert::AreEqual(StandartIndexMapper::DefaultStartIndex, idx);
		}
		TEST_METHOD(Add_And_Get_RealIndex)
		{
			//arrange
			StandartIndexMapper indexer;
			size_t idx0 = 0;
			size_t idx1 = 1;
			size_t idx2 = 2;

			//act
			indexer.Add(idx0);
			indexer.Add(idx1);
			indexer.Add(idx2);

			size_t actual_idx0 = indexer.RealIndex(idx0);
			size_t actual_idx1 = indexer.RealIndex(idx1);
			size_t actual_idx2 = indexer.RealIndex(idx2);

			//assert
			Assert::AreEqual(idx0, actual_idx0);
			Assert::AreEqual(idx1, actual_idx1);
			Assert::AreEqual(idx2, actual_idx2);

		}

		TEST_METHOD(Get_Real_Index_At_Negative_Index)
		{
			//arrange
			StandartIndexMapper indexer;
			size_t idx0 = 0;
			size_t idx1 = 1;
			size_t idx2 = 2;
			indexer.Add(idx0);
			indexer.Add(idx1);
			indexer.Add(idx2);

			//act
			size_t idx = indexer.RealIndex(-2);

			//act
			Assert::AreEqual(StandartIndexMapper::npos, idx);
		}

		TEST_METHOD(Get_Count)
		{
			//arrange
			StandartIndexMapper indexer;
			size_t idx = 0;			

			indexer.Add(idx++);
			indexer.Add(idx++);
			indexer.Add(idx++);

			size_t count = idx;

			//act
			size_t actual_count = indexer.Count();

			//assert
			Assert::AreEqual(count, actual_count);
		}
	
		TEST_METHOD(Get_First)
		{
			//Arrange
			StandartIndexMapper indexer;
			size_t expected_index = 0;
			size_t expected_index2 = expected_index;
			size_t actual_index;
			indexer.Add(expected_index2++);
			indexer.Add(expected_index2++);
			indexer.Add(expected_index2++);

			//act
			actual_index = indexer.First();

			//assert
			Assert::AreEqual(expected_index, actual_index);
		}

		TEST_METHOD(Get_Last)
		{
			//arrange
			StandartIndexMapper indexer;
			size_t expected_index = 0;
			size_t actual_index;

			indexer.Add(expected_index++);
			indexer.Add(expected_index++);
			indexer.Add(expected_index);

			//act
			actual_index = indexer.Last();

			//assert
			Assert::AreEqual(expected_index, actual_index);
		}

		TEST_METHOD(Get_Next)
		{
			//arrange
			StandartIndexMapper indexer;
			size_t idx0 = 0;
			size_t idx1 = 1;
			size_t idx2 = 2;
			
			indexer.Add(idx0);
			indexer.Add(idx1);
			indexer.Add(idx2);

			//act
			size_t actual_idx1 = indexer.Next(idx0);
			size_t actual_idx2 = indexer.Next(idx1);
			size_t actual_idx3 = indexer.Next(idx2);

			//assert
			Assert::AreEqual(idx1, actual_idx1);
			Assert::AreEqual(idx2, actual_idx2);
			Assert::AreEqual(StandartIndexMapper::npos,actual_idx3);
		}

		TEST_METHOD(Get_Prev)
		{
			StandartIndexMapper indexer;
			size_t idx0 = 0;
			size_t idx1 = 1;
			size_t idx2 = 2;

			indexer.Add(idx0);
			indexer.Add(idx1);
			indexer.Add(idx2);

			//act
			size_t actual_idx0 = indexer.Prev(idx1);
			size_t actual_idx1 = indexer.Prev(idx2);
			size_t actual_idxNpos = indexer.Prev(idx0);

			//assert
			Assert::AreEqual(idx0, actual_idx0);
			Assert::AreEqual(idx1, actual_idx1);
			Assert::AreEqual(StandartIndexMapper::npos, actual_idxNpos);
			
		}

		TEST_METHOD(Get_Real_Index)
		{
			//arrange
			StandartIndexMapper indexer;
			size_t idx0 = 0;
			size_t idx1 = 1;
			size_t idx2 = 2;

			indexer.Add(idx0);
			indexer.Add(idx1);
			indexer.Add(idx2);

			//act
			size_t actual_idx0 = indexer.RealIndex(idx0);
			size_t actual_idx1 = indexer.RealIndex(idx1);
			size_t actual_idx2 = indexer.RealIndex(idx2);
			size_t actual_idx3 = indexer.RealIndex(idx2 + 1);

			//assert
			Assert::AreEqual(idx0, actual_idx0);
			Assert::AreEqual(idx1, actual_idx1);
			Assert::AreEqual(idx2, actual_idx2);
			Assert::AreEqual(StandartIndexMapper::npos, actual_idx3);
		}

		TEST_METHOD(All_Method_Together_When_All_Data_Correct)
		{
			srand((uint32_t)time(nullptr));
			//arrange
			StandartIndexMapper indexer;
			const size_t COUNT = 10000;
			size_t random_index = rand() % COUNT;
			//act
			for (size_t i = 0; i < COUNT; i++)
				indexer.Add(indexer.CreateIndex());
			
			size_t next1 = indexer.Next(random_index);
			size_t next2 = indexer.Next(next1);
			size_t prev_after_next1 = indexer.Prev(next2);
			size_t prev_after_next2 = indexer.Prev(prev_after_next1);

			size_t prev1 = indexer.Prev(random_index);
			size_t prev2 = indexer.Prev(prev1);
			size_t next_after_prev1 = indexer.Next(prev2);
			size_t next_after_prev2 = indexer.Next(next_after_prev1);

			size_t real = indexer.RealIndex(random_index);
			size_t count = indexer.Count();


			//assert
			Assert::AreEqual(StandartIndexMapper::DefaultStartIndex, indexer.First());
			Assert::AreEqual(COUNT - 1, indexer.Last());
			Assert::AreEqual(count, indexer.Count());


			Assert::AreEqual(next1, random_index + 1);
			Assert::AreEqual(next2, random_index + 2);
			Assert::AreEqual(prev_after_next1, next1);
			Assert::AreEqual(prev_after_next2, random_index);

			Assert::AreEqual(prev1, random_index - 1);
			Assert::AreEqual(prev2, random_index - 2);
			Assert::AreEqual(next_after_prev1, prev1);
			Assert::AreEqual(next_after_prev2, random_index);

			Assert::AreEqual(real, random_index);

			

		}

	};

	TEST_CLASS(SparseIndexerTest)
	{
		
	};
}
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonTests
{
	TEST_CLASS(FArrayBaseTest)
	{
	public:
		
		static constexpr size_t SIZE = 1000;
		static constexpr size_t ELEMENT_SIZE = 15; //��� ������������ ����� �������������� ������ �� 15 double
		static std::vector<std::vector<double>> expected;
		static std::vector<size_t> random_unique_indexes;

		static void GenerateRandomVector(std::vector<double>& vec)
		{
			if (vec.size() != ELEMENT_SIZE)
				vec.resize(ELEMENT_SIZE);

			srand((uint32_t)time(nullptr));
			for (size_t i = 0; i < vec.size(); i++)
				vec[i] = (double)(rand() % 10000);
			
		}

		static void AddArrayElementFromExpectedVector(FArrayBase<SparseIndexMapper>& vec)
		{
			for (size_t i = 0; i < expected.size(); i++)
			{
				auto& var = expected[i];
				vec.Add(var.data());
			}
		}

		TEST_CLASS_INITIALIZE(ClassInitialize)
		{		
			expected.clear();
			std::vector<double> vec(ELEMENT_SIZE);
			for (size_t i = 0; i < SIZE; i++)
			{
				GenerateRandomVector(vec);
				expected.push_back(vec);

				srand((uint32_t)time(nullptr));

				size_t indexes = rand() % SIZE;
				random_unique_indexes.push_back(indexes);
			}

			auto it = std::unique(std::begin(random_unique_indexes), std::end(random_unique_indexes));
			random_unique_indexes.resize(std::distance(std::begin(random_unique_indexes), it));
			
		}

		TEST_METHOD(Initialize_Size)
		{
			//arrange
			const size_t expected_size = 15;
			FArrayBase arr;

			//act
			arr.SetObjectSize(expected_size);
			

			//assert
			size_t actual = arr.GetObjectSize();

			Assert::AreEqual(expected_size, actual);
		}

		//����������� ���������������� ���������� ���������

		TEST_METHOD(Add_New_Element_And_Get_Them)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<std::vector<double>> actual;
			std::vector<double> vec(ELEMENT_SIZE);

			//act
			AddArrayElementFromExpectedVector(ar);
				
			for (size_t i = 0; i < ar.Count(); i++)
			{
				ar.GetAt(i, vec.data());
				actual.push_back(vec);
			}

			//assert

			Assert::AreEqual(expected.size(), actual.size());

			for (size_t i = 0; i < SIZE; i++)
			{
				auto& act_val = actual[i];
				auto& expected_val = expected[i];
				Assert::AreEqual(act_val.size(), expected_val.size());

				for (size_t i = 0; i < ELEMENT_SIZE; i++)
				{
					double val1 = act_val[i];
					double val2 = expected_val[i];
					Assert::AreEqual(val2,val1 );
				}
					
			}
		}

		//����������� ������������ ������ �� ��������
		TEST_METHOD(Add_New_Element_And_Get_At_Random_Index_Them)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<std::vector<double>> actual;
			std::vector<std::vector<double>> expeted_vec;
			std::vector<double> vec(ELEMENT_SIZE);
			AddArrayElementFromExpectedVector(ar);

			//act
			for (size_t i = 0; i < SIZE; i++)
			{
				srand((uint32_t)time(nullptr));
				size_t random_index = (size_t)rand() % SIZE;
				ar.GetAt(random_index, vec.data());

				//for expected
				actual.push_back(vec);

				auto& v = expected[i];

				//for expected
				expeted_vec.push_back(v);
			}

			//asssert
			Assert::AreEqual(expeted_vec.size(), actual.size());

			for (size_t i = 0; i < SIZE; i++)
			{
				auto& act_val = actual[i];
				auto& expected_val = expeted_vec[i];
				Assert::AreEqual(act_val.size(), expected_val.size());

				for (size_t i = 0; i < ELEMENT_SIZE; i++)
				{
					double val1 = act_val[i];
					double val2 = expected_val[i];
					Assert::AreEqual(val2, val1);
				}

			}
		}

		//����������� ���������� ��� ������������������ ����������, ������, ������� �������� �� �� �������

		TEST_METHOD(Set_New_Element_At_Random_Index_And_After_Get_Them)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<std::vector<double>> actual(SIZE);
			std::vector<std::vector<double>> expeted_vec(SIZE);
			std::vector<double> vec(ELEMENT_SIZE);

			//act
			for (size_t i = 0; i < random_unique_indexes.size(); i++)
			{
				size_t& index = random_unique_indexes[i];
				ar.SetAt(index, expected[index].data());
				expeted_vec[index] = expected[index];			
			}

			//post arrange
			for (size_t i = 0; i < random_unique_indexes.size(); i++)
			{
				size_t& index = random_unique_indexes[i];
				ar.GetAt(index, vec.data());
				actual[index] = vec;
			}
			//assert


			for (size_t i = 0; i < random_unique_indexes.size(); i++)
			{
				auto& act_val = actual[random_unique_indexes[i]];
				auto& expected_val = expeted_vec[random_unique_indexes[i]];
				Assert::AreEqual(act_val.size(), expected_val.size());

				for (size_t i = 0; i < ELEMENT_SIZE; i++)
				{
					double val1 = act_val[i];
					double val2 = expected_val[i];
					Assert::AreEqual(val2, val1);
				}

			}
		}

		TEST_METHOD(Add_All_Element_And_After_Set_Some_Element_At_Radndom_index)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<std::vector<double>> actual(SIZE);
			std::vector<std::vector<double>> expeted_vec = expected;
			std::vector<double> vec(ELEMENT_SIZE);

			AddArrayElementFromExpectedVector(ar);

			for (size_t i = 0; i < random_unique_indexes.size(); i++)
			{
				size_t index = random_unique_indexes[i];
				srand((uint32_t)time(nullptr));
				size_t random_index = (size_t)rand() % SIZE;

				ar.SetAt(index, expected[random_index].data());

				expeted_vec[index] = expected[random_index];
			}

			//assert
			for (size_t i = 0; i < SIZE; i++)
			{
				auto& act_val = expeted_vec[i];
				ar.GetAt(i, vec.data());
				auto& expected_val = vec;

				Assert::IsTrue(expected_val == act_val);
			}
		}

		//size_t index = 1;
		//���������� ������ Add(value), Add(value), SetAt(index -1, value)
		//��� ������������� � PMMQ ��� ���������� ������ � ������ ���������� ����� �������� ������ � ���, ��� ��� ����� SetAt ������� Add �� ��������� � �����
		//�.� � �� ��������� ������� � ������ �� ����� ��� Add

		TEST_METHOD(Add_Element_And_After_Set_At_For_Before_Element)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<std::vector<double>> actual(SIZE);
			std::vector<std::vector<double>> expeted_vec(SIZE);
			std::vector<double> vec(ELEMENT_SIZE);

			//add First element
			ar.Add(expected[0].data());
			expeted_vec[0] = expected[0];

			//act
			for (size_t i = 1; i < SIZE; i++)
			{
				//Add
				ar.Add(expected[i].data());
				expeted_vec[i] = expected[i];

				//SetAt
				ar.SetAt(i - 1, expected[i].data());
				expeted_vec[i -1 ] = expected[i];
			}

			//assert
			for (size_t i = 0; i < SIZE; i++)
			{
				auto& act_val = expeted_vec[i];
				ar.GetAt(i, vec.data());
				auto& expected_val = vec;

				Assert::IsTrue(expected_val == act_val);
			}
		}

	};

	std::vector<std::vector<double>> FArrayBaseTest::expected;
	std::vector<size_t> FArrayBaseTest::random_unique_indexes;
}

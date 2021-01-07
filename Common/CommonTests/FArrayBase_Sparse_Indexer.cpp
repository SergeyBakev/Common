#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::string_literals;
namespace CommonTests
{
	TEST_CLASS(FArrayBaseTest_Sparse_Indexer)
	{
	public:
		
		static constexpr size_t SIZE = 50000;
		static constexpr size_t ELEMENT_SIZE = 26; //Для тестирования будет использоваться вектор из 26 double
		static std::vector<std::vector<double>> expected;
		static std::vector<double> expected_doubles;
		static std::vector<size_t> random_unique_indexes;
		static constexpr size_t OBJECTS_SIZE = 10;

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
				double v = (double)i;
				vec = { v,v,v,v,
						v,v,v,v,
						v,v,v,v,
						v,v,v,v,
						v,v,v,v,
						v,v,v,v,
						v,v
				};
				//GenerateRandomVector(vec);
				expected.push_back(vec);
				for (auto& i : vec)
					expected_doubles.push_back(i);
				
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

		//Так как был добавлен признак нужно ли сделать флаш, стоит проверить

		TEST_METHOD(Verify_Is_Data_Flush_After_Add_And_Get)
		{
			//arrange
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<std::vector<double>> actual;
			std::vector<double> vec(ELEMENT_SIZE);
			AddArrayElementFromExpectedVector(ar);

			//act
			ar.GetAt(SIZE - 1, vec.data());

			//assert
			Assert::IsTrue(vec == expected[SIZE - 1]);

		}

		//Проверяется последовательное добавление елементов

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

		//Проверяется произвольный доступ по инедексу
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

				auto& v = expected[random_index];

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

		//Проверяется добавление при непоследовательном заполнении, тоесть, индексы приходят не по порядку

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
		//Комбинация команд Add(value), Add(value), SetAt(index -1, value)
		//При использовании в PMMQ при посторении дерева в момент добавления узлов возникла ошибка в том, что при после SetAt функция Add не добавляла в конец
		//т.к я не перемещал позицию в потоке на конец при Add

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

		TEST_METHOD(Set_Get_Element)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<std::vector<double>> actual;
			std::vector<std::vector<double>> expected_vec;
			std::vector<double> vec(ELEMENT_SIZE);

			//act
			ar.Add(expected[0].data());
			ar.Add(expected[1].data());


			expected_vec.push_back(expected[0]);
			expected_vec.push_back(expected[1]);

			for (size_t i = 2; i < SIZE; i++)
			{
				ar.SetAt(i-1, expected[i-2].data());
				ar.SetAt(i-2, expected[i-1].data());

				ar.GetAt(i-1,vec.data());
				actual.push_back(vec);

				ar.GetAt(i-2,vec.data());
				actual.push_back(vec);
				ar.Add(expected[i].data());

				expected_vec[i - 1] = expected[i-2];
				expected_vec[i - 2] = expected[i-1];


				expected_vec.push_back(expected[i]);
			}

			//assert
			for (size_t i = 0; i < SIZE; i++)
			{
				auto& act_val = expected_vec[i];
				ar.GetAt(i, vec.data());
				auto& expected_val = vec;

				std::wstring str;
				std::wstring str2;
				std::for_each(act_val.begin(), act_val.end(), [&](double v) { str += std::to_wstring(v) + L" "s; });
				std::for_each(expected_val.begin(), expected_val.end(), [&](double v) { str2 += std::to_wstring(v) + L" "s; });
				auto msg = StringHelper::Concatenate(L" ", "Expected vector\n: ", str, "\nActual data:\n", str2, "\nError in iteration i:", i);
				Assert::IsTrue(expected_val == act_val, msg.c_str());
			}
		}

		//С это проблемой я столкнулся сразу же при внедрении в CFD/Stress
		//Результати загружались поэтапно. То есть допустим index = 5, сначала в data[26] записывались по индексам(допустим 1-5). После чего обратно сохранялось в массив по index
		//После чего в какой то момент доставался элемент index и данные дозаписывались(допустим) (6-10). По какой то причние я уменя сбоило. Причину точно уже не помню
		TEST_METHOD(Add_Element_Data_Patrial)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<std::vector<double>> actual(SIZE);
			std::vector<double> vec(ELEMENT_SIZE);
			std::vector<double> vec1(ELEMENT_SIZE);

			//act
			for (size_t i = 0; i < SIZE; i++)
			{
				for (size_t j = 0; j < 5; j++)
					vec1[j] = expected[i][j];

				ar.SetAt(i, vec1.data());
			}

			for (size_t i = 0; i < SIZE; i++)
			{
				ar.GetAt(i, vec.data());
				for (size_t j = 5; j < 10; j++)
					vec[j] = expected[i][j];

				
				ar.SetAt(i, vec.data());
			}

			for (size_t i = 0; i < SIZE; i++)
			{
				ar.GetAt(i, vec.data());
				for (size_t j = 10; j < 15; j++)
					vec[j] = expected[i][j];

				ar.SetAt(i, vec.data());
			}

			for (size_t i = 0; i < SIZE; i++)
			{
				ar.GetAt(i, vec.data());
				for (size_t j = 15; j < 20; j++)
					vec[j] = expected[i][j];

				ar.SetAt(i, vec.data());
			}

			for (size_t i = 0; i < SIZE; i++)
			{
				ar.GetAt(i, vec.data());
				for (size_t j = 20; j < 26; j++)
					vec[j] = expected[i][j];

				ar.SetAt(i, vec.data());
			}


			//assert
			for (size_t i = 0; i < SIZE; i++)
			{
				auto& act_val = expected[i ];
				ar.GetAt(i, vec.data());
				auto& expected_val = vec;

				auto line = StringHelper::Concatenate(L" ", "Data at index: ", i, "invalid");
				Assert::IsTrue(expected_val == act_val, line.c_str());
			}

		}

		TEST_METHOD(Add_Element_Data_Patrial_Assert_In_All_Iterations)
		{

			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<std::vector<double>> actual(SIZE);
			std::vector<double> vec(ELEMENT_SIZE);

			//act
			for (size_t i = 0; i < SIZE; i++)
			{
				for (size_t j = 0; j < 5; j++)
					vec[j] = expected[i][j];

				ar.SetAt(i, vec.data());
			}	

			for (size_t i = 0; i < SIZE; i++)
			{
				ar.GetAt(i, vec.data());

				//Assert
				for (size_t j = 0; j < 5; j++)
				{
					Assert::AreEqual(vec[j], expected[i][j],
						StringHelper::Concatenate(L" ", "Get data at i: ", i, "j: ", j, "fail").c_str()
					);
				}

				for (size_t j = 5; j < 10; j++)
					vec[j] = expected[i][j];


				ar.SetAt(i, vec.data());
			}
		}

		/*
		TEST_METHOD(Add_Element_Data_Patrial_Assert_In_All_Iterations)
		{
			
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<std::vector<double>> actual(SIZE);
			std::vector<double> vec(ELEMENT_SIZE);

			//act
			for (size_t i = 0; i < SIZE; i++)
			{
				for (size_t j = 0; j < 5; j++)
					vec[j] = expected[i][j];

				ar.SetAt(i, vec.data());
			}

			for (size_t i = 0; i < SIZE; i++)
			{
				ar.GetAt(i, vec.data());
				//Assert
				for (size_t j = 0; j < 5; j++)
				{
					Assert::AreEqual(vec[j], expected[i][j],
						StringHelper::Concatenate(L" ", "Get data at i: ", i, "j: ", j, "fail").c_str()
					);
				}
			}

			for (size_t i = 0; i < SIZE; i++)
			{
				ar.GetAt(i, vec.data());

				//Assert
				for (size_t j = 0; j < 5; j++)
				{
					Assert::AreEqual(vec[j], expected[i][j],
						StringHelper::Concatenate(L" ","Get data at i: ", i, "j: ",j,"fail").c_str()
						);
				}

				for (size_t j = 5; j < 10; j++)
					vec[j] = expected[i][j];

				
				ar.SetAt(i, vec.data());
			}

			for (size_t i = 0; i < SIZE; i++)
			{
				ar.GetAt(i, vec.data());

				//Assert
				for (size_t j = 0; j < 10; j++)
				{
					Assert::AreEqual(vec[j], expected[i][j]);
				}

				for (size_t j = 10; j < 15; j++)
					vec[j] = expected[i][j];

				ar.SetAt(i, vec.data());
			}

			for (size_t i = 0; i < SIZE; i++)
			{
				ar.GetAt(i, vec.data());
				//Assert
				for (size_t j = 0; j < 15; j++)
				{
					Assert::AreEqual(vec[j], expected[i][j]);
				}

				for (size_t j = 15; j < 20; j++)
					vec[j] = expected[i][j];

				ar.SetAt(i, vec.data());
			}

			for (size_t i = 0; i < SIZE; i++)
			{
				ar.GetAt(i, vec.data());

				//Assert
				for (size_t j = 0; j < 20; j++)
				{
					Assert::AreEqual(vec[j], expected[i][j]);
				}

				for (size_t j = 20; j < 26; j++)
					vec[j] = expected[i][j];

				ar.SetAt(i, vec.data());
			}


			//assert
			for (size_t i = 0; i < SIZE; i++)
			{
				auto& act_val = expected[i ];
				ar.GetAt(i, vec.data());
				auto& expected_val = vec;

				auto line = StringHelper::Concatenate(L" ", "Data at index: ", i, "invalid");
				Assert::IsTrue(expected_val == act_val, line.c_str());
			}

		}*/

		TEST_METHOD(Serialize_Deserialize_Arr)
		{
			//arrange
			FArrayBase arExpected;
			FArrayBase arActual;
			arActual.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			arExpected.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<std::vector<double>> actual;
			std::vector<double> vec(ELEMENT_SIZE);
			std::vector<double> vec2(ELEMENT_SIZE);

			AddArrayElementFromExpectedVector(arActual);

			std::string file_name = "arr.bin";

			std::ofstream fsSave;
			fsSave.open(file_name, std::ios::out | std::ios::binary);
			if (!fsSave.is_open())
				throw std::exception();

			std::ifstream fsLoad;
			//act
			arActual.Serialize(fsSave);
			fsSave.close();

			fsLoad.open(file_name, std::ios::in | std::ios::binary);
			if (!fsLoad.is_open())
				throw std::exception();
			arExpected.Deserialize(fsLoad);

			//assert
			Assert::AreEqual(arExpected.Count(), arActual.Count());
			for (size_t i = 0; i < arExpected.Count(); i++)
			{
				arActual.GetAt(i, vec.data());
				arExpected.GetAt(i, vec2.data());
				Assert::IsTrue(vec == vec2);
			}

			std::remove(file_name.c_str());
		}

		//С данной проблемой я столкнулся при очистики масива, но при этому не сбрасывая индексы
		TEST_METHOD(Add_Element_Clear_Array_And_Try_Get)
		{
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<std::vector<double>> actual(SIZE);
			AddArrayElementFromExpectedVector(ar);

			//act
			ar.Clear();
			auto func = [&]() {ar.GetAt(0, actual.data()); };
			Assert::ExpectException<std::out_of_range>(func);
		}

		//Решил добавить функциональность по чтению больше чем ObjectSize
		TEST_METHOD(Get_Elemet_More_Then_One_Elemnt)
		{
			//arrange
			SparseFArray arr;
			arr.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			AddArrayElementFromExpectedVector(arr);
			std::vector<double> data(OBJECTS_SIZE * ELEMENT_SIZE);		
			std::vector<double> actual;

			//act
			for (size_t i = 0; i < SIZE; i += 10)
			{
				arr.GetAt(i, data.data(), OBJECTS_SIZE);
				for (auto& i : data)
					actual.push_back(i);

			}

			//assert
			Assert::AreEqual(actual.size(), expected_doubles.size());
			Assert::IsTrue(actual == expected_doubles);
		}

		TEST_METHOD(Get_Begin_Iterator_If_Array_And_Try_Get_Data)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));

			//act
			auto it = ar.begin();
			Assert::ExpectException<std::out_of_range>([&]() {it.Get<double>(); });

		}
		TEST_METHOD(Get_Begin_Iterator)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<double> expected_vec = expected[0];
			ar.Add(expected_vec.data());

			//act
			auto it = ar.begin();
			auto elem = it.Get<double>();
			
			//assert
			std::vector<double> actual(elem, elem + ELEMENT_SIZE);
			Assert::IsTrue(expected_vec == actual);
		}

		TEST_METHOD(Get_End_Iterator_And_Try_Get_Data)
		{

			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<double> expected_vec = expected[0];
			ar.Add(expected_vec.data());

			//act
			auto it = ar.end();
			Assert::ExpectException<std::out_of_range>([&]() {it.Get<double>(); });	
		}

		TEST_METHOD(End_Equals_Begin_If_Array_Empty)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));

			//act
			auto begin = ar.begin();
			auto end = ar.end();
			//assert
			Assert::IsTrue(begin == end);
		}

		TEST_METHOD(End_Not_Equals_Begin_If_Array_Not_Empty)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			std::vector<double> expected_vec = expected[0];
			ar.Add(expected_vec.data());
			//act
			auto begin = ar.begin();
			auto end = ar.end();
			//assert
			Assert::IsTrue(begin != end);
		}

		TEST_METHOD(For_Each_Get)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			AddArrayElementFromExpectedVector(ar);
			std::vector<std::vector<double>> actual;
			size_t count = 0;
			//act
			for (auto& it : ar)
			{
				double* data = it.Get<double>();
				std::vector < double> v;
				v.assign(data, data + it.ObjectSize() / sizeof(double));
				actual.push_back(std::move(v));
				count++;
			}

			//assert
			Assert::AreEqual(count, ar.Count());
			Assert::AreEqual(actual.size(), expected.size());
			
			for (size_t i = 0; i < SIZE; i++)
			{
				auto& act_val = actual[i];
				auto& expected_val = expected[i];

				Assert::IsTrue(expected_val == act_val);
			}
			
		}

		TEST_METHOD(For_Each_Get_Set)
		{
			//arrange
			srand((uint32_t)time(nullptr));
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			AddArrayElementFromExpectedVector(ar);

			std::vector<std::vector<double>> actual;
			std::vector<std::vector<double>> expected_vec;

			//act

			//set
			for (auto& it : ar)
			{
				size_t index = rand() % SIZE;
				auto& data = expected[index];
				it.Set(data.data());
				expected_vec.push_back(data);	
			}

			//get
			for (auto& it : ar)
			{
				double* data =  it.Get<double>();
				std::vector < double> v;
				v.assign(data, data + it.ObjectSize() / sizeof(double));
				actual.push_back(std::move(v));
			}

			//assert

			for (size_t i = 0; i < SIZE; i++)
			{
				auto& act_val = actual[i];
				auto& expected_val = expected_vec[i];

				Assert::IsTrue(expected_val == act_val);
			}
		}

		TEST_METHOD(Postfix_Increment)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			AddArrayElementFromExpectedVector(ar);
			
			//act
			auto el = ar[0];
			auto postEl = el++;

			//assert
			std::vector<double> v1;
			std::vector<double> v2;
			v1.assign(el.Get<double>(), el.Get<double>() +el.ObjectSize() / sizeof(double));
			v2.assign(postEl.Get<double>(), postEl.Get<double>() + postEl.ObjectSize() / sizeof(double));

			Assert::IsTrue(v2 == expected[0]);
			Assert::IsTrue(v1 == expected[1]);
		}

		TEST_METHOD(Get_Count_Doubles_If_Array_Contains_Doubles)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));

			//act
			size_t cnt = ar.GetCountElement<double>();

			//assert
			Assert::AreEqual(ELEMENT_SIZE, cnt);

		}

		TEST_METHOD(Get_Count_Doubles_If_Array_Contains_Int32)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(sizeof(uint32_t));
			
			//act
			size_t cnt = ar.GetCountElement<double>();

			//assert
			Assert::AreEqual(ELEMENT_SIZE, cnt);

		}

		TEST_METHOD(Get_RValue_Iterator_And_Write_Into_Them)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			AddArrayElementFromExpectedVector(ar);
			
			//act
			ar[0].Set<double>(expected[5].data());

			//assert
			auto it = ar[0];
			auto data = it.Get<double>();
			std::vector<double> v;
			v.assign(data, data + ar.GetCountElement<double>());

			Assert::IsTrue(expected[5] == v);
			
		}

		//Конструкто копирования не сбрасывает данные в массив. Право сбросить не передается другому обьекту
		TEST_METHOD(Call_Copy_Constructor_Proxy_Object_When_Data_Modified)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			AddArrayElementFromExpectedVector(ar);
		
			{
				//act
				auto it2 = ar[0];
				{
					auto it = ar[0];
					it.Set<double>(expected[1].data());
					it2 = it;
				}

				//assert
				std::vector<double> actual(ELEMENT_SIZE);
				ar.GetAt(0, actual.data());
				Assert::IsTrue(expected[1] == actual);
			}
		}

		TEST_METHOD(Verify_Algorithm_STD_Find)
		{
			//arrange
			FArrayBase ar;
			ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
			AddArrayElementFromExpectedVector(ar);
			auto& exp = expected[37];

			//act
			auto it = std::find(ar.begin(), ar.end(), exp);

			//asssert
			Assert::IsFalse(it == ar.end());

			double* data = it.Get<double>();
			DoubleVector actual;
			std::copy(data, data + ar.GetCountElement<double>(), std::back_inserter(actual));

			Assert::IsTrue(exp == actual);
		}
	};

	

	std::vector<std::vector<double>> FArrayBaseTest_Sparse_Indexer::expected;
	std::vector<double> FArrayBaseTest_Sparse_Indexer::expected_doubles;
	std::vector<size_t> FArrayBaseTest_Sparse_Indexer::random_unique_indexes;
}

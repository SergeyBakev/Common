#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonTests
{
	TEST_CLASS(FArrayBaseTest_Standart_Indexer)
	{
		static constexpr size_t SIZE = 1000;
		static constexpr size_t ELEMENT_SIZE = 26; //ƒл€ тестировани€ будет использоватьс€ вектор из 15 double
		static std::vector<std::vector<double>> expected;

		static void GenerateRandomVector(std::vector<double>& vec)
		{
			if (vec.size() != ELEMENT_SIZE)
				vec.resize(ELEMENT_SIZE);

			srand((uint32_t)time(nullptr));
			for (size_t i = 0; i < vec.size(); i++)
				vec[i] = (double)(rand() % 10000);

		}

		static void AddArrayElementFromExpectedVector(FArrayBase<StandartIndexMapper>& vec)
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
			}
		}

		
	};

	std::vector<std::vector<double>> FArrayBaseTest_Standart_Indexer::expected;
}
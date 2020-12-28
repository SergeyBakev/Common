#include <iostream>
#include <fstream>
#include "..\Common\Arrays\ke_farray_base.h"
using namespace std;

static constexpr size_t SIZE = 50000;
static constexpr size_t ELEMENT_SIZE = 26; //ƒл€ тестировани€ будет использоватьс€ вектор из 26 double


int main()
{
    FArrayBase<StandartIndexMapper> ar;
    ar.SetObjectSize(26 * sizeof(double));
	std::vector<double> vec(ELEMENT_SIZE);
	std::vector<double> vec1(ELEMENT_SIZE);

	//act
	for (size_t i = 0; i < SIZE; i++)
	{
		for (size_t j = 0; j < 5; j++)
			vec1[j] = (double)i;

		ar.SetAt(i, vec1.data());
	}
    
	for (size_t i = 0; i < SIZE; i++)
	{
		ar.GetAt(i, vec.data());
		for (size_t j = 5; j < 10; j++)
			vec[j] = (double)j;


		ar.SetAt(i, vec.data());
	}
}



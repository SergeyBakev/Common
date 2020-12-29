#include <iostream>
#include <fstream>
#include "..\Common\Arrays\ke_farray_base.h"
using namespace std;

static constexpr size_t SIZE = 10;
static constexpr size_t ELEMENT_SIZE = 26; //ƒл€ тестировани€ будет использоватьс€ вектор из 26 double

void test2()
{
	std::fstream _fs;
	_fs.open("temp.bin", ios::binary | ios::in | ios::out |ios::trunc);
	std::vector<double> vec(ELEMENT_SIZE);
	std::streamoff _current_putpos = 0;
	std::streamoff _end_pos = 0;
	size_t size = ELEMENT_SIZE * sizeof(double);
	for (size_t i = 0; i < SIZE; i++)
	{
		for (size_t j = 0; j < 5; j++)
			vec[j] = (double)i;
		
		_fs.write((const char*)vec.data(), size);
		_end_pos += ELEMENT_SIZE * sizeof(double);
		_current_putpos = _end_pos;
	}

	auto posp = _fs.tellp();
	auto posg = _fs.tellg();

	for (size_t i = 0; i < SIZE; i++)
	{
		//get at
		posp = _fs.tellp();
		posg = _fs.tellg();
		std::streamoff sizeoffset = static_cast<std::streamoff>(i * size);
		std::streamoff seek = sizeoffset - _current_putpos;
		if (seek != static_cast<std::streamoff>(0))
			_fs.seekg(seek, std::ios::cur);

		if (_fs.fail() || _fs.bad() )
			throw std::exception("Stream has invalid state");

		posp = _fs.tellp();
		posg = _fs.tellg();

		memset(vec.data(), 0, size);
		_fs.read(reinterpret_cast<char*>(vec.data()), size);
		_current_putpos = _current_putpos + seek + static_cast<std::streamoff>(size);

		if (_fs.fail() || _fs.bad())
			throw std::exception("Stream has invalid state");

		posp = _fs.tellp();
		posg = _fs.tellg();

		//set at
		for (size_t j = 5; j < 10; j++)
			vec[j] = (double)j;
		sizeoffset = static_cast<std::streamoff>(i * size);
		seek = sizeoffset - _current_putpos;
		if (seek != static_cast<std::streamoff>(0))
			_fs.seekp(seek, std::ios::cur);

		if (_fs.fail() || _fs.bad())
			throw std::exception("Stream has invalid state");

		posp = _fs.tellp();
		posg = _fs.tellg();
		
		_fs.write(reinterpret_cast<const char*>(vec.data()), size);
		_current_putpos = _current_putpos + seek + static_cast<std::streamoff>(size);

		if (_fs.fail() || _fs.bad())
			throw std::exception("Stream has invalid state");
	}


}
int main()
{
	test2();
    FArrayBase<StandartIndexMapper> ar;
    ar.SetObjectSize(ELEMENT_SIZE * sizeof(double));
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



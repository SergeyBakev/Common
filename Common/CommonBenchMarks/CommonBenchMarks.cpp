#include "benchmark/benchmark.h"
#include "..\Common\Arrays\ke_farray_base.h"
namespace BenchMarksFArray
{
	static constexpr size_t ITER = 1000000;
	static std::vector<double> vec;
	static FArrayBase<SparseIndexMapper> ar;
	static std::fstream _fs;

	void PushBackOneDoubleElementAtVector(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			double data = (double)state.range_x();
			benchmark::DoNotOptimize(data);
			vec.push_back(data);
		}
	}

	void WriteOneDoubleElementAtFile(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			double data = (double)state.range_x();
			benchmark::DoNotOptimize(data);
			_fs.write(reinterpret_cast<char*>(&data), sizeof(data));
		}
	}

	void AddOneDoubleElementAtFArray(benchmark::State& state)
	{	
		while (state.KeepRunning())
		{
			double data = (double)state.range_x();
			benchmark::DoNotOptimize(data);
			ar.Add(&data);
		}
	}

	void GetElementAtIndexFormVector(benchmark::State& state)
	{
		while (state.KeepRunning())
		{

		}
	}
	BENCHMARK(PushBackOneDoubleElementAtVector)->Arg(ITER);
	BENCHMARK(WriteOneDoubleElementAtFile)->Arg(ITER);
	BENCHMARK(AddOneDoubleElementAtFArray)->Arg(ITER);
}

int main(int argc, char** argv)
{

	using namespace BenchMarksFArray;

	std::wstring file_name;
	wchar_t buffer[_MAX_PATH];
	_wtmpnam_s(buffer, _countof(buffer));
	file_name = buffer;

	int mode = std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc;
	_fs.open(file_name, mode);
	if (!_fs.is_open())
		throw std::exception();
	std::vector<double> v;
	auto it = v.begin();
	auto el = *it;
	::benchmark::Initialize(&argc, argv);
	::benchmark::RunSpecifiedBenchmarks();
}
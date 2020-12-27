#include "benchmark/benchmark.h"
#include "..\Common\Arrays\ke_farray_base.h"
namespace BenchMarksFArray
{
	static constexpr size_t ITER = 100000;
	static std::vector<double> vec;
	static FArrayBase<SparseIndexMapper> ar;
	static std::fstream _fs;
	

	static size_t iter = 0;
	void PushBackOneDoubleElementAtVector(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			state.PauseTiming();
			double data = (double)state.iterations();
			iter++;
			state.ResumeTiming();
			
			benchmark::DoNotOptimize(data);
			vec.push_back(data);
		}
	}
	
	void SpeedIF(benchmark::State& state)
	{

		while (state.KeepRunning())
		{
		}
	}
	void WriteOneDoubleElementAtFile(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			state.PauseTiming();
			double data = (double)state.iterations();
			state.ResumeTiming();
			benchmark::DoNotOptimize(data);
			_fs.write(reinterpret_cast<char*>(&data), sizeof(data));
		}
	}

	void AddOneDoubleElementAtFArray(benchmark::State& state)
	{	
		while (state.KeepRunning())
		{
			state.PauseTiming();
			double data = (double)state.iterations();
			state.ResumeTiming();
			benchmark::DoNotOptimize(data);
			ar.Add(&data);
		}
	}

	void GetElementAtIndexFormVector(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			state.PauseTiming();
			size_t index = (size_t)state.iterations();
			state.ResumeTiming();

			auto& data = vec[index];
			benchmark::DoNotOptimize(data);
		}
	}
	
	static double data;
	void GetElementAtIndexFormStream(benchmark::State& state)
	{
		
		while (state.KeepRunning())
		{
			_fs.read((char*)&data, sizeof(data));
			benchmark::DoNotOptimize(data);
		}
	}


	void GetElementAtIndexFormFArray(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			ar.GetAt(state.range_x(), &data);
			benchmark::DoNotOptimize(data);
		}
	}

	void GetMoreThenOneElementFormArray(benchmark::State& state)
	{
		std::vector<double> v(10);
		size_t i = 0;
		while (state.KeepRunning())
		{
			ar.GetAt(state.range_x(), v.data(),10);
			benchmark::DoNotOptimize(v);
		}
	}
	BENCHMARK(PushBackOneDoubleElementAtVector)->Arg(ITER);
	//BENCHMARK(WriteOneDoubleElementAtFile)->Arg(ITER);
	BENCHMARK(AddOneDoubleElementAtFArray)->Arg(ITER);

	//BENCHMARK(GetElementAtIndexFormVector)->Arg(ITER);
	//BENCHMARK(GetElementAtIndexFormStream)->Arg(ITER);
	//BENCHMARK(GetElementAtIndexFormFArray)->Arg(ITER);

}

int main(int argc, char** argv)
{

	using namespace BenchMarksFArray;
	ar.SetObjectSize(sizeof(double));
	std::wstring file_name;
	wchar_t buffer[_MAX_PATH];
	_wtmpnam_s(buffer, _countof(buffer));
	file_name = buffer;

	int mode = std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc;
	_fs.open(file_name, mode);
	if (!_fs.is_open())
		throw std::exception();
	std::vector<double> v;


	::benchmark::Initialize(&argc, argv);
	::benchmark::RunSpecifiedBenchmarks();

	return 0;
}
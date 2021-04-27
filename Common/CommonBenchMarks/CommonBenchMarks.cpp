#include "benchmark/benchmark.h"
#include "..\Common\Arrays\ke_farray_base.h"
#include "..\Common\Algorithm\ke_algorithm.h"
#include "..\Common\FormatedString.h"

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
			ar.GetAt(state.range_x(), v.data(), 10);
			benchmark::DoNotOptimize(v);
		}
	}

	static std::wstring  test = L"1.3.3.4.6.7.8.9.0.1.345.3456.657";

	void SplitStl(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			auto ret = Common::algorithms::string::Split(L".", test);
			benchmark::DoNotOptimize(ret);
		}
	}

	void SplitBoost(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			auto ret = Common::algorithms::string::Split2(L".", test);
			benchmark::DoNotOptimize(ret);
		}
	}

	/*BENCHMARK(SplitStl)->Arg(ITER);
	BENCHMARK(SplitBoost)->Arg(ITER);*/

	//BENCHMARK(PushBackOneDoubleElementAtVector)->Arg(ITER);
	//BENCHMARK(WriteOneDoubleElementAtFile)->Arg(ITER);
	//BENCHMARK(AddOneDoubleElementAtFArray)->Arg(ITER);

	//BENCHMARK(GetElementAtIndexFormVector)->Arg(ITER);
	//BENCHMARK(GetElementAtIndexFormStream)->Arg(ITER);
	//BENCHMARK(GetElementAtIndexFormFArray)->Arg(ITER);

}

Common::FormattedStringV2 f1;
Common::FormattedStringV2 f2;
Common::FormattedStringV2 f3;
double toDouble(const std::wstring& v) { return std::stod(v); }

constexpr size_t ITER = 1000000;
namespace STLBenchMarks
{
	std::wstring str1 = L"545.387";
	std::wstring str2 = L"545.387";
	std::wstring str3 = L"545.387";
	std::wstring str4 = L"545.387";

	void Convert_String_To_Double_IStringStream_In_Consructor(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			std::wistringstream ss(str1);
			double ret;
			ss >> ret;
			benchmark::DoNotOptimize(ret);
			benchmark::DoNotOptimize(str1);
		}
	}

	void Convert_String_To_Double_IOStringStream_In_Consructor(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			std::wstringstream ss(str2);
			double ret;
			ss >> ret;
			benchmark::DoNotOptimize(ret);
			benchmark::DoNotOptimize(str2);
		}
	}

	void Convert_String_To_Double_IOStringStream_In_Stream_Operator(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			std::wstringstream ss;
			ss >> str3;
			double ret;
			ss >> ret;
			benchmark::DoNotOptimize(ret);
			benchmark::DoNotOptimize(str3);
		}
	}

	Common::FormattedStringV2::DefaultConverter<double> cnv;
	void Convert_String_To_Double_IOStringStream_In_Stream_Operator_In_Callable_Object(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			
			double ret = cnv(str4);
			benchmark::DoNotOptimize(ret);
		}
	}

	void Convert_String_To_Double_STOD(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			auto ret = std::stod(str4);
			benchmark::DoNotOptimize(ret);
			benchmark::DoNotOptimize(str4);
		}
	}

	void byStrv(std::wstring_view str)
	{
		benchmark::DoNotOptimize(str);
	}

	void bystrref(const std::wstring& str)
	{
		benchmark::DoNotOptimize(str);
	}

	void Pass_String_By_Const_Ref(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			byStrv(str1);
		}
	}

	void Pass_String_By_String_View(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			byStrv(str2);
		}
	}
	//BENCHMARK(Convert_String_To_Double_IStringStream_In_Consructor)->Arg(ITER);
	//BENCHMARK(Convert_String_To_Double_IOStringStream_In_Consructor)->Arg(ITER);
	BENCHMARK(Convert_String_To_Double_IOStringStream_In_Stream_Operator)->Arg(ITER);
	BENCHMARK(Convert_String_To_Double_IOStringStream_In_Stream_Operator_In_Callable_Object)->Arg(ITER);
	//BENCHMARK(Pass_String_By_String_View)->Arg(ITER);
	//BENCHMARK(Pass_String_By_Const_Ref)->Arg(ITER);
	//BENCHMARK(Convert_String_To_Double_STOD)->Arg(ITER);
}

namespace FormatedStringBanchMark
{
	void ConvertWithStream(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			auto ret = f1.GetParameter<int>(L"Tag");
			benchmark::DoNotOptimize(ret);
		}
	}

	void ConvertWithStod(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			auto ret = f1.GetParameter(L"Tag", toDouble);
			benchmark::DoNotOptimize(ret);
		}
	}

	void TryConvertWithStream(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			auto ret = f1.TryGetParameter<int>(L"Tag");
			benchmark::DoNotOptimize(ret);
		}
	}

	void TryConvertWithStod(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			auto ret = f1.TryGetParameter(L"Tag", toDouble);
			benchmark::DoNotOptimize(ret);
		}
	}

	void ConvertToString(benchmark::State& state)
	{
		while (state.KeepRunning())
		{
			std::wstring ret = f1.GetParameter(L"Tag");
			benchmark::DoNotOptimize(ret);
		}
	}

	//BENCHMARK(ConvertWithStream)->Arg(ITER);
	//BENCHMARK(ConvertWithStod)->Arg(ITER);
	//BENCHMARK(ConvertToString)->Arg(ITER);
}

int main(int argc, char** argv)
{
	f1.Str(L"Tag2 = 45;Tag3 = 45;Tag = 45;");
	f2.Str(L"Tag2 = 45;Tag3 = 45;Tag = 45;");
	f3.Str(L"Tag2 = 45;Tag3 = 45;Tag = 45;");
	//using namespace BenchMarksFArray;
	//ar.SetObjectSize(sizeof(double));
	//std::wstring file_name;
	//wchar_t buffer[_MAX_PATH];
	//_wtmpnam_s(buffer, _countof(buffer));
	//file_name = buffer;

	//int mode = std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc;
	//_fs.open(file_name, mode);
	//if (!_fs.is_open())
	//	throw std::exception();
	//std::vector<double> v;


	::benchmark::Initialize(&argc, argv);
	::benchmark::RunSpecifiedBenchmarks();

	return 0;
}
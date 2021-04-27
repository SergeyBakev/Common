#include <iostream>
#include <fstream>
#include <Windows.h>
#include <comutil.h>
#pragma comment(lib,"comsuppwd.lib")

#include "..\Common\Arrays\ke_farray_base.h"
#include "..\Common\Arrays\ke_typed_farray.h"
#include "..\Common\ke_type_traits.h"
#include "..\Common\Algorithm\ke_algorithm.h"
#include "..\Common\Algorithm\ke_string.h"
#include "..\Common\FormatedString.h"
#include "..\Common\Helpers\StringHelper.h"
#include "..\Common\Arrays\ke_array_indexer.h"
#include "..\Common\Logger\EventLogger\JournalLogger.h"
#include "..\Common\Logger\EventLogger\WinLogReaderV2.h"
#include "..\Common\Logger\EventLogger\WinLogFilter.h"


//#include "..\Common\Arrays\ke_farray_base.h"
//#include "..\Common\Process.h"
//#include <boost/process.hpp>
//#include <boost/asio.hpp>
using namespace std;
#include <variant>
//using namespace Common;
//using namespace boost;
//using namespace boost::process;
//
//void Test1()
//{
//    std::string exeName = "EchoConsole.exe";
//    std::string output;
//    std::string error;
//    std::string input = "hello";
//
//    asio::io_service ios;
//
//    std::vector<char> vOut(128 << 10);
//    auto outBuffer{ asio::buffer(vOut) };
//    process::async_pipe pipeOut(ios);
//
//    std::function<void(const system::error_code& ec, std::size_t n)> onStdOut;
//    onStdOut = [&](const system::error_code& ec, size_t n)
//    {
//        output.reserve(output.size() + n);
//        output.insert(output.end(), vOut.begin(), vOut.begin() + n);
//        if (!ec)
//        {
//            asio::async_read(pipeOut, outBuffer, onStdOut);
//        }
//    };
//
//    std::vector<char> vErr(128 << 10);
//    auto errBuffer{ asio::buffer(vErr) };
//    process::async_pipe pipeErr(ios);
//    std::function<void(const system::error_code& ec, std::size_t n)> onStdErr;
//    onStdErr = [&](const system::error_code& ec, size_t n)
//    {
//        error.reserve(error.size() + n);
//        error.insert(error.end(), vErr.begin(), vErr.begin() + n);
//        if (!ec)
//        {
//            asio::async_read(pipeErr, errBuffer, onStdErr);
//        }
//    };
//
//    auto inBuffer{ asio::buffer(input) };
//    process::async_pipe pipeIn(ios);
//
//    process::child c(
//        exeName + (" "),
//        process::std_out > pipeOut,
//        process::std_err > pipeErr,
//        process::std_in < pipeIn
//    );
//
//
//    asio::async_write(pipeIn, inBuffer,
//        [&](const system::error_code& ec, std::size_t n)
//        {
//            pipeIn.async_close();
//        });
//
//    asio::async_read(pipeOut, outBuffer, onStdOut);
//    asio::async_read(pipeErr, errBuffer, onStdErr);
//
//    ios.run();
//    c.wait();
//}

static inline std::wstring providerName = L"AxStream License Service v3.0";


int main()
{
	//arange
	std::shared_ptr<WinLogReaderV2> reader(new WinLogReaderV2(providerName, GetModuleHandle(nullptr)));
	//act
	WinLogFilter filter;
	filter.name = providerName;
	reader->Select(filter);
	auto records = reader->GetRecords();
	return 0;

}



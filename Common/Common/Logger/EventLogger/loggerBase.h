#include <sstream>
#include <fstream>
#include <filesystem>
#include <mutex>

namespace fs = std::filesystem;

enum class LogType
{
	eInfo = 1, //console
	eError, //file
	eDebug //VStudio debug output
};

//time flush in seconds
enum class FlushType : __int64
{
	None = 0,
//#ifdef _DEBUG
	OneSeconds = 1,
	TenSeconds = 10,
	OneMinute = 60,
//#endif // DEBUG
	OneHour = 3600,
	OneDay = 86400
};

std::wostream& operator<<(std::wostream&, const LogType&);

class LoggerBase :public std::wstreambuf
{
public:
	LoggerBase();
protected:
	virtual unsigned short overflow(std::char_traits<wchar_t>::int_type c) override;
	friend std::wostream& operator<<(std::wostream&, const LogType&);
	virtual int sync() override;
protected:

	virtual void OnSync() {};
	virtual int OnOverflow(std::char_traits<wchar_t>::int_type c) { return 0; };

protected:
	LoggerBase(std::wostream* infoStream, std::wostream* errorsStream);
	void Reinitialize(std::wostream* infoStream, std::wostream* errorsStream);

private:
	std::wostream* _infoStream = nullptr;
	std::wostream* _errorStream = nullptr;
	LogType  _priority = LogType::eInfo;

	//runtime data
	std::wstringstream _buffer;
	std::mutex _mutex;
};
#include "pch.h"
#include "loggerBase.h"
#include <Windows.h>
#include <debugapi.h>

//-------------------------------------------------------------------------
unsigned short LoggerBase::overflow(std::char_traits<wchar_t>::int_type c)
{
	if (c == traits_type::eof())
		return traits_type::eof();

	char_type ch = static_cast<char_type>(c);
	std::lock_guard lock(_mutex);
	_buffer << ch;
	OnOverflow(c);
	return c;
}
//-------------------------------------------------------------------------
LoggerBase::LoggerBase()
{
}
//-------------------------------------------------------------------------
int LoggerBase::sync()
{
	std::lock_guard lock(_mutex);
	if (_priority == LogType::eInfo)
	{
		*_infoStream << _buffer.str();
	}
	else if (_priority == LogType::eError)
	{
		*_errorStream << _buffer.str();
	}
	else if (_priority == LogType::eDebug)
	{
		OutputDebugString(_buffer.str().c_str());
	}

	_buffer.str({});
	_buffer.clear();

	OnSync();

	return 0;
}
//-------------------------------------------------------------------------
LoggerBase::LoggerBase(std::wostream* infoStream, std::wostream* errorsStream)
{
	Reinitialize(infoStream, errorsStream);
}
//-------------------------------------------------------------------------
void LoggerBase::Reinitialize(std::wostream* infoStream, std::wostream* errorsStream)
{
	_infoStream = infoStream;
	_errorStream = errorsStream;
}
//-------------------------------------------------------------------------
std::wostream& operator<<(std::wostream& os, const LogType& _buftype)
{
	LoggerBase* buf = dynamic_cast<LoggerBase*>(os.rdbuf());
	if (buf != nullptr)
		buf->_priority = _buftype;
	return os;
}

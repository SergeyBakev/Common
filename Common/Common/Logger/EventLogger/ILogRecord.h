#pragma once
#include <string>
#include <ctime>
#include <iostream>
#include <memory>

using  ILogRecordPtr = std::shared_ptr<class ILogRecord>;

class ILogRecord
{
public:
	virtual ~ILogRecord() = default;
	virtual int GetID() const = 0;
	virtual std::wstring GetProvider() const = 0;
	virtual std::wstring GetMassage() const = 0;
	virtual std::time_t GetTime() const = 0;
	virtual unsigned short GetType()  const = 0;
	virtual unsigned short GetCategory()  const = 0;

	virtual void Serialize(std::wostream& ar) const = 0;
	virtual bool Deserialize(std::wistream& ar) = 0;

};
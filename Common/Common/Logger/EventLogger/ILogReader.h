#pragma once
#include "ILogRecord.h"
#include "ILogFilter.h"
#include <vector>
#include <memory>
#include "log_record_type.h"
using LogRecordsArray = std::vector<ILogRecordPtr>;
using ILogReaderPtr = std::shared_ptr<class ILogReader>;

class ILogReader
{
public:
	virtual ~ILogReader() = default;
	virtual const LogRecordsArray& GetRecords() const = 0;
	virtual size_t CountRecords() const = 0;
	virtual bool HasRecord() const = 0;
	virtual LogRecordType GetLogRecordType() const = 0;

	virtual ILogReaderPtr Select(const ILogFilter& filter) = 0;
	virtual void ToStream(std::wostream& ar) = 0;

	virtual bool IsFiltred() const = 0;
};
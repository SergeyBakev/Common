#pragma once
#include "ILogRecord.h"
#include "log_record_type.h"

class LogFactory
{
public:
	static ILogRecordPtr MakeRecord(LogRecordType type);
};


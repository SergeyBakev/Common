#include "pch.h"
#include "LogFactory.h"
#include "WinEventRecord.h"

ILogRecordPtr LogFactory::MakeRecord(LogRecordType type)
{
	ILogRecordPtr record;
	switch (type)
	{
	case LogRecordType::WinLogRecord:
		record = std::make_shared<WinEventRecord>();
		break;
	default:
		break;
	}
	_ASSERT_EXPR(record != nullptr, L"LogFactory::MakeRecord undefined type");
	return record;
}

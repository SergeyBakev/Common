#pragma once
#include "ILogReader.h"
#include "WinEventRecord.h"
#include <Windows.h>
#include "..\..\Helpers\win_handle_ptr.h"

using ByteBuffer = std::vector<unsigned char>;

class WinLogReaderV2 : public ILogReader, public std::enable_shared_from_this<WinLogReaderV2>
{
public:
	WinLogReaderV2(std::wstring_view providerName, HMODULE msgModule);

	virtual const LogRecordsArray& GetRecords() const override;

	virtual size_t CountRecords() const override;

	virtual bool HasRecord() const override;

	virtual LogRecordType GetLogRecordType() const override;

	virtual ILogReaderPtr Select(const ILogFilter& filter) override;

	virtual void ToStream(std::wostream& ar) override;

	virtual bool IsFiltred() const override;


private:
	bool RenderEvents(HandlePtr hResults);
	bool RenderEvent(HandlePtr hResults);
private:
	LogRecordsArray records_;
	std::wstring provider_;
	HMODULE resDll_;
};
#pragma once
#include "ILogReader.h"
#include "WinEventRecord.h"
#include <Windows.h>

using ByteBuffer = std::vector<unsigned char>;

class WinLogReader : public ILogReader, public std::enable_shared_from_this<WinLogReader>
{
public:
	WinLogReader(std::wstring_view providerName, HMODULE msgModule);

	virtual const LogRecordsArray& GetRecords() const override;
	virtual size_t CountRecords() const override;
	virtual bool HasRecord() const override;
	virtual LogRecordType GetLogRecordType() const override;

	virtual ILogReaderPtr Select(const ILogFilter& filter) override;
	virtual void ToStream(std::wostream& ar) override;

	virtual bool IsFiltred() const override;

private:
	DWORD DumpRecordsInBuffer(PBYTE pBuffer, DWORD dwBytesRead, const ILogFilter& filter);
	bool TryRead(ByteBuffer& pBuffer, DWORD* dwBytesRead, DWORD* dwMinimumBytesNeeded);
	ILogRecordPtr ToRecord(PEVENTLOGRECORD pRecord, std::wstring name);

private:
	LogRecordsArray records_;
	std::wstring provider_;
	HMODULE resDll_;
	bool isFiltred_ = false;

};


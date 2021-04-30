#pragma once
#include "ILogReader.h"
#include "WinEventRecord.h"
#include <Windows.h>
#include <winevt.h>
#include "..\..\Helpers\win_handle_ptr.h"
//#define USE_ASYNC
#ifdef USE_ASYNC
#include <thread>
#include <future>

#endif // USE_ASYNC

using ByteBuffer = std::vector<unsigned char>;
class WinLogReaderV2 : public ILogReader, public std::enable_shared_from_this<WinLogReaderV2>
{
public:
	WinLogReaderV2(std::wstring_view providerName);
	~WinLogReaderV2();

	virtual const LogRecordsArray& GetRecords() const override;

	virtual size_t CountRecords() const override;

	virtual bool HasRecord() const override;

	virtual LogRecordType GetLogRecordType() const override;

	virtual ILogReaderPtr Select(const ILogFilter& filter) override;

	virtual void ToStream(std::wostream& ar) override;

	virtual bool IsFiltred() const override;
private:
	struct EventDeleter
	{
		void operator()(void* hEvent) const;
	};

private:
	bool RenderEvents(const HandlePtr& hResults, const ILogFilter& filter);
	ILogRecordPtr RenderEvent(const HandlePtr& hEvent);

	ILogRecordPtr ToRecord(const PEVT_VARIANT pRenderedValues, const HandlePtr& hEvent);
private:

#ifdef USE_ASYNC
	std::future<void> future_;
#endif // USE_ASYNC

	LogRecordsArray records_;
	bool isFiltred_ = false;
	std::wstring provider_;
	HMODULE resDll_;
};
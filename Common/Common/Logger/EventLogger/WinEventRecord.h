#include "ILogRecord.h"

class WinEventRecord : public ILogRecord
{
public:
	WinEventRecord() = default;

	WinEventRecord(unsigned short cat,
		unsigned short type,
		std::wstring_view provider,
		std::wstring_view message,
		std::time_t date,
		int eventId
	);

	virtual int GetID() const override;
	virtual std::wstring GetProvider() const override;
	virtual std::wstring GetMassage() const override;
	virtual std::time_t GetTime() const override;
	virtual unsigned short GetType() const override;
	virtual unsigned short GetCategory() const override;
	virtual void Serialize(std::wostream& ar) const override;
	virtual bool Deserialize(std::wistream& ar) override;

	
private:
	unsigned short category_ = -1;
	unsigned short type_ = -1;
	std::wstring provider_ = L"Undefined";
	std::wstring message_ = L"Undefined";
	std::time_t date_ = -1;
	int eventID_ = -1;
};


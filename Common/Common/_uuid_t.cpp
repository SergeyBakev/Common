#include "pch.h"
#include "_uuid_t.h"
#pragma comment(lib, "Rpcrt4.lib")

int UuidCompare(const UUID* a, const UUID* b)
{
	RPC_STATUS Status;

	int rc = ::UuidCompare((UUID*)a, (UUID*)b, &Status);

	return rc;
}

int UuidCompare(const UUID& a, const UUID& b)
{
	return ::UuidCompare(&a, &b);
}

bool UuidIsNil(
	const UUID& uuid
)
{
	RPC_STATUS Status;
	int rc = ::UuidIsNil((UUID*)&uuid, &Status);
	return rc != 0;
}


bool UuidIsNotNil(
	const UUID& uuid
)
{
	return !UuidIsNil(uuid);
}

bool UuidEqual(
	const UUID& a,
	const UUID& b
)
{
	RPC_STATUS Status;
	int rc = ::UuidEqual((UUID*)&a, (UUID*)&b, &Status);
	return rc != 0;
}

#ifdef _UNICODE
#define 	RPC_TSTR		RPC_WSTR
#else
#define 	RPC_TSTR		RPC_CSTR
#endif


std::wstring UuidToString(const UUID& uuid)
{
	RPC_TSTR StringUuid;
	::UuidToString((UUID*)&uuid, &StringUuid);
	std::wstring s = (LPCTSTR)StringUuid;

	::RpcStringFree(&StringUuid);
	return s;
}

const UUID _nil_uuid = { 0,0,0,0 };

bool CreateUuid(UUID& new_uuid)
{
	::UuidCreate((_GUID*)(&new_uuid));
	return true;
}

UUID UuidFromString(const char* sUUID)
{
	RPC_STATUS st;
	UUID id;
	st = UuidFromStringA((unsigned char*)sUUID, &id);
	if (st != RPC_S_OK)
	{
		memset(&id, 0, sizeof(id));
	}
	return id;
}

UUID UuidFromString(const wchar_t* sUUID)
{
	RPC_STATUS st;
	UUID id;
	st = UuidFromStringW((RPC_WSTR)sUUID, &id);
	if (st != RPC_S_OK)
	{
		memset(&id, 0, sizeof(id));
	}
	return id;
}

//////////////////////////////////////////////////////////////////////////
// _uuid_t

_uuid_t::_uuid_t()
{
	*this = _nil_uuid; //CreateUuid(*this);
}

_uuid_t::_uuid_t(
	unsigned long  data1,
	unsigned short data2,
	unsigned short data3,
	unsigned char  data4_0,
	unsigned char  data4_1,
	unsigned char  data4_2,
	unsigned char  data4_3,
	unsigned char  data4_4,
	unsigned char  data4_5,
	unsigned char  data4_6,
	unsigned char  data4_7
)
{
	Data1 = data1;
	Data2 = data2;
	Data3 = data3;
	Data4[0] = data4_0;
	Data4[1] = data4_1;
	Data4[2] = data4_2;
	Data4[3] = data4_3;
	Data4[4] = data4_4;
	Data4[5] = data4_5;
	Data4[6] = data4_6;
	Data4[7] = data4_7;
}

/*
_uuid_t::_uuid_t(int null)
{
	ASSERT(!null);
	*this = _nil_uuid;
}
*/
/*
_uuid_t::_uuid_t(_bstr_t bstr)
{
	*this = bstr;
}

_uuid_t::_uuid_t(LPCTSTR str)
{
	*this = str;
}
*/

_uuid_t::_uuid_t(const GUID& id)
{
	if (this != &id)
		*this = id;
}

_uuid_t::_uuid_t(const _uuid_t& id)
{
	if (this != &id)
		*this = id;
}

_uuid_t _uuid_t::New()
{
	UUID id;
	::CreateUuid(id);
	return _uuid_t(id);
}
// operators
_uuid_t::operator std::wstring()const
{
	if (*this == _nil_uuid)
	{
		return L"";
	}
	std::wstring bstr = ::UuidToString(*this);
	return bstr;
}

_uuid_t::operator GUID& ()
{
	return *this;
}

_uuid_t::operator GUID() const
{
	return *this;
}

bool _uuid_t::IsNil() const
{
	return ::UuidIsNil(*this);
}


GUID& _uuid_t::operator=(std::wstring bstr)
{
	*this = ::UuidFromString(bstr.c_str());

	return *this;
}
GUID& _uuid_t::operator=(LPCTSTR str)
{
	GUID id = UuidFromString((const char*)str);
	return *this = id;
}

GUID& _uuid_t::operator=(const UUID& id)
{
	return (*(UUID*)this) = id;
}

bool _uuid_t::operator ==(int null) const
{
	return ::UuidIsNil(*this);
}

bool _uuid_t::operator!=(int null) const
{
	return ::UuidIsNotNil(*this);
}

bool _uuid_t::operator ==(const UUID& id) const
{
	return ::UuidEqual(*this, id);
}

bool _uuid_t::operator !=(const UUID& id) const
{
	return !::UuidEqual(*this, id);
}

_uuid_t _uuid_t::Create(std::wstring bstr)
{
	_uuid_t id = ::UuidFromString(bstr.c_str());
	return id;
}

_uuid_t _uuid_t::Nil()
{
	UUID id = { 0, 0, 0, {0,0,0,0,0,0,0,0} };
	return id;
}

_uuid_t _uuid_t::Create(LPCTSTR str)
{
	_uuid_t id = ::UuidFromString((LPCTSTR)str);
	return id;
}

std::wstring _uuid_t::ToString() const
{
	auto str = ::UuidToString(*this);
	return str;
}


size_t _uuid_t::SizeOf() const
{
	return sizeof(UUID);
}

bool _uuid_t::operator <(const _uuid_t& id) const
{
	if (Data1 > id.Data1)
		return false;
	if (Data1 < id.Data1)
		return true;

	if (Data2 > id.Data2)
		return false;
	if (Data2 < id.Data2)
		return true;

	if (Data3 > id.Data3)
		return false;
	if (Data3 < id.Data3)
		return true;

	for (int i = 0; i < 8; i++)
	{
		if (Data4[i] > id.Data4[i])
			return false;
		if (Data4[i] < id.Data4[i])
			return true;
	}
	return false;
}

bool _uuid_t::operator <=(const _uuid_t& id) const
{
	if (Data1 > id.Data1)
		return false;
	if (Data1 < id.Data1)
		return true;

	if (Data2 > id.Data2)
		return false;
	if (Data2 < id.Data2)
		return true;

	if (Data3 > id.Data3)
		return false;
	if (Data3 < id.Data3)
		return true;

	for (int i = 0; i < 8; i++)
	{
		if (Data4[i] > id.Data4[i])
			return false;
		if (Data4[i] < id.Data4[i])
			return true;
	}
	return true;
}
bool _uuid_t::operator >(const _uuid_t& id) const
{
	if (Data1 > id.Data1)
		return true;
	if (Data1 < id.Data1)
		return false;

	if (Data2 > id.Data2)
		return true;
	if (Data2 < id.Data2)
		return false;

	if (Data3 > id.Data3)
		return true;
	if (Data3 < id.Data3)
		return false;

	for (int i = 0; i < 8; i++)
	{
		if (Data4[i] > id.Data4[i])
			return true;
		if (Data4[i] < id.Data4[i])
			return false;
	}
	return false;
}

bool _uuid_t::operator >=(const _uuid_t& id) const
{
	if (Data1 > id.Data1)
		return true;
	if (Data1 < id.Data1)
		return false;

	if (Data2 > id.Data2)
		return true;
	if (Data2 < id.Data2)
		return false;

	if (Data3 > id.Data3)
		return true;
	if (Data3 < id.Data3)
		return false;

	for (int i = 0; i < 8; i++)
	{
		if (Data4[i] > id.Data4[i])
			return true;
		if (Data4[i] < id.Data4[i])
			return false;
	}
	return true;
}
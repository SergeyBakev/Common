#pragma once
#include <rpc.h>
#include <string>
// UUID is a 16 byte universally unique identifier
//#if defined(UUID_DEFINED)
//typedef UUID UUID;
//#elif defined(GUID_DEFINED)
//typedef GUID UUID;
//#else
/*
#define UUID_DECLARED_AS_CLASS
// For uuids, it is critical that the DataN fields have
// exactly the sizes specified below.  For that reason,
// the ON__UINTnn typedefs are used.
class CLASS UUID
{
public:
  UINT32     Data1;    // 32 bit unsigned integer
  UINT       Data2;    // 16 bit unsigned integer
  UINT       Data3;    // 16 bit unsigned integer
  unsigned char  Data4[8];

  bool operator==(const UUID& other) const;
  bool operator!=(const UUID& other) const;
};

//#endif*/

typedef GUID UUID;


// All bits are zero in _nil_uuid and
// UUIDCompare( _nil_uuid, U ) < 0 if U != _nil_uuid.
extern const UUID _nil_uuid;

// All bits are one in _max_uuid and
// UUIDCompare( U, _max_uuid ) < 0 if U != _max_uuid.
extern const UUID _max_uuid;

/*
Description:
  Creates a new uuid.(&a,&b) compares two uuids.
Parameters:
  new_uuid - [out]
Returns:
  True if successful.
Remarks:
  Only works on Windows.
*/
bool CreateUuid(UUID& uuid);


#define INT2ID(x) _uuid_t((x), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#define ID2INT(id) (id).Data1

/*
Description:
  This class is used by UUIDIndexList.  It is used when
  uuids are used to search for items that can be found by
  an integer index.
*/
class _uuid_t : public GUID
{
public:
	_uuid_t();
	//	_uuid_t(int null);
	// 	_uuid_t(_bstr_t bstr);
	// 	_uuid_t(LPCTSTR str);
	_uuid_t(
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
	);

	_uuid_t(const GUID& id);
	_uuid_t(const _uuid_t& id);
	static _uuid_t New();
	static _uuid_t Create(LPCTSTR str);
	static _uuid_t Create(std::wstring bstr);
	static _uuid_t Nil();
	std::wstring ToString() const;
	size_t SizeOf()const;
	bool IsNil()const;


	operator std::wstring() const;
	operator GUID& ();
	operator GUID() const;
	//	operator bool() const;

	GUID& operator=(std::wstring bstr);
	GUID& operator=(LPCTSTR str);
	GUID& operator=(const UUID& id);

	bool operator ==(int null) const;
	/*bool operator == (const _uuid_t& other) const;*/
	bool operator!=(int null) const;

	bool operator ==(const UUID& id) const;
	bool operator !=(const UUID& id) const;

	bool operator <(const _uuid_t& id) const;
	bool operator <=(const _uuid_t& id) const;
	bool operator >(const _uuid_t& id) const;
	bool operator >=(const _uuid_t& id) const;

};


/*
Description:
  UUIDCompare(&a,&b) compares two uuids.
Parameters:
  a - [in]
  b - [in]
Returns:
  @untitled table
  -1    a < b
   0    a == b
  +1    a > b
Remarks:
  A NULL pointer is considered < a non-NULL pointer.
*/
int UuidCompare(
	const UUID* a,
	const UUID* b
);

/*
Description:
  UUIDCompare(a,b) compares two uuids.
Parameters:
  a - [in]
  b - [in]
Returns:
  @untitled table
  -1    a < b
   0    a == b
  +1    a > b
*/
int UuidCompare(
	const UUID& a,
	const UUID& b
);

/*
Description:
  Test uuid to see if it is nil (identically zero).
Parameters:
  uuid - [in]
Returns:
  true if uuid is nil.
*/
bool UuidIsNil(
	const UUID& uuid
);

/*
Description:
  Test uuid to see if it is not nil (not identically zero).
Parameters:
  uuid - [in]
Returns:
  true if uuid is not nil (non zero)
*/
bool UuidIsNotNil(
	const UUID& uuid
);

bool UuidEqual(
	const UUID& a,
	const UUID& b
);

/*
Description:
  Converts a string like
	"{85A08515-f383-11d3-BFE7-0010830122F0}"
  into a uuid.
  The brackets are optional and are ignored.
  Hyphens can appear anywhere or be missing.
  The hex digits can be upper or lower case.
Parameters:
  s - [in]
Returns:
  uuid.
  If the string is not a uuid, then _nil_uuid is returnd.
*/
UUID UuidFromString(const char* s);

/*
Description:
  Converts a string like
	"{85A08515-f383-11d3-BFE7-0010830122F0}"
  into a uuid.
  The brackets are optional and are ignored.
  Hyphens can appear anywhere or be missing.
  The hex digits can be upper or lower case.
Parameters:
  s - [in]
Returns:
  uuid.
  If the string is not a uuid, then _nil_uuid is returnd.
*/
UUID UuidFromString(LPCTSTR s);

/*
Description:
  Converts a uuid to a null termintated UNICODE string like
	 "85a08515-f383-11d3-bfe7-0010830122f0".
Parameters:
  uuid - [in]
  s - [out]  The s[] wchar_t array must have length >= 37.
			 The returned char array will have a 36
			 character uuid in s[0..35] and a null in s[36].
Returns:
  The pointer to the array is returned.
*/
std::wstring	 UuidToString(const UUID& uuid);

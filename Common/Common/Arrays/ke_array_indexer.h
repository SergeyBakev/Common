#pragma once
#include <map>

template <typename InnerIndexType, typename ExternalIndexType, class U>
class IndexerBase
{
public:
	using external_idx_type = ExternalIndexType;
	using inner_idx_type = InnerIndexType;

	inline static const inner_idx_type npos = static_cast<InnerIndexType>(-1);
	inline static const external_idx_type eof = static_cast<ExternalIndexType>(-1);
	inline static  const external_idx_type StartIndex = 0;

public:

	bool IsEmpty() const
	{
		return This()->GetCount() == 0;
	}

	external_idx_type CreateNewIndex() const
	{
		auto index = This()->GetLast();
		return index == npos ? StartIndex : ++index;
	}
#ifdef MFC

	void Serialize(CArchive& ar)
	{
		uint64_t size = inner_index_to_external.size();
		ar << size;
		for (auto& [ext_idx, in_idx] : inner_index_to_external)
		{
			uint64_t idx1 = static_cast<uint64_t>(ext_idx);
			uint64_t idx2 = static_cast<uint64_t>(in_idx);
			ar << idx1;
			ar << idx2;
		}
	}

	void Deserialize(CArchive& ar)
	{
		uint64_t size;
		ar >> size;

		while (size)
		{
			uint64_t idx1;
			uint64_t idx2;
			ar >> idx1 >> idx2;


			inner_index_to_external.insert({ static_cast<int>(idx1), static_cast<int>(idx2) });
			size--;
		}
	}

#endif
private:
	auto This() const
	{
		return static_cast<const U*>(this);
	}

	auto This()
	{
		return static_cast<U*>(this);
	}

private:
	template <class T, class U, class Z> class This;
};

class StandartIndexMapper : public IndexerBase<size_t, size_t, StandartIndexMapper>
{
public:

	inner_idx_type GetRealIndex(external_idx_type external_idx) const
	{
		inner_idx_type pos = static_cast<inner_idx_type>(external_idx);
		return pos == _ammount ? npos : pos;
	}

	void Insert(external_idx_type external_idx)
	{
		_ASSERT(external_idx == _ammount);
		_ammount = external_idx + 1;
	}

	external_idx_type GetFirst() const
	{
		return StartIndex;
	}

	external_idx_type GetLast() const
	{
		return _ammount == StartIndex ? StartIndex : _ammount - 1;
	}

	external_idx_type GetNext(external_idx_type external_idx)
	{
		external_idx_type ammount = static_cast<external_idx_type>(_ammount);

		if (external_idx < ammount)
			external_idx++;

		if (external_idx == ammount)
		{
			//Ситуация не правильная, но может и возможна
			_ASSERT(0);
			return eof;
		}

		return external_idx;
	}

	external_idx_type GetCount() const
	{
		return _ammount;
	}

private:
	size_t _ammount = 0;

};

class SparseIndexMapper : public IndexerBase<size_t, size_t, SparseIndexMapper>
{
public:

	inner_idx_type GetRealIndex(external_idx_type external_idx) const
	{
		auto it = inner_index_to_external.find(external_idx);
		if (it != std::end(inner_index_to_external))
		{
			return it->second;
		}
		return npos;
	}

	void Insert(external_idx_type external_idx)
	{
		inner_index_to_external.insert(std::make_pair(external_idx, GetCount()));
	}

	external_idx_type GetFirst() const
	{
		if (IsEmpty())
			return StartIndex;

		return inner_index_to_external.begin()->first;
	}

	external_idx_type GetLast() const
	{
		if (IsEmpty())
			return npos;

		return inner_index_to_external.rbegin()->first;
	}

	external_idx_type GetNext(external_idx_type external_idx) const
	{
		auto it = inner_index_to_external.find(external_idx);
		if (++it != std::end(inner_index_to_external))
			return (it)->first;

		return npos;
	}

	size_t GetCount() const noexcept
	{
		return inner_index_to_external.size();
	}

	void Clear()
	{
		inner_index_to_external.clear();
	}

	void Serialize(std::ofstream& storage) const
	{
		size_t size = inner_index_to_external.size();
		storage.write(reinterpret_cast<const char*>(&size), sizeof(size));
		for(auto& [in, ext] : inner_index_to_external)
		{
			storage.write(reinterpret_cast<const char*>(&in), sizeof(in));
			storage.write(reinterpret_cast<const char*>(&ext), sizeof(ext));
		}	
	}

	void Deserialize(std::ifstream& storage)
	{
		inner_index_to_external.clear();
		size_t size = 0;
		size_t in;
		size_t ext;
		storage.read(reinterpret_cast<char*>(&size), sizeof(size));
		for (size_t i = 0; i < size; i++)
		{
#ifdef _DEBUG
			if (storage.fail())
				throw std::exception("Stream has invalid state");
#endif // _DEBUG

			storage.read(reinterpret_cast<char*>(&in), sizeof(in));
			storage.read(reinterpret_cast<char*>(&ext), sizeof(ext));
			inner_index_to_external.insert({ in,ext });
		}
	}
private:
	std::map<external_idx_type, inner_idx_type> inner_index_to_external;
};
#pragma once
#include <map>

template <typename InnerIndexType, typename ExternalIndexType, class U>
class IndexerBase
{
public:
	using external_idx_type = ExternalIndexType;
	using inner_idx_type = InnerIndexType;

	inline static constexpr inner_idx_type npos = static_cast<InnerIndexType>(-1);
	inline static  constexpr external_idx_type DefaultStartIndex = 0;	
};

class StandartIndexMapper : public IndexerBase<size_t, size_t, StandartIndexMapper>
{
public:

	inner_idx_type RealIndex(external_idx_type external_idx) const
	{
#ifdef _DEBUG
		_ASSERT(external_idx != npos);
		_ASSERT(external_idx <= _ammount);
#endif // _DEBUG
		if (external_idx == npos || external_idx >= Count() || external_idx < 0)
			return npos;

		return static_cast<inner_idx_type>(external_idx);

	}

	void Add(external_idx_type external_idx)
	{
#ifdef _DEBUG
		_ASSERT(external_idx != npos);
		_ASSERT((_ammount + 1) == external_idx);
#endif // _DEBUG
		_ammount = external_idx;
	}

	external_idx_type First() const
	{
		return IsEmpty() ? npos : DefaultStartIndex;
	}

	external_idx_type Last() const
	{
		return  _ammount;
	}

	external_idx_type Next(external_idx_type external_idx)
	{
#ifdef _DEBUG
		_ASSERT((external_idx) != npos);
		_ASSERT((external_idx + 1 ) != Count());
#endif // _DEBUG
		return IsEmpty() || external_idx + 1 == Count() ? npos : external_idx + 1;
	}

	external_idx_type Prev(external_idx_type external_idx)
	{
#ifdef _DEBUG
		_ASSERT((external_idx - 1) != npos);
		_ASSERT((external_idx -1 ) != npos);
#endif // _DEBUG
		return IsEmpty() ? npos : external_idx - 1;
	}

	external_idx_type Count() const
	{
		return _ammount + 1;
	}

	external_idx_type IsEmpty() const
	{
		return _ammount == npos;
	}

	external_idx_type CreateIndex()
	{
		return _ammount + 1;
	}

private:
	size_t _ammount = npos;

};

class SparseIndexMapper : public IndexerBase<size_t, size_t, SparseIndexMapper>
{
public:

	inner_idx_type RealIndex(external_idx_type external_idx) const
	{
		auto it = inner_index_to_external.find(external_idx);
		if (it != std::end(inner_index_to_external))
		{
			return it->second;
		}
		return npos;
	}

	void Add(external_idx_type external_idx)
	{
		inner_index_to_external.insert(std::make_pair(external_idx, Count()));
	}

	external_idx_type First() const
	{
		if (Count() == 0)
			return npos;

		return inner_index_to_external.begin()->first;
	}

	external_idx_type CreateIndex() const
	{
		auto index = Last();
		return index == npos ? DefaultStartIndex : ++index;
	}

	external_idx_type Last() const
	{
		if (Count() == 0)
			return npos;

		return inner_index_to_external.rbegin()->first;
	}

	external_idx_type Next(external_idx_type external_idx) const
	{
		auto it = inner_index_to_external.find(external_idx);
		if (++it != std::end(inner_index_to_external))
			return (it)->first;

		return npos;
	}

	size_t Count() const noexcept
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
#pragma once
#include "IRepository.h"
#include <stdexcept>


template <class Model>
class VectorRepository : public IRepository<Model>
{
public:
    using Base = IRepository<Model>;
    using ModelOpt = Base::ModelOpt;
    using Action = Base::Action;
    using Predicate = Base::Predicate;
    using ModelComparer = std::function<bool(const Model&, const Model&)>;
    using Storage = std::vector<Model>;


    VectorRepository(Storage& repository) : storage_(repository), comparer_(DefaultPredicate())
    {

    }

    VectorRepository(Storage& repository, ModelComparer comparer) : storage_(repository), comparer_(comparer)
    {

    }

    virtual bool IsEmpty() const override
    {
        return storage_.empty();
    }

    virtual ModelOpt Get(const Model& model) const override
    {
        auto it = Get_(BindComparerByFirstParameter(model));
        if (it == std::end(storage_))
            return std::nullopt;

        return *it;
    }

    virtual bool Find(Predicate predicate) const
    {
        auto it = Get_(predicate);
        return it != std::end(storage_);
    }

    void DeleteAll()
    {
        storage_.clear();
    }

    virtual size_t Count() const override
    {
        return storage_.size();
    }

    virtual ModelOpt Get(Predicate pred) const override
    {
        _ASSERT(pred != nullptr);
        auto& this_ = const_cast<VectorRepository&>(*this);
        ModelOpt model;
        auto it = this_.Get_(pred);
        if (it != std::end(storage_))
            model = *it;

        return model;
    }

    virtual bool Create(const Model& model) override
    {
        bool rc = false;
        _ASSERT(!model.ID.IsNil());
        auto it = Get_(BindComparerByFirstParameter(model));
        if (it == std::end(storage_))
        {
            rc = true;
            storage_.push_back(model);
        }
        return rc;
    }

    virtual bool Create(Model&& model) override
    {
        bool rc = false;
        auto it = Get_(BindComparerByFirstParameter(model));
        if (it == std::end(storage_))
        {
            rc = true;
            storage_.push_back(std::move(model));
        }
        return rc;
    }

    virtual void CreateOrUpdate(const Model& model) override
    {
        auto it = Get_(BindComparerByFirstParameter(model));
        if (it != std::end(storage_))
        {
            *it = model;
            return;
        }
        Create(model);
    }


    virtual void CreateOrUpdate(Model&& model) override
    {
        auto it = Get_(BindComparerByFirstParameter(model));
        if (it != std::end(storage_))
        {
            *it = std::move(model);
            return;
        }
        Create(model);
    }

    virtual bool Delete(const Model& model) override
    {
        return Delete(BindComparerByFirstParameter(model));
    }

    virtual bool Update(const Model& model)
    {
        return DoUpdate(model, BindComparerByFirstParameter(model));
    }

    virtual void ForEach(Action action) override
    {
        for (auto& model : storage_)
        {
            if (action != nullptr)
                action(model);
        }
    }

    virtual void ForModel(const Model& model, Action action) override
    {
        _ASSERT(action != nullptr);
        auto it = Get_(BindComparerByFirstParameter(model));
        if (it != std::end(storage_))
            action(*it);
    }

    virtual bool Delete(Predicate pred) override
    {
        bool rc = false;
        auto it = Get_(pred);
        if (it != std::end(storage_))
        {
            rc = true;
            storage_.erase(it);
        }
        return rc;
    }

    void UpdateStorage(const Storage& storage)
    {
        storage_ = storage;
    }

    void UpdateStorage(Storage&& storage)
    {
        storage_ = std::move(storage);
    }

    Storage& GetStorage()
    {
        return storage_;
    }

    Storage& GetStorage() const
    {
        return storage_;
    }
private:

    bool DoUpdate(const Model& model, const Predicate& comparer)
    {
        auto it = Get_(comparer);

        if (it == std::end(storage_))
            return false;

        *it = model;
        return true;
    }


    template <class T>
    typename Storage::iterator Get_(const T& predicate) const
    {
        return std::find_if(std::begin(storage_), std::end(storage_), predicate);
    }

private:

    struct DefaultPredicate
    {
        bool operator()(const Model& model, const Model& other) const
        {
            return model.ID == other.ID;
        }
    };

    auto BindComparerByFirstParameter(const Model& model) const
    {
        return std::bind(comparer_,model, std::placeholders::_1);
    }

    ModelComparer comparer_;
    Storage& storage_;
};


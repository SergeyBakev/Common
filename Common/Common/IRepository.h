#pragma once
#include <functional>
#include <optional>
#include <memory>
#include "_uuid_t.h"

template <class Model>
class IRepository
{
public:
    using Action = std::function<void(Model&)>;
    using Predicate = std::function<bool(const Model&)>;
    using ModelOpt = std::optional<Model>;

    virtual ~IRepository() = default;
    virtual ModelOpt Get(const Model& id) const = 0;
    virtual ModelOpt Get(Predicate equalityComparer) const = 0;

    virtual bool IsEmpty() const = 0;
    virtual size_t Count() const = 0;


    virtual bool Create(const Model& model) = 0;
    virtual bool Create(Model&& model) = 0;

    virtual void CreateOrUpdate(const Model& model) = 0;
    virtual void CreateOrUpdate(Model&& model) = 0;

    virtual  bool Update(const Model& model) = 0;

    virtual  bool Delete(const Model& model) = 0;
    virtual  bool Delete(Predicate pred) = 0;

    virtual void ForEach(Action action) = 0;
    virtual void ForModel(const Model& model, Action action) = 0;
};

template <class T>
using IRepositoryPtr = std::shared_ptr<IRepository<T>>;
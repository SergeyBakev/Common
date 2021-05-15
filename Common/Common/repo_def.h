#pragma once
#define DECLARE_PTR_(Class) using Class##Ptr = std::shared_ptr<Class>
#define DECLARE_PTR(Class) DECLARE_PTR_(Class)

#define DECLARE_PREFIX_PTR_(Class,Prefix) using Prefix##Class##Ptr = std::shared_ptr<Class>
#define DECLARE_PREFIX_PTR(Class,Prefix) DECLARE_PREFIX_PTR_(Class,Prefix)

#define DECLARE_CTOR_REPOSITORY_(CLASS,BASE)\
CLASS(BASE::Storage& storage) : BASE(storage){}

#define DECLARE_CTOR_REPOSITORY(CLASS,BASE) DECLARE_CTOR_REPOSITORY_(CLASS,BASE)

#define DECLARE_VECTOR_REPOSITORY(CLASS,Model)\
class CLASS : public VectorRepository<Model>{\
public:\
using Base = VectorRepository<Model>;\
DECLARE_CTOR_REPOSITORY(CLASS,Base)\
};\
DECLARE_PTR(CLASS);\
DECLARE_PREFIX_PTR_(CLASS,I)
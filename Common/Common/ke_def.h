#pragma once
#define INIT_STATIC_(CLASS, Field) decltype(CLASS::Field) CLASS::Field{}
#define INIT_STATIC(CLASS, Field) INIT_STATIC_(CLASS,Field)

#define INIT_STATIC_FIELD(CLASS, Field,Value) INIT_STATIC(CLASS,Field){}
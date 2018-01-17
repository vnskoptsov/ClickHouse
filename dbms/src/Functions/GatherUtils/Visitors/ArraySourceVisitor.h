#pragma once
#include <Common/Visitor.h>
#include <Core/TypeListNumber.h>

namespace DB::GatherUtils
{

using NumericArraySources = typename TypeListMap<NumericArraySource, TypeListNumbers>::Type;
using BasicArraySources = typename AppendToTypeList<GenericArraySource, NumericArraySources>::Type;
using NullableArraySources = typename TypeListMap<NullableArraySource, BasicArraySources>::Type;
using BasicAndNullableArraySources = typename TypeListConcat<BasicArraySources, NullableArraySources>::Type;
using ConstArraySources = typename TypeListMap<ConstSource, BasicAndNullableArraySources>::Type;
using TypeListArraySources = typename TypeListConcat<BasicAndNullableArraySources, TypeListArraySources>::Type;

class ArraySourceVisitor : public ApplyTypeListForClass<Visitor, TypeListArraySources>::Type {};

template <class Derived>
class ArraySourceVisitorImpl : public VisitorImpl<Derived, ArraySourceVisitor>
{
};

}

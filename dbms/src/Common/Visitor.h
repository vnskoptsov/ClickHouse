#pragma once

#include <Common/demangle.h>
#include <Common/TypeList.h>

#include <string>

namespace DB
{

template <typename ... Types>
class Visitor;

template <>
class Visitor<>
{
public:
    using List = TypeList<>;

    virtual ~Visitor() = default;
};

template <typename Type>
class Visitor<Type> : public Visitor<>
{
public:
    using List = TypeList<Type>;

    virtual void visit(const Type &) = 0;

    virtual void visit(Type &) = 0;
};

template <typename Type, typename ... Types>
class Visitor<Type, Types ...> : public Visitor<Types ...>
{
public:
    using List = TypeList<Type, Types ...>;
    using Visitor<Types ...>::visit;

    virtual void visit(const Type &) = 0;

    virtual void visit(Type &) = 0;
};

template <typename Derived, typename VisitorBase, typename ... Types>
class VisitorImplHelper;

template <typename Derived, typename VisitorBase>
class VisitorImplHelper<Derived, VisitorBase> : public VisitorBase
{
};

template <typename Derived, typename VisitorBase, typename Type, typename ... Types>
class VisitorImplHelper<Derived, VisitorBase, Type, Types ...>
        : public VisitorImplHelper<Derived, VisitorBase, Types ...>
{
public:
    void visit(const Type & value) override { static_cast<Derived *>(this)->visitImpl(value); }

    void visit(Type & value) override { static_cast<Derived *>(this)->visitImpl(value); }

protected:
    template <typename T>
    void visitImpl(const Type &)
    {
        throw Exception(std::string("visitImpl(const ") + demangle(typeid(T).name()) + " &)"
                        + " is not implemented for class" + demangle(typeid(Derived).name()));
    }

    template <typename T>
    void visitImpl(Type &)
    {
        throw Exception(std::string("visitImpl(") + demangle(typeid(T).name()) + " &)"
                        + " is not implemented for class" + demangle(typeid(Derived).name()));
    }
};

template <typename Derived, typename VisitorBase>
class VisitorImpl :
        ApplyTypeListForClass<
                VisitorImplHelper,
                TypeListConcat<
                        TypeList<Derived, VisitorBase>,
                        typename VisitorBase::List
                >::Type
        >::Type
{
};

template <typename Derived, typename Base, typename Visitor>
class Visitable : public Base
{
public:
    void accept(Visitor & visitor) override { visitor.visit(*static_cast<Derived *>(this)); }
    void accept(Visitor & visitor) const override { visitor.visit(*static_cast<const Derived *>(this)); }
};

}

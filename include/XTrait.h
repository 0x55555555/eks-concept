#pragma once
#include "Utilities/XTightPointer.h"
#include "Utilities/XPrivateImpl.h"
#include "Utilities/XMacroHelpers.h"

namespace Eks
{

namespace Trait
{

namespace detail
{

#define X_TRAIT_IMPL protected: \
  Derived* derived() { return static_cast<Derived*>(this); } \
  const Derived* derived() const { return static_cast<const Derived*>(this); } \
  detail::PrivateData *data() { return derived()->data(); } \
  const detail::PrivateData *data() const { return derived()->data(); } \
  const Trait::Functions *functions() const { return derived()->functions(); }


#define X_TRAIT_FUNCTION_IMPL_FN(variable, name, n) name.variable = Impl::template variable<Type>;
#define X_TRAIT_FUNCTION_IMPL(name, ...) \
template <typename Type, typename Impl> void init() { \
  X_EXPAND_ARGS(X_TRAIT_FUNCTION_IMPL_FN, name, __VA_ARGS__) }

typedef PrivateImpl<sizeof(void*)*4> PrivateData;

template <typename Derived, typename Trait1, typename... Traits>
  class BaseObject
    : public Trait1::template Trait<Derived>,
      public BaseObject<Derived, Traits...>
  {
public:
  struct Functions
      : public Trait1::template Trait<Derived>::Functions,
        public BaseObject<Derived, Traits...>::Functions
    {
    template <typename Type, typename Trait1Impl, typename... TraitsImpl> void init()
      {
      Trait1::template Trait<Derived>::Functions::template init<Type, Trait1Impl>();

      typedef BaseObject<Derived, Traits...> Base;
      Base::Functions::template init<Type, TraitsImpl...>();
      }
    };

  };

template <typename Derived, typename Trait>
  class BaseObject<Derived, Trait>
    : public Trait::template Trait<Derived>
  {
public:
  struct Functions
      : public Trait::template Trait<Derived>::Functions
    {
    template <typename Type, typename TraitImpl> void init()
      {
      Trait::template Trait<Derived>::Functions::template init<Type, TraitImpl>();
      }
    };

  };

// A wrapper for returning const object memory for casting.
class ConstObjectWrapper
  {
public:
  ConstObjectWrapper(const void *mem) : _mem(const_cast<void *>(mem)) { }
  template <typename T>const T *as() const
    {
    return static_cast<T *>(_mem);
    }

protected:
  void *_mem;
  };

// A wrapper for returning object memory for casting.
class ObjectWrapper : public ConstObjectWrapper
  {
public:
  ObjectWrapper(void *mem) : ConstObjectWrapper(mem) { }
  template <typename T>T *as() const
    {
    return static_cast<T *>(ConstObjectWrapper::_mem);
    }
  };
}

template <typename Holder, typename... Traits>
    class Object : public detail::BaseObject<Object<Holder, Traits...>, Holder, Traits...>
  {
  typedef detail::BaseObject<Object<Holder, Traits...>, Holder, Traits...> Base;
  typedef typename Base::Functions Functions;

public:
  Object()
      : _functions(nullptr)
    {
    }

  ~Object()
    {
    // Someone in the base traits must implement clear.
    Base::clear();
    }

  template <typename Type, typename... TraitsImpl> void init(const Type& t)
    {
    static const Functions fn = buildFunctions<Type, TraitsImpl...>();
    _functions = &fn;

    Base::assign(t);
    }

  template <typename Type, typename... TraitsImpl> void init(const Object<Holder, Traits...>& t)
    {
    static const Functions fn = buildFunctions<Type, TraitsImpl...>();
    _functions = &fn;

    Base::assignObject(t);
    }

  const Functions *functions() const { return _functions; }
  detail::PrivateData *data() { return &_data; }
  const detail::PrivateData *data() const { return &_data; }

private:
  template <typename Type, typename... TraitsImpl> Functions buildFunctions()
    {
    Functions fn;
    fn.template init<Type, TraitsImpl...>();
    return fn;
    }

  const Functions *_functions;
  detail::PrivateData _data;
  };

}

}

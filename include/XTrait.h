#pragma once
#include "Utilities/XTightPointer.h"
#include "Utilities/XPrivateImpl.h"

namespace Eks
{

namespace Trait
{

namespace detail
{

#define X_TRAIT_IMPL(CLS, DERIVED) protected: \
  DERIVED* derived() { return static_cast<Derived*>(this); } \
  const DERIVED* derived() const { return static_cast<const Derived*>(this); } \
  detail::PrivateData *data() { return derived()->data(); } \
  const detail::PrivateData *data() const { return derived()->data(); } \
  const CLS::Functions *functions() const { return derived()->functions(); }

typedef PrivateImpl<sizeof(void*)*4> PrivateData;

template <typename Derived,
          template <typename D1> class Trait1,
          template <typename D> class... Traits>
  class BaseObject
    : public Trait1<Derived>,
      public BaseObject<Derived, Traits...>
  {
public:
  struct Functions
      : public Trait1<Derived>::Functions,
        public BaseObject<Derived, Traits...>::Functions
    {
    template <typename Type, typename Trait1Impl, typename... TraitsImpl> void init()
      {
      Trait1<Derived>::Functions::template init<Type, Trait1Impl>();

      typedef BaseObject<Derived, Traits...> Base;
      Base::Functions::template init<Type, TraitsImpl...>();
      }
    };

  };

template <typename Derived,
          template <typename D1> class Trait>
  class BaseObject<Derived, Trait>
    : public Trait<Derived>
  {
public:
  struct Functions
      : public Trait<Derived>::Functions
    {
    template <typename Type, typename TraitImpl> void init()
      {
      Trait<Derived>::Functions::template init<Type, TraitImpl>();
      }
    };

  };

}

template <template <typename D> class... Traits>
    class Object : public detail::BaseObject<Object<Traits...>, Traits...>
  {
  typedef detail::BaseObject<Object<Traits...>, Traits...> Base;
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

  template <typename Type, typename... TraitsImpl> void init()
    {
    static const Functions fn = buildFunctions<Type, TraitsImpl...>();
    _functions = &fn;
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

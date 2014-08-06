#pragma once
#include "XTrait.h"

namespace Eks
{

namespace Trait
{

template <typename Derived> class Destroyable
  {
public:
  struct Functions
    {
    struct
      {
      void *(*object)(detail::PrivateData *ths);
      void (*assign)(detail::PrivateData *ths, const void *obj);
      void (*destroy)(detail::PrivateData *ths);
      } destroy;

    template <typename Type, typename Impl> void init()
      {
      destroy.object = Impl::template object<Type>;
      destroy.assign = Impl::template assign<Type>;
      destroy.destroy = Impl::template destroy<Type>;
      }
    };

  X_TRAIT_IMPL(Destroyable, Derived)

public:
  const void *object() const
    {
    return functions()->destroy.object(const_cast<Destroyable<Derived>*>(this)->data());
    }

  void *object()
    {
    return functions()->destroy.object(data());
    }

  template <typename T> void assign(const T &t)
    {
    clear();

    functions()->destroy.assign(data(), &t);
    }

  void assign(const Derived &t)
    {
    clear();

    functions()->destroy.assign(data(), t.object());
    }

  void clear()
    {
    auto d = data();
    if(d->isValid())
      {
      functions()->destroy.destroy(d);
      }
    }
  };

class DestroyableHelpers
  {
public:
  struct Copied
    {
    template <typename T>
        static void *object(detail::PrivateData *ths)
      {
      return ths->data<T>();
      }

    template <typename T>
        static void assign(detail::PrivateData *ths, const void *t)
      {
      ths->create<T>(*static_cast<const T *>(t));
      }

    template <typename T>
        static void copy(detail::PrivateData *ths, const detail::PrivateData *src)
      {
      ths->create<T>(*src->data<T>());
      }

    template <typename T>
        static void destroy(detail::PrivateData *ths)
      {
      ths->destroy<T>();
      }
    };

  struct Referenced
    {
    template <typename T>
        static void *object(detail::PrivateData *ths)
      {
      return *ths->data<T*>();
      }

    template <typename T>
        static void assign(detail::PrivateData *ths, const void *t)
      {
      ths->create<T*>(const_cast<T *>(static_cast<const T *>(t)));
      }

    template <typename T>
        static void copy(detail::PrivateData *ths, const detail::PrivateData *src)
      {
      const T *srcD = *src->data<T*>();
      ths->create<T*>(const_cast<T *>(srcD));
      }

    template <typename T>
        static void destroy(detail::PrivateData *ths)
      {
      ths->destroy<T*>();
      }
    };
  };

}

}

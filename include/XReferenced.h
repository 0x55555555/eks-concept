#pragma once
#include "XTrait.h"

namespace Eks
{

namespace Trait
{

class Referenced
  {
public:
  template <typename Derived> class Trait
    {
  public:
    struct Functions
      {
      struct
        {
        void (*assign)(detail::PrivateData *ths, const detail::ConstObjectWrapper &obj);
        void (*destroy)(detail::PrivateData *ths);
        } referenced;

      X_TRAIT_FUNCTION_IMPL(referenced, assign, destroy);
      };

    X_TRAIT_IMPL

  public:
    detail::ConstObjectWrapper object() const
      {
      detail::PrivateData* d = const_cast<Trait<Derived>*>(this)->data();
      return *d->data<void*>();
      }

    detail::ObjectWrapper object()
      {
      detail::PrivateData* d = data();
      return *d->data<void*>();
      }

    void clear()
      {
      auto d = data();
      if(d->isValid())
        {
        functions()->referenced.destroy(d);
        }
      }
  protected:
    void assignObject(const Derived &t)
      {
      clear();

      functions()->referenced.assign(data(), t.object());
      }

    template <typename T> void assign(const T &t)
      {
      clear();

      functions()->referenced.assign(data(), &t);
      }

    };

  struct Copied
    {
    template <typename T>
        static void assign(detail::PrivateData *ths, const detail::ConstObjectWrapper &t)
      {
      ths->create<T>(*t.as<T>());
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

  struct Default
    {
    template <typename T>
        static void *object(detail::PrivateData *ths)
      {
      return *ths->data<T*>();
      }

    template <typename T>
        static void assign(detail::PrivateData *ths, const detail::ConstObjectWrapper &t)
      {
      ths->create<T*>(const_cast<T *>(t.as<T>()));
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

#pragma once
#include "XTrait.h"

namespace Eks
{

namespace Trait
{

template <typename El> class Indexable
  {
public:

  template <typename Derived> class Trait
    {
  public:
    struct Functions
      {
      struct
        {
        El &(*at)(void *ths, xsize i);
        xsize (*size)(const void *ths);
        } indexable;

      template <typename Type, typename Impl> void init()
        {
        indexable.at = (decltype(indexable.at))Impl::template at<Type>;
        indexable.size = (decltype(indexable.size))Impl::template size<Type>;
        }
      };

    X_TRAIT_IMPL(Trait, Derived)

  public:
    El &at(xsize i)
      {
      return functions()->indexable.at(derived()->object(), i);
      }

    const El &at(xsize i) const
      {
      return functions()->indexable.at(const_cast<void *>(derived()->object()), i);
      }

    El &operator[](xsize i)
      {
      return at(i);
      }

    const El &operator[](xsize i) const
      {
      return at(i);
      }

    xsize size() const
      {
      return functions()->indexable.size(const_cast<void *>(derived()->object()));
      }
    };

  struct StdInterface
    {
    template <typename T> static El &at(T *ths, xsize i)
      {
      return (*ths)[i];
      }

    template <typename T> static xsize size(T *ths)
      {
      return ths->size();
      }
    };

  };

}

}

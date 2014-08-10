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
        El &(*at)(const detail::ObjectWrapper &ths, xsize i);
        xsize (*size)(const detail::ConstObjectWrapper &ths);
        } indexable;

      X_TRAIT_FUNCTION_IMPL(indexable, at, size);
      };

    X_TRAIT_IMPL

  public:
    El &at(xsize i)
      {
      return functions()->indexable.at(derived()->object(), i);
      }

    const El &at(xsize i) const
      {
      return functions()->indexable.at(const_cast<Derived*>(derived())->object(), i);
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
      return functions()->indexable.size(derived()->object());
      }
    };

  struct StdInterface
    {
    template <typename T> static El &at(const detail::ObjectWrapper &ths, xsize i)
      {
      return (*ths.as<T>())[i];
      }

    template <typename T> static xsize size(const detail::ConstObjectWrapper &ths)
      {
      return ths.as<T>()->size();
      }
    };

  };

}

}

#pragma once
#include "XTrait.h"

namespace Eks
{

namespace Trait
{

template <typename El> class Iterable
  {
public:
  typedef Eks::PrivateImpl<sizeof(void*) * 3> IteratorData;

  template <typename Derived> class Trait
    {
  public:
    struct Functions
      {
      struct
        {
        void (*begin)(const detail::ObjectWrapper &ths, IteratorData *it);
        void (*end)(const detail::ObjectWrapper &ths, IteratorData *it);
        void (*copy)(const detail::ObjectWrapper &ths, IteratorData *it, const IteratorData *src);
        void (*destroy)(const detail::ObjectWrapper &ths, IteratorData *it);
        void (*increment)(const detail::ObjectWrapper &ths, IteratorData *it);
        El &(*get)(const detail::ObjectWrapper &ths, IteratorData *it);
        bool (*equal)(const detail::ObjectWrapper &ths, const IteratorData *a, const IteratorData *b);
        } iterable;

      X_TRAIT_FUNCTION_IMPL(iterable, begin, end, copy, destroy, increment, get, equal);
      };

    X_TRAIT_IMPL

  public:
    class ConstIterator
      {
    public:
      ConstIterator()
          : _derived(nullptr)
        {
        }

      ConstIterator(const ConstIterator &it)
          : _derived(it._derived)
        {
        _derived->functions()->iterable.copy(_derived->object(), &_data, &it._data);
        }

      ~ConstIterator()
        {
        _derived->functions()->iterable.destroy(_derived->object(), &_data);
        }

      void operator++()
        {
        return _derived->functions()->iterable.increment(_derived->object(), &_data);
        }

      const El &operator*() const
        {
        return _derived->functions()->iterable.get(_derived->object(), const_cast<IteratorData *>(&_data));
        }

      bool operator==(const ConstIterator &it) const
        {
        return _derived->functions()->iterable.equal(_derived->object(), &_data, &it._data);
        }

    protected:
      Derived *_derived;
      IteratorData _data;
      friend class Trait;
      };

    class Iterator : public ConstIterator
      {
    public:
      El &operator*()
        {
        auto fn = ConstIterator::_derived->functions();
        return fn->iterable.get(ConstIterator::_derived->object(), ConstIterator::_data);
        }
      };

    Iterator begin()
      {
      Iterator it;
      it._derived = derived();
      functions()->iterable.begin(derived()->object(), &it._data);
      return it;
      }

    Iterator end()
      {
      Iterator it;
      it._derived = derived();
      functions()->iterable.end(derived()->object(), &it._data);
      return it;
      }

    ConstIterator begin() const
      {
      return const_cast<Derived *>(derived())->begin();
      }

    ConstIterator end() const
      {
      return const_cast<Derived *>(derived())->end();
      }
    };

  struct StdInterface
    {
    template <typename T> static void begin(const detail::ObjectWrapper &ths, IteratorData *it)
      {
      it->create<typename T::iterator>(ths.as<T>()->begin());
      }

    template <typename T> static void end(const detail::ObjectWrapper &ths, IteratorData *it)
      {
      it->create<typename T::iterator>(ths.as<T>()->end());
      }

    template <typename T> static void copy(const detail::ObjectWrapper &, IteratorData *it, const IteratorData *src)
      {
      it->create<typename T::iterator>(*src->data<typename T::iterator>());
      }

    template <typename T> static void destroy(const detail::ObjectWrapper &, IteratorData *it)
      {
      it->destroy<typename T::iterator>();
      }

    template <typename T> static void increment(const detail::ObjectWrapper &, IteratorData *it)
      {
      ++(*it->data<typename T::iterator>());
      }

    template <typename T> static El &get(const detail::ObjectWrapper &, IteratorData *it)
      {
      return **it->data<typename T::iterator>();
      }

    template <typename T> static bool equal(const detail::ObjectWrapper &, const IteratorData *a, const IteratorData *b)
      {
      return *a->data<typename T::iterator>() == *b->data<typename T::iterator>();
      }
    };

  };

}

}

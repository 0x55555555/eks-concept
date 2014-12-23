#pragma once
#include "XTrait.h"
#include "XIterable.h"
#include "XReferenced.h"
#include <vector>

namespace Eks
{

template <class T> using Iterable = Trait::Object<Trait::Referenced, Trait::Iterable<T>>;


template <typename T> Iterable<T> makeIterable()
  {
  Iterable<T> it;
  it.template init<std::vector<T>, Trait::Referenced::Copied, Trait::Iterable<T>::StdInterface>(std::vector<T>());

  return it;
  }
}

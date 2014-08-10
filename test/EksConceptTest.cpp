#include <QtTest>
#include <vector>
#include <list>
#include "QVector"
#include "XIterable.h"
#include "XIndexable.h"
#include "XDestroyable.h"
#include "XReferenced.h"

class EksConceptTest : public QObject
  {
  Q_OBJECT

private Q_SLOTS:
  void destroyableTest();
  void destroyableCopyTest();
  void iterableTest();
  void indexableTest();
  void defaultTraitsTest();
  void customTest();
  };

struct DestroyTest
  {
  DestroyTest()
    {
    _data = 643;
    ++ctor;
    }

  DestroyTest(const DestroyTest &i)
    {
    _data = i._data;
    if (_data != 643)
      {
      throw std::runtime_error("invalid src");
      }
    ++copy;
    }

  ~DestroyTest()
    {
    ++dtor;
    }

  static void reset()
    {
    ctor = 0;
    copy = 0;
    dtor = 0;
    }

  int _data;
  static int ctor;
  static int copy;
  static int dtor;
  };

int DestroyTest::ctor = 0;
int DestroyTest::copy = 0;
int DestroyTest::dtor = 0;

void EksConceptTest::destroyableTest()
  {
  using namespace Eks::Trait;

  DestroyTest::reset();

  {
    DestroyTest a;
    QCOMPARE(DestroyTest::ctor, 1);
    QCOMPARE(DestroyTest::copy, 0);
    QCOMPARE(DestroyTest::dtor, 0);

    Object<Destroyable> d;
    d.init<DestroyTest, Destroyable::Default>(DestroyTest());

    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 1);

    Object<Referenced> d2;

    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 1);

    d2.init<DestroyTest, Referenced::Default>(a);

    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 1);

    d.clear();

    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 2);

    d2.clear();

    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 2);
    }

  QCOMPARE(DestroyTest::ctor, 2);
  QCOMPARE(DestroyTest::copy, 1);
  QCOMPARE(DestroyTest::dtor, 3);
  }

void EksConceptTest::destroyableCopyTest()
  {
  using namespace Eks::Trait;

  DestroyTest::reset();

    {
    Object<Destroyable> d;
    d.init<DestroyTest, Destroyable::Default>(DestroyTest());

    QCOMPARE(DestroyTest::ctor, 1);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 1);

    Object<Referenced> d2;

    Object<Destroyable> d3;

    DestroyTest a;

    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 1);

    d2.init<DestroyTest, Referenced::Default>(a);
    QCOMPARE(d2.object().as<void>(), &a);
    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 1);

    d2.init<DestroyTest, Referenced::Default>(*d.object().as<DestroyTest>());
    QCOMPARE(d2.object().as<void>(), d.object().as<void>());
    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 1);

    d3.init<DestroyTest, Destroyable::Default>(d);
    QVERIFY(d3.object().as<void>() != d.object().as<void>());
    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 2);
    QCOMPARE(DestroyTest::dtor, 1);
    }

  QCOMPARE(DestroyTest::ctor, 2);
  QCOMPARE(DestroyTest::copy, 2);
  QCOMPARE(DestroyTest::dtor, 4);
  }

void EksConceptTest::iterableTest()
  {
  using namespace Eks::Trait;

  std::vector<int> intV;
  std::list<int> intL;
  QVector<int> intQ;

  intV.push_back(0);
  intV.push_back(1);
  intV.push_back(2);
  intV.push_back(3);
  intV.push_back(4);

  intL.push_back(0);
  intL.push_back(1);
  intL.push_back(2);
  intL.push_back(3);
  intL.push_back(4);

  intQ << 0 << 1 << 2 << 3 << 4;

  auto checkVector = [](const Object<Referenced, Iterable<int>> &i)
    {
    auto begin = i.begin();
    auto end = i.end();

    QCOMPARE(*begin, 0); ++begin;
    QCOMPARE(*begin, 1); ++begin;
    QCOMPARE(*begin, 2); ++begin;
    QCOMPARE(*begin, 3); ++begin;
    QCOMPARE(*begin, 4); ++begin;

    QCOMPARE(begin, end);
    };

  Object<Referenced, Iterable<int>> i;
  i.init<decltype(intV), Referenced::Default, Iterable<int>::StdInterface>(intV);
  checkVector(i);

  i.init<decltype(intQ), Referenced::Default, Iterable<int>::StdInterface>(intQ);
  checkVector(i);

  i.init<decltype(intL), Referenced::Default, Iterable<int>::StdInterface>(intL);
  checkVector(i);
  }

void EksConceptTest::indexableTest()
  {
  using namespace Eks::Trait;

  std::vector<int> intV;
  QVector<int> intQ;

  intV.push_back(0);
  intV.push_back(1);
  intV.push_back(2);
  intV.push_back(3);
  intV.push_back(4);

  intQ << 0 << 1 << 2 << 3 << 4;

  auto checkVector = [](const Object<Referenced, Iterable<int>, Indexable<int>> &i)
    {
    auto size = i.size();
    QCOMPARE(size, 5UL);

    QCOMPARE(i[0], 0);
    QCOMPARE(i[1], 1);
    QCOMPARE(i[2], 2);
    QCOMPARE(i[3], 3);
    QCOMPARE(i[4], 4);
    };

  Object<Referenced, Iterable<int>, Indexable<int>> i;
  i.init<decltype(intV), Referenced::Default, Iterable<int>::StdInterface, Indexable<int>::StdInterface>(intV);
  checkVector(i);

  i.init<decltype(intQ), Referenced::Default, Iterable<int>::StdInterface, Indexable<int>::StdInterface>(intQ);
  checkVector(i);
  }

void EksConceptTest::defaultTraitsTest()
  {
  using namespace Eks::Trait;

  Object<Destroyable, Iterable<float>> a;
  }

void EksConceptTest::customTest()
  {
  }

QTEST_APPLESS_MAIN(EksConceptTest)

#include "EksConceptTest.moc"

#include <QtTest>
#include <vector>
#include <list>
#include "QVector"
#include "XIterable.h"
#include "XIndexable.h"
#include "XDestroyable.h"

class EksConceptTest : public QObject
  {
  Q_OBJECT

private Q_SLOTS:
  void destroyableTest();
  void destroyableCopyTest();
  void iterableTest();
  void indexableTest();
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
    d.init<DestroyTest, DestroyableHelpers::Copied>();
    d.assign(DestroyTest());

    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 1);

    Object<Destroyable> d2;
    d2.init<DestroyTest, DestroyableHelpers::Referenced>();

    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 1);

    d2.assign(a);

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
    d.init<DestroyTest, DestroyableHelpers::Copied>();
    d.assign(DestroyTest());

    QCOMPARE(DestroyTest::ctor, 1);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 1);

    Object<Destroyable> d2;
    d2.init<DestroyTest, DestroyableHelpers::Referenced>();

    Object<Destroyable> d3;
    d3.init<DestroyTest, DestroyableHelpers::Copied>();

    DestroyTest a;

    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 1);

    d2.assign(a);
    QCOMPARE(d2.object(), &a);
    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 1);

    d2.assign(d);
    QCOMPARE(d2.object(), d.object());
    QCOMPARE(DestroyTest::ctor, 2);
    QCOMPARE(DestroyTest::copy, 1);
    QCOMPARE(DestroyTest::dtor, 1);

    d3.assign(d);
    QVERIFY(d3.object() != d.object());
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

  auto checkVector = [](const Object<Destroyable, Iterable<int>::Trait> &i)
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

  Object<Destroyable, Iterable<int>::Trait> i;
  i.init<decltype(intV), DestroyableHelpers::Referenced, Iterable<int>::StdInterface>();
  i.assign(intV);
  checkVector(i);

  i.init<decltype(intQ), DestroyableHelpers::Referenced, Iterable<int>::StdInterface>();
  i.assign(intQ);
  checkVector(i);

  i.init<decltype(intL), DestroyableHelpers::Referenced, Iterable<int>::StdInterface>();
  i.assign(intL);
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

  auto checkVector = [](const Object<Destroyable, Iterable<int>::Trait, Indexable<int>::Trait> &i)
    {
    auto size = i.size();
    QCOMPARE(size, 5UL);

    QCOMPARE(i[0], 0);
    QCOMPARE(i[1], 1);
    QCOMPARE(i[2], 2);
    QCOMPARE(i[3], 3);
    QCOMPARE(i[4], 4);
    };

  Object<Destroyable, Iterable<int>::Trait, Indexable<int>::Trait> i;
  i.init<decltype(intV), DestroyableHelpers::Referenced, Iterable<int>::StdInterface, Indexable<int>::StdInterface>();
  i.assign(intV);
  checkVector(i);

  i.init<decltype(intQ), DestroyableHelpers::Referenced, Iterable<int>::StdInterface, Indexable<int>::StdInterface>();
  i.assign(intQ);
  checkVector(i);
  }

QTEST_APPLESS_MAIN(EksConceptTest)

#include "EksConceptTest.moc"

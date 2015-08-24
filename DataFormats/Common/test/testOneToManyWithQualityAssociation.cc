#include "cppunit/extensions/HelperMacros.h"
#include <algorithm>
#include <iterator>
#include <iostream>
#include "DataFormats/Common/interface/Proxy.h"
#include "DataFormats/Common/interface/AssociationToManyWithQuality.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/Common/interface/TestHandle.h"
using namespace edm;

class testOneToManyWithQualityAssociation : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(testOneToManyWithQualityAssociation);
  CPPUNIT_TEST(checkAll);
  CPPUNIT_TEST_SUITE_END();

public:
  typedef std::vector<int> IdVal;
  typedef std::vector<double> CVal;

  testOneToManyWithQualityAssociation();
  void setUp() {}
  void tearDown() {}
  void checkAll(); 
  void test(const edm::AssociationToManyWithQuality<CVal> &);
  void proxyUnitTest();
 
  std::vector<int> k;
  CVal v;
 
  edm::TestHandle<IdVal> handleK;
  edm::TestHandle<CVal> handleV;

	std::multimap<int,int> w;
};

CPPUNIT_TEST_SUITE_REGISTRATION(testOneToManyWithQualityAssociation);

testOneToManyWithQualityAssociation::testOneToManyWithQualityAssociation() {
	v.push_back(1.1);
	v.push_back(2.2);
	v.push_back(3.3);
	v.push_back(1.4);
	v.push_back(2.4);
	v.push_back(3.4);
	v.push_back(1.6);
	v.push_back(2.6);
	v.push_back(3.6);


  ProductID const pidV(1, 1);

  handleV = edm::TestHandle<CVal>(&v, pidV);


  k.push_back(1);
  k.push_back(2);
  k.push_back(3);
  k.push_back(4);
  
  ProductID const pidK(1, 2);
  handleK = edm::TestHandle<IdVal>(&k, pidK);


	w = {
				{ 1,1},
				{ 1,7},
				{ 2,2},
				{ 3,3},
				{ 1,8},
				{ 4,4},
				{ 3,6},
				{ 4,5},
				{ 1,9}
			};

}

void testOneToManyWithQualityAssociation::checkAll() {
  {
    edm::AssociationToManyWithQuality<CVal> assoc(handleV);

    assoc.insert(handleK, w);

    test(assoc);
    proxyUnitTest();
  } 

}

void testOneToManyWithQualityAssociation::test(const edm::AssociationToManyWithQuality<CVal> & assoc) {
  CPPUNIT_ASSERT(assoc.contains(1));
  CPPUNIT_ASSERT(assoc.contains(2));
  CPPUNIT_ASSERT(assoc.contains(3));
  CPPUNIT_ASSERT(assoc.contains(4));
  CPPUNIT_ASSERT(!assoc.contains(5));
  CPPUNIT_ASSERT(!assoc.contains(7));

  edm::Proxy<CVal> p1 = assoc[edm::Ref<IdVal>(handleK, 1)];
  edm::Proxy<CVal> p2 = assoc[edm::Ref<IdVal>(handleK, 2)];
  edm::Proxy<CVal> p3 = assoc[edm::Ref<IdVal>(handleK, 3)];
  edm::Proxy<CVal> p4 = assoc[edm::Ref<IdVal>(handleK, 4)];

}

void testOneToManyWithQualityAssociation::proxyUnitTest() {

  std::vector<int> ids;
  ids.push_back(1);
  ids.push_back(2);
  ids.push_back(3);
  ids.push_back(4);
  ids.push_back(5);
  ids.push_back(6);
  ids.push_back(7);
  ids.push_back(8);


  std::vector<double> values;
  values.push_back(17.1);
  values.push_back(18.1);
  values.push_back(19.1);
  values.push_back(20.1);
  values.push_back(21.1);
  values.push_back(22.2);
  values.push_back(23.3);
  values.push_back(24.3);

  // test size()- function
  edm::Proxy<CVal> pr(2,6,ids,values);

  CPPUNIT_ASSERT(pr.size() == 4);

  ProxyIterator<CVal> prIter = pr.begin();
  CPPUNIT_ASSERT(pr.size() == prIter.size());

  // test getValues()- function
  std::vector<Ref<CVal>> refVect = pr.getValues();
  
  float epsilon = 0.001;
  
  int index = 2;
  for(Ref<CVal> ref: refVect)
  {
    // double comparison in c++
    CPPUNIT_ASSERT( fabs(*(ref.get()) - values[index++]) < epsilon );
  }

}


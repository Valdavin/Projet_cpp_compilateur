/*
 * File:   instSiTest.h
 * Author: davinv
 *
 * Created on 23 oct. 2015, 01:06:21
 */

#ifndef INSTSITEST_H
#define	INSTSITEST_H

#include <cppunit/extensions/HelperMacros.h>

class instSiTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(instSiTest);

    CPPUNIT_TEST(testMethod);
    CPPUNIT_TEST(testFailedMethod);

    CPPUNIT_TEST_SUITE_END();

public:
    instSiTest();
    virtual ~instSiTest();
    void setUp();
    void tearDown();

private:
    void testMethod();
    void testFailedMethod();
};

#endif	/* INSTSITEST_H */


/*
 * File:   instSiTest.cpp
 * Author: davinv
 *
 * Created on 23 oct. 2015, 01:06:21
 */

#include "instSiTest.h"
// #include "../ArbreAbstrait.h" 
#include "Interpreteur.h"
#include "Exceptions.h"


CPPUNIT_TEST_SUITE_REGISTRATION(instSiTest);

instSiTest::instSiTest() {
}

instSiTest::~instSiTest() {
}

void instSiTest::setUp() {
}

void instSiTest::tearDown() {
}

void instSiTest::testMethod() {
    ifstream fichier("testSi.txt");
    Interpreteur interpreteur(fichier);
    interpreteur.analyse();
    CPPUNIT_ASSERT_EQUAL_MESSAGE ("Test 1 : ", 1, interpreteur.getTable()[7].getChaine())
}

void instSiTest::testFailedMethod() {
    CPPUNIT_ASSERT(false);
}


/*
 * File:   newtestclass.cpp
 * Author: davinv
 *
 * Created on 22 oct. 2015, 22:12:34
 */

#include "newtestclass.h"
#include "ArbreAbstrait.h"


CPPUNIT_TEST_SUITE_REGISTRATION(newtestclass);

newtestclass::newtestclass() {
}

newtestclass::~newtestclass() {
}

void newtestclass::setUp() {
}

void newtestclass::tearDown() {
}

void newtestclass::testExecuter() {
    NoeudInstSi noeudInstSi;
    int result = noeudInstSi.executer();
    if (true /*check result*/) {
        CPPUNIT_ASSERT(false);
    }
}


/*
 * File:   instSiTest.cpp
 * Author: davinv
 *
 * Created on 23 oct. 2015, 01:06:21
 */

#include "instSiTest.h"
// #include "../ArbreAbstrait.h" 
#include "../Interpreteur.h"
#include "../Exceptions.h"
#include <iostream>
using namespace std;

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
    
    Symbole test1("test1");
    Symbole test2("test2");
    Symbole test3("test3");
    Symbole test4("test4");
    
    
    if (interpreteur.sansErreur()) {
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Analyse fonctionelle. Pas d'erreurs de syntaxe", interpreteur.analyse());
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Arbre fonctionnel", interpreteur.getArbre()->executer());
        
        TableSymboles Ts = interpreteur.getTable();
        
        CPPUNIT_ASSERT_EQUAL_MESSAGE ("Test 1 : ", 1, Ts.chercheAjoute(test1)->executer());
        CPPUNIT_ASSERT_EQUAL_MESSAGE ("Test 2 : ", 1, Ts.chercheAjoute(test2)->executer());
        CPPUNIT_ASSERT_EQUAL_MESSAGE ("Test 3 : ", 1, Ts.chercheAjoute(test3)->executer());
        CPPUNIT_ASSERT_EQUAL_MESSAGE ("Test 4 : ", 1, Ts.chercheAjoute(test4)->executer());
    
    }
    
    ifstream fichier1("testSiBug1.txt");
    Interpreteur interpreteur1(fichier1);
    CPPUNIT_ASSERT_THROW(interpreteur1.analyse(), SyntaxeException);
    
    
    ifstream fichier2("testSiBug2.txt");
    Interpreteur interpreteur2(fichier2);
    interpreteur2.analyse();
    CPPUNIT_ASSERT(!interpreteur2.sansErreur());
    
    
    ifstream fichier3("testSiBug3.txt");
    Interpreteur interpreteur3(fichier3);
    interpreteur3.analyse();
    CPPUNIT_ASSERT(!interpreteur3.sansErreur());
}

void instSiTest::testFailedMethod() {
    //CPPUNIT_ASSERT(false);
}


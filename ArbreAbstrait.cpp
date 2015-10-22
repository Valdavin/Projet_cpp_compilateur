#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <vector>
#include <string>
#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

void NoeudSeqInst::traduitEnCPP(ostream & cout, unsigned int identation) const {
    for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->traduitEnCPP(cout, identation); // on exécute chaque instruction de la séquence
    
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudAffectation::traduitEnCPP(ostream & cout, unsigned int identation) const {
    cout << setw(4*identation) << "";
    m_variable->traduitEnCPP(cout, 0);
    cout << " = " << endl;
    m_expression->traduitEnCPP(cout, 0);
    cout << ";" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

void NoeudOperateurBinaire::traduitEnCPP(ostream & cout, unsigned int identation) const {
cout << ((SymboleValue*) m_operandeGauche)->getChaine();
    int fin = 0;
    if (this->m_operateur == "+") { cout<< " && ";}
    else if (this->m_operateur == "-") { cout<< " - ";}
    else if (this->m_operateur == "*") { cout<< " * ( ";fin =1;}
    else if (this->m_operateur == "==") { cout<< " == ";}
    else if (this->m_operateur == "!=") { cout<< " != ";}
    else if (this->m_operateur == "<") { cout<< " < ";}
    else if (this->m_operateur == ">") { cout<< " > ";}
    else if (this->m_operateur == "<=") { cout<< " <= ";}
    else if (this->m_operateur == ">=") { cout<< " >= ";}
    else if (this->m_operateur == "et") { cout<< " && ";}
    else if (this->m_operateur == "ou") { cout<< " || ";}
    else if (this->m_operateur == "non") { cout<< " ! ";}
    else if (this->m_operateur == "/") { cout<< " / ( ";fin =1;}
    else { cout << " " << m_operateur << " ";}
    cout << ((SymboleValue*) m_operandeDroit)->getChaine();
    if(fin==1)
        cout << " )";
}


////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence, vector<Noeud*> condition2, vector<Noeud*> sequence2, Noeud* sequence3)
: m_condition(condition), m_sequence(sequence), m_condition2(condition2), m_sequence2(sequence2), m_sequence3(sequence3) {
}

int NoeudInstSi::executer() {
    int fin = 0;
    if (m_condition->executer()){ m_sequence->executer();fin=1;}
    else if (fin==0)for (unsigned int i = 0; i < m_condition2.size() && fin==0; i++){
            if (m_condition2[i]->executer()){
                m_sequence2[i]->executer();
                fin=1;
            }
        }
    else if ( fin == 0 && m_sequence3 != NULL ){
        m_sequence3->executer();
    }
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstSi::traduitEnCPP(ostream & cout, unsigned int identation) const {
    cout << setw(4*identation) << "" << "if ( ";
    m_condition->traduitEnCPP(cout, 0);
    cout << " )  {" << endl;
    m_sequence->traduitEnCPP(cout, identation+1);
    cout << setw(4*identation) << "" << "}";
    for (unsigned int i = 0; i < m_condition2.size(); i++){
        cout << " else if ( ";
        m_condition2[i]->traduitEnCPP(cout, 0);
        cout << " )  {" << endl;
        m_sequence2[i]->traduitEnCPP(cout, identation+1);
        cout << setw(4*identation) << "" << "}";
    }
    if ( m_sequence3 != NULL ){
        cout << setw(4*identation) << "" << "else {" << endl;
        m_sequence3->traduitEnCPP(cout, identation+1);
        cout << setw(4*identation) << "" << "}" << endl;
    }
    m_condition->traduitEnCPP(cout, 0);
    cout << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
  while (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}
void NoeudInstTantQue::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    //<instTantQue>  ::= tantque (  <expression>  )  <seqInst>  fintantque

    cout << setw(4 * indentation) << "" << "while (";
    m_condition->traduitEnCPP(cout, 0);
    cout << ") {" << endl;
    m_sequence->traduitEnCPP(cout, indentation + 1);
    cout << setw(4 * indentation) << "" << "}" << endl;
}
////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* sequence, Noeud* condition)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstRepeter::executer() {
    do{
        m_sequence->executer();
    }while (!m_condition->executer());
  return 0; // La valeur renvoyée ne représente rien !
}
void NoeudInstRepeter::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    //<instRepeter> ::= repeter <seqInst> jusquexpressiona( <expression>)

    cout << setw(4 * indentation) << "" << "do { ";
    m_sequence->traduitEnCPP(cout, indentation + 1);
    cout << "}" << endl;
    cout << "" << "while (!(";
    m_condition->traduitEnCPP(cout, 0);
    cout << "));";
}
////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////
NoeudInstPour::NoeudInstPour(Noeud* affectation1, Noeud* condition, Noeud* affectation2, Noeud* sequence)
: m_condition(condition), m_sequence(sequence),m_affectation1(affectation1), m_affectation2(affectation2){
}

int NoeudInstPour::executer() {
    if(m_affectation1==NULL){
        if(m_affectation2==NULL){
        for(;m_condition->executer();)
        m_sequence->executer();
        }else{
            for(;m_condition->executer();m_affectation2->executer())
   m_sequence->executer();
        }
    }else{
        if(m_affectation2==NULL){
        for(m_affectation1->executer();m_condition->executer();)
        m_sequence->executer();
        }else{
            for(m_affectation1->executer();m_condition->executer();m_affectation2->executer())
   m_sequence->executer();
        }
    }
    //for(m_affectation1->executer();m_condition->executer();m_affectation2->executer())
   m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}
void NoeudInstPour::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    //<instRepeter> ::= repeter <seqInst> jusquexpressiona( <expression>)
    cout << setw(4 * indentation) << "" << "for ( ";
    if (m_affectation1 != NULL){
        m_affectation1->traduitEnCPP(cout, 0);
        cout << " ";
    }
    cout << ";";
    m_condition->traduitEnCPP(cout, 0);
    cout << ";";

    if (m_affectation2 != NULL) {
        cout << " ";
        m_affectation2->traduitEnCPP(cout, 0);
    }
    cout << ") {" << endl;
    m_sequence->traduitEnCPP(cout, indentation + 1);
    cout << setw(4 * indentation) << "" << "}";
}
//////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire(){
}

int NoeudInstEcrire::executer() {
    for (unsigned int i = 0; i < m_variable.size(); i++) {
        if ( (typeid(*m_variable[i])==typeid(SymboleValue) &&  *((SymboleValue*)m_variable[i])== "<CHAINE>" )){
            string chaine = ((SymboleValue*) m_variable[i])-> getChaine();
            cout << chaine.substr(1, chaine.size() - 2);
        }else{
            cout << m_variable[i]->executer(); // On exécute chaque instruction de la séquence -> récupérer valeur
        }
    }
    cout << endl;
  return 0; // La valeur renvoyée ne représente rien !
}
void NoeudInstEcrire::ajoute(Noeud * instruction) {
    if (instruction != nullptr) m_variable.push_back(instruction);
}
void NoeudInstEcrire::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    //On considère qu'on met tout en chaines de caractères
    cout << setw(4 * indentation) << "" << "cout";
    for (unsigned int i = 0; i < m_variable.size(); i++) {
        cout << " << ";
        if ( (typeid(*m_variable[i])==typeid(SymboleValue) &&  *((SymboleValue*)m_variable[i])== "<CHAINE>" )){
            string chaine = ((SymboleValue*) m_variable[i])-> getChaine();
            cout << "\"" << chaine.substr(1, chaine.size() - 2) << "\"";
        }else{
            m_variable[i]->traduitEnCPP(cout, 0);
        }
    }

    cout << " ;";
}
////////////////////////////////////////////////////////////////////////////////
// NoeudLire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire(vector<Noeud*> variable)
: m_variable(variable) {
}

int NoeudInstLire::executer() {
  //int valeur = m_expression->executer(); // On exécute (évalue) l'expression 
    for (unsigned int i = 0; i < m_variable.size(); i++) {
        int valeur;
        cin >> valeur;
        ((SymboleValue*) m_variable[i])->setValeur(valeur);
    }
  return 0; // La valeur renvoyée ne représente rien !
}
void NoeudInstLire::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    //On considère que tout sera transformé en chaines pour l'affichage
    cout << setw(4 * indentation) << "" << "cin";
    for (unsigned int i = 0; i < m_variable.size(); i++) {
        cout << " >> ";
        m_variable[i]->traduitEnCPP(cout, 0);
    }
    cout << " ;";
}
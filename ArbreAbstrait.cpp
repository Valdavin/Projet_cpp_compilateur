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

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence, vector<Noeud*> condition2, vector<Noeud*> sequence2, Noeud* sequence3)
: m_condition(condition), m_sequence(sequence), m_condition2(condition2), m_sequence2(sequence2), m_sequence3(sequence3) {
}

int NoeudInstSi::executer() {
  if (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
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

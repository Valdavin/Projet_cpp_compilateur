#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
#include <sstream>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr) {
}

bool Interpreteur::sansErreur() {
    return m_exception.empty();
}

void Interpreteur::afficherErreur(ostream & cout) {
    cout<<endl<< " ================ Erreur Syntaxe : " << m_exception.size() << " erreur(s) trouvée(s) !" 
            << " ================" << endl;
    int i = 1;
    for (auto e : m_exception) {
        cout << i << " : " << e.what() << endl;
        i++;
    }
}

void Interpreteur::analyse() {
    m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::tester(const string & symboleAttendu) {
    // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une erreur
    try {
        if (m_lecteur.getSymbole() != symboleAttendu) {
        stringstream messageWhat;
        messageWhat << "Ligne " << m_lecteur.getLigne() << " Colonne " << m_lecteur.getColonne() << " - Erreur de syntaxe - Symbole attendu : "
                << symboleAttendu << " - Symbole trouvé : " << m_lecteur.getSymbole().getChaine();
        throw SyntaxeException(messageWhat.str());
        }
    } catch (SyntaxeException & e) { // RECUPERATION DES ERREURS - "GRAIN FIN"
        m_exception.push_back(e);
    }
    
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une erreur puis avance. 
    // Si une erreur est trouvé, le programme ne seras pas interprété.
    
    tester(symboleAttendu);
    m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const throw (SyntaxeException) {
    // Lève une exception contenant le message et le symbole courant trouvé
    // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
    stringstream messageWhat;
    messageWhat << "Ligne " << m_lecteur.getLigne() << " Colonne " << m_lecteur.getColonne() << " - Erreur de syntaxe - "
            << message << " - Symbole trouvé : " << m_lecteur.getSymbole().getChaine();
    throw SyntaxeException(messageWhat.str());
}

void Interpreteur::traduireEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(4*indentation) << "" << "int main () {" << endl;
    
    for (int i = 0; i<m_table.getTaille(); i++) { // On Déclare toute les variables utilisées
        if (m_table[i]=="<VARIABLE>") {
             cout << setw(4*(indentation+1)) << "" << "int " << m_table[i].getChaine() << ";"<<endl;
        }
       
    }
    
    getArbre()->traduitEnCPP(cout, indentation+1);
    cout << setw(4*(indentation+1)) << "" << "return 0;" << endl ;
    cout << setw(4*(indentation)) << "" << "}" << endl ;
}

Noeud* Interpreteur::programme() {
    // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    testerEtAvancer("procedure");
    testerEtAvancer("principale");
    testerEtAvancer("(");
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finproc");
    tester("<FINDEFICHIER>");
    return sequence;
}

Noeud* Interpreteur::seqInst() {
    // <seqInst> ::= <inst> { <inst> }
    NoeudSeqInst* sequence = new NoeudSeqInst();
    do {
        sequence->ajoute(inst());
    } while (m_lecteur.getSymbole() == "<VARIABLE>" || (m_lecteur.getSymbole() == "si" || m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "repeter" || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "ecrire" || m_lecteur.getSymbole() == "lire"));
    // Tant que le symbole courant est un début possible d'instruction...
    // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
    return sequence;
}

Noeud* Interpreteur::inst() {
    // <inst> ::= <affectation>  ; | <instSi>
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        Noeud *affect = affectation();
        testerEtAvancer(";");
        return affect;
    } else if (m_lecteur.getSymbole() == "si")
        return instSi();
    else if (m_lecteur.getSymbole() == "tantque")
        return instTantQue();
    else if (m_lecteur.getSymbole() == "repeter")
        return instRepeter();
    else if (m_lecteur.getSymbole() == "pour")
        return instPour();
    else if (m_lecteur.getSymbole() == "ecrire")
        return instEcrire();
    else if (m_lecteur.getSymbole() == "lire")
        return instLire();
        // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
    else erreur("Instruction incorrecte");
}

Noeud* Interpreteur::affectation() {
    // <affectation> ::= <variable> = <expression> 
    tester("<VARIABLE>");
    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
    m_lecteur.avancer();
    testerEtAvancer("=");

    Noeud* exp = expression(); // On mémorise l'expression trouvée
    return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
    // <expression> ::= <facteur> { <opBinaire> <facteur> }
    //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
    Noeud* fact = facteur();
    while (m_lecteur.getSymbole() == "+" || m_lecteur.getSymbole() == "-" ||
            m_lecteur.getSymbole() == "*" || m_lecteur.getSymbole() == "/" ||
            m_lecteur.getSymbole() == "<" || m_lecteur.getSymbole() == "<=" ||
            m_lecteur.getSymbole() == ">" || m_lecteur.getSymbole() == ">=" ||
            m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
            m_lecteur.getSymbole() == "et" || m_lecteur.getSymbole() == "ou") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* factDroit = facteur(); // On mémorise l'opérande droit
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
    }
    return fact; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::facteur() {
    // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
    Noeud* fact = nullptr;
    if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
        fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
        m_lecteur.avancer();
    } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
    } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
    } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
        m_lecteur.avancer();
        fact = expression();
        testerEtAvancer(")");
    } else
        erreur("Facteur incorrect");
    return fact;
}

Noeud* Interpreteur::instSi() {
    //<instSi> ::=si( <expression> ) <seqInst> {sinonsi( <expression> ) <seqInst> }[sinon <seqInst>]finsi
    testerEtAvancer("si");
    testerEtAvancer("(");
    Noeud* condition = expression(); // On mémorise la condition
    testerEtAvancer(")");
    Noeud* sequence = seqInst(); // On mémorise la séquence d'instruction
    vector<Noeud*> condition2;
    vector<Noeud*> sequence2;
    Noeud* condition3=NULL;
    Noeud* sequence3=NULL;
    //if(sequence3==NULL){cout<<"NULL";}else{cout<<"pasNULL";}
    while(m_lecteur.getSymbole() == "sinonsi"){
        //m_lecteur.avancer();
        testerEtAvancer("sinonsi");
        testerEtAvancer("(");
        condition2.push_back(expression()); 
        testerEtAvancer(")");
        sequence2.push_back(seqInst());
        
    }
    if(m_lecteur.getSymbole() == "sinon"){
        testerEtAvancer("sinon");
        sequence3=seqInst();
        
    }
    testerEtAvancer("finsi");
    return new NoeudInstSi(condition, sequence, condition2, sequence2, sequence3); // Et on renvoie un noeud Instruction Si
}

Noeud* Interpreteur::instTantQue() {
    //<instTantQue> ::= tantque ( <expression> ) <seqInst> fintantque
    //  return nullptr;
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expression(); // On mémorise la condition
    testerEtAvancer(")");
    Noeud* sequence = seqInst(); // On mémorise la séquence d'instruction
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition, sequence); // Et on renvoie un noeud Instruction Si
}

Noeud* Interpreteur::instRepeter() {
    //<instRepeter> ::=repeter <seqInst> jusqua( <expression> )
    //return nullptr;
    testerEtAvancer("repeter");
    Noeud* sequence = seqInst(); // On mémorise la séquence d'instruction
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expression(); // On mémorise la condition
    testerEtAvancer(")");
    return new NoeudInstRepeter(sequence, condition); // Et on renvoie un noeud Instruction Si
}

Noeud* Interpreteur::instPour() {
    // <instPour> ::= pour ( [ <affectation> ] ; <expression> ; [ <affectation> ] ) <seqInst> finpour
    testerEtAvancer("pour");
    testerEtAvancer("(");
    Noeud* affectation1 = NULL;
    if (m_lecteur.getSymbole() == "<VARIABLE>")
        affectation1 = affectation();
    testerEtAvancer(";");
    Noeud * condition = expression();
    testerEtAvancer(";");
    Noeud* affectation2 = NULL;
    if (m_lecteur.getSymbole() == "<VARIABLE>")
        affectation2 = affectation();
    testerEtAvancer(")");
Noeud* sequence = seqInst(); // On mémorise la séquence d'instruction
testerEtAvancer("finpour");
return new NoeudInstPour(affectation1, condition, affectation2, sequence); // Et on renvoie un noeud Instruction Si
}

Noeud* Interpreteur::instEcrire() {
    //<instEcrire>  ::=ecrire( <expression> | <chaine> {, <expression> | <chaine> } )
    //return nullptr;
    testerEtAvancer("ecrire");
    NoeudInstEcrire* valeur = new NoeudInstEcrire;
    //vector<String> Chaine;
    //vector<int> info;
    testerEtAvancer("(");
    if (m_lecteur.getSymbole() == "<CHAINE>") {
        valeur->ajoute(m_table.chercheAjoute(m_lecteur.getSymbole()));
        m_lecteur.avancer();
    }else {
        valeur->ajoute(expression());
    }
    while(m_lecteur.getSymbole() == ","){
        testerEtAvancer(",");
        if (m_lecteur.getSymbole() == "<CHAINE>") {
            valeur->ajoute(m_table.chercheAjoute(m_lecteur.getSymbole()));
            m_lecteur.avancer();
        }else {
            valeur->ajoute(expression());
        }
    }
    testerEtAvancer(")");    // On mémorise la séquence d'instruction
    //testerEtAvancer(";");
    return valeur; // Et on renvoie un noeud Instruction Si
}

Noeud* Interpreteur::instLire() {
    //<instLire>   ::=lire( <variable> {, <variable> } )
    testerEtAvancer("lire");
    vector<Noeud*> variable;
    testerEtAvancer("(");
        variable.push_back(expression()); // On mémorise la condition
    while(m_lecteur.getSymbole() == ","){
        testerEtAvancer(",");
        variable.push_back(expression());
    }
    testerEtAvancer(")");
    return new NoeudInstLire(variable); // Et on renvoie un noeud Instruction Si*/
}


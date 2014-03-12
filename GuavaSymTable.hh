/*
 * =====================================================================================
 *
 *       Filename:  GuavaSymTable.hh
 *
 *    Description:  Clase de tabla de simbolo para Guava.
 *
 *        Version:  1.0
 *        Created:  05/03/14 13:50:44
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ruben Serradas, Michael Woo 
 *   Organization: USB 
 *
 * =====================================================================================
 */
#include <string>
#include <stack>
#include <list>
#include <map>
#include <iostream>

/* Clase simbolo de Guava. */
class Symbol{
public:
    /**
     * Constructor de la clase Symbol.
     */
    Symbol(std::string name, std::string catg, int scop, Symbol s) {
        sym_name = name;
        sym_catg = catg;
        scope = scop;
        *type = s;
    }
    
    /**
     * Constructor de la clase Symbol.
     */
    Symbol(std::string name, std::string catg, int scop) {
        sym_name = name;
        sym_catg = catg;
        scope = scop;
    }

    /**
     * Destructor de la clase Symbol.
     * */
    ~Symbol(){
        delete type;
    }

    std::string sym_name; /* Nombre del simbolo */
    std::string sym_catg; /* Categoria del simbolo */
    int scope;            /* Identificador del scope del simbolo */
    Symbol *type;         /* Apuntador la descripcion del tipo del simbolo */


    /**
     * Se utiliza el operador == para comparar. 
     */
    bool operator==(Symbol b) {
        return ((sym_name.compare(b.sym_name) == 0) && scope == b.scope);
    }

    /** 
     * Compara nombres y alcance 
     */
    bool compare(std::string s, int sc){
        return ((sym_name.compare(s) == 0) && scope == sc);
    }

    std::string show(){
        std::string result;
        result = "Name: " + sym_name;
        result +="\nCategory: " +sym_catg;
        //result +="\ntype: " + type->sym_name;
        return result;
    }

};

/**
 * Declaracion de la clase para la tabla de simbolos.
 * */
class GuavaSymTable{
public:
    GuavaSymTable();                                      /* Constructor de la clase */
    virtual ~GuavaSymTable();                             /*  Destructor */

    void insert(Symbol elem);                             /* Inserta un simbolo a la tabla */
    int enterScope();                                     /* Entra un nuevo alcance  */
    int exitScope();                                      /* Sale del alcance  */
    Symbol* lookup(std::string elem);         /*  Busca un simbolo en la tabla y retorna NULL o el simbolo. */
    std::string show(int scope);                          /* Muestra la tabla */
    std::list<int> pila;                                 /* Pila de alcances */
    std::map<std::string, std::list<Symbol> > tabla;      /* Tabla que representa la tabla de simbolos. */
    int alcance;                                          /* Alcance en numeros. */
};

/**
 * Constructor de la clase
 */
GuavaSymTable::GuavaSymTable(){
    this->alcance = 0;
}

GuavaSymTable::~GuavaSymTable(){
}

/**
 * Inserta un simbolo en la tabla de simbolos.
 * */
void GuavaSymTable::insert(Symbol elem) {
    /* Se verifica si el simbolo ya existe en la tabla */
    if(this->tabla.count(elem.sym_name) > 0) {
        for(std::list<Symbol>::iterator it = this->tabla[elem.sym_name].begin();
            it != this->tabla[elem.sym_name].end(); it++) {
            /* Caso en el que el simbolo se encuentra en el mismo scope. */
            if(elem == *it) {
                /* EL SIMBOLO EXISTE EN ESTE SCOPE */
            }
        }
        /* Caso en el que el simbolo no existe en el scope, pero si en otros */
        this->tabla[elem.sym_name].push_back(elem);
    }
    /* Caso en el que el simbolo no pertenece a la tabla. */
    else {
        std::list<Symbol> empty_list;
        empty_list.push_front(elem);
        this->tabla[elem.sym_name] = empty_list;
    }
}
/**
 * Busca el simbolo a ser evaluado.
 * 
 * Voy revisando por toda la pila de alcances 
 * a ver si encuentro el simbolo deseado. Si 
 * el elemento con el alcance concuerdan 
 * entonces retorno un apuntador 
 * a el simbolo deseado.
 */
Symbol* GuavaSymTable::lookup(const std::string elem){
    if (!this->tabla[elem].empty()){
    for (std::list<int>::iterator pilaIt = this->pila.begin();
         pilaIt != this->pila.end() ; 
         ++pilaIt){
            int alcance = *pilaIt;
            
            std::list<Symbol>::iterator it = this->tabla[elem].begin();

            for ( it ; it != this->tabla[elem].end() ; ++it){
                Symbol tmp = *it;
                if (tmp.compare(elem, alcance)) return &(*it);
            }

        }
    }
    return 0; /* Null si no lo encuentra */
}

/**
 * Entra en un nuevo scope y empila el numero identificador del mismo.
 */
int GuavaSymTable::enterScope(){
    ++alcance;
    this->pila.push_front(alcance);
    return alcance;
}

/**
 * Sale del scope y desempila el identificador.
 */
int GuavaSymTable::exitScope(){
    if (!this->pila.empty()){
        int tmp = pila.front();
        pila.pop_front();
        return tmp;
    } else{
        return -1;
    }
}
/**
 * Retorna un string para mostrar.
 * */
std::string GuavaSymTable::show(int scope){
    std::map<std::string, std::list<Symbol> >::iterator itTabla = this->tabla.begin();
    std::string result = "";
    for (itTabla ; itTabla != this->tabla.end() ; ++itTabla){
        std::list<Symbol>::iterator itList = itTabla->second.begin();
        for (itList ; itList != itTabla->second.end() ; ++itList){
            Symbol tmp = *itList;
            result += tmp.show();
        }
    }
    return result;
}

/* No falta la funcion update? para actualizar los valores dentro de la Tabla? */

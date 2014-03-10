/*
 * =====================================================================================
 *
 *       Filename:  GuavaTree.hh
 *
 *    Description:  Clases para la construccion de arbol.
 *
 *        Version:  1.0
 *        Created:  07/03/14 23:14:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ruben Serradas (), 
 *   Organization:  USB
 *
 * =====================================================================================
 */

/**
 * PARTE DE OBSERVACIONES:
 *
 * 1- Para la regla de lvariables: tipo no puede ser void, no puede existir
 *    una variable que sea de tipo void.
 * 2- Para la regla de lcorchetes: Esta regla es para la declaracion de
 *    arreglos, estamos permitiendo que entre corchetes pueda existir una
 *    exp, solo puede ser una expresion si esta retorna un valor integer.
 *    Lo dejaremos asi y lo verificamos en la verificacion estatica de tipos?
 *    O lo cambiaremos a solo expresiones de tipo integer?
 * 3- Me tienes que explicar la regla de arreglo y larreglo.
 * 4- Podemos considerar las uniones y records como tipo o son los IDs de
 *    cada uno los que definen el nombre del tipo?
 * 5- Se debe definir de abajo hacia arriba, sino la compilacion no funcionara.
 *
 */
 
#include "GuavaSymTable.hh"



class LoopWhile: public Instruccion{
    Exp exp;
    BloqueDeclare declaraciones;
    ListaInstrucciones listainstrucciones;
    LoopWhile(Exp e, BloqueDeclare bd, ListaInstrucciones li){
        exp = e;
        declaraciones = bd;
        listainstrucciones = li;
    }
    ~LoopWhile(){}

    void show(std::string);
    void verificar(GuavaSymTable);
}

class LoopFor: public Instruccion{
public:
    std::string identificador;
    Exp exp;
    Asignacion* asignacion;
    Exp* exp2;
    BloqueDeclare declaraciones;
    ListaInstrucciones listainstrucciones;
    
    LoopFor(std::string id, Exp e1, Asignacion* a = 0,
            Exp* e2 = 0, BloqueDeclare d, ListaInstrucciones l){
                identificador = id;
                exp = e1;
                asignacion = a;
                exp2 = e2
                declaraciones = d;
                listainstrucciones = l;
            }
    ~LoopFor(){
        delete asignacion;
        delete exp2;
    }
    void show(std::string);
    void verificar(GuavaSymTable);
};

class PlusMinus: public Instruccion{
public:
    Exp exp;
    std::string tipo;
    PlusMinus(Exp e, std::string t){
        exp = e;
        tipo = t;
    }
    ~PlusMinus(){}

    void show(std::string);
    void verificar(GuavaSymTable);
};

class LVaroValor{
public:
    Exp* exp;
    LVaroValor* lvarovalor;
    LVaroValor(Exp* e = 0, LVaroValor* lv = 0){
        exp = e;
        *lvarovalor = *lv;
    }
    ~LVaroValor(){
        delete lvarovalor;
    }
    void show(std::string);
    void verificar(GuavaSymTable);
};

class LlamadaFuncion: public Instruccion{
public:
    std::string identificador;
    LVaroValor lvarovalor;
    LlamadaFuncion(std::string id, LVaroValor lv){
        identificador = id;
        lvarovalor = lv;
    }
    ~LlamadaFuncion(){}
    void show(std::string);
    void verificar(GuavaSymTable);

};

class Asignacion: public Instruccion{
    std::string identificador;
    std::string* identificador2;
    LCorchetes* lcorchetes;
    Arreglo* arreglo;
    Exp* exp;
    Asignacion(std::string id, Exp* e = 0, Lcorchetes* lc = 0 
              , std::string* id2 = 0, Arreglo* arr = 0){
                identificador = id;
                *exp = *e;
                *lcorchetes = *lc;
                *identificador2 = *id2;
                *arreglo = *arr;
              }
    ~Asignacion(){
        delete identificador2;
        delete lcorchetes;
        delete arreglo;
        delete exp;
    }
    void show(std::string);
    void verificar(GuavaSymTable);
};

/**
 * Esta clase es una "interfaz" (al estilo de java) de instruccion.
 */
class Instruccion(){
public:
    virtual void show(std::string) = 0;
    virtual void verificar(GuavaSymTable) = 0;
}

class ListaInstrucciones{
public:
    Instruccion* instruccion;
    ListaInstrucciones* listainstrucciones;

    ListaInstrucciones(Instruccion* inst = 0; ListaInstrucciones* li = 0){
        *instruccion = inst;
        *li = 0;
    }
    ~ListaInstrucciones(){
        delete instruccion;
        delete listainstrucciones;
    }

    void show(std::string);
    void verificar(GuavaSymTable);
};

class LParam{
public:
    Tipo tipo;
    std::string* identificador;

    LParam(Tipo t, std::string* id= 0 ){
        t = tipo;
        *identificador = id; 
    }
    ~LParam(){ delete identificador; }

    void show(std::string);
    void verificar(GuavaSymTable);
};

class Funcion{
public:
    Tipo tipo;
    std::string identificador;
    LParam* parametros;
    BloqueDeclare* declaraciones;
    ListaInstrucciones* listaI;
    Exp* retorno;

    Funcion(Tipo t, std::string id, LParam* param = 0, BloqueDeclare* decl = 0,
            ListaInstrucciones* li = 0, Exp * r = 0){
                tipo = t;
                *parametros = *param;
                *declaraciones = *decl;
                *listaI = *li;
                *retorno r = 0;
            }
    ~Funcion(){
        delete parametros;
        delete declaraciones;
        delete listaI;
        delete retorno;
    }
    void show(std::string);
    void verificar(GuavaSymTable);
};

class LFunciones{
public:
    Funcion funcion;
    LFunciones* lista;

    LFunciones(Funcion f, LFunciones* l = 0){
        funcion = f;
        lista = l;
    }

    ~LFunciones(){delete[] lista;}

    void show(std::string);
    void verificar(GuavaSymTable);
};

class LArreglo{
public:
    Exp* exp;
    LArreglo* larr;
    Arreglo* arr;

    LArreglo(Arreglo a, LArreglo* lar = 0){
        *arr = a;
        *exp = 0;
        *larr = *lar;
    }

    LArreglo(Exp e, LArreglo * lar = 0){
        *exp = e;
        *arr = 0;
        *larr = lar;
    }
    
    void show(std::string);
    void verificar(GuavaSymTable);
};

class Arreglo{
public:
    LArreglo listaA;
    
    Arreglo(LArreglo l){
        listaA = l;
    }
    
    ~Arreglo(){}
    
    void show(std::string);
    void verificar(GuavaSymTable);
};

class LCorchetes{
public:
    Exp exp;
    LCorchetes* lista;
    LCorchetes(Exp e, LCorchetes l){
        exp = e;
        lista* = l;
    }
    LCorchetes(Exp e){
        exp = e;
        lista = 0;
    }
    ~LCorchetes(){}
    void show(std::string);
    void verificar(GuavaSymTable);
};

class LVarArreglo{
public:
    std::string tipo;     /* Tipo */
    LCorchetes corchetes  /* Corchetes del arreglo */
    LVarArreglo lista;    /* Lista  */
    LVarArreglo(std::string t, LVar l){
        tipo = t;
        lista = l;
    }

    LVarArreglo(std::string t){
        tipo = t;
    }
    ~LVarArreglo(){}
    void show(std::string);
    void verificar(GuavaSymTable);
};

class LVar{
public:
    std::string tipo; /* Tipo del lvar */
    LVar lista;       /* lista de LVar */
    LVar(std::string t, LVar l){
        tipo = t;
        lista = l;
    }

    LVar(std::string t){
        tipo = t;
    }

    ~LVar(){}

    void show(std::string);
    void verificar(GuavaSymTable);
};

class Record{
public:
    std::string identificador;     /* Nombre del record. */
    LVariables   lista;            /* Lista de variables. */
    
    Record(std::string id, Lvariables l){
        lista = l;
        identificador = id;
    }
    ~Record(){
    }
    void show(std::string);
    void verificador(GuavaSymTable);
};

class Union{
public:
    std::string identificador;     /* Nombre del union. */
    LVariables   lista;        /* Lista de variables. */
    
    Union(std::string id, Lvariables l){
        lista = l;
        identificador = id;
    }
    ~Union(){
    }
    void show(std::string);
    void verificador(GuavaSymTable);
};

/**
 * Clase que define una lista de variables a ser declaradas.
 * No se si sea bueno, o se vea bien, colocar todos los tipos de clases que
 * puede tener LVariables dependiendo de lo que se este declarando. Si no
 * entiendes preguntame para que te lo explique mejor.
 */
class LVariables {
public:
    Tipo t;              /*Tipo de las variables a declarar */
    
    /* Aqui pensaba colocar 'Modo' para ver si son pasadas por valor o por referencia*/

    LVar listaIds;          /* Lista de identificadores de variables */
    LVarArreglo listaIdsAr; /* Lista de identificadores de variables de tipo arreglo */
    Union u;                /* Bloque que define una union */
    Record r;               /* Bloque que defina un record */
    LVariables listaVar;    /* En caso de declarar mas de una lista de variables. */

    /*Constructores*/
    /*Caso en el que se declaran variables simples*/

    LVariables(Tipo tipo, LVar v) {
        t = tipo;
        listaIds = v;
    }
    LVariables(Tipo tipo, LVar v, LVariables listaVariables) {
        t = tipo;
        listaIds = v;
        listaVar = listaVariables;
    }
    
    /*Caso en el que se declaran variables de tipo arreglo*/

    LVariables(Tipo tipo, LVarArreglo va) {
        t = tipo;
        listaIdsAr = va;
    }
    LVariables(Tipo tipo, LVarArreglo va, LVariables listaVariables) {
        t = tipo;
        listaIdsAr = va;
        listaVar = listaVariables;
    }
    
    /*Para el caso en que se declaran uniones y records, simplemente no se
     *podria poner que el Tipo t = Union o Record o el ID de la estructura?
     *VER OBSERVACION 4
     */
    
    /*Caso definicion de Uniones*/
    LVariables(Union estructura) {
        u = estructura;
    }
    LVariables(Union estructura, LVariables listaVariables) {
        u = estructura;
        listaVar = listaVariables;
    }

    /*Caso definicion de Records*/
    LVariables(Record estructura) {
        r = estructura;
    }
    LVariables(Record estructura, LVariables listaVariables) {
        r = estructura;
        listaVar = listaVariables;
    }
    ~LVariables(){}
    void show(std::string);
    
    /*Investigar: Como realizar la verificacion si los tipos de listas de
     *            variables son diferentes? No es como python que por ser
     *            debilmente tipado una misma verificacion servia para todo.
     */

    void verificacion(GuavaSymTable);
}

/**
 * Clase de bloque de declaraciones.
 */
class BloqueDeclare {
public:
    LVariables listaVar; /* Lista de variables a declarar */
    BloqueDeclare(LVariables l) {
        listaVar = l;
    }
    ~BloqueDeclare(){}
    void show(std::string);
    void verificacion(GuavaSymTable);
}

/**
 * Clase de bloque principal.
 */
class BloquePrincipal {
public:
    BloqueDeclare globalD; /* Declaraciones globales. */
    LFunciones funciones;  /* Lista de funciones. */
    BloquePrincipal(BloqueDeclare b, LFunciones l) {
        globalD = b;
        funciones = l;
    }
    ~BloquePrincipal(){}
    void show(std::string);
    void verificacion(GuavaSymTable);
};


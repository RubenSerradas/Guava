/*
 * =====================================================================================
 *
 *       Filename:  Symbol.hh
 *
 *    Description:  Contiene los headers para las clases referentes a simbolos.
 *
 *        Version:  1.0
 *        Created:  12/03/14 14:25:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ruben Serradas, Michael Woo 
 *   Organization:  
 *
 * =====================================================================================
 */

# ifndef SYMBOL_HH
# define SYMBOL_HH
#include <string>
#include <unordered_map>
#include <sstream>
#include <regex>
#include "Types.hh"
#include "Generator.hh"
#define UNDEF -1

using namespace std;

bool is_number_func(const std::string& s);




/**
 * Simbolo que solo tiene el nombre, utilizado en la generacion de codigo
 * intermedio.
 */
class SimpleSymbol{
public: 
    std::string sym_name;                  /* Nombre del simbolo */
    /* Informacion con respecto a proximo uso. */
    int proximo_uso = UNDEF ;          
    /* Informacion con respecto a la vida del simbolo. */
    bool live = true;                    

    SimpleSymbol(){}

    /** 
     * Construye un Simbolo Simple 
     * @param name Nombre del simbolo
     */
    SimpleSymbol(std::string name): sym_name(name){}

    /**
     * Convierte un numero a string y construye el simbolo
     * @param a Entero.
     */
    SimpleSymbol(int a){
        std::ostringstream convert;         
        convert << a;
        sym_name = convert.str(); 
    }

    ~SimpleSymbol(){}

    virtual bool is_simple(){ return true; }

    /** 
     * Funcion que "mata" a un simbolo, es decir, 
     * coloca los atributos tiene_uso y live en false
     */
    void kill(){
        proximo_uso = UNDEF;
        live = false;
    }

    /** 
     * Actualiza la informacion de uso y liveness.
     * @param Numero del statement. Este es el que es usado como id en cualquier GuavaQuad.
     */
    void update_use(unsigned int statement){
        live = true;
        proximo_uso = statement;
    }

    /** 
     * Operador de comparacion para SimpleSymbol.
     */
    bool operator==(SimpleSymbol s){
        return (sym_name.compare(s.sym_name) == 0);
    }

    /** 
     * Indica que un Simbolo es temporal.
     * @return true si el nombre del simbolo comienza por _t o false en el caso contrario.
     */
    bool is_temp();

    /** 
     * Nos dice si el simbolo es global.
     */
    virtual bool is_global(){
        return false;
    }

    /** 
     * Nos dice si es arreglo
     */
    virtual bool is_array(){
        return false;
    }

    virtual bool is_reg(){
        return false;
    }

    bool is_bool(){
        if (sym_name.compare(std::string("true")) == 0 || sym_name.compare(std::string("false")) == 0){
            return true;
        }
        return false;
    }

    bool is_true(){
        if (sym_name.compare(std::string("true")) == 0){
            return true;
        }
        return false;
    }

    bool is_false(){
        if (sym_name.compare(std::string("true")) == 0){
            return true;
        }
        return false;
    }

    bool is_number(){
        return is_number_func(sym_name);
    }

    bool is_bp(){
        return (sym_name.compare("bp") == 0);
    }

    virtual string bp_mips(){
        return "0($sp)";
    }

    virtual string get_mips_name();

    virtual bool is_float_reg(){
        return false;
    }


};

/** 
 * Clase que representa un Registro.
 */
class SymbolReg: public SimpleSymbol{
public:


    /** 
     * Construye un Simbolo registro con su nombre.
     * @param nombre del registro.
     */
    SymbolReg(std::string nombre): SimpleSymbol(nombre){}


    /** 
     * Construye un simbolo de registro con el lugar que ocupa en la pila
     * @param lugar lugar que ocupa en la pila
     */
    SymbolReg(int lugar): SimpleSymbol(lugar){}

    /** 
     * Destructor de la clase
     */
    ~SymbolReg(){}


    virtual bool is_reg(){
        return true;
    }

    bool is_float_reg(){
        if (sym_name.find(std::string("fp")) != string::npos) return true;
        return false;
    }

};

/** 
 * Clase simbolo de Guava.
 */
class Symbol: public SimpleSymbol{
public:

    std::string sym_catg;   /* Categoria del simbolo */
    int scope;              /* Identificador del scope del simbolo */

    int line;               /* Linea en la que fue encontrado el simbolo */ 
    int column;             /* Columna en la que fue encontrado el simbolo */ 

    TypeS* true_type;       /* Tipo correcto */
    Symbol* type_pointer;   /* Apuntador a tipo */

    int offset;             /* Offset del simbolo. */
    int width;              /* Anchura del simbolo: tamano del tipo del simbolo. */
    
    string contenido = "";  /* Contenido del string si es necesario */

    int scope_func;             /* Scope de las funciones. */

    /* Direccion relativa al desplazamiento para alcanzar un elemento dentro
     * de una estructura o arreglo.
     */   
    Symbol* desp = 0;
    /* Direccion del elemento de la estructura o arreglo buscado, necesaria 
     * para el calculo de expresiones que involucren el elemento en cuestion.
     */  
    Symbol* elem = 0;


    /**
     * Constructor para variable.
     */
    Symbol(std::string name, std::string catg, int scop, Symbol* type, int linea, int columna, int offset); // Variable que hace referencia a un tipo


    /**
     * Constructor para tipos basicos, estructuras, uniones y arreglos.
     */
    Symbol(std::string name, std::string catg,int scop, TypeS* type,int linea = 0,int columna = 0, int offset = 0);

    /**
     * Constructor basico para nombres y valores.
     */
    Symbol(std::string name);


    /** 
     * Constructor para offsets y valores enteros.
     */
    Symbol(int o): SimpleSymbol(o){}

    /**
     * Constructor para simbolo vacio.
     */
    Symbol();

    /**
     * Destructor de la clase Symbol.
     */
    ~Symbol();

    /**
     * Se utiliza el operador == para comparar. 
     */
    bool operator==(Symbol b);

    /** 
     * Compara nombres y alcance con un simbolo. 
     */
    bool compare(std::string s, int sc);
    
    /**
     * Muestra lo que tiene el Symbol. 
     */
    virtual void show(std::string);

    bool is_simple(){ return false; }

    bool is_global();

    TypeS* get_tipo();

    void generar_mips(GuavaGenerator* gen);

    string bp_mips(){
        ostringstream convert;
        convert << offset;
        return convert.str() + "($fp)";
    }

    string get_mips_name();

};
#endif

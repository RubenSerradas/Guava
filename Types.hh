/*
 * =====================================================================================
 *
 *       Filename :  Types.hh
 *
 *    Description :  Clases de tipos para el sistema de tipos de Guava.
 *
 *        Version :  1.0
 *        Created :  08/05/14 11 :17 :16
 *       Revision :  none
 *       Compiler :  gcc
 *
 *         Author :  Ruben Serradas, Michael Woo 
 *   Organization :  USB
 *
 * =====================================================================================
 */

# ifndef TYPE_S_HH
# define TYPE_S_HH

/* Aqui definimos el tamaño de todos los tipos basicos. */ 

#define SIZE_REFERENCE 4
#define SIZE_INT       4
#define SIZE_CHAR      1
#define SIZE_REAL      8
#define SIZE_BOOL      1
#define WORD           4

#include <list>
#include <unordered_map>
#include <string>


class TypeS{
public :
    virtual bool is_int() { return false; }; 
    virtual bool is_real() { return false; };
    virtual bool is_error() { return false; };
    virtual bool is_bool() { return false; };
    virtual bool is_char() { return false; };
    virtual bool is_str() { return false; };
    virtual bool is_func() { return false; };
    virtual bool is_structure() { return false; };
    virtual bool is_union() { return false; };
    virtual bool is_void() { return false; };
    virtual bool is_reference() { return false; };
    virtual bool is_array() { return false; }
    virtual TypeS* get_tipo()=0;
    virtual std::string get_name() { return "defaulttype"; };
    virtual int get_dimensiones() { return -1; };
    virtual std::list<TypeS*> get_parametros()  { std::list<TypeS*> s; return s; } 
    virtual void size_inc(){}
    virtual TypeS* get_tipoEstructura(){ return 0; }
    bool compare(TypeS*);
};

class TypeReal :public TypeS{
public :
    ~TypeReal();
    static TypeReal* Instance();
    bool is_int() ; 
    bool is_real ();
    bool is_error() ;
    bool is_bool() ;
    bool is_char() ;
    bool is_str() ;
    bool is_func() ;
    bool is_structure() ;
    bool is_union() ;
    bool is_void();
    bool is_reference();
    bool is_array();
    TypeS* get_tipo();
    std::string get_name();
    
private:
    TypeReal(){};
    static TypeReal* real_instance;
};

class TypeInt :public TypeS{
public :
    ~TypeInt();
    static TypeInt* Instance();
    bool is_int() ; 
    bool is_real ();
    bool is_error() ;
    bool is_bool() ;
    bool is_char() ;
    bool is_str() ;
    bool is_func() ;
    bool is_structure() ;
    bool is_union() ;
    bool is_void();
    bool is_reference();   
    bool is_array();   
    TypeS* get_tipo();
    std::string get_name();
    
private:
    TypeInt(){};
    static TypeInt* int_instance;
};

class TypeError :public TypeS{
public:
    ~TypeError();
    static TypeError* Instance();
    bool is_int() ; 
    bool is_real ();
    bool is_error() ;
    bool is_bool() ;
    bool is_char() ;
    bool is_str() ;
    bool is_func() ;
    bool is_structure() ;
    bool is_union() ;
    bool is_void();
    bool is_reference();
    bool is_array();   
    TypeS* get_tipo();
    std::string get_name();
private:
    TypeError(){};
    static TypeError* error_instance;
};

class TypeVoid :public TypeS{
public:
    ~TypeVoid();
    static TypeVoid* Instance();
    bool is_int() ; 
    bool is_real ();
    bool is_error() ;
    bool is_bool() ;
    bool is_char() ;
    bool is_str() ;
    bool is_func() ;
    bool is_structure() ;
    bool is_union() ;
    bool is_void();
    bool is_reference();
    bool is_array();   
    TypeS* get_tipo();
    std::string get_name();
    
private:
    TypeVoid(){};
    static TypeVoid* void_instance;
};

class TypeBool:public TypeS{
public:
    ~TypeBool();
    static TypeBool* Instance();
    bool is_int() ; 
    bool is_real ();
    bool is_error() ;
    bool is_bool() ;
    bool is_char() ;
    bool is_str() ;
    bool is_func() ;
    bool is_structure() ;
    bool is_union() ;
    bool is_void();
    bool is_reference();
    bool is_array();   
    TypeS* get_tipo();
    std::string get_name();
private:
    TypeBool(){};
    static TypeBool* bool_instance;
};

class TypeChar:public TypeS{
public:
    ~TypeChar();
    static TypeChar* Instance();
    bool is_int() ; 
    bool is_real ();
    bool is_error() ;
    bool is_bool() ;
    bool is_char() ;
    bool is_str() ;
    bool is_func() ;
    bool is_structure() ;
    bool is_union() ;
    bool is_void();
    bool is_reference();
    bool is_array();   
    TypeS* get_tipo();
    std::string get_name();
    
private:
    TypeChar(){};
    static TypeChar* char_instance;
};

class TypeString:public TypeS{
public:
    ~TypeString();
    static TypeString* Instance();
    bool is_int() ; 
    bool is_real ();
    bool is_error() ;
    bool is_bool() ;
    bool is_char() ;
    bool is_str() ;
    bool is_func() ;
    bool is_structure() ;
    bool is_union() ;
    bool is_void();
    bool is_reference();
    bool is_array();   
    TypeS* get_tipo();
    std::string get_name();
    
private:
    TypeString(){};
    static TypeString* string_instance;
};

class TypeFunction:public TypeS{
public:
    TypeS* tipo;
    std::list<TypeS*> parametros;
    TypeFunction(TypeS*,std::list<TypeS*>); 
    ~TypeFunction(); 
    bool is_int() ; 
    bool is_real ();
    bool is_error() ;
    bool is_bool() ;
    bool is_char() ;
    bool is_str() ;
    bool is_func() ;
    bool is_structure() ;
    bool is_union() ;
    bool is_void();
    bool is_reference();
    bool is_array();   
    std::list<TypeS*> get_parametros();
    TypeS* get_tipo();
    std::string get_name();
};

/**
 * Clase que representa un Tipo Arreglo de manera recursiva.
 * Cuando el tipo estructura del arreglo sea igual a 0, significa que el
 * arreglo es de 1 sola dimensión.
 */
class TypeArray:public TypeS{
public :
    TypeArray(TypeS*,TypeS*,int);
    ~TypeArray();
    //Para obtener el tipo primitivo del arreglo.
    TypeS* get_tipo();
    //Para obtener el tipo de la estructura de los elementos del arreglo.
    TypeS* get_tipoEstructura();
    std::string get_name();
    bool is_array(); 
    int get_dimensiones();
    void size_inc();
private:
    //Tipo primivito del arreglo: El que define los elementos del arreglo
    //e.g. "Los elementos del arreglo son de tipo integer".
    TypeS* tipo_primitivo;
    //Tipo que define la estructura de los elementos del arreglo.
    //e.g. "El tipo de la variable es arreglo de arreglos de integer".
    TypeS* tipo_estructura;
    int size;
};

class TypeReference: public TypeS{
public :
    TypeReference(TypeS*);
    ~TypeReference();
    bool is_int() ; 
    bool is_real ();
    bool is_error() ;
    bool is_bool() ;
    bool is_char() ;
    bool is_str() ;
    bool is_func() ;
    bool is_structure() ;
    bool is_union() ;
    bool is_void();
    bool is_reference();
    bool is_array();   
    TypeS* get_tipo();
    std::string get_name();
    
private:
    TypeS* referencia;   
};

int encajar_en_palabra(int tam);

int tamano_tipo(TypeS* t);


# endif // TYPE_S_HH

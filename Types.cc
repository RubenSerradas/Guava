/*
 * =====================================================================================
 *
 *       Filename:  Types.cc
 *
 *    Description: Implementacion de Types.hh 
 *
 *        Version:  1.0
 *        Created:  15/04/14 14:52:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author: Ruben Serradas, Michael Woo
 *   Organization: USB 
 *
 * =====================================================================================
 */

#include "Types.hh"

/* TypeS */
/** 
 * Compara dos tipos de manera "casi" estructural.
 * @return bool
 */
bool TypeS::compare(TypeS* tipo){
    if (this == 0 || tipo == 0) return false;
    //Caso en el que son arreglos.
    if (this->is_array()){
        if (tipo->is_array()){
           return (this->get_dimensiones() == tipo->get_dimensiones() &&
                   this->get_tipo()->compare(tipo->get_tipo()) &&
                   (this->get_tipoEstructura()->compare(tipo->get_tipoEstructura()) ||
                    (this->get_tipoEstructura() == 0 && tipo->get_tipoEstructura() == 0 )) 
                    );
        }else{
            return false;
        }
    //Caso en el que son estructuras.
    } else if (this->is_structure()) {
        if (tipo->is_structure()){
            // Si las estructuras tienen el mismo nombre entonces son la misma.
            return (this->get_name().compare(tipo->get_name()));
        }else{
            return false;
        }
    //Caso en el que son funciones.
    } else if (this->is_func()){
        if (tipo->is_func()){
            if (this->get_tipo()->compare(tipo->get_tipo())){
                if (this->get_parametros().size() == tipo->get_parametros().size()){
                    std::list<TypeS*>::iterator it = this->get_parametros().begin();
                    std::list<TypeS*>::iterator it_2 = tipo->get_parametros().begin();
                    while (it != this->get_parametros().end() && it_2 != tipo->get_parametros().end()){
                        if (!(*it)->compare(*it_2)) return false;
                        ++it;
                        ++it_2;
                    }
                    return true; 
                }
            }else{
                return false;
            }
        }else {
            return false;
        }
    }else{
        return (this == tipo);
    }
    return false;
}





/**
 * Apuntadores globales a las unicas instancias de las clases de cada tipo.
 */
TypeReal* TypeReal::real_instance = 0;
TypeInt* TypeInt::int_instance = 0;
TypeError* TypeError::error_instance = 0;
TypeBool* TypeBool::bool_instance = 0;
TypeChar* TypeChar::char_instance = 0;
TypeString* TypeString::string_instance = 0;
TypeVoid* TypeVoid::void_instance = 0;

/* class TypeReal  */

TypeReal* TypeReal::Instance() {
    if(real_instance == 0)
        real_instance = new TypeReal;

    return real_instance;
}

bool TypeReal::is_real()      { return true; }
bool TypeReal::is_int()       { return false; }
bool TypeReal::is_error()     { return false; }
bool TypeReal::is_bool()      { return false; }
bool TypeReal::is_char()      { return false; }
bool TypeReal::is_str()       { return false; }
bool TypeReal::is_func()      { return false; }
bool TypeReal::is_structure() { return false; }
bool TypeReal::is_union()     { return false; }
bool TypeReal::is_void()      { return false; }
bool TypeReal::is_reference() { return false; }
bool TypeReal::is_array()     { return false; }

TypeS* TypeReal::get_tipo() { return 0; }

std::string TypeReal::get_name() { return std::string("real"); }



/* class TypeInt */

TypeInt* TypeInt::Instance() {
    if(int_instance == 0)
        int_instance = new TypeInt;

    return int_instance;
}

bool TypeInt::is_real()      { return false; }
bool TypeInt::is_int()       { return true; }
bool TypeInt::is_error()     { return false; }
bool TypeInt::is_bool()      { return false; }
bool TypeInt::is_char()      { return false; }
bool TypeInt::is_str()       { return false; }
bool TypeInt::is_func()      { return false; }
bool TypeInt::is_structure() { return false; }
bool TypeInt::is_union()     { return false; }
bool TypeInt::is_void()      { return false; }
bool TypeInt::is_reference() { return false; }
bool TypeInt::is_array()     { return false; }

TypeS* TypeInt::get_tipo() { return 0; }

std::string TypeInt::get_name() { return std::string("integer"); }


/* class TypeError */

TypeError* TypeError::Instance() {
    if(error_instance == 0)
        error_instance = new TypeError;

    return error_instance;
}

bool TypeError::is_real()      { return false; }
bool TypeError::is_int()       { return false; }
bool TypeError::is_error()     { return true; }
bool TypeError::is_bool()      { return false; }
bool TypeError::is_char()      { return false; }
bool TypeError::is_str()       { return false; }
bool TypeError::is_func()      { return false; }
bool TypeError::is_structure() { return false; }
bool TypeError::is_union()     { return false; }
bool TypeError::is_void()      { return false; }
bool TypeError::is_reference() { return false; }
bool TypeError::is_array()       { return false; }

TypeS* TypeError::get_tipo() { return 0; }

std::string TypeError::get_name() { return std::string("error"); }


/* class TypeBool */

TypeBool* TypeBool::Instance() {
    if(bool_instance == 0)
        bool_instance = new TypeBool;

    return bool_instance;
}

bool TypeBool::is_real()      { return false; }
bool TypeBool::is_int()       { return false; }
bool TypeBool::is_error()     { return false; }
bool TypeBool::is_bool()      { return true; }
bool TypeBool::is_char()      { return false; }
bool TypeBool::is_str()       { return false; }
bool TypeBool::is_func()      { return false; }
bool TypeBool::is_structure() { return false; }
bool TypeBool::is_union()     { return false; }
bool TypeBool::is_void()      { return false; }
bool TypeBool::is_reference() { return false; }
bool TypeBool::is_array()     { return false; }

TypeS* TypeBool::get_tipo() { return 0; }

std::string TypeBool::get_name() { return std::string("boolean"); }


/* class TypeChar */

TypeChar* TypeChar::Instance() {
    if(char_instance == 0)
        char_instance = new TypeChar;

    return char_instance;
}
bool TypeChar::is_real()      { return false; }
bool TypeChar::is_int()       { return false; }
bool TypeChar::is_error()     { return false; }
bool TypeChar::is_bool()      { return false; }
bool TypeChar::is_char()      { return true; }
bool TypeChar::is_str()       { return false; }
bool TypeChar::is_func()      { return false; }
bool TypeChar::is_structure() { return false; }
bool TypeChar::is_union()     { return false; }
bool TypeChar::is_void()      { return false; }
bool TypeChar::is_reference() { return false; }
bool TypeChar::is_array()     { return false; }

TypeS* TypeChar::get_tipo() { return 0; }

std::string TypeChar::get_name() { return std::string("character"); }


/* class TypeString */

TypeString* TypeString::Instance() {
    if(string_instance == 0)
        string_instance = new TypeString;

    return string_instance;
}

bool TypeString::is_real()      { return false; }
bool TypeString::is_int()       { return false; }
bool TypeString::is_error()     { return false; }
bool TypeString::is_bool()      { return false; }
bool TypeString::is_char()      { return false; }
bool TypeString::is_str()       { return true; }
bool TypeString::is_func()      { return false; }
bool TypeString::is_structure() { return false; }
bool TypeString::is_union()     { return false; }
bool TypeString::is_void()      { return false; }
bool TypeString::is_reference() { return false; }
bool TypeString::is_array()     { return false; }

TypeS* TypeString::get_tipo() { return 0; }

std::string TypeString::get_name() { return std::string("string"); }



/* class TypeFunction */

TypeFunction::TypeFunction(TypeS* r,std::list<TypeS*> lp):tipo (r){

    parametros = lp;

}

bool TypeFunction::is_real()      { return false; }
bool TypeFunction::is_int()       { return false; }
bool TypeFunction::is_error()     { return false; }
bool TypeFunction::is_bool()      { return false; }
bool TypeFunction::is_char()      { return false; }
bool TypeFunction::is_str()       { return false; }
bool TypeFunction::is_func()      { return true; }
bool TypeFunction::is_structure() { return false; }
bool TypeFunction::is_union()     { return false; }
bool TypeFunction::is_void()      { return false; }
bool TypeFunction::is_reference() { return false; }
bool TypeFunction::is_array()     { return false; }

TypeS* TypeFunction::get_tipo() { return tipo; }

std::string TypeFunction::get_name() { return std::string("function"); }


std::list<TypeS*> TypeFunction::get_parametros(){
    return parametros;
}

/* class TypeVoid */

TypeVoid* TypeVoid::Instance() {
    if(void_instance == 0)
        void_instance = new TypeVoid;

    return void_instance;
}

bool TypeVoid::is_real()      { return false; }
bool TypeVoid::is_int()       { return false; }
bool TypeVoid::is_error()     { return false; }
bool TypeVoid::is_bool()      { return false; }
bool TypeVoid::is_char()      { return false; }
bool TypeVoid::is_str()       { return false; }
bool TypeVoid::is_func()      { return false; }
bool TypeVoid::is_structure() { return false; }
bool TypeVoid::is_union()     { return false; }
bool TypeVoid::is_void()      { return true; }
bool TypeVoid::is_reference() { return false; }
bool TypeVoid::is_array()     { return false; }

TypeS* TypeVoid::get_tipo() { return 0; }

std::string TypeVoid::get_name() { return std::string("void"); }



/* class TypeArray */

/**
 * @param tp Tipo primitivo del arreglo
 * @param te Tipo estructura del arreglo.
 * @param size Tamaño del arreglo
 */
TypeArray::TypeArray(TypeS* tp, TypeS* te, int s): tipo_primitivo(tp), tipo_estructura(te), size(s){}

bool TypeArray::is_array() { return true; } 

TypeS* TypeArray::get_tipo() { return tipo_primitivo; }

TypeS* TypeArray::get_tipoEstructura() { return tipo_estructura; }

/** 
 * 
 */
std::string TypeArray::get_name() { 
    if (tipo_estructura !=0) {
        return "array of " + tipo_estructura->get_name();
    }
    else {
        return tipo_primitivo->get_name();
    }
}

int TypeArray::get_dimensiones(){
    return size;
}

void TypeArray::size_inc(){
    size++;
}

/* class Reference */

TypeReference::TypeReference(TypeS* t): referencia(t){}

TypeReference::~TypeReference(){}

bool TypeReference::is_real()      { return false; }
bool TypeReference::is_int()       { return false; }
bool TypeReference::is_error()     { return false; }
bool TypeReference::is_bool()      { return false; }
bool TypeReference::is_char()      { return false; }
bool TypeReference::is_str()       { return false; }
bool TypeReference::is_func()      { return false; }
bool TypeReference::is_structure() { return false; }
bool TypeReference::is_union()     { return false; }
bool TypeReference::is_void()      { return false; }
bool TypeReference::is_reference() { return true; }
bool TypeReference::is_array()     { return false; }

TypeS* TypeReference::get_tipo() { return referencia; }

std::string TypeReference::get_name() { return std::string(""); }




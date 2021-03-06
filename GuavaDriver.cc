/*
 * =====================================================================================
 *
 *       Filename:  GuavaDriver.cc
 *
 *    Description:  Manejador de parser.
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ruben Serradas, Michael Woo
 *
 * =====================================================================================
 */
#include "GuavaDriver.hh"
#include "GuavaParser.hh"
#include "Generator.hh"


/* Variables Globales. */
int current_scope;
int attribute_scope;  
int declare_scope;
int error_state;
std::string identacion ("");
std::list<int> offset_actual;
std::list<GuavaSymTable*> tabla_actual;
int nombre_cadena  (1);
int nombre_arreglo_valor (1);
int secuencia_temporales (1);
SimpleSymbol* basepointer = new SimpleSymbol(std::string("bp"));


/**
 * Constructor de la clase GuavaDriver
 */
GuavaDriver::GuavaDriver ()
  : trace_scanning (false), trace_parsing (false)
{
    TypeS* integer = TypeInt::Instance();
    TypeS* real = TypeReal::Instance();
    TypeS* error = TypeError::Instance();
    TypeS* boolean = TypeBool::Instance();
    TypeS* character = TypeChar::Instance();
    TypeS* string = TypeString::Instance();
    TypeS* voidt = TypeVoid::Instance(); 
    
    tablaSimbolos = *(new GuavaSymTable());
    tablaSimbolos.insert_type(std::string("integer"),std::string("type"),0,integer);
    tablaSimbolos.insert_type(std::string("real"),std::string("type"),0,real);
    tablaSimbolos.insert_type(std::string("error"),std::string("type"),0,error);
    tablaSimbolos.insert_type(std::string("boolean"),std::string("type"),0,boolean);
    tablaSimbolos.insert_type(std::string("character"),std::string("type"),0,character);
    tablaSimbolos.insert_type(std::string("string"),std::string("type"),0,string);
    tablaSimbolos.insert_type(std::string("void"),std::string("type"),0,voidt);
}
/**
 * Destructor de la clase GuavaDriver 
 */
GuavaDriver::~GuavaDriver ()
{
}

/** 
 * Funcion que llama al parser.
 * @param std::string &f: Referencia a nombre del string a parsear. 
 * @return int res: Resultado del parseo.
 */
int GuavaDriver::parse (const std::string &f)
{
    file = f;
    guava_gen = new GuavaGenerator(file);
    scan_begin ();
    yy::GuavaParser parser (*this);
    parser.set_debug_level (trace_parsing);
    int res = parser.parse ();
    scan_end ();
    return res;
}
/**
 * Funcion que imprime un mensaje de error.
 * @param yy::location& l: Referencia al lugar del error.
 * @param std::string& m: Referencia a mensaje de error.
 */
void GuavaDriver::error (const yy::location& l, const std::string& m)
{
  std::cerr << l << ": " << m << std::endl;
}
/**
 * Funcion que imprime un mensaje de error.
 * @param std::string& m: Referencia a mensaje de error.
 */
void GuavaDriver::error (const std::string& m)
{
  std::cerr << m << std::endl;
}

/* Funciones Auxiliares. */

/**
 * Funcion que obtiene recursivamente el TypeS* de un
 * simbolo id.
 */ 
TypeS* obtener_tipo_simbolo(Symbol* id){
    if (id == 0) return 0;
    return id->get_tipo();
}

/**
 * Retorna un mensaje de error para la impresion y
 * pone error_state en 1
 */
std::string mensaje_estructura_error(std::string nombre){
    std::string msg2 ("The variable '");
    msg2 += nombre+"' is not a record nor an union.";
    error_state = 1;
    return msg2;
}

/**
 * Indica si una categoria es igual a unionVar
 * o recordVar
 */
bool es_estructura(std::string categoria){
    return ((categoria.compare("unionVar") == 0) || categoria.compare("recordVar") == 0);
}

/**
 * Verifica que una funcion que debe retornar algo
 * retorne algo.
 */
void funcion_sin_return(Identificador* fname,GuavaDriver* driver,const yy::location & loc){
    std::string msg ("The function ");
    msg += fname->identificador;
    msg += " doesn't return anything.";
    driver->error(loc,msg);
    error_state = 1;
}


void continue_break_libres(std::list<Instruccion*> continues,GuavaDriver* driver){
    error_state = 1;
    std::ostringstream convert_line,convert_column;
    for (std::list<Instruccion*>::iterator it = continues.begin(); it != continues.end(); ++it){
        ContinueBreak* continue_break = (ContinueBreak*) *it;
        convert_line << continue_break->line; 
        convert_column << continue_break->column; 
        std::string msg ("");
        if (continue_break->instruccion == 0){
            msg += "continue statement not within a loop at line " + convert_line.str()+ " column " +convert_column.str()+ " \n";
        }else {
            msg+= "break statement not with a loop at line " + convert_line.str()+ " column " +convert_column.str()+ " \n";
        }
        convert_line.flush();
        convert_column.flush();
        driver->error(msg);
    }
}

/**
 * Verifica que todos los returns tengan el tipo correcto.
 * Incompleto.
 */
bool verificar_return(Identificador *fname, TypeS* tipo, std::list<Instruccion*> lista ,GuavaDriver* driver){
    bool result = true;
    std::ostringstream convert;
    for (std::list<Instruccion*>::iterator it = lista.begin(); it != lista.end(); ++it){
        Instruccion* tmp = *it;
        if (tmp->get_tipo() != tipo){
            std::string msg ("Returning ");
            msg += tmp->get_tipo()->get_name();
            msg += " instead of " + tipo->get_name();
            convert << tmp->get_line();
            msg += " at line " + convert.str() ;
            convert.flush();
            convert << tmp->get_column();
            msg += " ,column "+convert.str();
            driver->error(msg);
            error_state = 1;
            result = false;
        }
    }
    return result;
}

/**
 * Reporta cuando una variable se trata de usar y esta no 
 * estaba declarada.
 */
Symbol* variable_no_declarada(std::string name, GuavaDriver* driver, const yy::location& loc, GuavaSymTable* t){
    Symbol* id;
    if ((id = t->lookup(name)) == 0) {
        std::string msg ("Undeclared identifier '");
        msg += name;
        msg += "'";
        driver->error(loc,msg);
        error_state = 1;
        return id;
    }
    return id;
}

/**
 * Reporta cuando una variable se trata de usar y esta no 
 * estaba declarada con un scope determinado.
 */
Symbol* variable_no_declarada(std::string name, GuavaDriver* driver, const yy::location& loc, int scope, GuavaSymTable* t){
    Symbol* id = 0;
    if ((id = t->lookup(name, scope )) == 0) {
        std::string msg ("Undeclared identifier '");
        msg += name;
        msg += "'";
        driver->error(loc,msg);
        error_state = 1;
        return id;
    }
    return id;
}
/**
 * Retorna el mensaje de error de que una variable ya ha sido
 * declarada.
 */
std::string reportar_existencia(Symbol *s, std::string id) {
    std::stringstream linea, columna;
    std::string msg("variable name: '");
    msg += id;
    msg += "' already used in line: ";
    linea << s->column;
    msg += columna.str();
    error_state = 1;
    return msg;
}

/**
 * Retorna el mensaje de error de un tipo ya ha sido
 * declarada.
 */
std::string reportar_existencia_tipo(Symbol *s, std::string id) {
    std::stringstream linea, columna;
    std::string msg("Type name '");
    msg += id;
    msg += "' already used in this context.";
    error_state = 1;
    return msg;
}

/**
 * Retorna un mensaje de que
 * el tipo id no existe.
 */
std::string tipo_no_existe(std::string id){
    std::string msg("Type '");
    msg += id;
    msg += "' wasn't declared or doesn't exists in current context.";
    error_state = 1;
    return msg;
}

/**
 * Retorna un mensaje de que el id
 * no es un tipo en verdad.
 */
std::string no_es_tipo(std::string id){
    std::string msg("Identifier '");
    msg += id;
    msg += "' is not a type.";
    error_state = 1;
    return msg;
}

/**
 * Retorna un mensaje de error de tipos.
 */
std::string mensaje_error_tipos(std::string esperado, std::string encontrado) {
    std::string msg ("Type error: expected '");
    msg += esperado+"' found: '"+encontrado+"'";
    error_state = 1;
    return msg;
}
/**
 * Retorna un mensaje de error cuando los dos tipos son incompatibles.
 */
std::string mensaje_diff_operandos(std::string operador, std::string op1, std::string op2, std::string expected) {
    std::string msg ("Type error: cannot use operator: '");
    msg += operador+"' with types: '"+op1+"' and '"+op2+"', expected both: '"+expected+"'";
    return msg;
}

/**
 * Funcion que dado una instancia de la clase Tipo
 * y la tabla de simbolos retorna la direccion en donde esta el tipo.
 * DEPRECADA. La d nunca se usa.
 */
Symbol* obtener_tipo(std::string str, GuavaDriver *d, GuavaSymTable* t){
    Symbol *s = t->lookup(str);
    if (s == 0) return 0;
    if (s->true_type == 0) return 0;
    return s;
}

/**
 * Funcion que dado una instancia de la clase Tipo
 * y la tabla de simbolos retorna la direccion en donde esta el tipo.
 */
Symbol* obtener_tipo(std::string str, GuavaSymTable* t){
    Symbol *s = t->lookup(str);
    if (s == 0) return 0;
    if (s->true_type == 0) return 0;
    return s;
}


/**
 * Inserta un simbolo simple.
 * $4 -> vars ; $2 -> t
 * @param vars Lista de Variables.
 * @param t Tipo de las variables que se van a insertar
 * @param estilo 
 * @param d Manejador de Guava
 * @param yy::location Lugar del simbolo.
 */
void insertar_simboloSimple(LVar *vars, TypeS *t, std::string estilo, GuavaDriver *d, const yy::location& loc) {
    std::list<Identificador> l = vars->get_list();
    int scope,line, column;
    Symbol *s;

    GuavaSymTable *tabla = tabla_actual.front();

    Symbol *p=  obtener_tipo(t->get_name(),d, &d->tablaSimbolos);
    if (p == 0) {
        d->error(loc,tipo_no_existe(t->get_name()));
        return;
    }

    for(std::list<Identificador>::iterator it = l.begin(); it!=l.end(); ++it) {
        s = tabla->simple_lookup(it->identificador);
        if(s != 0)
            d->error(loc,reportar_existencia(s,it->identificador));
        else {
            scope = tabla->currentScope();
            line = it->line;
            column = it->column;
            int offset = offset_actual.front();
            if (offset != -1) {
                offset_actual.pop_front();
                Symbol* nuevo = 0;
                /* Se verifica si se trata de variables globales o locales,
                 * lo que incidira directamente en el calculo de offset de 
                 * cada atributo.
                 */
                //Caso variables globales
                if(tabla->alcance == 1) {
                    //Caso atributos de estructuras globales
                    if(scope == 0)
                        nuevo = tabla->insert(it->identificador,estilo,scope,p,line,column,offset);
                    //Caso variables globales
                    else
                        nuevo = tabla->insert(it->identificador,estilo,scope,p,line,column,0);
                }
                //Caso variables locales
                else {
                    nuevo = tabla->insert(it->identificador,estilo,scope,p,line,column,offset);
                }
                nuevo->width = tamano_tipo(t); 
                offset += nuevo->width; 
                offset_actual.push_front(offset);
            } else {
                Symbol* nuevo = tabla->insert(it->identificador,estilo,scope,p,line,column,0);
                nuevo->width = tamano_tipo(t);
            }
        }
    }
}

/**
 * Inserta una sola variable simple.
 */
void insertar_simboloSimple(Identificador* identificador, TypeS *t, std::string estilo, GuavaDriver *d, const yy::location& loc) {
    int scope,line, column;
    Symbol *s;

    GuavaSymTable *tabla = tabla_actual.front();

    Symbol *p =  d->tablaSimbolos.lookup(t->get_name());
    if (p == 0)  d->error(loc,tipo_no_existe(t->get_name()));

    line = loc.begin.line;
    column = loc.begin.column;
    scope = tabla->currentScope();
    s = tabla->simple_lookup(identificador->identificador);
    if (s != 0) {
        d->error(loc,reportar_existencia(s,identificador->identificador));
        return;
    }
    int offset = offset_actual.front();

    if (offset != -1){
        offset_actual.pop_front();
        Symbol* nuevo = tabla->insert(identificador->identificador,estilo,scope,p,line,column,offset);
        nuevo->width = tamano_tipo(t);
        offset += nuevo->width; 
        offset_actual.push_front(offset);
    } else {
        Symbol* nuevo = tabla->insert(identificador->identificador,estilo,scope,p,line,column,0);
        nuevo->width = tamano_tipo(t);
    }
}

/**
 * Inserta una variable arreglo.
 * Cosas que usan el driver:
 * int scop = drvier.tablaSimbolos.currentScope();
 * Symbol *tmp = driver.tablaSimbolos.simple_lookup(par.first.identificador)
 * driver.tablaSimbolos.insert(it.first.identificador,std::string("array"),scop,t->tipo,line,column,arreglo,size);
 */
void insertar_simboloArreglo(LVarArreglo *vars, TypeS *t, GuavaDriver *d, const yy::location& loc) {
    std::list< std::pair<Identificador, LCorchetes> > l = vars->get_list();
    std::list<int>::reverse_iterator itInt;
    std::pair<Identificador, LCorchetes> par;
    int size, scope, line, column;
    int *arreglo;
    Symbol *s;

    GuavaSymTable *tabla = tabla_actual.front();

    for(std::list< std::pair<Identificador, LCorchetes> >::iterator it = l.begin(); it != l.end(); ++it) {
        par = *it;
        s = tabla->simple_lookup(par.first.identificador);
        if(s != 0)
            d->error(loc,reportar_existencia(s,par.first.identificador));
        else {

            //Caso base: Arreglos simples
            itInt = par.second.lista.rbegin();
            size = *itInt;
            TypeArray* tmp = 0;
            TypeArray* arr = new TypeArray(t,0,size);
            ++itInt;
            //Caso recursivo: Arreglo de arreglos
            for(itInt ; itInt != par.second.lista.rend(); ++itInt) {
                size = *itInt;
                tmp = new TypeArray(t,tmp,size);
                arr = tmp;
            }
            scope = tabla->currentScope();
            line = par.first.line;
            column = par.first.column;
            int offset = offset_actual.front();
            if (offset != -1){
                offset_actual.pop_front();
                Symbol* nuevo = tabla->insert(par.first.identificador,std::string("array"),scope,arr,line,column,offset);
                nuevo->width = tamano_tipo(arr);
                offset += nuevo->width; 
                offset_actual.push_front(offset);
            } else {
                Symbol* nuevo = tabla->insert(par.first.identificador,std::string("array"),scope,arr,line,column,offset);
                nuevo->width = tamano_tipo(arr);
            }

        }
    }
}
/**
 * Funcion que calcula el tamaño de una cadena de caracteres.
 */
int tamano_cadena(std::string cadena){
   return encajar_en_palabra(cadena.size());
}

/**
 * Funcion que inserta una cadena de caracteres en la tabla de
 * simbolos.
 * Voy a meter en la tabla de simbolos las variables _a
 */
Symbol* insertar_cadena_caracteres(std::string cadena, GuavaDriver *d, const yy::location& loc){
    int scope, line, column;
    GuavaSymTable *tabla = tabla_actual.front();
    line = loc.begin.line;
    std::ostringstream convert;
    column = loc.begin.column;
    scope = 1; //Todo string es global

    int offset = offset_actual.front();
    Symbol* nuevo;
    convert << nombre_cadena; // Variable global
    if (offset != -1){
        offset_actual.pop_front();
        nuevo = tabla->insert("_a"+convert.str(),std::string("cadena"),scope,TypeString::Instance(),line,column,offset);
        nuevo->width = tamano_cadena(cadena);
        nuevo->contenido = cadena;
        offset += nuevo->width; 
        offset_actual.push_front(offset);
    } else {
        nuevo = tabla->insert("_a"+convert.str(),std::string("cadena"),scope,TypeString::Instance(),line,column,0);
        nuevo->width = tamano_cadena(cadena);
        nuevo->contenido = cadena;
    }
    nombre_cadena++;
    return nuevo;
}

/**
 * Funcion que inserta a la tabla de simbolos un arreglo
 * de expresiones o valores. Esta es similar a insertar_cadena_caracteres.
 *
 * @param arreglo Arreglo de valores a meter en la tabla de simbolos.
 * @param d Manejador de Guava
 * @param loc Variable que contiene la información de la ubicación actual del parser.
 * @return direccion Retorna la variable en donde se encuentra el arreglo en memoria.
 */
Symbol* insertar_arreglo_valor(LArreglo* arreglo, GuavaDriver *d, const yy::location& loc){
    int scope, line, column;
    GuavaSymTable *tabla = tabla_actual.front();
    line = loc.begin.line;
    std::ostringstream convert;
    column = loc.begin.column;
    scope = tabla->currentScope();

    int offset = offset_actual.front();
    Symbol* direccion;

    convert << nombre_cadena; // Variable global
    if (offset != -1){
        offset_actual.pop_front();
        direccion = tabla->insert("_a"+convert.str(),std::string("arreglo valor"),scope,TypeString::Instance(),line,column,offset);
        direccion->width = tamano_tipo(arreglo->get_tipoEstructura()); 
        offset += direccion->width; 
        offset_actual.push_front(offset);
    } else {
        direccion = tabla->insert("_a"+convert.str(),std::string("arreglo valor"),scope,TypeString::Instance(),line,column,0);
        direccion->width = tamano_tipo(arreglo->get_tipoEstructura()); 
    }
    nombre_cadena++;
    return direccion;
}


/**
 * Dado un tipo "tipo", busca en la tabla ese tipo y
 * retorna un TypeS*.
 * Revisa los tipos basicos en la tabla de Driver.
 * Revisa otros tipos en t.
 */
TypeS* obtener_tipo_real(std::string tipo ,GuavaDriver *d, const yy::location& loc, GuavaSymTable* t){
    Symbol *p1 = obtener_tipo(tipo,d, t);

    if (p1 == 0) { 
        d->error(loc,tipo_no_existe(tipo));
        return 0;
    }
    if (p1->true_type == 0) { 
        d->error(loc,no_es_tipo(tipo));
        return 0;
    }
    if (p1 != 0 && p1->true_type != 0) return p1->true_type; 
    return 0;
}

/**
 * Funcion que retorna mensaje de tipo recursivo
 * @param t Tipo que se esta buscando
 * @return msg Mensaje final
 */
std::string reportar_tipo_recursivo(std::string t){
    std::string msg ("Cannot declare a variable of a undefined type ''");
    msg += t;
    msg += "'";
    error_state = 1;
    return msg;
}
/**
 * Inserta una lista de variables de tipo estructura a la tabla
 * y retorna el tipo de esta.
 */
TypeS* insertar_simboloEstructura(LVar *vars, std::string tipo,std::string estilo,GuavaDriver *d, const yy::location& loc){
    std::list<Identificador> l = vars->get_list();
    int scope,line, column;
    Symbol *s;

    GuavaSymTable *tabla = tabla_actual.front();
    TypeS* reference = obtener_tipo_real(tipo,d,loc, tabla);
    if (reference == 0) return 0;
    TypeS* parent = 0;

    for(std::list<Identificador>::iterator it = l.begin(); it!=l.end(); ++it) {
        s = tabla->simple_lookup(it->identificador);
        if(s != 0)
            d->error(loc,reportar_existencia(s,it->identificador));
        else if ( (parent = tabla->get_parent()) != 0) {
           if (parent->get_name() == tipo){
                d->error(loc,reportar_tipo_recursivo(tipo));
           } else {
                goto agregar;
           }
        }
        else {
        agregar:
            scope = tabla->currentScope();
            line = it->line;
            column = it->column;
            int offset = offset_actual.front();
            if (offset != -1){
                offset_actual.pop_front();
                Symbol* nuevo = 0;
                /* Se verifica si se trata de variables gloables o locales,
                 * lo que incidira directamente en el calculo de offset de
                 * cada atributo.
                 */
                //Caso variables globales
                if(tabla->alcance == 1) {
                    //Caso atributos de estructuras globales
                    if(scope == 0)
                        nuevo = tabla->insert(it->identificador,estilo,scope,reference,line,column,offset);
                    //Caso variables globales
                    else
                        nuevo = tabla->insert(it->identificador,estilo,scope,reference,line,column,0);
                }
                //Caso variables locales
                else {
                    nuevo = tabla->insert(it->identificador,estilo,scope,reference,line,column, offset);
                }
                nuevo->width = tamano_tipo(reference); 
                offset += nuevo->width; 
                offset_actual.push_front(offset);
            } else {
                Symbol* nuevo = tabla->insert(it->identificador,estilo,scope,reference,line,column, 0);
                nuevo->width = tamano_tipo(reference); 
            }
        }
    }
    return reference;
}

/**
 * Inserta una variable arreglo de estructura
 * El arreglo de estructura es un arreglo de variables que hacen
 * referencia a las estructuras. A su vez la variable arreglo
 * es una referencia al arreglo.
 */
TypeS* insertar_simboloArregloEstructura(LVarArreglo *vars, std::string t, GuavaDriver *d, const yy::location& loc) {
    std::list< std::pair<Identificador, LCorchetes> > l = vars->get_list();
    std::list<int>::reverse_iterator itInt;
    std::pair<Identificador, LCorchetes> par;
    int size, scope, line, column;
    int *arreglo;
    Symbol *s;

    GuavaSymTable *tabla = tabla_actual.front();
    TypeS* reference0 = obtener_tipo_real(t,d,loc, tabla); 
    if (reference0 == 0) return 0;
    TypeS* parent;

    for(std::list< std::pair<Identificador, LCorchetes> >::iterator it = l.begin(); it != l.end(); ++it) {
        par = *it;
        s = tabla->simple_lookup(par.first.identificador);
        if(s != 0)
            d->error(loc,reportar_existencia(s,par.first.identificador));
        else if ( (parent = tabla->get_parent()) != 0) {
           if (parent->get_name() == t){
            d->error(loc,reportar_tipo_recursivo(t));
           } else {
            goto agregar;
           }
        }
        else {
        agregar:
            size = par.second.lista.size();
            TypeArray* arr = 0;
            TypeArray* tmp = new TypeArray(reference0,0,size);
            
            for(itInt = par.second.lista.rbegin() ; itInt != par.second.lista.rend(); ++itInt) {
                size = *itInt;
                arr = new TypeArray(reference0,tmp,size);
                tmp = arr;
            }
            
            scope = tabla->currentScope();
            line = par.first.line;
            column = par.first.column;
            int offset = offset_actual.front();
            if (offset != -1){
                offset_actual.pop_front();
                Symbol* nuevo = tabla->insert(par.first.identificador,std::string("array"),scope,arr,line,column, offset);
                nuevo->width = tamano_tipo(arr);
                offset += nuevo->width; 
                offset_actual.push_front(offset);
            } else {
                Symbol* nuevo  = tabla->insert(par.first.identificador,std::string("array"),scope,arr,line,column, 0);
                nuevo->width = tamano_tipo(arr);
            }
        }
    }
    return reference0;
}
/**
 * Inserta una funcion a la tabla.
 * @param tipo          Tipo que retorna la funcion
 * @param lp            Lista de parametros.
 * @param d             Clase manejadora GuavaDriver
 * @param current_scope Entorno actual
 * @param loc           Lugar actual de la funcion.
 */
void insertar_funcion(TypeS* tipo, Identificador* id, LParam* lp ,GuavaDriver* d,int current_scope, const yy::location& loc){
    std::list<std::pair<TypeS*,Identificador*> > lista = lp->get_list();
    std::list <TypeS*> parametros;
    std::pair<TypeS*, Identificador*> par;
    for (std::list<std::pair<TypeS*,Identificador*> >::iterator it = lista.begin() ; it != lista.end(); ++it){
        par = *it;
        parametros.push_front(par.first); 
    }
    std::list<TypeS*> prueba = parametros;
    TypeFunction* function = new TypeFunction(tipo,prueba);
    GuavaSymTable *tabla = tabla_actual.front();
    Symbol *s = d->tablaSimbolos.lookup(id->identificador);
    //Se verifica la existencia de una funcion con el mismo nombre
    if (s != 0 && !s->sym_catg.compare("function")) {
        std::string msg("Function: '");
        msg += id->identificador;
        msg += "' has been already declared in the current context.";
        d->error(loc,msg);
    }
    else {
        Symbol * nuevo = tabla->insert_type(id->identificador,std::string("function"),0,function);
        nuevo->scope_func = current_scope;
    }
}

/**
 * Dereferencia un TypeS* tipo Referencia.
 * Funciona como un & en C/C++
 * @param referencia Referencia a la cual hacerle dereferencia
 */
TypeS* dereference(TypeS* referencia){
    TypeS* tmp = referencia->get_tipo();
    return tmp;
}

/**
 * Verifica la existencia de un tipo y lo agrega a la tabla. 
 * @param identificador Identificador del tipo
 * @param d Manejador GuavaDriver
 * @param loc Lugar en donde se encuentra el parser en el momento de llamar a la funcion
 * @param is_union Variable booleana que nos dice si el tipo es union o no
 */
void verificar_existencia_tipo(Identificador* id, GuavaDriver* d,const yy::location& loc, bool is_union){
    TypeStructure *structure;
    
    if (is_union) {
        structure = new TypeUnion();
        offset_actual.push_front(-1);
        GuavaSymTable* tmp = structure->get_tabla();
        tmp->set_parent(structure);
    } else {
        structure = new TypeRecord();
        offset_actual.push_front(0);
        GuavaSymTable* tmp = structure->get_tabla();
        tmp->set_parent(structure);
    }
    structure->set_name(id->identificador);
    GuavaSymTable *tabla = tabla_actual.front();
    int n = tabla->currentScope();
    identacion += "  ";
    Symbol *tmp;
    if ( (tmp = tabla->lookup(id->identificador)) == 0){
        if (tabla->get_parent() == 0){
            tabla->insert_type(id->identificador, std::string("unionType"),n,structure); 
        } else {
            if (tabla->get_parent()->get_name() == id->identificador){
                d->error(loc, reportar_existencia_tipo(tmp,id->identificador));
            } else {
                tabla->insert_type(id->identificador, std::string("unionType"),n,structure); 
            }
        }
    } else {
        d->error(loc, reportar_existencia_tipo(tmp,id->identificador));
    }

    /* Se asigna que el alcance de toda la estructura sera el mismo que el
     * alcance de su estructura padre, entendiendo alcance: El scope que rige
     * a todos sus atributos.
     */
    GuavaSymTable* struc_tabla = structure->get_tabla();
    struc_tabla->alcance = tabla->alcance;
    tabla_actual.push_front(struc_tabla);
}

/**
 * Revisa si un identificador es una variable global y le da un
 * address.
 *
 * Caso estructuras.
 */
void revision_scope_id(Symbol* id, ExpID* result, GuavaDriver* driver, const yy::location& loc, GuavaSymTable* tabla){
    //Caso variables globales y temporales de codigo intermedio
    if (id->scope == 1) {
        result->addr = id;
    } 
    //Caso variables locales y atributos de estructuras
    else {
        //Caso atributos de estructuras
        if (result->exp_id != 0) {
            /* Se copia la estructura base y a partir de ella se define la
             * posicion del atributo utilizando su offset.
             */
            std::string base = result->exp_id->addr->sym_name;
            int atr_offset = result->exp_id->addr->offset + id->offset;
            result->addr = new Symbol(base,id->sym_catg,id->scope,result->exp_id->addr->true_type,id->line,id->column,atr_offset);
            result->addr->type_pointer = id->type_pointer;

            //EN LUGAR DE CREAR UN NEW CON EL NOMBRE DE LA EXP_ID, QUE SEA UN APUNTADOR AL NOMBRE, ASI CUANDO CAMBIA EL PADRE, CAMBIAN LOS HIJOS

        }
        else {
            result->addr = new Symbol(std::string("bp"),id->sym_catg,id->scope,id->type_pointer,id->line,id->column,id->offset);
        }
    }
}

/**
 * Revisa si el atributo pertenece a una estructura global. 
 */
bool revision_scope_estructura(ExpID* result) {
    if(result->exp_id != 0)
        return revision_scope_estructura(result->exp_id);
    else
        return result->addr->is_global();
}

/**
 * Revisa si un identificador es una variable global y le da un
 * address.
 *
 * Caso variables simples.
 */
void revision_scope_id(Symbol* id, ExpID* result, GuavaDriver* driver, const yy::location& loc){
    //Caso variables globales y temporales de codigo intermedio
    if (id->scope == 1) {
        result->addr = id;
    } 
    else {
        //Caso atributos de estructuras
        if (result->exp_id != 0) {
            std::string base = result->exp_id->addr->sym_name;
            //Caso en que la estructura es global
            if(revision_scope_estructura(result))
                result->addr = new Symbol(base,id->sym_catg,id->scope,result->exp_id->addr->true_type,id->line,id->column,id->offset);
            //Caso en que la estructura es local
            else {
                int atr_offset = result->exp_id->addr->offset + id->offset;
                result->addr = new Symbol(base,id->sym_catg,id->scope,result->exp_id->addr->true_type,id->line,id->column,atr_offset);
            }
            result->addr->type_pointer = id->type_pointer;
        }
        //Caso variables locales
        else {
            /*Se hace una copia del simbolo pero indicando que lo define el
            * basepointer*/
            result->addr = new Symbol(std::string("bp"),id->sym_catg,id->scope,id->type_pointer,id->line,id->column,id->offset);
        }
    }
}

/**
 * Revisa si un identificador es una variable global y le da un
 * address
 */
void revision_scope_id(Symbol* id, Identificador* result, GuavaDriver* driver, const yy::location& loc){
    if (id->is_global()) { 
        result->addr = id;
    } else {
        result->bp = (Symbol*) basepointer;
        result->addr = NewTemp::newtemp(&secuencia_temporales,
                                id->get_tipo(),loc.begin.line,loc.begin.column,&driver->tablaSimbolos);
    }
}

/**
 * Obtencion de los quads para la clase ExpID
 * Si quieres ver el viejo codigo revisa el github
 */
std::list<GuavaQuads*>* ExpID::generar_quads(){
    return 0;
}

/**
 * Obtener quads para ExpIDLCorchetes
 */
std::list<GuavaQuads*>* ExpIDLCorchetes::generar_quads(){
    std::list<GuavaQuads*>* result = new std::list<GuavaQuads*>; 
    std::ostringstream convert;
    if (identificador == 0) return 0;

    if(exp_id != 0)
        result = exp_id->generar_quads();

    std::list<Exp*>::iterator it = lcorchetesexp->lista.begin();
    std::list<GuavaQuads*>* quads_expresion;
    Exp *exp_ini = *it;
    ++it;
    Symbol* t0;
    Symbol* t1;
    Symbol* t2;
    t0 = temp->newtemp();
    convert << tamano_tipo(tipo);
    //Se generan los quads asociados a la expresion dentro de los corchetes
    quads_expresion = exp_ini->generar_quads();
    //Se verifica que la expresion no sea un valor constante
    if (quads_expresion != 0) result->splice(result->end(),*quads_expresion);
    /*Se calcula el resultado de la expresion dentro de los corchetes por el
     *tamano del tipo del arreglo.
     */
    Symbol* width = new Symbol(convert.str());
    GuavaQuads* nuevo_q = new GuavaQuadsExp("*",exp_ini->addr,width, t0);
    result->push_back(nuevo_q);
    /**
     * Se crean las instrucciones de codigo intermedio para el calculo de
     * tamano del arreglo. Multiplicaciones para numero de elementos por fila,
     * suma para arreglos multidimensionales (columnas).
     **/
    for (it; it != lcorchetesexp->lista.end(); ++it){
        Exp* exp_ = *it;
        t1 = temp->newtemp();
        quads_expresion = exp_->generar_quads();
        if (quads_expresion != 0) result->splice(result->end(),*quads_expresion);
        GuavaQuads* nuevo_q1 = new GuavaQuadsExp("*",exp_->addr,width,t1);
        t2 = temp->newtemp();
        GuavaQuads* nuevo_q2 = new GuavaQuadsExp("+",t0,t1,t2);
        result->push_back(nuevo_q1);
        result->push_back(nuevo_q2);
        t0 = t2;
    }
    //Quad final que define la posicion del arreglo que se esta accediendo.
    GuavaQuads* nuevo_q3;
    //Caso en el que el arreglo no es global
    if (addr->sym_name.compare(std::string("bp")) == 0) {
         t1 = temp->newtemp();
        
        //Caso arreglos multidimensionales
        if (lcorchetesexp->lista.size() > 1) {
            //Se verifica si se trata de atributos de arreglo de estructuras
            if(exp_id != 0) {
                Symbol* est_padre = exp_id->addr;
                Symbol* desp_padre = 0;
                Symbol* base_arr = new Symbol(offset);
                //Temporal que contendra la posicion del arreglo (sin la base)
                Symbol* t4 = temp->newtemp();
                //Temporal que contendra la posicion del arreglo (con la base)
                Symbol* t5 = temp->newtemp();
                //Calculo de desplazamiento relativo a la estructura padre
                if(est_padre->desp != 0)
                    desp_padre = est_padre->desp;
                else
                    desp_padre = new Symbol(0);
                //Calculo de posicion final del elemento (sin la base del arreglo)
                nuevo_q3 = new GuavaQuadsExp("+",desp_padre,t2,t1);
                result->push_back(nuevo_q3);
                //Calculo de posicion final del elemento (con la base del arreglo)
                nuevo_q3 = new GuavaQuadsExp("+",t1,base_arr,t4);
                result->push_back(nuevo_q3);
                //Posicion final del elemento accedido en el arreglo
                nuevo_q3 = new GuavaQuadsExp("[]",addr,t4,t5);
                result->push_back(nuevo_q3);
                //Asignacion de nuevos indicadores de desplazamiento y elemento
                addr->desp = t4;
                addr->elem = t5;
            }
            else {
                Symbol* base_arr = new Symbol(offset);
                Symbol* t4 = temp->newtemp();
                nuevo_q3 = new GuavaQuadsExp("+",t2,base_arr,t4);
                result->push_back(nuevo_q3);
                nuevo_q3 = new GuavaQuadsExp("[]",addr,t4,t1);
                result->push_back(nuevo_q3);
                addr->desp = t4;
                addr->elem = t1;
            }
        }
        //Casp arreglos unidimensionales
        else {
            //Se verifica si se trata de atributos de arreglo de estructuras
            if(exp_id != 0) {
                Symbol* est_padre = exp_id->addr;
                Symbol* desp_padre = 0;
                Symbol* base_arr = new Symbol(offset);
                Symbol* t4 = temp->newtemp();
                Symbol* t5 = temp->newtemp();
                //Calculo de desplazamiento relativo a la estructura padre
                if(est_padre->desp != 0)
                    desp_padre = est_padre->desp;
                else
                    desp_padre = new Symbol(0);
                //Calculo de posicion final del elemento (sin la base del arreglo)
                nuevo_q3 = new GuavaQuadsExp("+",desp_padre,t0,t1);
                result->push_back(nuevo_q3);
                //Calculo de posicion final del elemento (con la base del arreglo)
                nuevo_q3 = new GuavaQuadsExp("+",t1,base_arr,t4);
                result->push_back(nuevo_q3);
                //Posicion final del elemento accedido en el arreglo
                nuevo_q3 = new GuavaQuadsExp("[]",addr,t4,t5);
                result->push_back(nuevo_q3);
                addr->desp = t4;
                addr->elem = t5;
            }
            else {
                Symbol* base_arr = new Symbol(offset);
                Symbol* t4 = temp->newtemp();
                nuevo_q3 = new GuavaQuadsExp("+",t0,base_arr,t4);
                result->push_back(nuevo_q3);
                nuevo_q3 = new GuavaQuadsExp("[]",addr,t4,t1);
                result->push_back(nuevo_q3);
                addr->desp = t4;
                addr->elem = t1;
            }
        }  
    }
    
    //REVISAR
    //Caso en el que el arreglo es global
    else {
        t1 = temp->newtemp();
        
        //Caso arreglos multidimensionales
        if (lcorchetesexp->lista.size() > 1) {
            //Se verifica si se trata de atributos de arreglo de estructuras
            if(exp_id != 0) {
                Symbol* est_padre = exp_id->addr;
                Symbol* desp_padre = 0;
                Symbol* base_arr = new Symbol(offset);
                //Temporal que contendra la posicion del arreglo (sin la base)
                Symbol* t4 = temp->newtemp();
                //Temporal que contendra la posicion del arreglo (con la base)
                Symbol* t5 = temp->newtemp();
                //Calculo de desplazamiento relativo a la estructura padre
                if(est_padre->desp != 0)
                    desp_padre = est_padre->desp;
                else
                    desp_padre = new Symbol(0);
                //Calculo de posicion final del elemento (sin la base del arreglo)
                nuevo_q3 = new GuavaQuadsExp("+",desp_padre,t2,t1);
                result->push_back(nuevo_q3);
                //Calculo de posicion final del elemento (con la base del arreglo)
                nuevo_q3 = new GuavaQuadsExp("+",t1,base_arr,t4);
                result->push_back(nuevo_q3);
                //Posicion final del elemento accedido en el arreglo
                nuevo_q3 = new GuavaQuadsExp("[]",addr,t4,t5);
                result->push_back(nuevo_q3);
                //Asignacion de nuevos indicadores de desplazamiento y elemento
                addr->desp = t4;
                addr->elem = t5;
            }
            else {
                nuevo_q3 = new GuavaQuadsExp("[]",addr,t2,t1);
                result->push_back(nuevo_q3);
                addr->desp = t2;
                addr->elem = t1;
            }
        }
        //Casp arreglos unidimensionales
        else {
            //Se verifica si se trata de atributos de arreglo de estructuras
            if(exp_id != 0) {
                Symbol* est_padre = exp_id->addr;
                Symbol* desp_padre = 0;
                Symbol* base_arr = new Symbol(offset);
                Symbol* t4 = temp->newtemp();
                Symbol* t5 = temp->newtemp();
                //Calculo de desplazamiento relativo a la estructura padre
                if(est_padre->desp != 0)
                    desp_padre = est_padre->desp;
                else
                    desp_padre = new Symbol(0);
                //Calculo de posicion final del elemento (sin la base del arreglo)
                nuevo_q3 = new GuavaQuadsExp("+",desp_padre,t0,t1);
                result->push_back(nuevo_q3);
                //Calculo de posicion final del elemento (con la base del arreglo)
                nuevo_q3 = new GuavaQuadsExp("+",t1,base_arr,t4);
                result->push_back(nuevo_q3);
                //Posicion final del elemento accedido en el arreglo
                nuevo_q3 = new GuavaQuadsExp("[]",addr,t4,t5);
                result->push_back(nuevo_q3);
                addr->desp = t4;
                addr->elem = t5;
            }
            else {
                nuevo_q3 = new GuavaQuadsExp("[]",addr,t0,t1);
                result->push_back(nuevo_q3);
                addr->desp = t0;
                addr->elem = t1;
            }
        }
    }
    
    GuavaQuads* comentario = new GuavaComment("EXP ARREGLO",line,column);
    result->push_front(comentario);
    return result;
}


/**
 * Funcion que dada una lista de quads imprime toda la lista.
 * @param lista Lista de quads.
 */
void imprimir_quads(std::list<GuavaQuads*>* lista){
    for (std::list<GuavaQuads*>::iterator it = lista->begin(); it != lista->end(); ++it){
        std::cout << (*it)->gen();
    }
}

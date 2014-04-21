%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "2.6"
/*%require "2.6.90.8-d4fe"*/
%defines
%define parser_class_name "GuavaParser"
%code requires {
# include <string>
# include <iostream>
# include <sstream>
# include <utility>
# include "GuavaTree.hh"
class GuavaDriver;
}

%parse-param { GuavaDriver& driver }
%lex-param   { GuavaDriver& driver }

%locations
%initial-action
{
  @$.begin.filename = @$.end.filename = &driver.file;
};
%debug
%error-verbose
//Simbolos
%union
{
    int          intval;
    char         *strval;
    bool         boolval;
    char         charval;
    int          subtok;
    float        realval;
    /* Clases  */
    Valor *classValor;
    Exp *classExp;
    ExpUn *classExpUn;
    ExpBin *classExpBin;
    LlamadaFuncion *classLlamadaFuncion;
    SelectorIf *classSelectorIf;
    LoopWhile *classLoopWhile;
    LoopFor *classLoopFor;
    Asignacion *classAsignacion;
    Instruccion *classInstruccion;
    ListaInstrucciones *classListaInstrucciones;
    LParam *classLParam;
    LElseIf *classLElseIf;
    Funcion *classFuncion;
    LFunciones *classLFunciones;
    LVariables *classLVariables;
    Union *classUnion;
    Record *classRecord;
    LVar *classLVar;
    LVaroValor *classLVaroValor;
    LVarArreglo *classLVarArreglo;
    Arreglo *classArreglo;
    LArreglo *classLArreglo;
    LCorchetes *classLCorchetes;
    Tipo *classTipo;
    BloqueDeclare *classBloqueDeclare;
    BloquePrincipal *classBloquePrincipal;
    EntradaSalida *classEntradaSalida;
    Identificador *classIdentificador;

    /*ExpParentizada *classExpParentizada;
    Real *classReal;
    Integer *classInteger;
    Char *classChar;
    String *classString;
    Bool *classBool;
    Estructura *classEstructura;*/
    PlusMinus *classPlusMinus;
    Program *classProgram;
};

%code {
# include "GuavaDriver.hh"
int current_scope;
int attribute_scope; 
int declare_scope;
int error_state;
std::string identacion ("");

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

/* $4 -> vars ; $2 -> t*/
void insertar_simboloSimple(LVar *vars, Tipo *t, std::string estilo, GuavaDriver *d, const yy::location& loc) {
    std::list<Identificador> l = vars->get_list();
    std::list<Identificador>::iterator it = l.begin();
    int scope,line, column;
    Symbol *s;

    Symbol *p=  d->tablaSimbolos.lookup(t->tipo);

    for(it; it!=l.end(); ++it) {
        s = d->tablaSimbolos.simple_lookup(it->identificador);
        if(s != 0)
            d->error(loc,reportar_existencia(s,it->identificador));
        else {
            scope = d->tablaSimbolos.currentScope();
            line = it->line;
            column = it->column;
            d->tablaSimbolos.insert(it->identificador,estilo,scope,p,line,column);
        }
    }
}

void insertar_simboloSimple(std::string identificador, Tipo *t, std::string estilo, GuavaDriver *d, const yy::location& loc) {
    int scope,line, column;
    Symbol *s;

    Symbol *p=  d->tablaSimbolos.lookup(t->tipo);
    line = loc.begin.line;
    column = loc.begin.column;
    scope = d->tablaSimbolos.currentScope();
    d->tablaSimbolos.insert(identificador,estilo,scope,p,line,column);
}

/* $3 -> LVarArreglo vars ; $1 -> Tipo t*/

/*Cosas que usan el driver
 *
 * int scop = drvier.tablaSimbolos.currentScope();
 * Symbol *tmp = driver.tablaSimbolos.simple_lookup(par.first.identificador)
 * driver.tablaSimbolos.insert(it.first.identificador,std::string("array"),scop,t->tipo,line,column,arreglo,size);
 */
void insertar_simboloArreglo(LVarArreglo *vars, Tipo *t, GuavaDriver *d, const yy::location& loc) {
    std::list< std::pair<Identificador, LCorchetes> > l = vars->get_list();
    std::list< std::pair<Identificador, LCorchetes> >::iterator it = l.begin();
    std::list<Integer>::iterator itInt;
    std::pair<Identificador, LCorchetes> par;
    int size, scope, line, column;
    int *arreglo;
    Symbol *s;

    for(it; it != l.end(); ++it) {
        par = *it;
        s = d->tablaSimbolos.simple_lookup(par.first.identificador);
        if(s != 0)
            d->error(loc,reportar_existencia(s,par.first.identificador));
        else {
            size = par.second.lista.size();
            arreglo = new int[size];
            itInt = par.second.lista.begin();
            for(int i=0; i != size; i++) {
                arreglo[i] = itInt->getValor();
                ++itInt;
            }
            scope = d->tablaSimbolos.currentScope();
            line = par.first.line;
            column = par.first.column;
            d->tablaSimbolos.insert(par.first.identificador,std::string("array"),scope,t->tipo,line,column,arreglo,size);
        }
    }
}

}

%token            END       0 "end of block" 
%right            ASSIGN     
%token <strval>   ID          "identifier"
%token <intval>   INTEGER     "integer value"
%token <strval>   STRING      "string"
%token <charval>  CHAR        "character"
%token <realval>  REAL        "real value"
%token <boolval>  BOOL        "boolean value"
%token TYPE_INTEGER TYPE_REAL TYPE_CHAR TYPE_VOID TYPE_BOOLEAN TYPE_STRING
%token FOR "for loop" MAIN "main function" IF "if" THEN "then" ELSE "else" WHILE "while loop" 
       DO "do clause" RETURN "return statement" BREAK "break statement" CONTINUE "continue statement"
       RECORD "record" UNION "union" REFERENCE "reference" FUNCTION "function" 
       DECLARE "declare statement" ARRAY "array"
%token PRINT "print statement" READ "read statement"
%token <operator> UFO "<=> operator" AND "and operator" OR "or operator" NOT "not operator"
                  '+' "+ operator" '-' "- operator" '*' "* operator" '/' "/ operator"
                  DIV "div operator" MOD "mod operator" PLUSPLUS "++ operator" 
                  MINUSMINUS "-- operator" POW "** operator" UMINUS "unary - operator"
%left <subtok> COMPARISON
%left UFO
%left AND
%left OR
%left NOT /* Revisar esta */
%left '+' '-'
%left '*' '/' DIV MOD
%nonassoc PLUSPLUS MINUSMINUS
%right POW
%nonassoc UMINUS
%left ','
/* Clases correspondientes. */
%type <classValor> valor
%type <classExp> exp
%type <int> expBool         /* Tipo */
%type <int> expAritmetica /* Falta el tipo para esto. */
%type <classLlamadaFuncion> llamadafuncion 
%type <classSelectorIf> selectorif selectorifLoop 
%type <classLoopWhile> loopwhile 
%type <classLoopFor> loopfor 
%type <classAsignacion> asignacion
%type <classInstruccion> instruccion instruccionLoop
%type <classInstruccion> instruccion1 instruccionLoop1
%type <classListaInstrucciones> listainstrucciones listainstruccionesLoop
%type <classLParam> lparam lparam2
%type <classLElseIf> lelseif lelseifLoop
%type <classFuncion> funcion funcionmain
%type <classLFunciones> lfunciones
%type <classLVariables> lvariables
%type <classUnion> union
%type <classIdentificador> identificador
%type <classRecord> record
%type <classLVar> lvar
%type <classLVaroValor> lvarovalor lvarovalor2
%type <classLVarArreglo> lvararreglo
%type <classArreglo> arreglo
%type <classLArreglo> larreglo
%type <classLCorchetes> lcorchetes
%type <classTipo> tipo
%type <classBloqueDeclare> bloquedeclare
%type <classBloquePrincipal> bloqueprincipal
%type <classProgram> program
%type <classEntradaSalida> entradasalida

%start program
/*%destructor { delete $$; } ID*/
%% /* Reglas */

program: bloqueprincipal { //*$$ = Program(*$1); 
                         };

bloqueprincipal: { 
                  driver.tablaSimbolos.enterScope(); 
                 } 
                 bloquedeclare lfunciones  { //*$$ = new BloquePrincipal(*$2, *$3);
                                             if (!error_state) {
                                                std::cout << "Funciones: " << '\n';
                                                driver.tablaSimbolos.show(0,identacion+ "  ");
                                                std::cout << "Variables globales: \n";
                                                driver.tablaSimbolos.show(1,identacion+ "  ");
                                             }
                                           };

bloquedeclare: /* Vacio */                { $$ = new BloqueDeclare(-1); 
                                          }
             | { declare_scope = driver.tablaSimbolos.currentScope(); 
               }
               DECLARE '{' lvariables '}' { $$ = new BloqueDeclare(declare_scope); 
                                          };

lvariables: lvariables tipo REFERENCE lvar ';'          { insertar_simboloSimple($4,$2,std::string("reference"),&driver,yylloc); }
          | tipo REFERENCE lvar ';'                     { insertar_simboloSimple($3,$1,std::string("reference"),&driver,yylloc); }
          | lvariables tipo lvar ';'                    { insertar_simboloSimple($3,$2,std::string("value"),&driver,yylloc); }
          | tipo lvar ';'                               { insertar_simboloSimple($2,$1,std::string("value"),&driver,yylloc); } 
          | tipo ARRAY lvararreglo ';'                  { insertar_simboloArreglo($3,$1,&driver,yylloc); } 
          | lvariables tipo ARRAY lvararreglo ';'       { insertar_simboloArreglo($4,$2,&driver,yylloc); }
          | identificador UNION lvar ';'                { Tipo t = Tipo($1->identificador);
                                                          insertar_simboloSimple($3,&t,std::string("unionVar"),&driver,yylloc); 
                                                        }
          | lvariables identificador UNION lvar ';'     { Tipo t = Tipo($2->identificador);
                                                          insertar_simboloSimple($4,&t,std::string("unionVar"),&driver,yylloc); }
          | identificador RECORD lvar ';'               { Tipo t = Tipo($1->identificador);
                                                          insertar_simboloSimple($3,&t,std::string("recordVar"),&driver,yylloc); 
                                                        }
          | lvariables identificador RECORD lvar ';'    { Tipo t = Tipo($2->identificador);
                                                          insertar_simboloSimple($4,&t,std::string("recordVar"),&driver,yylloc); 
                                                        }
          | lvariables  union ';'                       {}
          | lvariables  record  ';'                     {}
          | union  ';'                                  {}
          | record ';'                                  {}
          /*Errores*/
          | tipo lvar error ';'                         {/*Error en la declaracion del tipo y modo de la variable*/}
          | tipo REFERENCE lvar error ';'               {/*Caracteres inesperados luego de lista de declaraciones*/};

union: UNION identificador '{' { int n = driver.tablaSimbolos.currentScope();
                                int fsc = driver.tablaSimbolos.enterScope();
                                int line = yylloc.begin.line;
                                int column = yylloc.begin.column;
                                driver.tablaSimbolos.insert($2->identificador,std::string("union"),n,std::string("unionType"),line,column,fsc);
                                identacion += "  ";
                               }
                              lvariables '}' {  if (!error_state) {
                                                    identacion.erase(0,2);
                                                    std::cout << identacion << "Union " << $2->identificador << " {\n";
                                                    driver.tablaSimbolos.show(driver.tablaSimbolos.currentScope(),identacion+ "  "); 
                                                    std::cout << identacion <<"}\n";
                                                    driver.tablaSimbolos.exitScope(); 
                                                }
                                             }

record: RECORD identificador '{'{
                                 int n = driver.tablaSimbolos.currentScope();
                                 int fsc = driver.tablaSimbolos.enterScope();
                                 int line = yylloc.begin.line;
                                 int column = yylloc.begin.column;
                                 driver.tablaSimbolos.insert($2->identificador,std::string("record"),n,std::string("recordType"),line,column,fsc);
                                 identacion += "  ";
                                } 
                                lvariables '}' { if (!error_state) {
                                                   std::cout << identacion << "Union " << $2->identificador << " {\n";
                                                   driver.tablaSimbolos.show(driver.tablaSimbolos.currentScope(),identacion+ "  "); 
                                                   std::cout << identacion <<"}\n";
                                                   identacion.erase(0,2);
                                                   driver.tablaSimbolos.exitScope();
                                                 }
                                               }

lvar: identificador           { LVar *tmp = new LVar();
                                tmp->append(*$1);
                                $$ = tmp;
                              }
    | lvar ',' identificador  { 
                                $3->line = yylloc.begin.line;
                                $3->column = yylloc.begin.column;
                                $1->append(*$3);
                                $$ = $1;
                              }
    /*Errores*/
    | error                   { LVar *tmp = new LVar ();
                                $$ = tmp;
                              }
    | lvar ',' error          { $$ = $1; };

lvararreglo: identificador lcorchetes                  { LVarArreglo* tmp = new LVarArreglo(*$1,*$2);
                                                         $$ = tmp;
                                                       }
            | lvararreglo ',' identificador lcorchetes { $1->append(*$3,*$4);
                                                         $$ = $1;
                                                       }
            /*Errores*/
            | error lcorchetes                         { LVarArreglo* tmp = new LVarArreglo();
                                                         $$ = tmp;
                                                       }
            | lvararreglo ',' error lcorchetes         {};



lcorchetes: '[' INTEGER ']'         { 
                                      LCorchetes *nuevo =  new LCorchetes();
                                      nuevo->append(*(new Integer($2)));
                                      $$ = nuevo;
                                    }
          | lcorchetes '[' INTEGER ']' { 
                                         $1->append(*(new Integer($3)));
                                         $$ = $1; 
                                       }
          /*Errores*/
          | '[' error ']'           {/*Definicion erronea del tamano del arreglo*/}
          | lcorchetes '[' error ']' {};

lcorchetesExp: '[' exp ']'               {}
             | lcorchetesExp '[' exp ']' {}
             | '[' error ']'             {}
             |  lcorchetesExp '[' error ']' {}


lfunciones: funcionmain                    { //*$$ = LFunciones(*$2,0);
                                           }
          | lfunciones1 funcionmain        { //*$$ = LFunciones(*$2,$1);  
                                           }

lfunciones1: funcion              { //*$$ = LFunciones(*$1,0);
                                  }
           | lfunciones1 funcion  { //*$$ = LFunciones(*$2,$1);
                                  }

funcionmain: FUNCTION TYPE_VOID MAIN '(' ')' '{' { current_scope = driver.tablaSimbolos.enterScope(); 
                                                   int line = yylloc.begin.line;
                                                   int column = yylloc.begin.column;
                                                   driver.tablaSimbolos.insert(std::string("main"),std::string("function"),0,std::string("void")
                                                   ,line,column,current_scope);
                                                   identacion += "  ";
                                                 } 
                                                bloquedeclare listainstrucciones  '}' { /*Tipo v = Tipo(std::string("void"));
                                                                                        LParam lp = LParam();
                                                                                        *$$ = Funcion(v,Identificador(std::string("main")),lp,*$8,*$9,0);*/ 
                                                                                        if (!error_state) {
                                                                                          std::cout <<  "main {\n"; 
                                                                                          std::cout << "Parametros y variables:\n";
                                                                                          driver.tablaSimbolos.show(current_scope,identacion);
                                                                                          driver.tablaSimbolos.exitScope(); 
                                                                                          std::cout << "}\n";
                                                                                          identacion.erase(0,2);
                                                                                        }
                                                                                      }
/*Errores*/
/*Mala especificacion del encabezado de la funcion*/
          | FUNCTION TYPE_VOID MAIN '(' error ')' '{' { current_scope = driver.tablaSimbolos.enterScope();
                                                        int line = yylloc.begin.line;
                                                        int column = yylloc.begin.column;
                                                        driver.tablaSimbolos.insert(std::string("main"),std::string("function"),0,
                                                        std::string("void"),line,column,current_scope);
                                                        identacion += "  ";
                                                      } 
                                                     bloquedeclare listainstrucciones  '}' { /*Tipo v = Tipo(std::string("void"));
                                                                                             LParam lp = LParam();
                                                                                             *$$ = Funcion(v,Identificador(std::string("main")),lp,*$8,*$9,0);*/
                                                                                            }


funcion: FUNCTION tipo identificador '(' { current_scope = driver.tablaSimbolos.enterScope();
                                           int line = yylloc.begin.line;
                                           int column = yylloc.begin.column;
                                           driver.tablaSimbolos.insert($3->identificador,std::string("function"),0
                                                                    ,$2->tipo,line,column,current_scope);
                                           identacion += "  ";
                                         } lparam ')' '{' 
                                                       bloquedeclare listainstrucciones RETURN exp ';' '}' { /**$$ = Funcion(*$2,Identificador(std::string($3))
                                                                                                                            ,*$5,*$9,*$10,*$12);*/ 
                                                                                                             if (!error_state) {
                                                                                                               std::cout << $3->identificador << "{\n";
                                                                                                               std::cout << "Parametros y variables:\n";
                                                                                                               driver.tablaSimbolos.show(current_scope,identacion);
                                                                                                               std::cout << "}\n";
                                                                                                               driver.tablaSimbolos.exitScope();
                                                                                                               identacion.erase(0,2);
                                                                                                             }
                                                                                                           }

        | FUNCTION TYPE_VOID identificador '('  { current_scope = driver.tablaSimbolos.enterScope(); 
                                                  int line = yylloc.begin.line;
                                                  int column = yylloc.begin.column;
                                                  driver.tablaSimbolos.insert($3->identificador,std::string("function"),0
                                                                ,std::string("void"),line,column,current_scope);
                                                   identacion += "  ";
                                                 } lparam ')' '{' bloquedeclare listainstrucciones '}'     { /*Tipo v = Tipo(std::string("void"));
                                                                                                              *$$ = Funcion(v,Identificador(std::string($3)),
                                                                                                                            *$5,*$9,*$10,0);*/
                                                                                                              if (!error_state) {
                                                                                                                std::cout << $3->identificador << "{\n";
                                                                                                                std::cout << "Parametros y variables:\n";
                                                                                                                driver.tablaSimbolos.show(current_scope,identacion);
                                                                                                                std::cout << "}\n";
                                                                                                                driver.tablaSimbolos.exitScope();
                                                                                                                identacion.erase(0,2);
                                                                                                              }
                                                                                                            }

/*Errores*/
/*Mala especificacion del encabezado de la funcion*/
        | FUNCTION tipo identificador '(' error ')' '{' { current_scope =  driver.tablaSimbolos.enterScope(); 
                                                          int line = yylloc.begin.line;
                                                          int column = yylloc.begin.column;
                                                          driver.tablaSimbolos.insert($3->identificador,std::string("function"),0
                                                                    ,$2->tipo,line,column,current_scope);
                                                          identacion += "  ";
                                                        }
                                                       bloquedeclare listainstrucciones RETURN exp ';' '}' { /**$$ = Funcion(*$2,Identificador(std::string($3))
                                                                                                                             ,*$5,*$9,*$10,*$12); */ 
                                                                                                           }

/*Mala especificacion del encabezado de la funcion*/
        | FUNCTION TYPE_VOID identificador '(' error ')' '{' { current_scope = driver.tablaSimbolos.enterScope(); 
                                                               int line = yylloc.begin.line;
                                                               int column = yylloc.begin.column;
                                                               driver.tablaSimbolos.insert($3->identificador,std::string("function"),0
                                                                    ,std::string("void"),line,column,current_scope);
                                                               identacion += "  ";
                                                             }
                                                            bloquedeclare listainstrucciones '}'           { /*Tipo v = Tipo(std::string("void"));
                                                                                                             *$$ = Funcion(v,Identificador(std::string($3)),
                                                                                                             *$5,*$9,*$10,0);*/
                                                                                                            };
/*LISTO*/
lparam: /* Vacio */          { $$ = new LParam(); 
                             } 
      | lparam2              { 
                             } 

lparam2: tipo identificador               { $$ = new LParam(); 
                                            insertar_simboloSimple($2->identificador,$1,std::string("value"),&driver,yylloc);
                                          } 
        | tipo REFERENCE identificador    { $$ = new LParam();
                                            insertar_simboloSimple($3->identificador,$1,std::string("value"),&driver,yylloc);
                                          } 
        | lparam2 ',' tipo identificador  { 
                                            insertar_simboloSimple($4->identificador,$3,std::string("value"),&driver,yylloc);
                                            $$ = $1;
                                          }
        | lparam2 ',' tipo REFERENCE 
                      identificador       { 
                                            insertar_simboloSimple($5->identificador,$3,std::string("value"),&driver,yylloc);
                                            $$ = $1;
                                          }
        
        | tipo error                      {/*$$ = LParam();*/}
        | tipo REFERENCE error            {/*$$ = LParam():*/}
        | lparam2 ',' tipo error          {}
        | lparam2 ',' tipo REFERENCE error  {};


listainstrucciones: /* Vacio */                        { //*$$ = ListaInstrucciones(); 
                                                       }
                  |listainstrucciones instruccion ';'  { //*$$ = ListaInstrucciones($1,$3); 
                                                       }
                  | listainstrucciones instruccion1    { //*$$ = ListaInstrucciones($1,$2); 
                                                       };

instruccion: asignacion     { 
                            }
           | llamadafuncion { 
                            }
           | MINUSMINUS identificador  {
                                         if (driver.tablaSimbolos.lookup($2->identificador) == 0){
                                            std::string msg ("Undeclared identifier '");
                                            msg += $2->identificador;
                                            msg += "'";
                                            driver.error(yylloc,msg);
                                            error_state = 1;
                                         } else{
                                            $$ = new PlusMinus(*$2, 0);
                                         }
                                       }
           | identificador MINUSMINUS { if (driver.tablaSimbolos.lookup($1->identificador) == 0){
                                            std::string msg ("Undeclared identifier '");
                                            msg += $1->identificador;
                                            msg += "'";
                                            driver.error(yylloc,msg);
                                            error_state = 1;
                                         } else{
                                            $$ = new PlusMinus(*$1, 1); 
                                         }
                                       }
           | PLUSPLUS identificador    { if (driver.tablaSimbolos.lookup($2->identificador) == 0){
                                            std::string msg ("Undeclared identifier '");
                                            msg += $2->identificador;
                                            msg += "'";
                                            driver.error(yylloc,msg);
                                            error_state = 1;
                                         } else{
                                            $$ = new PlusMinus(*$2, 2); 
                                         }
                                       }
           | identificador PLUSPLUS    { if (driver.tablaSimbolos.lookup($1->identificador) == 0){
                                            std::string msg ("Undeclared identifier '");
                                            msg += $1->identificador;
                                            msg += "'";
                                            driver.error(yylloc,msg);
                                            error_state = 1;
                                         } else{
                                            $$ = new PlusMinus(*$1, 3); 
                                         }
                                       }
           | entradasalida  { 
                            }
           | CONTINUE       {
                            }
           | BREAK          {
                            }
           /*Errores*/
           | error identificador       {/*Error en la especificacion del incremento o decremento*/}
           | identificador error       {/*Error en la especificacion del incremento o decremento*/}



instruccion1: loopfor        { 
                             }
            | loopwhile      { 
                             }
            | selectorif     { 
                             };

listainstruccionesLoop: /* Vacio */                                {}
                      | listainstruccionesLoop instruccionLoop ';' {}
                      | listainstruccionesLoop instruccionLoop1    {} 

instruccionLoop: asignacion     { 
                                }
           | llamadafuncion     { 
                                }
           | MINUSMINUS identificador  {
                                         if (driver.tablaSimbolos.lookup($2->identificador) == 0){
                                            std::string msg ("Undeclared identifier '");
                                            msg += $2->identificador;
                                            msg += "'";
                                            driver.error(yylloc,msg);
                                            error_state = 1;
                                         } else{
                                            $$ = new PlusMinus(*$2, 0);
                                         }
                                       }
           | identificador MINUSMINUS { if (driver.tablaSimbolos.lookup($1->identificador) == 0){
                                            std::string msg ("Undeclared identifier '");
                                            msg += $1->identificador;
                                            msg += "'";
                                            driver.error(yylloc,msg);
                                            error_state = 1;
                                         } else{
                                            $$ = new PlusMinus(*$1, 1); 
                                         }
                                       }
           | PLUSPLUS identificador    { if (driver.tablaSimbolos.lookup($2->identificador) == 0){
                                            std::string msg ("Undeclared identifier '");
                                            msg += $2->identificador;
                                            msg += "'";
                                            driver.error(yylloc,msg);
                                            error_state = 1;
                                         } else{
                                            $$ = new PlusMinus(*$2, 2); 
                                         }
                                       }
           | identificador PLUSPLUS    { if (driver.tablaSimbolos.lookup($1->identificador) == 0){
                                            std::string msg ("Undeclared identifier '");
                                            msg += $1->identificador;
                                            msg += "'";
                                            driver.error(yylloc,msg);
                                            error_state = 1;
                                         } else{
                                            $$ = new PlusMinus(*$1, 3); 
                                         }
                                       }
           | entradasalida  { 
                            }
           | CONTINUE       {
                            }
           | BREAK          {
                            }
           /*Errores*/
           | error identificador       {/*Error en la especificacion del incremento o decremento*/}
           | identificador error       {/*Error en la especificacion del incremento o decremento*/}


instruccionLoop1: loopfor        { 
                                 }
                | loopwhile      { 
                                 }
                | selectorifLoop { 
                                 };



asignacion: expID ASSIGN exp  {
                              } 
            /*Errores*/
            | error ASSIGN exp  {
                                }
            | expID ASSIGN error {};


/*Esto hay que cambiarlo.*/
entradasalida: READ '(' lvarovalor ')' { //*$$ = EntradaSalida(0, *$3); 
                                       }
             | PRINT '(' lvarovalor ')'  { //*$$ = EntradaSalida(1, *$3); 
                                         };

loopfor: FOR '(' identificador ';' expBool ';' errorloopfor ')' '{' { if (driver.tablaSimbolos.lookup($3->identificador) == 0){
                                                                      std::string msg ("Undeclared identifier '");
                                                                      msg += $3->identificador;
                                                                      msg += "'";
                                                                      driver.error(yylloc,msg);
                                                                      error_state = 1;
                                                                  } 
                                                                  driver.tablaSimbolos.enterScope();   
                                                                  identacion += "  "; 
                                                                }
                                                            bloquedeclare listainstruccionesLoop '}' { /*Identificador id = Identificador(std::string($3));
                                                                                                    *$$ = LoopFor(id,$5,*$7,*$11,*$12);*/ 
                                                                                                    if (!error_state) {
                                                                                                      std::cout << identacion << "for {\n";
                                                                                                      int cscope = driver.tablaSimbolos.currentScope();
                                                                                                      driver.tablaSimbolos.show(cscope,identacion+ "  ");
                                                                                                      std::cout << identacion << "}\n";
                                                                                                      driver.tablaSimbolos.exitScope();
                                                                                                      identacion.erase(0,2);
                                                                                                    }
                                                                                                 }
       /*Errores*/
       | FOR '(' error ';' expBool ';' errorloopfor ')' '{' { 
                                                        }
                                                      bloquedeclare listainstruccionesLoop '}' { /*Identificador id = Identificador(std::string($3));
                                                                                              *$$ = LoopFor(id,$5,*$7,*$11,*$12);*/ 
                                                                                           }
       | FOR '(' identificador ';' error  ';' errorloopfor ')' '{' { if (driver.tablaSimbolos.lookup($3->identificador) == 0){
                                                                        std::string msg ("Undeclared identifier '");
                                                                        msg += $3->identificador;
                                                                        msg += "'";
                                                                        driver.error(yylloc,msg);
                                                                        error_state = 1;
                                                                     }
                                                                   }
                                                                 bloquedeclare listainstruccionesLoop '}' { /*Identificador id = Identificador(std::string($3));
                                                                                                         *$$ = LoopFor(id,$5,*$7,*$11,*$12);*/ 
                                                                                                      };
/**
 * Regla utilizada en el manejo de errores del encabezado de una iteracion
 * acotada
 */
errorloopfor : asignacion {}
             | exp        {}
             | error      {};


/*LISTO*/
loopwhile: WHILE '(' errorloopwhile ')' DO '{' { 
                                                 driver.tablaSimbolos.enterScope();   
                                                 identacion += "  ";
                                               }
                                             bloquedeclare listainstruccionesLoop '}' { /**$$ = LoopWhile($3,*$8,*$9); */
                                                                                    if (!error_state) {
                                                                                      std::cout << identacion << "while {\n"; 
                                                                                      driver.tablaSimbolos.show(driver.tablaSimbolos.currentScope(),identacion+"  ");
                                                                                      std::cout << identacion << "}\n ";
                                                                                      driver.tablaSimbolos.exitScope();
                                                                                      identacion.erase(0,2);
                                                                                    }
                                                                                  }
         | DO '{' { 
                   driver.tablaSimbolos.enterScope();   
                   identacion += "  ";
                  } 
                   bloquedeclare listainstruccionesLoop '}' WHILE '(' errorloopwhile ')' { /**$$ = LoopWhile($9,*$4,*$5); */
                                                                                       if (!error_state) {
                                                                                         std::cout << identacion << "while {\n";
                                                                                         driver.tablaSimbolos.show(driver.tablaSimbolos.currentScope(),identacion+"  ");
                                                                                         std::cout << identacion << "}\n ";

                                                                                         driver.tablaSimbolos.exitScope();
                                                                                         identacion.erase(0,2);
                                                                                       }
                                                                                     };
/**
 * Regla utilizada para el manejo de errores en iteraciones indeterminadas.
 */
errorloopwhile : expBool    {}
               | error  {};

selectorif: IF '(' errorif ')' THEN '{' { 
                                          driver.tablaSimbolos.enterScope();   
                                          identacion += "  ";
                                        }
                                        bloquedeclare listainstrucciones '}' lelseif { /**$$ = SelectorIf($3,$8,$9,$11);*/
                                                                                        driver.tablaSimbolos.exitScope();
                                                                                        identacion.erase(0,2);
                                                                                    
                                                                                 }
           | IF '(' errorif ')' THEN instruccion ';'                             { //*$$ = SelectorIf($3,$6,0); 
                                                                                 }
           | IF '(' errorif ')' THEN instruccion ELSE instruccion ';'            { //*$$ = SelectorIf($3,$6,$8); 
                                                                                 };


lelseif: /* Vacio */                                                               { //*$$ = LElseIf(); 
                                                                                   }
       | lelseif1 ELSE '{' { 
                             driver.tablaSimbolos.enterScope();   
                           }
                           bloquedeclare listainstrucciones '}'                              { /**$$ = LElseIf(*$4,*$5); */
                                                                                               driver.tablaSimbolos.exitScope();
                                                                                             };

lelseif1: /*Vacio*/
        | lelseif1 ELSE IF '(' errorif ')' THEN '{' { 
                                                      driver.tablaSimbolos.enterScope();   
                                                    }
                                                    bloquedeclare listainstrucciones '}' { /**$$ = LElseIf($4,*$9,*$10,$12);*/
                                                                                           driver.tablaSimbolos.exitScope();
                                                                                         };


selectorifLoop: IF '(' errorif ')' THEN '{' { 
                                              driver.tablaSimbolos.enterScope();   
                                              identacion += "  ";
                                            }
                                           bloquedeclare listainstruccionesLoop '}' lelseifLoop { /**$$ = SelectorIf($3,$8,$9,$11);*/
                                                                                                  driver.tablaSimbolos.exitScope();
                                                                                                  identacion.erase(0,2);
                                                                                                }
           | IF '(' errorif ')' THEN instruccionLoop ';'                                        { 
                                                                                                 //*$$ = SelectorIf($3,$6,0); 
                                                                                                }
           | IF '(' errorif ')' THEN instruccion ELSE instruccionLoop ';'                       { //*$$ = SelectorIf($3,$6,$8); 
                                                                                                };


lelseifLoop: /* Vacio */                                                               { //*$$ = LElseIf(); 
                                                                                   }
           | lelseifLoop1 ELSE '{' { 
                                   driver.tablaSimbolos.enterScope();   
                                   }
                                 bloquedeclare listainstruccionesLoop '}'  { 
                                                                             driver.tablaSimbolos.exitScope();
                                                                           };

lelseifLoop1: /*Vacio*/
        | lelseifLoop1 ELSE IF '(' errorif ')' THEN '{' { 
                                                          driver.tablaSimbolos.enterScope();   
                                                        }
                                                      bloquedeclare listainstruccionesLoop '}' { /**$$ = LElseIf($4,*$9,*$10,$12);*/
                                                                                                driver.tablaSimbolos.exitScope();
                                                                                               };

/**
 * Regla utilizada para el manejo de errores de los selectores de bloques e
 * instrucciones if-then-else.
 */
errorif : expBool   {}
        | error     {};

llamadafuncion: identificador '(' lvarovalor ')' { //*$$ = LlamadaFuncion(Identificador(std::string($1)),*$3); 
                                                    if (driver.tablaSimbolos.lookup($1->identificador,0) == 0){
                                                        std::string msg ("Undefined function '");
                                                        msg += $1->identificador;
                                                        msg += "'";
                                                        driver.error(yylloc,msg);
                                                        error_state = 1;
                                                    }
                                                 }
              | error '(' lvarovalor ')'         {/*Llamado a una funcion con identificador erroneo*/};


lvarovalor: /* Vacio */   { 
                            $$ = new LVaroValor(); 
                          }
          | lvarovalor2   { $$ = $1;
                          };      
          
lvarovalor2: lvarovalor2 ',' exp     { 
                                      $1->append(*$3);
                                      $$ = $1;
                                     }
           | exp                     {
                                       LVaroValor *tmp = new LVaroValor();
                                       tmp->append(*$1);
                                       $$ = tmp; 
                                     }
           | lvarovalor2 ',' error   {}
           | error                   {
                                       LVaroValor *tmp = new LVaroValor();
                                       $$ = tmp;
                                     };

exp: expAritmetica       {  
                         }
   | expBool             {  
                         } 
   | valor        {  
                  }
   | expID        {} 
   | '(' exp ')'  { 
                  }
   | '(' error ')'  {}
   | llamadafuncion {};
      

expID: identificador    { if (driver.tablaSimbolos.lookup($1->identificador) == 0){
                            std::string msg ("Undeclared identifier '");
                            msg += $1->identificador;
                            msg += "'";
                            driver.error(yylloc,msg);
                            error_state = 1;
                        }
                       }
     | identificador lcorchetesExp     { Symbol * id;
                                        if ((id = driver.tablaSimbolos.lookup($1->identificador)) == 0){
                                            std::string msg ("Undeclared identifier '");
                                            msg += $1->identificador;
                                            msg += "'";
                                            driver.error(yylloc,msg);
                                            error_state = 1;
                                        }
                                      }
      | identificador { Symbol * id;
                     if ((id = driver.tablaSimbolos.lookup($1->identificador)) == 0){
                        std::string msg ("Undeclared identifier '");
                        msg += $1->identificador;
                        msg += "'";
                        driver.error(yylloc,msg);
                        error_state = 1;
                      } else {
                            if ((id->sym_catg.compare("unionVar") != 0) && id->sym_catg.compare("recordVar") != 0){
                            std::string msg2 ("The ");
                            msg2 += id->sym_catg;
                            msg2 += "  ";
                            msg2 += id->sym_name;
                            msg2 += " is not a record nor an union";
                            driver.error(yylloc,msg2);
                            error_state = 1;
                        } else {
                            // DEBE HACERSE DE OTRA MANERA CON EL CAMBIO DE TIPOS
                            //Symbol* structure;
                            //structure = driver.tablaSimbolos.lookup(id->type);
                            //attribute_scope = structure->fieldScope;
                        }
                      } 

                    } lAccesoAtributos {
                                       };

 


/*Faltan pruebas*/
expBool: exp AND exp          { 
                             }
      | exp OR exp           { 
                             }
      | exp COMPARISON exp   { int cmpv = $2;
                               //ExpBin *eb;
                               switch(cmpv){
                                    case 1:
                                        //eb = new ExpBin(*$1,*$3,std::string(">"));
                                        //$$ = eb;
                                        break;
                                    case 2:
                                        //eb = new ExpBin(*$1,*$3,std::string("<"));
                                        //$$ = eb;
                                        break;
                                    case 3:
                                        //eb = new ExpBin(*$1,*$3,std::string("<="));
                                        //$$ = eb;
                                        break;
                                    case 4:
                                        //eb = new ExpBin(*$1,*$3,std::string(">="));
                                        //$$ = eb;
                                        break;
                                    case 5:
                                        //eb = new ExpBin(*$1,*$3,std::string("="));
                                        //$$ = eb;
                                        break;
                                    case 6:
                                        //eb = new ExpBin(*$1,*$3,std::string("!="));
                                        //$$ = eb;
                                        break;
                               }
                             }
      | NOT exp               { 
                             };
      


expAritmetica: '-' exp %prec UMINUS  {  
                                     }
             | exp PLUSPLUS          {  
                                     }
             | exp MINUSMINUS        { 
                                     }
             | exp UFO exp           { 
                                     }
             | exp '+' exp           { 
                                     }
             | exp '-' exp           { 
                                     }
             | exp '*' exp           { 
                                     }
             | exp '/' exp           { 
                                     }
             | exp DIV exp           { 
                                     }
             | exp MOD exp           { 
                                     }
             | exp POW exp           { 
                                     };

/*Funciona*/
valor: BOOL     { 
                  $$ = new Bool($1);
                }
     | STRING   { 
                  $$ = new String($1);
                }
     | CHAR     { 
                  $$ = new Char($1);
                }
     | INTEGER  { 
                  $$ = new Integer($1);
                }
     | REAL     { 
                  $$ = new Real($1);
                }
     | arreglo  {
                  $$ = $1;
                }

/*Funciona*/
tipo: TYPE_REAL     { 
                      $$ = new Tipo(*(new std::string("real")));
                    }
     | TYPE_INTEGER { 
                      $$ = new Tipo(*(new std::string("integer")));
                    }
     | TYPE_BOOLEAN { 
                      $$ = new Tipo(*(new std::string("boolean")));
                    }
     | TYPE_CHAR    { 
                      $$ = new Tipo(*(new std::string("character")));
                    }
     | TYPE_STRING  { 
                      $$ = new Tipo(*(new std::string("string")));
                    };


/*Funciona*/
arreglo: '[' larreglo ']' {
                            Arreglo* tmp;
                            LArreglo *lr = $2;
                            tmp = new Arreglo(lr);
                            $$ = tmp; 
                          };

/*Funciona. Faltan ejemplos mas interesantes.*/

larreglo: larreglo ',' exp      { 
                                  $1->append(*$3);
                                  $$ = $1;
                                }
        | exp                   { 
                                  LArreglo *tmp = new LArreglo();
                                  tmp->append(*$1);
                                  $$ = tmp;
                                 
                                }
        /*Errores*/
        | larreglo ',' error    { $$ = $1; }
        | error                 { LArreglo *tmp = new LArreglo(); };

lAccesoAtributos: '.' identificador { Symbol* id;
                                      if ((id = driver.tablaSimbolos.lookup($2->identificador,attribute_scope)) == 0){
                                            std::string msg ("Undeclared identifier '");
                                            msg += $2->identificador;
                                            msg += "'";
                                            driver.error(yylloc,msg);
                                            error_state = 1;               
                                     } else if ((id->sym_catg.compare("unionVar") == 0) || id->sym_catg.compare("recordVar") == 0){
                                            Symbol* structure;
                                            structure = driver.tablaSimbolos.lookup(id->type,attribute_scope);
                                            if (structure != 0) attribute_scope = structure->fieldScope;
                                     }
                                    }
                | lAccesoAtributos '.' identificador {
                                                        Symbol* id;
                                                        if ((id = driver.tablaSimbolos.lookup($3->identificador,attribute_scope)) == 0){
                                                            std::string msg ("Undeclared identifier '");
                                                            msg += $3->identificador;
                                                            msg += "'";
                                                            driver.error(yylloc,msg);
                                                            error_state = 1;               
                                                        } else if ((id->sym_catg.compare("unionVar") == 0) || id->sym_catg.compare("recordVar") == 0){
                                                            Symbol* structure;
                                                            structure = driver.tablaSimbolos.lookup(id->type,attribute_scope);
                                                            if (structure != 0)  attribute_scope = structure->fieldScope;
                                                        }

                                                     };

identificador: ID { std::string str =  std::string($1);
                    Identificador* id = new Identificador(str);
                    id->line = yylloc.begin.line;
                    id->column = yylloc.begin.column;
                    $$ = id;
                  };


%%

void yy::GuavaParser::error (const yy::GuavaParser::location_type& l, const std::string& m)
{
  if(!error_state)
    error_state = 1;

  driver.error (l, m);
}

%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "2.6"
/*%require "2.6.90.8-d4fe"*/
%defines
%define parser_class_name "GuavaParser"
%code requires {
# include <string>
# include <iostream>
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
}

%token            END       0 
%right            ASSIGN     
%token <strval>   ID
%token <intval>   INTEGER    
%token <strval>   STRING
%token <charval>  CHAR       
%token <realval>  REAL       
%token <boolval>  BOOL       
%token TYPE_INTEGER TYPE_REAL TYPE_CHAR TYPE_VOID TYPE_BOOLEAN TYPE_STRING
%token FOR MAIN IF THEN ELSE WHILE DO RETURN BREAK CONTINUE RECORD UNION VAR FUNCTION DECLARE ARRAY
%token PRINT READ
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
%type <classExpUn> expun
%type <classExpBin> expbin
%type <classLlamadaFuncion> llamadafuncion 
%type <classSelectorIf> selectorif 
%type <classLoopWhile> loopwhile 
%type <classLoopFor> loopfor 
%type <classAsignacion> asignacion
%type <classInstruccion> instruccion
%type <classInstruccion> instruccion1
%type <classListaInstrucciones> listainstrucciones
%type <classLParam> lparam lparam2
%type <classLElseIf> lelseif
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
                 bloquedeclare lfunciones  { //*$$ = BloquePrincipal(*$2, *$3); 
                                           };

bloquedeclare: /* Vacio */                { $$ = new BloqueDeclare(); 
                                          }
             | DECLARE '{' lvariables '}' { $$ = new BloqueDeclare(*$3); 
                                          };

lvariables: tipo VAR lvar ';' lvariables                {
                                                            std::list<Identificador> l = $3->get_list();
                                                            std::list<Identificador>::iterator it = l.begin();
                                                            int scop = driver.tablaSimbolos.alcance;
                                                            for (it ; it != l.end(); ++it){
                                                               driver.tablaSimbolos.insert(it->identificador,std::string("var"),scop,$1->tipo); 
                                                            }
                                                            driver.tablaSimbolos.show(scop,std::string(""));
                                                        }
          | tipo VAR lvar ';'                           { 
                                                            std::list<Identificador> l = $3->get_list();
                                                            std::list<Identificador>::iterator it = l.begin();
                                                            int scop = driver.tablaSimbolos.alcance;
                                                            for (it ; it != l.end(); ++it){
                                                               driver.tablaSimbolos.insert(it->identificador,std::string("var"),scop,$1->tipo); 
                                                            }
                                                            driver.tablaSimbolos.show(scop,std::string(""));
                                                        }
          | tipo ARRAY lvararreglo ';'                  { 
                                                        }
          | tipo ARRAY lvararreglo ';' lvariables       { 
                                                        }
          | identificador   UNION lvar ';'              {}
          | identificador   UNION lvar ';' lvariables   {}
          | identificador   RECORD lvar ';'             {}
          | identificador   RECORD lvar ';' lvariables  {}
          | union ';' lvariables                        { 
                                                        }
          | record ';' lvariables                       { 
                                                        }
          | union  ';'                                  { 
                                                        }
          | record ';'                                  { 
                                                        }
          /*Errores*/
          | tipo lvar error ';'                         {/*Error en la declaracion del tipo y modo de la variable*/}
          | tipo VAR lvar error ';'                     {/*Caracteres inesperados luego de lista de declaraciones*/}

union: UNION identificador '{' lvariables '}' { //*$$ = Union(Identificador(std::string($2)), $4); 
                                              }
       /*Errores*/
       | UNION identificador '{' error '}'    {/*Definicion erronea de la estructura*/}; //OJO con este error y el de las reglas de lvariables

record: RECORD identificador '{' lvariables '}' { //*$$ = Record(Identificador(std::string($2)), $4); 
                                                }
       /*Errores*/
       | RECORD identificador '{' error '}'     {/*Definicion erronea de la estructura*/};

lvar: identificador           { LVar *tmp = new LVar();
                                tmp->append(*$1);
                                $$ = tmp;
                              }
    | lvar ',' identificador  { 
                                $1->append(*$3);
                                $$ = $1;
                              };

lvararreglo: identificador lcorchetes                 { //*$$ = LVarArreglo(Identificador(std::string($1)), $2, 0); 
                                                      }
            | identificador lcorchetes ',' lvararreglo { //*$$ = LVarArreglo(Identificador(std::string($1)), $2, $4); 
                                                       };

lcorchetes: '[' exp ']'             { 
                                      LCorchetes *nuevo =  new LCorchetes();
                                      nuevo->append(*$2);
                                      $$ = nuevo;
                                    }
          | lcorchetes '[' exp ']' { 
                                      $1->append(*$3);
                                      $$ = $1; 
                                    }
          /*Errores*/
          | '[' error ']'           {/*Definicion erronea del tamano del arreglo*/};

lfunciones: funcionmain        { //*$$ = LFunciones(*$1,0);  
                               }
          | funcion lfunciones { //*$$ = LFunciones(*$1,$2); 
                               };

funcionmain: FUNCTION TYPE_VOID MAIN '(' ')' '{' { driver.tablaSimbolos.enterScope(); } 
                                                bloquedeclare listainstrucciones  '}' { /*Tipo v = Tipo(std::string("void"));
                                                                                        LParam lp = LParam();
                                                                                        *$$ = Funcion(v,Identificador(std::string("main")),lp,*$8,*$9,0); 
                                                                                        driver.tablaSimbolos.exitScope(); */
                                                                                      }
/*Errores*/
/*Mala especificacion del encabezado de la funcion*/
          | FUNCTION TYPE_VOID MAIN '(' error ')' '{' { driver.tablaSimbolos.enterScope(); } 
                                                     bloquedeclare listainstrucciones  '}' { /*Tipo v = Tipo(std::string("void"));
                                                                                             LParam lp = LParam();
                                                                                             *$$ = Funcion(v,Identificador(std::string("main")),lp,*$8,*$9,0); 
                                                                                             driver.tablaSimbolos.exitScope(); */
                                                                                            }


funcion: FUNCTION tipo identificador '(' lparam ')' '{' { driver.tablaSimbolos.enterScope(); }
                                                       bloquedeclare listainstrucciones RETURN exp ';' '}' { /**$$ = Funcion(*$2,Identificador(std::string($3))
                                                                                                                            ,*$5,*$9,*$10,*$12); 
                                                                                                             driver.tablaSimbolos.exitScope();*/
                                                                                                           }

        | FUNCTION TYPE_VOID identificador '(' lparam ')' '{' { driver.tablaSimbolos.enterScope(); }
                                                            bloquedeclare listainstrucciones '}'           { /*Tipo v = Tipo(std::string("void"));
                                                                                                              *$$ = Funcion(v,Identificador(std::string($3)),
                                                                                                                            *$5,*$9,*$10,0);
                                                                                                              driver.tablaSimbolos.exitScope();*/
                                                                                                            }
/*Errores*/
/*Mala especificacion del encabezado de la funcion*/
        | FUNCTION tipo identificador '(' error ')' '{' { driver.tablaSimbolos.enterScope(); }
                                                       bloquedeclare listainstrucciones RETURN exp ';' '}' { /**$$ = Funcion(*$2,Identificador(std::string($3))
                                                                                                                             ,*$5,*$9,*$10,*$12); 
                                                                                                             driver.tablaSimbolos.exitScope();*/
                                                                                                           }

/*Mala especificacion del encabezado de la funcion*/
        | FUNCTION TYPE_VOID identificador '(' error ')' '{' { driver.tablaSimbolos.enterScope(); }
                                                            bloquedeclare listainstrucciones '}'           { /*Tipo v = Tipo(std::string("void"));
                                                                                                             *$$ = Funcion(v,Identificador(std::string($3)),
                                                                                                             *$5,*$9,*$10,0);
                                                                                                             driver.tablaSimbolos.exitScope();*/
                                                                                                            }



/*LISTO*/
lparam: /* Vacio */          { $$ = new LParam(); 
                             } 
      | lparam2              { $$ = $1; 
                             } 

lparam2: tipo identificador               { //*$$ = LParam(*$1,Identificador(std::string($2)), LParam());  
                                          } 
        | tipo identificador ',' lparam2  { //*$$ = LParam(*$1,Identificador(std::string($2)),*$4);
                                          };

/*LISTO*/
listainstrucciones: /* Vacio */                        { //*$$ = ListaInstrucciones(); 
                                                       }
| instruccion ';' listainstrucciones { //*$$ = ListaInstrucciones($1,$3); 
}
| instruccion1 listainstrucciones    { //*$$ = ListaInstrucciones($1,$2); 
};

/*LISTO*/
instruccion: asignacion     { 
                            }
           | llamadafuncion { 
                            }
           | MINUSMINUS identificador  {
                                         $$ = new PlusMinus(*$2, 0); 
                                       }
           | identificador MINUSMINUS { 
                                         $$ = new PlusMinus(*$1, 1); 
                                       }
           | PLUSPLUS identificador    { 
                                         $$ = new PlusMinus(*$2, 2); 
                                       }
           | identificador PLUSPLUS    { 
                                         $$ = new PlusMinus(*$1, 3); 
                                       }
           | entradasalida  { 
                            };

instruccion1: loopfor        { 
                             }
            | loopwhile      { 
                             }
            | selectorif     { 
                             };

/*LISTO*/
asignacion: identificador ASSIGN exp            { /*Identificador id = Identificador(std::string($1));
                                                   *$$ = Asignacion(id,$3);*/
            }
| identificador lcorchetes ASSIGN exp { /*Identificador id = Identificador(std::string($1));
                                         *$$ = Asignacion(id,*$2,$4);*/
}
| identificador '.' identificador ASSIGN exp     { /*Identificador id1 = Identificador(std::string($1));
                                                     Identificador id2 = Identificador(std::string($3));
                                                    *$$ = Asignacion(id1,id2,$5);*/
};

/*LISTO*/
entradasalida: READ '(' lvarovalor ')' { //*$$ = EntradaSalida(0, *$3); 
               }
| PRINT '(' lvarovalor ')'  { //*$$ = EntradaSalida(1, *$3); 
};

/*LISTO*/
loopfor: FOR '(' identificador ';' exp ';' asignacion ')' '{' { 
             driver.tablaSimbolos.enterScope();   
         }
bloquedeclare listainstrucciones '}' { /*Identificador id = Identificador(std::string($3));
                                        *$$ = LoopFor(id,$5,*$7,*$11,*$12); 
                                        driver.tablaSimbolos.exitScope();*/
}
| FOR '(' identificador ';' exp ';' exp ')' '{' { 
    driver.tablaSimbolos.enterScope();   
} 
bloquedeclare listainstrucciones '}'        { /*Identificador id = Identificador(std::string($3));
                                               *$$ = LoopFor(id,$5,$7,*$11,*$12);
                                               driver.tablaSimbolos.exitScope();*/
};

/*LISTO*/
loopwhile: WHILE '(' exp ')' DO '{' { 
               driver.tablaSimbolos.enterScope();   
           } 
bloquedeclare listainstrucciones '}' { /**$$ = LoopWhile($3,*$8,*$9); 
                                         driver.tablaSimbolos.exitScope();*/
}
| DO '{'{ 
driver.tablaSimbolos.enterScope();   
} 
bloquedeclare listainstrucciones '}' WHILE '(' exp ')' { /**$$ = LoopWhile($9,*$4,*$5); 
                                                           driver.tablaSimbolos.exitScope();*/
};

/*LISTO*/
selectorif: IF '(' exp ')' THEN '{' { 
                driver.tablaSimbolos.enterScope();   
            }
bloquedeclare listainstrucciones '}' lelseif { /**$$ = SelectorIf($3,$8,$9,$11);
                                                 driver.tablaSimbolos.exitScope();*/
}
| IF '(' exp ')' THEN instruccion                                      { //*$$ = SelectorIf($3,$6,0); 
}
| IF '(' exp ')' THEN instruccion ELSE instruccion                     { //*$$ = SelectorIf($3,$6,$8); 
};

/*LISTO*/
lelseif: /* Vacio */                                                               { //*$$ = LElseIf(); 
                                                                                   }
| ELSE IF '(' exp ')' THEN '{' { 
    driver.tablaSimbolos.enterScope();   
}
bloquedeclare listainstrucciones '}' lelseif { /**$$ = LElseIf($4,*$9,*$10,$12);
                                                 driver.tablaSimbolos.exitScope();*/
}
| ELSE '{' { 
    driver.tablaSimbolos.enterScope();   
}
bloquedeclare listainstrucciones '}'                              { /**$$ = LElseIf(*$4,*$5); 
                                                                      driver.tablaSimbolos.exitScope();*/
};

llamadafuncion: identificador '(' /* enterScope */ 
lvarovalor ')' { //*$$ = LlamadaFuncion(Identificador(std::string($1)),*$3); 
};


lvarovalor: /* Vacio */   { 
                            $$ = new LVaroValor(); 
                          }
          | lvarovalor2   { $$ = $1;
                          };      
          
lvarovalor2: exp ',' lvarovalor2     { 
                                       $$ = new  LVaroValor($1,$3);
                                     }
           | exp                     { Exp e = $1;
                                       LVaroValor *tmp = new LVaroValor(&e);
                                       $$ = tmp; 
                                     };	   

/*Aqui no es necesario poner nada. Revisar esto.*/
exp: expbin       {  
                  }
   | expun        {  
                  } 
   | valor        {  
                  }
   | identificador    { 
                      }
   | '(' exp ')'  { 
                  };

/*Faltan pruebas*/
expbin: exp AND exp          { 
                               $$ = new ExpBin(*$1,*$3,std::string("and"));
                             }
      | exp OR exp           { 
                               $$ = new ExpBin(*$1,*$3,std::string("or"));
                             }
      | exp COMPARISON exp   { int cmpv = $2;
                               ExpBin *eb;
                               switch(cmpv){
                                    case 1:
                                        eb = new ExpBin(*$1,*$3,std::string(">"));
                                        $$ = eb;
                                        break;
                                    case 2:
                                        eb = new ExpBin(*$1,*$3,std::string("<"));
                                        $$ = eb;
                                        break;
                                    case 3:
                                        eb = new ExpBin(*$1,*$3,std::string("<="));
                                        $$ = eb;
                                        break;
                                    case 4:
                                        eb = new ExpBin(*$1,*$3,std::string(">="));
                                        $$ = eb;
                                        break;
                                    case 5:
                                        eb = new ExpBin(*$1,*$3,std::string("="));
                                        $$ = eb;
                                        break;
                                    case 6:
                                        eb = new ExpBin(*$1,*$3,std::string("!="));
                                        $$ = eb;
                                        break;
                               }
                             }
      | exp UFO exp          { 
                               $$ = new ExpBin(*$1,*$3,std::string("<=>"));
                             }
      | exp '+' exp          { 
                               $$ = new ExpBin(*$1,*$3,std::string("+"));
                             }
      | exp '-' exp          { 
                               $$ = new ExpBin(*$1,*$3,std::string("-"));
                             }
      | exp '*' exp          { 
                               $$ = new ExpBin(*$1,*$3,std::string("*"));
                             }
      | exp '/' exp          { 
                               $$ = new ExpBin(*$1,*$3,std::string("/"));
                             }
      | exp DIV exp          { 
                               $$ = new ExpBin(*$1,*$3,std::string("div"));
                             }
      | exp MOD exp          { 
                               $$ =  new ExpBin(*$1,*$3,std::string("mod"));
                             }
      | exp POW exp          { 
                               $$ = new ExpBin(*$1,*$3,std::string("**"));
                             }
      | identificador '.' identificador { 
                             	          $$ = new ExpBin(*$1,*$3,std::string("."));
                                        };

/* Revisar si hay que poner new string */
expun: NOT exp               { std::string str = std::string("not");
                               ExpUn* tmp = new ExpUn(*$2, &str);
                               $$ = tmp; 
                             }
     | '-' exp %prec UMINUS  { std::string str = std::string("-");
                               ExpUn* tmp = new ExpUn(*$2, &str);
                               $$ = tmp; 
                             }
     | exp PLUSPLUS          { std::string str = std::string("++sufijo");
                               ExpUn* tmp = new ExpUn(*$1, &str);
                               $$ = tmp; 
                             }
     | exp MINUSMINUS        { std::string str = std::string("--sufijo");
                               ExpUn* tmp = new ExpUn(*$1, &str);
                               $$ = tmp;
                             }
     | PLUSPLUS exp         { std::string str = std::string("++prefijo");
                              ExpUn* tmp = new ExpUn(*$2, &str);
                              $$ = tmp; 
                            }
     | MINUSMINUS exp        { 
                               std::string str = std::string("--prefijo");
                               Exp e = *$2;
                               ExpUn* tmp = new ExpUn(e, &str);
                               $$ = tmp; 
                             }
     | identificador lcorchetes { Exp id = *$1;
                                  ExpUn* tmp = new ExpUn(id, $2);
                                  $$ = tmp;
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
                      $$ = new Tipo(std::string("real"));
                    }
     | TYPE_INTEGER { 
                      $$ = new Tipo(std::string("integer"));
                    }
     | TYPE_BOOLEAN { 
                      $$ = new Tipo(std::string("boolean"));
                    }
     | TYPE_CHAR    { 
                      $$ = new Tipo(std::string("character"));
                    }
     | TYPE_STRING  { 
                      $$ = new Tipo(std::string("string"));
                    };


/*Funciona*/
arreglo: '[' larreglo ']' {
                            Arreglo* tmp;
                            LArreglo *lr = $2;
                            tmp = new Arreglo(lr);
                            $$ = tmp; 
                          };

/*Funciona. Faltan ejemplos mas interesantes.*/

larreglo: exp ',' larreglo      { 
                                  Exp e = *$1;
                                  LArreglo *l = $3;
                                  LArreglo* tmp = new LArreglo(e,l); 
                                  $$ = tmp;
                                }
        | exp                   { 
                                  LArreglo *tmp = new LArreglo(*$1,0);
                                  $$ = tmp;
                                 
                                };

identificador: ID { std::string str =  std::string($1);
                    Identificador* id = new Identificador(str);
                    $$ = id;
                  };


%%

void yy::GuavaParser::error (const yy::GuavaParser::location_type& l, const std::string& m)
{
  driver.error (l, m);
}

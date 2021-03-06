/*
 * =====================================================================================
 *
 *       Filename:  GuavaTemplates.hh
 *
 *    Description:  Funciones templates.
 *
 *        Version:  1.0
 *        Created:  14/11/14 21:27:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ruben Serradas, Michael Woo 
 *   Organization:  USB
 *
 * =====================================================================================
 */

# ifndef GUAVA_TEMPLATES_HH
# define GUAVA_TEMPLATES_HH
# include <set>
# include "Symbol.hh"
# include "GuavaSymTable.hh"
# include "GuavaDescriptor.hh"
# include "Generator.hh"
# include "GetReg.hh"

using namespace std;

class RegisterAllocator;

class GuavaQuads;
class GuavaQuadsExp;

/** 
 * Clase de donde parten todos los templates.
 */
class GuavaTemplates{
protected:
    GuavaGenerator* generador;
    GuavaSymTable* table;
    GuavaDescTable* vars;
    GuavaDescTable* regs;
    GuavaDescTable* regs_float;
    int offset_actual = 0;
    RegisterAllocator* get_reg; /* Asignador de registros para registros normales */
    RegisterAllocator* get_reg_float; /* Asignador de registros para registros float */
    string funcion_actual;
public:

    GuavaTemplates(GuavaGenerator* g, GuavaSymTable* tabla, GuavaDescTable* vars_, GuavaDescTable* regs_, GuavaDescTable* floats);

    RegisterAllocator* get_reg_alloc();

    RegisterAllocator* get_reg_float_alloc();

    GuavaSymTable* get_table(){ return table; }

    void set_funcion_actual(string func);

    string get_funcion_actual();

    void print_descriptors();

    /** 
     * Destructor
     */
    ~GuavaTemplates(){}

    void set_regs(GuavaDescTable* r);

    void set_vars(GuavaDescTable* v);

    void set_vars(list<SimpleSymbol*> simbolos);
    
    void reset_offset();
    
    int get_offset();

    virtual void store(SimpleSymbol* var, GuavaDescriptor* reg){}

    virtual void store(GuavaDescriptor* reg){}

    virtual void store_spill(GuavaDescriptor* reg){}

    virtual void push(GuavaDescriptor* reg){}

    virtual void push(Symbol* var){}

    virtual void entry_main(){}

    virtual void exit_main(){}

    virtual void go_to(Symbol* to){}

    virtual void label(string label){}

    virtual void move(GuavaDescriptor* reg, GuavaDescriptor* reg2){}

    virtual void move(GuavaDescriptor* reg, string reg_2, Symbol* result){}

    virtual void print(Symbol* arg){}

    virtual void print(GuavaDescriptor* reg){}

    virtual void operacion(list<GuavaDescriptor*> regs, GuavaQuadsExp* instruccion){}

    virtual void load(GuavaDescriptor* reg,SimpleSymbol* var){}

    virtual void operacion_ternaria(GuavaDescriptor* Rx, GuavaDescriptor* Ry, GuavaDescriptor* Rz, GuavaQuadsExp* ins){}

    virtual void operacion_unaria(GuavaDescriptor* Rx, GuavaDescriptor* Ry, GuavaQuadsExp* inst){}

    virtual void push(int resta_){}

    virtual void prologo(){}

    virtual void epilogo(){}

    virtual void condicional(list<GuavaDescriptor*> regs, GuavaQuadsExp* inst){}

    virtual void condicional_not(list<GuavaDescriptor*> regs, GuavaQuadsExp* inst){}

    virtual void return_t( GuavaDescriptor* desc, GuavaQuadsExp* i){}

    virtual void gen_call(GuavaQuadsExp* e){}

    virtual void read(GuavaDescriptor* reg, Symbol* i){}

    virtual void end_block(){}

    virtual void main_return(){}
};

/** 
 * Clase que contiene las plantillas para el lenguaje MIPS. 
 */
class MIPS: public GuavaTemplates{
protected:
    int div_ = 0;
    int ufo = 0;
    int sec_if = 0;
    void revision_div(GuavaDescriptor* Rz);
    void generar_ufo(GuavaDescriptor* Rx);
    void pop_simple(string v);
    void push_simple(string v);
public:

    /** 
     * Constructor de la clase.
     */
    MIPS(GuavaGenerator* g, GuavaSymTable* t, GuavaDescTable* vars = 0): GuavaTemplates(g,t,vars,new DescTableMIPS(), new DescTableFloatMIPS()){}

    /** 
     * Destructor
     */
    ~MIPS(){}

    void store(SimpleSymbol* var, GuavaDescriptor* reg);

    void store(GuavaDescriptor* reg);

    void store_spill(GuavaDescriptor* reg);

    void push(GuavaDescriptor* reg);

    void push(Symbol* var);

    void push(int resta_);

    void entry_main();

    void exit_main();

    void label(string label);

    void go_to(Symbol* to);

    void move(GuavaDescriptor* reg, GuavaDescriptor* reg2);

    void move(GuavaDescriptor* reg, string reg_2, Symbol* result);
    
    void read(GuavaDescriptor* reg , Symbol* result);

    void print(Symbol* arg);

    void print(GuavaDescriptor* reg);

    void operacion(list<GuavaDescriptor*> regs, GuavaQuadsExp* instruccion);

    void load(GuavaDescriptor* reg, SimpleSymbol* var);

    void operacion_ternaria(GuavaDescriptor* Rx, GuavaDescriptor* Ry, GuavaDescriptor* Rz, GuavaQuadsExp* ins);

    void operacion_unaria(GuavaDescriptor* Rx, GuavaDescriptor* Ry, GuavaQuadsExp* inst);

    void prologo();

    void epilogo();

    void condicional(list<GuavaDescriptor*> regs, GuavaQuadsExp* inst);

    void condicional_not(list<GuavaDescriptor*> regs, GuavaQuadsExp* inst);

    void return_t( GuavaDescriptor* desc,GuavaQuadsExp* i);

    void gen_call(GuavaQuadsExp* e);

    void end_block();

    void main_return();
};

# endif

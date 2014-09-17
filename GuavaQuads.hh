/*
 * =====================================================================================
 *
 *       Filename:  GuavaQuads.hh
 *
 *    Description:  Quads para el lenguaje.
 *
 *        Version:  1.0
 *        Created:  16/09/14 10:20:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ruben Serradas, Michael Woo
 *   Organization:  USB
 *
 * =====================================================================================
 */
#include <string>
#include "GuavaSymTable.hh"
/**
 * Clase que representa un Quad
 */
class GuavaQuads{
private:
    std::string op;
    SimpleSymbol* arg1;
    SimpleSymbol* arg2;
    SimpleSymbol* result;
public: 
    /**
     * Constructor de la clase
     * @param op_ Operando
     * @param arg1_ Argumento 1
     * @param arg2_ Argumento 2
     * @param result_ Resultado
     */
    GuavaQuads(std::string op_, SimpleSymbol* arg1_, SimpleSymbol* arg2_, SimpleSymbol* result_): op(op_), arg1(arg1_), arg2(arg2_), result(result_){}
    /**
     * Destructor de la clase
     */
    ~GuavaQuads(){}
    /**
     * Getters de la clase
     */
    std::string get_op(){ return op; }
    SimpleSymbol* get_arg1()  { return arg1; }
    SimpleSymbol* get_arg2()  { return arg2; }
    SimpleSymbol* get_result(){ return result; }

    /**
     * Funcion que genera codigo a partir de un Quad
     */
    std::string gen(){
        std::string code ("");
        if (arg2 == 0){
            code += result->sym_name + ":=" + arg1->sym_name + op + arg2->sym_name;
        }else {
            if (op.compare(std::string(":=")) != 0){
                code += result->sym_name + ":=" + arg1->sym_name;
            } else {
                code += result->sym_name + op + arg1->sym_name;
            }
        }
        return code;
    }
};

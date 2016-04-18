//
// Created by epicfail on 04.04.16.
//

#ifndef TREE_EXPRESSION_PARSER_H
#define TREE_EXPRESSION_PARSER_H


#include "Node.h"

class Expression_parser
{
public:
    Expression_parser ();
    ~Expression_parser ();
    Expression_parser (const Expression_parser & that);

    void get_code (char *filename);
    Node *parse ();
private:
    char *code_;
    const char *code_begin_;
    long int code_size_;
    unsigned char n_if_;
    bool if_or_while_;      //true if 'if' and false if 'while'

    Node *text_parse ();
    Node *full_if_while_parse ();
    Node *str_parse ();
    Node *prog_parse ();
    Node *cond_parse ();
    Node *if_while_parse ();
    Node *assign_parse ();
    Node *exp_parse ();
    Node *e_parse ();
    Node *t_parse ();
    Node *p_parse ();
    Node *var_parse ();
    Node *num_parse ();

    void skip_spaces ();
    void skip_new_line_symb ();
    size_t count_vertical_dash ();
    static bool is_conditions (Math_Func mf);
};


#endif //TREE_EXPRESSION_PARSER_H

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

    void get_expression (char * filename);
    Node *parse ();
private:
    char * expression_;
    const char * expression_begin_;

    Node *mul_parse ();
    Node *br_mf_parse ();
    void skip_spaces ();
};


#endif //TREE_EXPRESSION_PARSER_H

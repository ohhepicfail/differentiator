//
// Created by epicfail on 29.03.16.
//

#ifndef TREE_NODE_H
#define TREE_NODE_H

#include <stdio.h>

enum Type
{
    NAT,
    VAR,
    VAL,
    MF
};

enum Math_Func
{
    MF_NAF,
    MF_ADD,
    MF_SUB,
    MF_MUL,
    MF_DIV,
    MF_SIN,
    MF_COS,
    MF_TG,
    MF_CTG,
    MF_EXP,
    MF_LN,
    MF_UADD,
    MF_USUB,
    MF_UMUL,
    MF_UDIV,
    MF_ASSIGN,
    MF_READ,
    MF_PRINT,
    MF_EQ,
    MF_NOTEQ,
    MF_LARGER,
    MF_SMALLER,
    MF_CODE,
    MF_IF
};

class Node
{
public:
    Node (Math_Func mf, Node * left = NULL, Node * right = NULL);
    Node (char var    , Node * left = NULL, Node * right = NULL);
    Node (double val  , Node * left = NULL, Node * right = NULL);

    ~Node ();

    bool is_ok () const;
    void del ();
    Node & copy () const;

    bool dumpDOT () const;
    bool dumpTEX (char * filename) const;

    Node & diff (char variable);
    bool simplify ();

    Node *get_right ();
    Node *get_left ();
    Node **get_pright ();
    Math_Func get_mf ();

private:
    Type type_;
    union
    {
        Math_Func mf_;
        char var_;
        double val_;
    };
    Node *left_;
    Node *right_;

    void dump_node_in_dot (FILE *dump_file) const;
    void dump_node_in_tex (FILE *tex_file) const;

    const Node & operator= (const Node & that);
};

Node & operator+ (Node & left, Node & right);
Node & operator- (Node & left, Node & right);
Node & operator* (Node & left, Node & right);
Node & operator/ (Node & left, Node & right);


#endif //TREE_NODE_H

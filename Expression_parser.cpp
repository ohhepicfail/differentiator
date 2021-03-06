//
// Created by epicfail on 04.04.16.
//

#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <climits>
#include "Expression_parser.h"
#include "Error.h"

Expression_parser::Expression_parser () :
    code_ (NULL),
    code_begin_ (NULL),
    code_size_  (0),
    n_if_ (0),
    if_or_while_ (false)
{}

Expression_parser::~Expression_parser ()
{
    free ((void *)code_begin_);
}

Expression_parser::Expression_parser (const Expression_parser & that)
{
    if (that.code_)
    {
        long int that_len = that.code_size_;
        code_ = (char *) malloc (sizeof (char) * that_len);
        if (!code_)
            THROW (CANNOT_ALLOC_MEMORY);
        code_begin_ = code_;

        strncpy (code_, that.code_, that_len);
    }
}

void Expression_parser::get_code (char *filename)
{
    if (!filename)
        THROW (NULL_POINTER);

    FILE *exp_file = fopen (filename, "rb");
    if (!exp_file)
        THROW (CANNOT_OPEN_FILE);

    fseek (exp_file, 0, SEEK_END);
    code_size_ = ftell (exp_file);
    fseek (exp_file, 0, SEEK_SET);

    size_t exp_in_char_size = sizeof (char) * code_size_;
    code_ = (char *) malloc (exp_in_char_size + 1); // because I want to add '\0' in the end
    code_[exp_in_char_size] = '\0';
    code_begin_ = code_;
    bool all_is_read = (code_size_ == fread (code_, sizeof (char), code_size_, exp_file));
    fclose (exp_file);
    if (!all_is_read)
        THROW (W_NOT_ALL_READ);
}

Node *Expression_parser::parse ()
{
    Node *node = NULL;
    try {node = text_parse ();} catch (Error err) {throw err;}
    if (!node->is_ok ())
        THROW (BAD_NODE);
    return node;
}

Node *Expression_parser::text_parse ()
{
    if (!code_)
        THROW (NULL_EXPRESSION);

    Node *node     = NULL;
    Node *right_n  = NULL;
    while (code_ - code_begin_ < code_size_ - 1)
    {
        right_n = full_if_while_parse ();

        if (node == NULL)
            node = right_n;
        else
            node = new Node (MF_CODE, node, right_n);
    }

    return node;
}

Node *Expression_parser::full_if_while_parse ()
{
    if (!code_)
        THROW (NULL_EXPRESSION);

    Node *right_n  = NULL;
    try {right_n = str_parse ();} catch (Error err) {throw err;}
    if (!right_n)
        return NULL;
    skip_new_line_symb ();

    if (is_conditions (right_n->get_mf ()))
    {
        Node *left_n = right_n;
        unsigned char old_n_if = n_if_;
        try {right_n = full_if_while_parse ();} catch (Error err) {throw err;}
        if (right_n == NULL)
            THROW (BAD_IF_OR_WHILE);

        Node *extra_node = NULL;
        if (old_n_if <= n_if_ && is_conditions (right_n->get_mf ()))
        {
            try {extra_node = full_if_while_parse ();} catch (Error err) {throw err;}
            if (!extra_node)
                THROW (BAD_EXPRESSION);
            right_n = new Node (MF_CODE, right_n, extra_node);
        }
        else
        {
            size_t n_vert_dash = count_vertical_dash ();
            while (old_n_if <= n_vert_dash)
            {
                try {extra_node = full_if_while_parse ();} catch (Error err) {throw err;}
                right_n = new Node (MF_CODE, right_n, extra_node);

                n_vert_dash = count_vertical_dash ();
            }
        }

        skip_new_line_symb ();

        if (if_or_while_)
            right_n = new Node (MF_IF, left_n, right_n);
        else
            right_n = new Node (MF_WHILE, left_n, right_n);
    }

    return right_n;
}

Node *Expression_parser::str_parse ()
{
    if (!code_)
        THROW (BAD_EXPRESSION);

    skip_spaces ();
    Node *node = NULL;
    if (*code_ != '\0')
    {
        try {node = prog_parse ();} catch (Error err) {throw err;}
        return node;
    }

    return NULL;
}

Node *Expression_parser::prog_parse ()
{
    if (!code_)
        THROW (BAD_EXPRESSION);

    skip_spaces ();
    if (*code_ != '@')
        THROW (BAD_EXPRESSION);
    code_++;
    skip_spaces ();

    Node *node = NULL;
    try {node = if_while_parse ();} catch (Error err) {throw err;}

    return node;
}

Node *Expression_parser::if_while_parse ()
{
    if (!code_)
        THROW (BAD_EXPRESSION);

    n_if_ = 0;
    skip_spaces ();
    while (*code_ == '|')
    {
        code_++;
        n_if_++;
        skip_spaces ();
    }

    Node *node = NULL;
    if (*code_ == '<' || *code_ == '>' ||
        *code_ == '=' || *code_ == '!')
    {
        try {node = cond_parse ();} catch (Error err) {throw err;}
        n_if_++;
    }
    else
        try {node = assign_parse ();} catch (Error err) {throw err;}

    return node;
}

Node *Expression_parser::cond_parse ()
{
    if (!code_)
        THROW (BAD_EXPRESSION);

    skip_spaces ();

    Math_Func mf = MF_NAF;
    if (*code_ == '<')
        mf = MF_SMALLER;
    else if (*code_ == '>')
        mf = MF_LARGER;
    else if (*code_ == '=')
        mf = MF_EQ;
    else if (*code_ == '!')
        mf = MF_NOTEQ;
    else
        THROW (BAD_EXPRESSION);
    code_++;

    skip_spaces ();
    if (*code_ == '?')
        if_or_while_ = true;    //if
    else if (*code_ == ':')
        if_or_while_ = false;   //while
    else
        THROW (BAD_EXPRESSION);
    code_++;
    skip_spaces ();

    Node *left_n  = NULL;
    Node *right_n = NULL;
    try {left_n = e_parse ();} catch (Error err) {throw err;}
    skip_spaces ();
    if (*code_ != ',')
        THROW (BAD_EXPRESSION);
    code_++;
    skip_spaces ();
    try {right_n = e_parse ();} catch (Error err) {throw err;}

    skip_spaces ();
    if (*code_ != ';')
        THROW (BAD_EXPRESSION);
    code_++;

    Node *node = new Node (mf, left_n, right_n);
    return node;
}

Node *Expression_parser::assign_parse ()
{
    if (!code_)
        THROW (BAD_EXPRESSION);

    skip_spaces ();

    Node *left_n = NULL;
    Node *right_n = NULL;
    if (isalpha (*code_))
        try {left_n = var_parse ();} catch (Error err) {throw err;}
    else
        THROW (BAD_EXPRESSION);

    skip_spaces ();
    unsigned char eq_symb_len = 2;          // <-
    char assignment[eq_symb_len + 1] = {'\0', '\0', '\0'};
    strncpy (assignment, code_, eq_symb_len);

    Node *node = left_n;
    if (strcmp (assignment, "<-") == 0)
    {
        code_ += eq_symb_len;
        skip_spaces ();

        if (*code_ == '?')
        {
            right_n = new Node (MF_READ);
            code_++;
        }
        else
            try {right_n = exp_parse ();} catch (Error err) {throw err;}

        if (MF_UADD <= right_n->get_mf () && right_n->get_mf () <= MF_UDIV)
        {
            Math_Func mf = MF_NAF;
            if (right_n->get_mf () == MF_UADD)
                mf = MF_ADD;
            else if (right_n->get_mf () == MF_USUB)
                mf = MF_SUB;
            else if (right_n->get_mf () == MF_UMUL)
                mf = MF_MUL;
            else if (right_n->get_mf () == MF_UDIV)
                mf = MF_DIV;
            else
                THROW (BAD_NODE);

            right_n = new Node (mf, &left_n->copy (), right_n->give_and_forget_right ());
        }
        skip_spaces ();

        node = new Node (MF_ASSIGN, left_n, right_n);
    }


    if (*code_ != ';')
        node = new Node (MF_PRINT, NULL, node);
    else
        code_++;

    skip_spaces ();
    if (*code_ != '\0' && *code_ != '\n')
        THROW (BAD_EXPRESSION);

    return node;
}

Node *Expression_parser::exp_parse ()
{
    if (!code_)
        THROW (BAD_EXPRESSION);

    skip_spaces ();

    Math_Func mf = MF_NAF;
    if (*code_ == '+')
        mf = MF_UADD;
    else if (*code_ == '-')
        mf = MF_USUB;
    else if (*code_ == '*')
        mf = MF_UMUL;
    else if (*code_ == '/')
        mf = MF_UDIV;
    else code_--;
    code_++;

    Node *right_n = NULL;
    try {right_n = e_parse ();} catch (Error err) {throw err;}

    Node *node = NULL;
    if (mf == MF_NAF)
        node = right_n;
    else
        node = new Node (mf, NULL, right_n);

    return node;
}

Node *Expression_parser::e_parse ()
{
    if (!code_)
        THROW (BAD_EXPRESSION);

    Node *node = NULL;
    try {node = t_parse ();} catch (Error err) {throw err;}

    skip_spaces ();
    while (*code_ == '+' || *code_ == '-')
    {
        Math_Func mf = MF_NAF;
        if (*code_ == '+')
            mf = MF_ADD;
        else if (*code_ == '-')
            mf = MF_SUB;
        code_++;
        skip_spaces ();

        Node *right_n = NULL;
        try {right_n = t_parse ();} catch (Error err) {throw err;}
        skip_spaces ();

        node = new Node (mf, node, right_n);
    }

    if (!node->is_ok ())
        THROW (BAD_NODE);

    return node;
}

Node *Expression_parser::t_parse ()
{
    if (!code_)
        THROW (BAD_EXPRESSION);

    Node *node = NULL;
    try {node = p_parse ();} catch (Error err) {throw err;}

    skip_spaces ();
    while (*code_ == '*' || *code_ == '/')
    {
        Math_Func mf = MF_NAF;
        if (*code_ == '*')
            mf = MF_MUL;
        else if (*code_ == '/')
            mf = MF_DIV;
        code_++;
        skip_spaces ();

        Node *right_n = NULL;
        try {right_n = p_parse ();} catch (Error err) {throw err;}
        skip_spaces ();

        node = new Node (mf, node, right_n);
    }

    if (!node->is_ok ())
        THROW (BAD_NODE);

    return node;
}

Node *Expression_parser::p_parse ()
{
    if (!code_)
        THROW (BAD_EXPRESSION);

    skip_spaces ();

    Node *node = NULL;
    if (isalpha (*code_))
        try {node = var_parse ();} catch (Error err) {throw err;}
    else if (isdigit (*code_))
        try {node = num_parse ();} catch (Error err) {throw err;}
    else if (*code_ == '(')
    {
        code_++;
        try {node = e_parse ();} catch (Error err) {throw err;}
        skip_spaces ();
        if (*code_ != ')')
            THROW (BAD_EXPRESSION);
        code_++;
    }
    else
        THROW (BAD_EXPRESSION);

    if (!node->is_ok ())
        THROW (BAD_NODE);

    return node;
}

Node *Expression_parser::var_parse ()
{
    if (!code_)
        THROW (BAD_EXPRESSION);

    char var = *code_;
    code_++;

    Node *node = new Node (var);

    return node;
}

Node *Expression_parser::num_parse ()
{
    if (!code_)
        THROW (BAD_EXPRESSION);

    long long int num = 0;
    while (isdigit (*code_))
    {
        num = num * 10 + *code_ - '0';
        code_++;
    }

    if (num > INT_MAX)
        THROW (TOO_BIG_NUMBER);

    Node *node = new Node ((double) num);

    return node;
}


void Expression_parser::skip_spaces ()
{
    if (!code_)
        THROW (NULL_EXPRESSION);

    while (isspace (*code_) && *code_ != '\n')
        code_++;
}

void Expression_parser::skip_new_line_symb ()
{
    if (*code_ == '\n')
        code_++;
}

size_t Expression_parser::count_vertical_dash ()
{
    size_t n_vert_dash = 0;
    char *symb = code_;
    while (*symb != '\0' && *symb != '\n')
        if (*(symb++) == '|')
            n_vert_dash++;

    return n_vert_dash;
}


bool Expression_parser::is_conditions (Math_Func mf)
{
    if (mf == MF_EQ     ||
        mf == MF_NOTEQ  ||
        mf == MF_LARGER ||
        mf == MF_SMALLER)
        return true;
    else
        return false;
}
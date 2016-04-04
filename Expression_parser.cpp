//
// Created by epicfail on 04.04.16.
//

#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include "Expression_parser.h"
#include "Error.h"

Expression_parser::Expression_parser () :
    expression_ (NULL),
    expression_begin_ (NULL)
{}

Expression_parser::~Expression_parser ()
{
    free ((void *)expression_begin_);
}

Expression_parser::Expression_parser (const Expression_parser & that)
{
    if (that.expression_)
    {
        long int that_len = strlen (that.expression_);
        expression_ = (char *) malloc (sizeof (char) * that_len);
        if (!expression_)
            THROW (CANNOT_ALLOC_MEMORY);
        expression_begin_ = expression_;

        strncpy (expression_, that.expression_, that_len);
    }
}

void Expression_parser::get_expression (char * filename)
{
    if (!filename)
        THROW (NULL_POINTER);

    FILE *exp_file = fopen (filename, "rb");
    if (!exp_file)
        THROW (CANNOT_OPEN_FILE);

    fseek (exp_file, 0, SEEK_END);
    long int exp_size = ftell (exp_file);
    fseek (exp_file, 0, SEEK_SET);

    size_t exp_in_char_size = sizeof (char) * exp_size;
    expression_ = (char *) malloc (exp_in_char_size + 1); // because I want to add '\0' in the end
    expression_[exp_in_char_size] = '\0';
    expression_begin_ = expression_;
    bool all_is_read = exp_size == fread (expression_, sizeof (char), exp_size, exp_file);
    fclose (exp_file);
    if (!all_is_read)
        THROW (W_NOT_ALL_READ);
}

Node *Expression_parser::parse ()
{
    if (!expression_)
        THROW (NULL_EXPRESSION);

    Node * exp_node = NULL;
    try {exp_node = mul_parse ();}
    catch (Error err)
    {
        // todo: error here
        printf (":(\n");
    }

    while (1)
    {
        char cur_exp_symbol = *expression_;
        if (cur_exp_symbol == '+' || cur_exp_symbol == '-')
        {
            expression_++;

            Node * right_exp_node = NULL;
            try {right_exp_node = mul_parse ();}
            catch (Error err)
            {
                // todo: error here
                printf (";(\n");
            }

            if (cur_exp_symbol == '+')
                exp_node = new Node (MF_ADD, exp_node, right_exp_node);
            else
                exp_node = new Node (MF_SUB, exp_node, right_exp_node);
            if (!exp_node)
                THROW (BAD_NODE);
        }
        else
            break;
    }

    if (*expression_ != '\0' && *expression_ != ')')
        THROW (BAD_EXPRESSION);

    return exp_node;
}

Node *Expression_parser::mul_parse ()
{
    skip_spaces ();

    Node * exp_node = NULL;
    try {exp_node = br_mf_parse ();}
    catch (Error err)
    {
        // todo: error here
        printf (":((\n");
    }

    while (1)
    {
        char cur_exp_symbol = *expression_;
        if (cur_exp_symbol == '*' || cur_exp_symbol == '/')
        {
            expression_++;

            Node * right_exp_node = NULL;
            try {right_exp_node = br_mf_parse ();}
            catch (Error err)
            {
                // todo: error here
                printf (";((\n");
            }

            if (cur_exp_symbol == '*')
                exp_node = new Node (MF_MUL, exp_node, right_exp_node);
            else
                exp_node = new Node (MF_DIV, exp_node, right_exp_node);
            if (!exp_node)
                THROW (BAD_NODE);
        }
        else
            break;
    }

    skip_spaces ();

    return  exp_node;
}

Node *Expression_parser::br_mf_parse ()
{
    skip_spaces ();

    Node * exp_node = NULL;

    if (*expression_ == '(')
    {
        expression_++;
        exp_node = parse ();

        if (*expression_ != ')')
            THROW (BAD_EXPRESSION);
        expression_++;
    }
    else if ('0' <= *expression_ && *expression_ <= '9')
    {
        double val = 0.0;
        sscanf (expression_, "%lf", &val);
        while (('0' <= *expression_ && *expression_ <= '9') || *expression_ == '.')
            expression_++;
        exp_node = new Node (val);
    }
    else if (isalpha (*expression_) && isalpha (*(expression_ + 1)))
    {
        char func[2] = {*expression_, *(expression_ + 1)};

        do
        {
            expression_++;
        }while (*(expression_ - 1) != '(');

        Node * func_argument = NULL;
        try {func_argument = parse ();}
        catch (Error err)
        {
            // todo: error here
            printf (":(((\n");
        }
        if (*expression_ != ')')
            THROW (BAD_EXPRESSION);
        expression_++;

        switch (func[0])
        {
            default:
                THROW (BAD_EXPRESSION);
            case 's':
                exp_node = new Node (MF_SIN, NULL, func_argument);
                break;
            case 'c':
                if (func[1] == 'o')
                    exp_node = new Node (MF_COS, NULL, func_argument);
                else if (func[1] == 't')
                    exp_node = new Node (MF_CTG, NULL, func_argument);
                else
                    THROW (BAD_EXPRESSION);
                break;
            case 't':
                exp_node = new Node (MF_TG, NULL, func_argument);
                break;
            case 'l':
                exp_node = new Node (MF_LN, NULL, func_argument);
                break;
            case 'e':
                exp_node = new Node (MF_EXP, NULL, func_argument);
                break;
        }

    }
    else if (isalpha (*expression_))
    {
        exp_node = new Node (*expression_, NULL, NULL);
        expression_++;
    }
    else
        THROW (BAD_EXPRESSION);

    skip_spaces ();

    return exp_node;
}

void Expression_parser::skip_spaces ()
{
    if (!expression_)
        THROW (NULL_EXPRESSION);

    while (isspace (*expression_))
        expression_++;
}
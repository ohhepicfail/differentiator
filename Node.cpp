//
// Created by epicfail on 29.03.16.
//

#include <math.h>
#include <assert.h>
#include "Node.h"
#include "Error.h"

Node::Node (Math_Func mf, Node * left, Node * right) :
    type_ (MF),
    mf_ (mf),
    left_ (left),
    right_ (right)
{ }

Node::Node (char var, Node * left, Node * right) :
    type_ (VAR),
    var_ (var),
    left_ (left),
    right_ (right)

{ }

Node::Node (double val, Node * left, Node * right) :
        type_ (VAL),
        val_ (val),
        left_ (left),
        right_ (right)
{ }

Node::~Node ()
{
    type_ = NAT;
    left_ = NULL;
    right_ = NULL;
}

bool Node::is_ok () const
{
    bool correct = true;

    if (!this)
        return false;

    switch (type_)
    {
        default:
        case NAT:
            correct = false;
            break;

        case VAR:
        case VAL:
            if (left_ || right_)
                correct = false;
            break;

        case MF:
            switch (mf_)
            {
                default:
                case MF_SIN:
                case MF_COS:
                case MF_TG:
                case MF_CTG:
                case MF_EXP:
                case MF_LN:
                case MF_UADD:
                case MF_USUB:
                case MF_UMUL:
                case MF_UDIV:
                case MF_PRINT:
                    if (!right_ || left_)
                        correct = false;
                    break;
                case MF_ADD:
                case MF_SUB:
                case MF_MUL:
                case MF_DIV:
                case MF_ASSIGN:
                case MF_EQ:
                case MF_NOTEQ:
                case MF_LARGER:
                case MF_SMALLER:
                    break;
                case MF_READ:
                    if (right_ || left_)
                        correct = false;
                    break;
                case MF_CODE:
                case MF_IF:
                case MF_WHILE:
                    break;
            }
            break;
    }

    if (correct && left_)
        correct = left_->is_ok ();
    if (correct && right_)
        correct = right_->is_ok ();

    return correct;
}

bool Node::dumpDOT () const
{
    if (!is_ok ())
        THROW (BAD_NODE);

    FILE * dot_file = fopen ("data/dump.dot", "wb");
    if (!dot_file)
        THROW (CANNOT_OPEN_FILE);

    fprintf (dot_file, "digraph vk\n{\n");
    try {
        this->dump_node_in_dot (dot_file);}
    catch (Error err)
    {
        err.print_error ();
    }
    fprintf (dot_file, "}\n");

    fclose (dot_file);

    return true;
}

bool Node::dumpTEX (char * filename) const
{
    if (!filename)
    THROW (NULL_POINTER);

    FILE *tex_file = fopen (filename, "wb");
    if (!tex_file)
    THROW (CANNOT_OPEN_FILE);

    fprintf (tex_file, "\\documentclass{article}\n\\begin{document}\n$");
    dump_node_in_tex (tex_file);
    fprintf (tex_file, "$\n\\end{document}\n");

    fclose (tex_file);
}

void Node::dump_node_in_tex (FILE * tex_file) const
{
    if (!tex_file)
        THROW (NULL_POINTER);
    if (!this)
        return;

    if (left_ && mf_ == MF_DIV)
    {
        fprintf (tex_file, "\\frac{");
        left_->dump_node_in_tex (tex_file);
        fprintf (tex_file, "}{");
        right_->dump_node_in_tex (tex_file);
        fprintf (tex_file, "}");
    }
    else
    {
        if (left_)
            left_->dump_node_in_tex (tex_file);

        switch (type_)
        {
            default:
            case NAT:
                THROW (BAD_NODE);
                break;
            case VAL:
                fprintf (tex_file, "(%g)", val_);
                break;
            case VAR:
                fprintf (tex_file, "%c", var_);
                break;
            case MF:
                switch (mf_)
                {
                    default:
                    case MF_NAF:
                        THROW (BAD_NODE);
                        break;
                    case MF_ADD:
                    case MF_UADD:
                        fprintf (tex_file, "+");
                        right_->dump_node_in_tex (tex_file);
                        break;
                    case MF_SUB:
                    case MF_USUB:
                        fprintf (tex_file, "-");
                        right_->dump_node_in_tex (tex_file);
                        break;
                    case MF_MUL:
                    case MF_UMUL:
                        fprintf (tex_file, "*");
                        right_->dump_node_in_tex (tex_file);
                        break;
                    case MF_UDIV:
                        fprintf (tex_file, "/");
                        right_->dump_node_in_tex (tex_file);
                        break;
                    case MF_SIN:
                        fprintf (tex_file, "sin(");
                        right_->dump_node_in_tex (tex_file);
                        fprintf (tex_file, ")");
                        break;
                    case MF_COS:
                        fprintf (tex_file, "cos(");
                        right_->dump_node_in_tex (tex_file);
                        fprintf (tex_file, ")");
                        break;
                    case MF_TG:
                        fprintf (tex_file, "tg(");
                        right_->dump_node_in_tex (tex_file);
                        fprintf (tex_file, ")");
                        break;
                    case MF_CTG:
                        fprintf (tex_file, "ctg(");
                        right_->dump_node_in_tex (tex_file);
                        fprintf (tex_file, ")");
                        break;
                    case MF_EXP:
                        fprintf (tex_file, "exp(");
                        right_->dump_node_in_tex (tex_file);
                        fprintf (tex_file, ")");
                        break;
                    case MF_LN:
                        fprintf (tex_file, "ln(");
                        right_->dump_node_in_tex (tex_file);
                        fprintf (tex_file, ")");
                        break;
                    case MF_ASSIGN:
                        fprintf (tex_file, "=");
                        right_->dump_node_in_tex (tex_file);
                        break;
                    case MF_READ:
                        fprintf (tex_file, "READ ");
                        break;
                    case MF_PRINT:
                        fprintf (tex_file, "PRINT (");
                        right_->dump_node_in_tex (tex_file);
                        fprintf (tex_file, ")");
                        break;
                    case MF_EQ:
                        fprintf (tex_file, "==");
                        right_->dump_node_in_tex (tex_file);
                        break;
                    case MF_NOTEQ:
                        fprintf (tex_file, "!=");
                        right_->dump_node_in_tex (tex_file);
                        break;
                    case MF_LARGER:
                        fprintf (tex_file, ">");
                        right_->dump_node_in_tex (tex_file);
                        break;
                    case MF_SMALLER:
                        fprintf (tex_file, "<");
                        right_->dump_node_in_tex (tex_file);
                        break;
                    case MF_CODE:
                        fprintf (tex_file, "CODE ");
                        right_->dump_node_in_tex (tex_file);
                        break;
                    case MF_IF:
                        fprintf (tex_file, "IF ");
                        right_->dump_node_in_tex (tex_file);
                        break;
                    case MF_WHILE:
                        fprintf (tex_file, "WHILE ");
                        right_->dump_node_in_tex (tex_file);
                        break;
                }
                break;
        }
    }
}

void Node::dump_node_in_dot (FILE *dump_file) const
{
    assert (dump_file);

    if (left_)
    {
        fprintf (dump_file, "\t%d->%d\n", this, this->left_);
        left_->dump_node_in_dot (dump_file);
    }
    if (right_)
    {
        fprintf (dump_file, "\t%d->%d\n", this, this->right_);
        right_->dump_node_in_dot (dump_file);
    }

    fprintf (dump_file, "\t%d [label = \"", this);
    switch (type_)
    {
        default:
        case NAT:
            THROW (BAD_NODE);
            break;
        case VAR:
            fprintf (dump_file, "VAR\\n char %c, int %d\\n", var_, var_);
            break;
        case VAL:
            fprintf (dump_file, "VAL\\n double %g\\n", val_);
            break;
        case MF:
            switch (mf_)
            {
                default:
                case MF_NAF:
                THROW (BAD_NODE);
                    break;
                case MF_ADD:
                case MF_UADD:
                    fprintf (dump_file, "+");
                    break;
                case MF_SUB:
                case MF_USUB:
                    fprintf (dump_file, "-");
                    break;
                case MF_MUL:
                case MF_UMUL:
                    fprintf (dump_file, "*");
                    break;
                case MF_DIV:
                case MF_UDIV:
                    fprintf (dump_file, "/");
                    break;
                case MF_SIN:
                    fprintf (dump_file, "sin");
                    break;
                case MF_COS:
                    fprintf (dump_file, "cos");
                    break;
                case MF_TG:
                    fprintf (dump_file, "tg");
                    break;
                case MF_CTG:
                    fprintf (dump_file, "ctg");
                    break;
                case MF_EXP:
                    fprintf (dump_file, "exp");
                    break;
                case MF_LN:
                    fprintf (dump_file, "ln");
                    break;
                case MF_ASSIGN:
                    fprintf (dump_file, "=");
                    break;
                case MF_READ:
                    fprintf (dump_file, "read");
                    break;
                case MF_PRINT:
                    fprintf (dump_file, "print");
                    break;
                case MF_EQ:
                    fprintf (dump_file, "==");
                    break;
                case MF_NOTEQ:
                    fprintf (dump_file, "!=");
                    break;
                case MF_LARGER:
                    fprintf (dump_file, ">");
                    break;
                case MF_SMALLER:
                    fprintf (dump_file, "<");
                    break;
                case MF_CODE:
                    fprintf (dump_file, "code");
                    break;
                case MF_IF:
                    fprintf (dump_file, "if");
                    break;
                case MF_WHILE:
                    fprintf (dump_file, "while");
                    break;
            }
    }
    fprintf (dump_file, " \"]\n");
}

void Node::del ()
{
    if (left_)
        left_->del ();
    if (right_)
        right_->del ();
    delete this;
}

Node & Node::copy () const
{
    if (!is_ok ())
        THROW (BAD_NODE);

    Node * new_node = NULL;
    switch (type_)
    {
        default:
        case NAT:
            THROW (BAD_NODE);
            break;
        case VAL:
            new_node = new Node (val_);
            break;
        case VAR:
            new_node = new Node (var_);
            break;
        case MF:
            Node *new_left = NULL;
            Node *new_right = NULL;

            if (left_)
                new_left = &left_->copy ();
            if (right_)
                new_right = &right_->copy ();
            else
                THROW (BAD_NODE);

            new_node = new Node (mf_, new_left, new_right);
            break;
    }
    return *new_node;
}

Node & Node::diff (char variable)
{
#define dL  left_->diff (variable)
#define dR  right_->diff (variable)
#define cL  left_->copy ()
#define cR  right_->copy ()
#define EXP *(new Node (MF_EXP, left_, &cR))
#define COS *(new Node (MF_COS, left_, &cR))
#define SIN *(new Node (MF_SIN, left_, &cR))
#define MINUS_SIN *(new Node (MF_SIN, left_, &cR)) * *(new Node (-1.0, NULL, NULL))

    if (!is_ok ())
        THROW (BAD_NODE);

    switch (type_)
    {
        default:
        case NAT:
            THROW (BAD_NODE);
            break;
        case VAL:
                return *(new Node (0.0));
        case VAR:
            if (var_ == variable)
                return *(new Node (1.0));
            else
                return *(new Node (0.0));
        case MF:
            switch (mf_)
            {
                default:
                    THROW (BAD_NODE);
                    break;
                case MF_ADD:
                    return dL + dR;
                case MF_SUB:
                    return dL - dR;
                case MF_MUL:
                    return dL * cR + cL * dR;
                case MF_DIV:
                    return (dL * cR - cL * dR) / (cR * cR);
                case MF_SIN:
                    if (right_->type_ == VAR && right_->var_ != variable)
                        return *(new Node (0.0));
                    else
                        return dR * COS;

                case MF_COS:
                    if (right_->type_ == VAR && right_->var_ != variable)
                        return *(new Node (0.0));
                    else
                        return dR * MINUS_SIN;
                case MF_TG:
                    if (right_->type_ == VAR && right_->var_ != variable)
                        return *(new Node (0.0));
                    else
                        return dR / (COS * COS);
                case MF_CTG:
                    if (right_->type_ == VAR && right_->var_ != variable)
                        return *(new Node (0.0));
                    else
                        return dR / (MINUS_SIN * SIN);
                case MF_EXP:
                    if (right_->type_ == VAR && right_->var_ != variable)
                        return *(new Node (0.0));
                    else
                        return dR * EXP;
                case MF_LN:
                    if (right_->type_ == VAR && right_->var_ != variable)
                        return *(new Node (0.0));
                    else
                        return dR / cR;
            }
    }
#undef dL;
#undef dR;
#undef cL;
#undef cR;
#undef EXP;
#undef COS;
#undef SIN;
#undef MINUS_SIN;
}

bool Node::simplify ()
{
    if (!this)
    {
        WPRINT (NULL_POINTER);
        return false;
    }
    try
    {
        if (left_)
            left_->simplify ();
        if (right_)
            right_->simplify ();
    }
    catch (Error err)
    {
        err.print_error ();
    }

    if (type_ == MF && right_->type_ == VAL)
    {
        if (!left_)
        {
            double val = right_->val_ * M_PI / 180;
            switch (mf_)
            {
                default:
                case MF_NAF:
                    THROW (BAD_NODE);
                    break;
                case MF_SIN:
                    val = sin (val);
                    break;
                case MF_COS:
                    val = cos (val);
                    break;
                case MF_TG:
                    val = tan (val);
                    break;
                case MF_CTG:
                    val = 1 / tan (val);
                    break;
                case MF_EXP:
                    val = exp (right_->val_);
                    break;
                case MF_LN:
                    val = right_->val_;
                    if (val <= 0)
                        THROW (BAD_EXPRESSION);
                    val = log (val);
                    break;
            }
            right_->del ();
            type_ = VAL;
            mf_ = MF_NAF;
            val_ = val;
            right_ = NULL;
        }
        else if (left_->type_ == VAL)
        {
            double val = left_->val_;
            switch (mf_)
            {
                default:
                case MF_NAF:
                    THROW (BAD_NODE);
                    break;
                case MF_ADD:
                    val += right_->val_;
                    break;
                case MF_SUB:
                    val -= right_->val_;
                    break;
                case MF_MUL:
                    val *= right_->val_;
                    break;
                case MF_DIV:
                    if (right_->val_ == 0.0)
                        THROW (BAD_EXPRESSION);
                    val /= right_->val_;
                    break;
            }
            right_->del ();
            left_->del ();
            right_ = NULL;
            left_ = NULL;
            mf_ = MF_NAF;
            type_ = VAL;
            val_ = val;
        }
    }
}

Node & operator+ (Node & left, Node & right)
{
    if (!left.is_ok ())
        THROW (BAD_NODE);
    if (!right.is_ok ())
        THROW (BAD_NODE);

    return *(new Node (MF_ADD, &left, &right));
}

Node & operator- (Node & left, Node & right)
{
    if (!left.is_ok ())
        THROW (BAD_NODE);
    if (!right.is_ok ())
        THROW (BAD_NODE);

    return *(new Node (MF_SUB, &left, &right));
}

Node & operator* (Node & left, Node & right)
{
    if (!left.is_ok ())
        THROW (BAD_NODE);
    if (!right.is_ok ())
        THROW (BAD_NODE);

    return *(new Node (MF_MUL, &left, &right));
}

Node & operator/ (Node & left, Node & right)
{
    if (!left.is_ok ())
        THROW (BAD_NODE);
    if (!right.is_ok ())
        THROW (BAD_NODE);

    return *(new Node (MF_DIV, &left, &right));
}

Node *Node::get_right ()
{
    return right_;
}

Node *Node::get_left ()
{
    return left_;
}

Node **Node::get_pright ()
{
    return &right_;
}

Math_Func Node::get_mf ()
{
    return mf_;
}


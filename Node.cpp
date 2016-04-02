//
// Created by epicfail on 29.03.16.
//

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
        correct = false;

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
                    if (!right_ || left_)
                        correct = false;
                    break;
                case MF_ADD:
                case MF_SUB:
                case MF_MUL:
                case MF_DIV:
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

bool Node::dump () const
{
    if (!is_ok ())
        THROW (BAD_NODE);

    FILE * dump_file = fopen ("dump.txt", "wb");
    if (!dump_file)
        THROW (CANNOT_OPEN_FILE);

    try { this->dump_node (dump_file, 1);}
    catch (Error err)
    {
        err.print_error ();
    }

    fclose (dump_file);

    return true;
}

void Node::dump_node (FILE * dump_file, size_t shift) const
{
    if (left_)
        left_->dump_node (dump_file, shift + 1);
    if (right_)
        right_->dump_node (dump_file, shift + 1);

    for (size_t i = 0; i < shift; i++)
        fprintf (dump_file, "----");

    fprintf (dump_file, "%d\ttype: %2u\t", this, type_);
    switch (type_)
    {
        default:
        case NAT:
            THROW (BAD_NODE);
            break;
        case VAR:
            fprintf (dump_file, "data: char %c, int %d\t", var_, var_);
            break;
        case VAL:
            fprintf (dump_file, "data: double %lf\t", val_);
            break;
        case MF:
            fprintf (dump_file, "data: Math_Func  %u\t", mf_);
            break;
    }
    fprintf (dump_file, "left: %8d\tright: %8d\n", left_, right_);
}

void Node::del ()
{
    if (left_)
        left_->del ();
    if (right_)
        right_->del ();
    delete this;
}

Node & Node::copy ()
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
                return *(new Node (var_));
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
                    return dR * COS;
                case MF_COS:
                    return dR * MINUS_SIN;
                case MF_TG:
                    return dR / (COS * COS);
                case MF_CTG:
                    return dR / (MINUS_SIN * SIN);
                case MF_EXP:
                    return dR * EXP;
                case MF_LN:
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


#include "Node.h"
#include "Error.h"

Node * syntactic_parcer (char ** expression);

int main ()
{
    char * str = "(ln(x)) + (x)";
    Node * new_nd = syntactic_parcer (&str);
    Node * nnd = &new_nd->diff ('x');
    new_nd->del ();
    nnd->dump ();
    nnd->del ();

    return 0;
}

Node * syntactic_parcer (char ** expression)
{
#define DER_EXP    (*expression)     //derefence the expression

    if (!expression)
        THROW (NULL_POINTER);
    if (!DER_EXP)
        THROW (NULL_POINTER);

    Node *left_child = NULL;
    Node *right_child = NULL;
    Type node_type = NAT;
    Math_Func func_or_op = MF_NAF;
    char variable = '\0';
    double value = 0.0;

    while (*DER_EXP == ' ')
        DER_EXP++;

    if (*DER_EXP == '(')
    {
        DER_EXP++;
        left_child = syntactic_parcer (expression);
        if (!left_child)
            THROW (NULL_POINTER);
    }

    while (*DER_EXP == ' ')
        DER_EXP++;

    char cur[2] = {'\0', '\0'};
    if (*DER_EXP != '\0')
    {
        cur[0] = *DER_EXP;
        cur[1] = *(DER_EXP + 1);
    }
    else
        THROW (BAD_EXPRESSION);

    switch (cur[0])
    {
        default:
            if ('a' <= *DER_EXP && *DER_EXP <= 'z')
            {
                node_type = VAR;
                variable = cur[0];
                DER_EXP += 2;
            }
            else
                THROW (BAD_EXPRESSION);
            break;

        case '+':
            if (cur[1] == ' ' || cur[1] == '(')
            {
                node_type = MF;
                func_or_op = MF_ADD;
                DER_EXP++;
            }
            else if ('0' <= cur[1] && cur[1] <= '9')
            {
                DER_EXP++;
                node_type = VAL;
                sscanf (DER_EXP, "%lf", &value);
                while (*DER_EXP != ')')
                    DER_EXP++;
            }
            else
                THROW (BAD_EXPRESSION);
            break;

        case '-':
            if (cur[1] == ' ' || cur[1] == '(')
            {
                node_type = MF;
                func_or_op = MF_SUB;
                DER_EXP++;
            }
            else if ('0' <= cur[1] && cur[1] <= '9')
            {
                DER_EXP++;
                node_type = VAL;
                sscanf (DER_EXP, "%lf", &value);
                value = -value;
                while (*DER_EXP != ')')
                    DER_EXP++;
            }
            else
                THROW (BAD_EXPRESSION);
            break;

        case '*':
            node_type = MF;
            func_or_op = MF_MUL;
            DER_EXP++;
            break;

        case '/':
            node_type = MF;
            func_or_op = MF_DIV;
            DER_EXP++;
            break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            node_type = VAL;
            sscanf (DER_EXP, "%lf", &value);
            while (*DER_EXP != ')')
                DER_EXP++;
            break;

        case 's':
            node_type = MF;
            DER_EXP += 3;
            if (cur[1] == 'i')
                func_or_op = MF_SIN;
            else
                THROW (BAD_EXPRESSION);
            break;

        case 'c':
            node_type = MF;
            DER_EXP += 3;
            if (cur[1] == 'o')
                func_or_op = MF_COS;
            else if (cur[1] == 't')
                func_or_op = MF_CTG;
            else
                THROW (BAD_EXPRESSION);
            break;

        case 't':
            node_type = MF;
            DER_EXP += 2;
            if (cur[1] == 'g')
                func_or_op = MF_TG;
            else
                THROW (BAD_EXPRESSION);
            break;

        case 'e':
            node_type = MF;
            DER_EXP += 3;
            if (cur[1] == 'x')
                func_or_op = MF_EXP;
            else
                THROW (BAD_EXPRESSION);
            break;

        case 'l':
            node_type = MF;
            DER_EXP += 2;
            if (cur[1] == 'n')
                func_or_op = MF_LN;
            else
                THROW (BAD_EXPRESSION);
            break;

    }

    while (*DER_EXP == ' ')
        DER_EXP++;

    if (*DER_EXP == '(')
    {
        DER_EXP++;
        right_child = syntactic_parcer (expression);
        if (!right_child)
            THROW (NULL_POINTER);
    }

    Node * node = NULL;
    if (node_type == VAR)
        node = new Node (variable, left_child, right_child);
    else if (node_type == VAL)
        node = new Node (value, left_child, right_child);
    else if (node_type == MF)
        node = new Node (func_or_op, left_child, right_child);

    while (*DER_EXP == ')')
        DER_EXP++;
    return node;

#undef DER_EXP
}
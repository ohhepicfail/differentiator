#include "Node.h"
#include "Expression_parser.h"
#include "Error.h"

int main ()
{
    Expression_parser expression;
    expression.get_expression ("data/exp.txt");
    Node * exp_node = NULL;
    try {exp_node = expression.parse ();} catch (Error err) {err.print_error (); return 1;}
//    exp_node->simplify ();
    exp_node->dumpDOT ();
//    exp_node = &exp_node->diff ('x');
//    exp_node->simplify ();
    exp_node->dumpTEX ("data/math.tex");
    exp_node->del ();

    return 0;
}
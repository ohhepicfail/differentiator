#include "Node.h"
#include "Error.h"
#include "Expression_parser.h"

int main ()
{
//    char * str =      "((((exp(x)) + (ln((x) * (x))))*((sin(((x)*(x))/((x) + (4)))) / (cos(x))))/(((exp(x)) + (ln((x) * (x))))*((sin(((x)*(x))/((x) + (4)))) / (cos(x)))))/((((exp(x)) + (ln((x) * (x))))*((sin(((x)*(x))/((x) + (4)))) / (cos(x))))/(((exp(x)) + (ln((x) * (x))))*((sin(((x)*(x))/((x) + (4)))) / (cos(x)))))";
//     "((((exp(x)) + (ln((x) * (x))))*((sin(((x)*(x))/((x) + (4)))) / (cos(x))))/(((exp(x)) + (ln((x) * (x))))*((sin(((x)*(x))/((x) + (4)))) / (cos(x)))))/((((exp(x)) + (ln((x) * (x))))*((sin(((x)*(x))/((x) + (4)))) / (cos(x))))/(((exp(x)) + (ln((x) * (x))))*((sin(((x)*(x))/((x) + (4)))) / (cos(x)))))"

    Expression_parser expression;
    expression.get_expression ("exp.txt");
    Node * exp_node = expression.parse ();
    exp_node->dump ();
    exp_node = &exp_node->diff ('x');
    exp_node->dumpTEX ("math.tex");
    exp_node->del ();

    return 0;
}
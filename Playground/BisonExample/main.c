/*
 * main.c file
 */

#include "Expression.h"
#include "Parser.h"
#include "Lexer.h"

#include <stdio.h>

int yyparse(SExpression** expression, yyscan_t scanner);

SExpression* getAST(const char* expr)
{
    SExpression* expression;
    yyscan_t scanner;
    YY_BUFFER_STATE state;

    if (yylex_init(&scanner))
    {
        // could not initialize
        return NULL;
    }

    state = yy_scan_string(expr, scanner);

    if (yyparse(&expression, scanner))
    {
        // error parsing
        return NULL;
    }

    yy_delete_buffer(state, scanner);

    yylex_destroy(scanner);

    return expression;
}

int evaluate(SExpression* e)
{
    switch (e->type)
    {
        case eVALUE:
            return e->value;
        case eMULTIPLY:
            return evaluate(e->left) * evaluate(e->right);
        case ePLUS:
            return evaluate(e->left) + evaluate(e->right);
        default:
            // should not be here
            return 0;
    }
}

void test(const char* text)
{
    SExpression* e = NULL;

    e = getAST(text);

    if (!e)
    {
        fprintf(stderr, "Failed to parse expression: %s\n", text);
        return;
    }

    int result = evaluate(e);

    if (result == 0)
    {
        printf("Result of '%s' is %d\n", text, result);
    }

    deleteExpression(e);
}

int main(void)
{
    test("4 ~ 3");
    test("4 + 3");
    test("4 + 2");
    return 0;
}

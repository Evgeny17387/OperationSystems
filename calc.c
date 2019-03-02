
////////////////////////////////////////////////// Includes

#include <stdio.h>
#include <string.h>

////////////////////////////////////////////////// Defines

// Constants

#define ERROR                       "error\n"
#define EXIT                        "exit"
#define EXPRESSION_MAX_SIZE         100
#define END_INPUT_CODE              13
#define DIGIT_FIRST                 '0'
#define DIGIT_LAST                  '9'
#define WHITE_SPACE                 ' '

// Error codes

#define SUCCESS                      0
#define ERROR_NOT_DIGIT             -1
#define ERROR_NOT_OPERATOR          -2
#define ERROR_INCORRECT_TEMPLATE    -3
#define ERROR_DIVISION_BY_ZERO      -4
#define ERROR_UNKNOWN               -5

// Calculation system states

#define STATE_DIGIT                 0
#define STATE_WHITE_SPACE_FIRST     1
#define STATE_WHITE_SPACE_MORE      2
#define STATE_OPERATOR_OR_DIGIT     3
#define STATE_CALCULATE             4
#define STATE_ERONEUSE              5

// Operatos

#define OPERATOR_ADD               '+'
#define OPERATOR_SUB               '-'
#define OPERATOR_MUL               '*'
#define OPERATOR_DIV               '/'

// Operatios

#define OPERATION_ADD               0
#define OPERATION_SUB               1
#define OPERATION_MUL               2
#define OPERATION_DIV               3

////////////////////////////////////////////////// Auxiliary functions

// Parse the expression from the user and prints 'error' if not in tenpate, otherwise calculates it
int parseExpression(char* input);

// Calculates digit out of char, otherwise returns error if not digit
int getNum(char input);

// If operator eligible, returns itm otherwise returns an error
int getOperation(char input);

// Calculates the result of an expression and returns it
int calculate(int left_operand, int right_operand, int operation);

////////////////////////////////////////////////// Main Function

int main() {

    // Variables

    char expression[EXPRESSION_MAX_SIZE];

    // First input from user

    fgets(expression, EXPRESSION_MAX_SIZE, stdin);

    // Calculate expressions until user enters 'exit'

    while (strcmp(expression, EXIT)){

        if (parseExpression(expression) != SUCCESS){
            printf(ERROR);
        }

        fgets(expression, EXPRESSION_MAX_SIZE, stdin);

    }

    return 0;

}

////////////////////////////////////////////////// Auxiliary functions

int parseExpression(char* input){

    int i                   = 0;
    int numberOfOperators   = 0;

    int state;
    int left_operand;
    int right_operand;
    int operation;
    int result;

    // Check first digit accordignly to template

    left_operand = getNum(input[i]);

    if (left_operand == ERROR_NOT_DIGIT){
        return ERROR_INCORRECT_TEMPLATE;
    }

    // Check rest of the expression using a state machine because we don't know the number of whitespaces

    state = STATE_WHITE_SPACE_FIRST;
    i++;

    while ((input[i] != END_INPUT_CODE) && (i < EXPRESSION_MAX_SIZE)){

//        printf("%c - %d - %d - %d\n", input[i], input[i], i, state);

        switch(state)
        {

            case STATE_WHITE_SPACE_FIRST:
                // must be at least one white space
                if (input[i] != WHITE_SPACE){
                    return ERROR_INCORRECT_TEMPLATE;
                }
                state = STATE_WHITE_SPACE_MORE;
                i++;
                break;

            case STATE_WHITE_SPACE_MORE:
                // can be more then one white space
                if (input[i] == WHITE_SPACE){
                    i++;
                    break;
                }
                state = STATE_OPERATOR_OR_DIGIT;
                break;

            case STATE_OPERATOR_OR_DIGIT:
                // we expect an operator
                if (numberOfOperators == 0){
                    operation = getOperation(input[i]);
                    if (operation == ERROR_NOT_OPERATOR){
                        return ERROR_INCORRECT_TEMPLATE;
                    }
                    state = STATE_WHITE_SPACE_FIRST;
                    numberOfOperators = 1;
                    i++;
                    break;
                }
                // otherwise, we expect a digit
                right_operand = getNum(input[i]);
                if (right_operand == ERROR_NOT_DIGIT){
                    return ERROR_INCORRECT_TEMPLATE;
                }
                state = STATE_CALCULATE;
                break;

            case STATE_CALCULATE:
                if ((operation == OPERATION_DIV) && (right_operand == 0)){
                    return ERROR_DIVISION_BY_ZERO;
                }
                result = calculate(left_operand, right_operand, operation);
                printf("%d\n", result);
                return SUCCESS;

        }

    }

    return ERROR_UNKNOWN;

}

int getNum(char input){
    if (input < DIGIT_FIRST || input > DIGIT_LAST){
        return ERROR_NOT_DIGIT;
    }
    return input - DIGIT_FIRST;
}

int getOperation(char input){

    int operation;

    switch(input)
    {
        case OPERATOR_ADD:
            operation = OPERATION_ADD;
            break;
        case OPERATOR_SUB:
            operation = OPERATION_SUB;
            break;
        case OPERATOR_MUL:
            operation = OPERATION_MUL;
            break;
        case OPERATOR_DIV:
            operation = OPERATION_DIV;
            break;
        default:
            operation = ERROR_NOT_OPERATOR;
            break;

    }

    return operation;

}

int calculate(int left_operand, int right_operand, int operation){

    int result;

    switch(operation)
    {
        case OPERATION_ADD:
            result = left_operand + right_operand;
            break;
        case OPERATION_SUB:
            result = left_operand - right_operand;
            break;
        case OPERATION_MUL:
            result = left_operand * right_operand;
            break;
        case OPERATION_DIV:
            result = left_operand / right_operand;
            break;

    }

    return result;

}

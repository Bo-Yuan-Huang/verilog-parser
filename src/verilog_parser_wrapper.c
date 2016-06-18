
/*!
@file verilog_parser.c
@brief Contains implementations of functions declared in verilog_parser.h
*/

#include "verilog_parser.h"

/*!
@brief Sets up the parser to accept more input from a new input file.
@details Acts as a wrapper around yyrestart()
*/
void    verilog_parser_setup(FILE * input_file)
{
    yyrestart(input_file);
}

/*!
@brief Perform a parsing operation on the currently selected buffer.
@details Acts as a wrapper around yyparse().
*/
int     verilog_parse_current_buffer()
{
    return yyparse();
}


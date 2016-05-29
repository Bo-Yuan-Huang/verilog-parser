/*!
@file verilog_ast.c
@brief Contains definitions of functions which 
       operate on the Verilog Abstract Syntax Tree (AST)
*/

#include "assert.h"
#include "stdio.h"

#include "verilog_ast.h"

/*!
@brief Creates a new empty ast_node and returns it.
*/
ast_node * ast_node_new()
{
    ast_node * tr = calloc(1, sizeof(ast_node));;

    tr-> type         = NONE;
    tr-> parent       = NULL;
    tr-> children     = NULL;
    tr-> child_count  = 0;
    
    return tr;
}

/*!
@brief Creates and returns a new node for the tree which contains a
       single simple identifier.
*/
ast_node * ast_new_identifier_node(ast_identifier identifier)
{
    ast_node * tr = ast_node_new();

    tr->type         = IDENTIFIER;
    tr->value.string = identifier;

    return tr;
}

/*!
@brief Creates and returns as a pointer a new attribute descriptor.
*/
ast_node_attributes * ast_new_attributes(ast_identifier name, ast_node * value)
{
    ast_node_attributes * tr = calloc(1, sizeof(ast_node_attributes));
    tr->attr_name   = name;
    tr->attr_value  = value;
    return tr;
}


/*!
@brief Creates and returns a new attribute node with the specified value
       and name.
*/
ast_node * ast_new_attribute_node(ast_node_attributes * value)
{
    ast_node * tr = ast_node_new();

    tr -> type = ATTRIBUTE_LIST;
    tr -> value.attributes = value;

    return tr;
}

/*!
@brief Creates and returns a new attribute node with the specified value
       and name.
@param [inout] parent - Pointer to the node which represents the list of
                        attribute name,value pairs.
@param [in]    toadd  - The new attribute to add.
*/
void ast_append_attribute(ast_node_attributes * parent, 
                          ast_node_attributes * toadd)
{
    // Add the new attribute to the end of the list.

    ast_node_attributes * walker = parent -> next;
    while(walker -> next != NULL)
        walker = walker -> next;
    walker -> next = toadd;

}

/*!
@brief Creates and returns a new @ref ast_lvalue pointer, with the data type
       being a single identifier of either @ref NET_IDENTIFIER or
       @ref VAR_IDENTIFIER.
*/
ast_lvalue * ast_new_lvalue_id(ast_lvalue_type type, ast_identifier id)
{
    assert(type == NET_IDENTIFIER || type == VAR_IDENTIFIER);
    ast_lvalue * tr = calloc(1, sizeof(ast_lvalue));
    tr -> type = type;
    tr -> data.identifier = id;
    return tr;
}

/*!
@brief Creates and returns a new @ref ast_lvalue pointer, with the data type
       being a concatenation holder of either @ref NET_CONCATENATION or
       @ref VAR_CONCATENATION.
*/
ast_lvalue * ast_new_lvalue_concat(ast_lvalue_type type, 
                                   ast_concatenation*concat)
{
    assert(type == NET_CONCATENATION || type == VAR_CONCATENATION);
    ast_lvalue * tr = calloc(1, sizeof(ast_lvalue));
    tr -> type = type;
    tr -> data.concatenation = concat;
    return tr;
}

/*!
@brief Creates a new ast primary which is part of a constant expression tree
       with the supplied type and value.
*/
ast_primary * ast_new_constant_primary(ast_primary_value_type type)
{
    ast_primary * tr = calloc(1, sizeof(ast_primary));

    tr -> primary_type  = CONSTANT_PRIMARY;
    tr -> value_type    = type;

    return tr;
}


/*!
@brief Creates a new AST primary wrapper around a function call.
*/
ast_primary * ast_new_primary_function_call(ast_function_call * call)
{
    ast_primary * tr = calloc(1, sizeof(ast_primary));

    tr -> primary_type  = PRIMARY;
    tr -> value_type    = PRIMARY_FUNCTION_CALL;
    tr -> value.function_call = call;

    return tr;
}


/*!
@brief Creates a new ast primary which is part of an expression tree
       with the supplied type and value.
*/
ast_primary * ast_new_primary(ast_primary_value_type type)
{
    ast_primary * tr = calloc(1, sizeof(ast_primary));

    tr -> primary_type  = PRIMARY;
    tr -> value_type    = type;

    return tr;
}

/*!
@brief Creates a new ast primary which is part of a constant expression tree
       with the supplied type and value.
*/
ast_primary * ast_new_module_path_primary(ast_primary_value_type type)
                                          
{
    ast_primary * tr = calloc(1, sizeof(ast_primary));

    tr -> primary_type  = MODULE_PATH_PRIMARY;
    tr -> value_type    = type;

    return tr;
}

/*!
@brief Creates and returns a new expression primary.
@description This is simply an expression instance wrapped around a
primary instance for the purposes of mirroring the expression tree gramamr.
Whether or not the expression is constant is denoted by the type member
of the passed primary.
*/
ast_expression * ast_new_expression_primary(ast_primary * p)
{
    ast_expression * tr = calloc(1, sizeof(ast_expression));
    
    tr -> attributes    = NULL;
    tr -> right         = NULL;
    tr -> left          = NULL;
    tr -> aux           = NULL;
    tr -> type          = PRIMARY_EXPRESSION;
    tr -> primary       = p;
    tr -> constant      = p -> primary_type == CONSTANT_PRIMARY ? AST_TRUE 
                                                                : AST_FALSE;

    return tr;
}


/*!
@brief Creates a new unary expression with the supplied operation.
*/
ast_expression * ast_new_unary_expression(ast_expression * operand,
                                          ast_operator     operation,
                                          ast_node_attributes * attr,
                                          ast_boolean       constant)
{
    ast_expression * tr = calloc(1, sizeof(ast_expression));
    
    tr -> operation     = operation;
    tr -> attributes    = attr;
    tr -> right         = operand;
    tr -> left          = NULL;
    tr -> aux           = NULL;
    tr -> type          = UNARY_EXPRESSION;
    tr -> constant      = constant;

    return tr;
}

/*!
@brief Creates a new range expression with the supplied operands.
*/
ast_expression * ast_new_range_expression(ast_expression * left,
                                          ast_expression * right)
{
    ast_expression * tr = calloc(1, sizeof(ast_expression));
    
    tr -> attributes    = NULL;
    tr -> right         = right;
    tr -> left          = left;
    tr -> aux           = NULL;
    tr -> type          = RANGE_EXPRESSION_UP_DOWN;

    return tr;
}
                                           
/*!
@brief Creates a new range index expression with the supplied operands.
*/
ast_expression * ast_new_index_expression(ast_expression * left)
{
    ast_expression * tr = calloc(1, sizeof(ast_expression));
    
    tr -> attributes    = NULL;
    tr -> right         = NULL;
    tr -> left          = left;
    tr -> aux           = NULL;
    tr -> type          = RANGE_EXPRESSION_INDEX;

    return tr;
}

/*!
@brief Creates a new primary expression with the supplied operation
and operands.
@note Sets the type of the expression 
*/
ast_expression * ast_new_binary_expression(ast_expression * left,
                                           ast_expression * right,
                                           ast_operator     operation,
                                           ast_node_attributes * attr,
                                           ast_boolean      constant)
{
    ast_expression * tr = calloc(1, sizeof(ast_expression));
    
    tr -> operation     = operation;
    tr -> attributes    = attr;
    tr -> right         = right;
    tr -> left          = left;
    tr -> aux           = NULL;
    tr -> type          = BINARY_EXPRESSION;
    tr -> constant      = constant;

    return tr;
}


/*!
@brief Creates a new string expression.
*/
ast_expression * ast_new_string_expression(ast_string string)
{
    ast_expression * tr = calloc(1, sizeof(ast_expression));

    tr -> attributes    = NULL;
    tr -> right         = NULL;
    tr -> left          = NULL;
    tr -> aux           = NULL;
    tr -> type          = STRING_EXPRESSION;
    tr -> constant      = AST_TRUE;
    tr -> string        = string;
    
    return tr;
}


/*!
@brief Creates a new conditional expression node. 
@note The condition is stored in the aux member, if_true in left, and if_false
on the right.
*/
ast_expression * ast_new_conditional_expression(ast_expression * condition,
                                                ast_expression * if_true,
                                                ast_expression * if_false,
                                                ast_node_attributes * attr)
{
    ast_expression * tr = calloc(1, sizeof(ast_expression));

    tr -> attributes    = attr;
    tr -> right         = if_false;
    tr -> left          = if_true;
    tr -> aux           = condition;
    tr -> type          = CONDITIONAL_EXPRESSION;
    
    return tr;
}

/*!
@brief Creates a new (min,typical,maximum) expression.
@decription If the mintypmax expression only specifies a typical value,
then the min and max arguments should be NULL, and only typ set. 
*/
ast_expression * ast_new_mintypmax_expression(ast_expression * min,
                                              ast_expression * typ,
                                              ast_expression * max)
{
    ast_expression * tr = calloc(1, sizeof(ast_expression));

    tr -> attributes    = NULL;
    tr -> right         = max;
    tr -> left          = min;
    tr -> aux           = typ;
    tr -> type          = MINTYPMAX_EXPRESSION;
    
    return tr;
}


/*!
@brief Creates and returns a new node representing a function call.
@param [in] arguments - list of elements of type ast_expression
representing the various parameters to the function. If the function has
no arguments, then it is an empty list, not NULL. If this is supplied as
NULL, then an empty list is added automatically by the function.
@todo Implement the internals of this.
*/
ast_function_call * ast_new_function_call(ast_identifier  id,
                                          ast_boolean     constant,
                                          ast_boolean     system,
                                          ast_node_attributes * attr,
                                          ast_list      * arguments)
{
    ast_function_call * tr = calloc(1, sizeof(ast_function_call));

    tr -> function  = id;
    tr -> constant  = constant;
    tr -> system    = system;
    tr -> arguments = arguments;
    tr -> attributes = attr;
    
    if(tr -> arguments == NULL)
    {
        tr -> arguments = ast_list_new();
    }

    return tr;
}


/*!
@brief Creates a new AST concatenation element with the supplied type and
initial starting value.
@param [in] repeat - Used for replications or multiple_concatenation
@description Depending on the type supplied, the type of first_value
should be:
    - CONCATENATION_EXPRESSION          : ast_expression
    - CONCATENATION_CONSTANT_EXPRESSION : ast_expression
    - CONCATENATION_NET                 : TBD
    - CONCATENATION_VARIABLE            : TBD
    - CONCATENATION_MODULE_PATH         : TBD
@todo Better implement repetition of elements.
*/
ast_concatenation * ast_new_concatenation(ast_concatenation_type type,
                                          ast_expression * repeat,
                                          void * first_value)
{
    ast_concatenation * tr = calloc(1,sizeof(ast_concatenation));

    tr -> repeat = repeat;
    tr -> type   = type;
    tr -> items  = ast_list_new();
    ast_list_append(tr -> items, first_value);

    return tr;
}

/*!
@brief Creates and returns a new empty concatenation of the specified type.
*/
ast_concatenation * ast_new_empty_concatenation(ast_concatenation_type type)
{
    ast_concatenation * tr = calloc(1,sizeof(ast_concatenation));

    tr -> repeat = NULL;
    tr -> type   = type;
    tr -> items  = ast_list_new();

    return tr;
}


/*!
@brief Adds a new data element on to the end of a concatenation.
@description Appends to the front because this naturally follows the
behaviour of a left-recursive grammar.
@todo Better implement repetition of elements.
*/
void                ast_extend_concatenation(ast_concatenation * element,
                                             ast_expression * repeat,
                                             void * data)
{
    ast_list_preappend(element -> items, data);
}

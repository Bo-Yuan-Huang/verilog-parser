/*!
@file verilog_ast.h
@brief Contains Declarations of datastructures and functions which represent
       and operate on the Verilog Abstract Syntax Tree (AST)
*/

#include "stdarg.h"
#include "stdlib.h"

#ifndef VERILOG_AST_H
#define VERILOG_AST_H

//! Forward declare. Defines the core node type for the AST.
typedef struct ast_node_t ast_node;

//! Typedef to make it easier to change into a proper structure later.
//! @todo add proper support for hierarchical identifiers and scope.
typedef char * ast_identifier;

//! Placeholder until this is implemented properly.
typedef void * ast_concatenation;

typedef struct ast_expression_t ast_expression;
    
typedef void * ast_number       ;
typedef char * ast_operator     ;
typedef void * ast_function_call;
typedef void * ast_system_call  ;
typedef void * ast_minmax_exp   ;
typedef void * ast_macro_use    ;
typedef char   ast_boolean      ;

//-------------- attributes ------------------------------------

/*!
@brief Node data describing an attribute.
*/
typedef struct ast_node_attributes_t ast_node_attributes;
struct ast_node_attributes_t
{
    char                * attr_name;    //!< Name of the attribute
    ast_node            * attr_value;   //!< Value of the attribute.

    ast_node_attributes * next;         //!< Next one in a linked list.
};


/*!
@brief Creates and returns a new attribute node with the specified value
       and name.
*/
ast_node * ast_new_attribute_node( ast_node_attributes* value);

/*!
@brief Creates and returns a new attribute node with the specified value
       and name.
@param [inout] parent - Pointer to the node which represents the list of
                        attribute name,value pairs.
@param [in]    toadd  - The new attribute to add.
*/
void ast_append_attribute(ast_node_attributes * parent, 
                          ast_node_attributes * toadd);

// -------------------------------- L Value ------------------------

/*!
@brief Identifies the kind of LValue the @ref ast_lvalue structure holds.
*/
typedef enum ast_lvalue_type_e
{
    NET_IDENTIFIER,
    VAR_IDENTIFIER,
    NET_CONCATENATION,
    VAR_CONCATENATION
} ast_lvalue_type;

/*!
@brief Storage for the data describing an assignment L Value.
*/
typedef union ast_lvalue_data_u
{
    ast_identifier      identifier;     //!< Identifier value.
    ast_concatenation   concatenation;  //!< Concatenation list.
} ast_lvalue_data ;

/*!
@brief Stores and describes an expression l value.
*/
typedef struct ast_lvalue_t
{
    ast_lvalue_data data; //!< The identifier or concattenation being assigned.
    ast_lvalue_type type; //!< The type of the L Value assignment.
} ast_lvalue;

/*!
@brief Creates and returns a new @ref ast_lvalue pointer, with the data type
       being a single identifier of either @ref NET_IDENTIFIER or
       @ref VAR_IDENTIFIER.
*/
ast_lvalue * ast_new_lvalue_id(ast_lvalue_type type, ast_identifier id);

/*!
@brief Creates and returns a new @ref ast_lvalue pointer, with the data type
       being a concatenation holder of either @ref NET_CONCATENATION or
       @ref VAR_CONCATENATION.
*/
ast_lvalue * ast_new_lvalue_concat(ast_lvalue_type type, ast_concatenation id);


// -------------------------------- Primaries ----------------------

//! Describes the kind of expression primary being represented.
typedef enum ast_primary_type_e
{
    CONSTANT_PRIMARY,
    PRIMARY,
    MODULE_PATH_PRIMARY
} ast_primary_type;


//! The kind of production the expression primary holds.
typedef enum ast_primary_value_type_e
{
    PRIMARY_NUMBER,
    PRIMARY_IDENTIFIER,
    PRIMARY_CONCATENATION,
    PRIMARY_FUNCTION_CALL,
    PRIMARY_SYSTEM_CALL,
    PRIMARY_MINMAX_EXP,
    PRIMARY_MACRO_USAGE    
} ast_primary_value_type;

//! The expression primary can produce several different sub-expressions:
typedef union ast_primary_value_e
{
    ast_number          number;
    ast_identifier      identifier;
    ast_concatenation   concatenation;
    ast_function_call   function_call;
    ast_system_call     system_call;
    ast_minmax_exp      minmax;
    ast_macro_use       macro;
} ast_primary_value;

//! Stores the type and value of an AST primary expression.
typedef struct ast_primary_t
{
    ast_primary_type        primary_type;
    ast_primary_value_type  value_type;
    ast_primary_value       value;
} ast_primary;


/*!
@brief Creates a new ast primary which is part of a constant expression tree
       with the supplied type and value.
*/
ast_primary * ast_new_constant_primary(ast_primary_value_type type);

/*!
@brief Creates a new ast primary which is part of an expression tree
       with the supplied type and value.
*/
ast_primary * ast_new_primary(ast_primary_value_type type);

/*!
@brief Creates a new ast primary which is part of a constant expression tree
       with the supplied type and value.
*/
ast_primary * ast_new_module_path_primary(ast_primary_value_type type);

// -------------------------------- Expressions --------------------

//! Describes the kind of expression a node contains.
typedef enum ast_expression_type_e
{
    PRIMARY_EXPRESSION,                 //!< A straight value
    UNARY_EXPRESSION,                   //!< A unary op: "~bits" for example.
    BINARY_EXPRESSION,                  //!< The "normal" expression
    RANGE_EXPRESSION_UP_DOWN,           //!< Bit range expression
    RANGE_EXPRESSION_INDEX,             //!< Bit index expression
    MINTYPMAX_EXPRESSION,               //!< Minimum typical maximum
    CONDITIONAL_EXPRESSION,             //!< Conditional expression
    MODULE_PATH_EXPRESSION,
    MODULE_PATH_MINTYPMAX_EXPRESSION,
    STRING_EXPRESSION
} ast_expression_type;


/*! 
@brief Storage type for an entire expression / subexpression tree.
*/
struct ast_expression_t
{
    ast_expression_type type;           //!< What sort of expression is this?
    ast_node_attributes * attributes;   //!< Additional expression attributes.
    ast_operator     operation;         //!< What are we doing?
    ast_expression * left;              //!< LHS of operation
    ast_expression * right;             //!< RHS of operation
    ast_expression * aux;               //!< Optional auxiliary/predicate.
    ast_boolean      constant;          //!< True iff constant_expression.
};

/*!
@brief Creates a new binary infix expression with the supplied operands.
@todo Set constant bit?
*/
ast_expression * ast_new_binary_expression(ast_expression * left,
                                           ast_expression * right,
                                           ast_operator     operation,
                                           ast_node_attributes * attr);

/*!
@brief Creates a new range expression with the supplied operands.
@todo Set constant bit?
*/
ast_expression * ast_new_range_expression(ast_expression * left,
                                          ast_expression * right);
                                           
/*!
@brief Creates a new range index expression with the supplied operands.
@todo Set constant bit?
*/
ast_expression * ast_new_index_expression(ast_expression * left);


/*!
@brief Creates a new unary expression tree branch.
@todo Set constant bit?
*/
ast_expression * ast_new_unary_expression(ast_expression * operand,
                                          ast_operator     operation,
                                          ast_node_attributes * attr);

/*!
@todo Implement.
*/
ast_expression * ast_new_conditional_expression();

/*!
@todo Implement.
*/
ast_expression * ast_new_modpath_expression();

/*!
@todo Implement.
*/
ast_expression * ast_new_modpath_conditional_expression();

/*!
@todo Implement.
*/
ast_expression * ast_new_mintypmax_expression();

/*!
@todo Implement.
*/
ast_expression * ast_new_modpath_mintypmax_expression();

// -------------------------------- Function Calls ---------------------------


// -------------------------------- Concatenations ---------------------------


// -------------------------------- Specify Blocks ---------------------------


// -------------------------------- Loop Statements --------------------------


// -------------------------------- Case Statements --------------------------


// -------------------------------- IF Else Statements -----------------------


// -------------------------------- Timing Control Statements ----------------


// -------------------------------- Fork Join Sequential ---------------------


// -------------------------------- Procedural Blocks and Assignments --------


// -------------------------------- UDP Blocks -------------------------------


// -------------------------------- Generate Statements ----------------------


// -------------------------------- Module Instantiation ---------------------


// -------------------------------- Primitives -------------------------------


// -------------------------------- Task Declaration -------------------------


// -------------------------------- Function Declaration ---------------------


// -------------------------------- Declaration Lists ------------------------


// -------------------------------- Delays -----------------------------------


// -------------------------------- Strengths --------------------------------


// -------------------------------- Nets and Variables -----------------------


// -------------------------------- Type Declarations ------------------------


// -------------------------------- Port Declarations ------------------------


// -------------------------------- Module Parameters ------------------------


// -------------------------------- Module Items -----------------------------


// -------------------------------- Module Declarations ----------------------


// -------------------------------- Configuration Source ---------------------


// -------------------------------- Library Source Text ----------------------


// -------------------------------- Compiler Directives ----------------------


// -------------------------------- Grammar Top Level ------------------------

/*!
@brief Stores the various data values that a node in the AST can represent.
*/
typedef union ast_value_u 
{
    int     integer;
    float   real;
    char  * string;
    ast_node_attributes * attributes;
}ast_value;

/*!
@brief Enum type describing the data value that an AST node holds.
*/
typedef enum ast_node_type_e
{
    ATTRIBUTE_LIST, //!< A design attribute. @ref ast_node_attributes_t
    EXPRESSION,     //!< A constant or variable expression.
    IDENTIFIER,     //<! An identifier.
    MODULE,         //<! A design module.
    NONE,           //!< The node has no stored data type.
} ast_node_type;

/*!
@brief Node type that forms the tree.
*/
struct ast_node_t
{
    ast_node     * parent;      //!< Parent node in the tree.
    unsigned int   child_count; //!< Number of children the node has.
    ast_node     * children;    //!< Linked list of children.

    ast_value      value;       //!< Data value of the node.
    ast_node_type  type;        //!< Datatype of the value stored in the node.
};

/*!
@brief Creates a new empty ast_node and returns it.
*/
ast_node * ast_node_new();
    
/*!
@brief Creates and returns a new node for the tree which contains a
       single simple identifier.
*/
ast_node * ast_new_identifier_node(char * identifier);



#endif

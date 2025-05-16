//parser.h

#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "lexer.h"

// Forward declaration of Node classes
class ASTNode;
// Add this line to the forward declarations
class ElifNode;
class ExpressionNode;
class AssignStmtNode;
class ComparisonExprNode; // Add this line

//AST Node Types
enum class NodeType {
    // Program structure
    PROGRAM,
    STATEMENT_LIST,
    STATEMENT,

    // Statement types
    ASSIGNMENT_STMT,
    IF_STMT,
    ELIF_CLAUSE,
    ELSE_CLAUSE,
    WHILE_STMT,
    FOR_STMT,
    FUNC_DEF,
    RETURN_STMT,
    IMPORT_STMT,
    ELSE_PART,

    // Expressions
    BINARY_EXPR,
    UNARY_EXPR,
    CALL_EXPR,
    SUBSCRIPT_EXPR,
    ATTR_REF,
    EXPRESSION,
    ASSIGNMENT_WRAPPER,
    COMPARISON_WRAPPER,

    // Primary values
    IDENTIFIER,
    LITERAL,
    LIST_LITERAL,
    DICT_LITERAL,

    // Other components
    PARAM_LIST,
    ARG_LIST,
    BLOCK,
    CONDITION_NODE,
    PARAMETER_NODE,
    TERMINAL, // Add this for terminal symbols like (, ), {, }, [, ]


    // Error recovery
    ERROR_NODE,
};

// Base AST Node class
class ASTNode {
public:
    NodeType type;
    int line_number;
    int column_number;

    ASTNode(NodeType type, int line = 0, int col = 0)
        : type(type), line_number(line), column_number(col) {}

    virtual ~ASTNode() = default;
    virtual std::string toString(int indent = 0) const = 0;
};

// Program node (root of AST)
class ProgramNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> statements;

    ProgramNode() : ASTNode(NodeType::PROGRAM) {}
    std::string toString(int indent = 0) const override;
};

// Statement list node
class StatementListNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> statements;

    StatementListNode() : ASTNode(NodeType::STATEMENT_LIST) {}
    std::string toString(int indent = 0) const override;
};

// Block of statements (indented code block)
class BlockNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> statements;

    BlockNode() : ASTNode(NodeType::BLOCK) {}
    std::string toString(int indent = 0) const override;
};

// Assignment statement
class AssignmentNode : public ASTNode {
public:
    std::string op;                        // <- store the operator lexeme
    std::shared_ptr<ASTNode> target;
    std::shared_ptr<ASTNode> value;

    // Updated constructor:
    AssignmentNode(std::shared_ptr<ASTNode> t,
                   std::shared_ptr<ASTNode> v,
                   const std::string &op_,
                   int line,
                   int col)
        : ASTNode(NodeType::ASSIGNMENT_STMT, line, col)
        , op(op_)
        , target(std::move(t))
        , value(std::move(v))
    {}

    std::string toString(int indent = 0) const override;
};

// If statement
class IfNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> if_block;
    // std::shared_ptr<ASTNode> elif_clauses;
    std::vector<std::shared_ptr<ElifNode>> elif_clauses;
    std::shared_ptr<ASTNode> else_block;
    bool hasParentheses; // New field to track parentheses

    IfNode(std::shared_ptr<ASTNode> cond, std::shared_ptr<ASTNode> block,
            int line, int col, bool parentheses = false)
         : ASTNode(NodeType::IF_STMT, line, col), condition(cond), if_block(block),
           else_block(nullptr), hasParentheses(parentheses) {}

    // IfNode(std::shared_ptr<ASTNode> cond, std::shared_ptr<ASTNode> block, int line, int col)
    //     : ASTNode(NodeType::IF_STMT, line, col), condition(cond), if_block(block), else_block(nullptr) {}
    // IfNode(std::shared_ptr<ASTNode> cond, std::shared_ptr<ASTNode> block, int line, int col)
    //     : ASTNode(NodeType::IF_STMT, line, col), condition(cond), if_block(block) {}
    std::string toString(int indent = 0) const override;
};

// Elif clause
// class ElifNode : public ASTNode {
// public:
//     std::shared_ptr<ASTNode> condition;
//     std::shared_ptr<ASTNode> block;
//
//     ElifNode(std::shared_ptr<ASTNode> cond, std::shared_ptr<ASTNode> b, int line, int col)
//         : ASTNode(NodeType::ELIF_CLAUSE, line, col), condition(cond), block(b) {}
//     std::string toString(int indent = 0) const override;
// };
// Add to the ElifNode class in parser.h
class ElifNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> block;
    bool hasParentheses; // Add this field

    ElifNode(std::shared_ptr<ASTNode> cond, std::shared_ptr<ASTNode> b, int line, int col, bool parentheses = false)
        : ASTNode(NodeType::ELIF_CLAUSE, line, col),
          condition(cond), block(b), hasParentheses(parentheses) {}
    std::string toString(int indent = 0) const override;
};
// Else clause
class ElseNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> block;

    ElseNode(std::shared_ptr<ASTNode> b, int line, int col)
        : ASTNode(NodeType::ELSE_CLAUSE, line, col), block(b) {}
    std::string toString(int indent = 0) const override;
};
class ElsePartNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ElifNode>> elif_clauses;
    std::shared_ptr<ElseNode> else_block;

    ElsePartNode(const std::vector<std::shared_ptr<ElifNode>>& elifs,
                std::shared_ptr<ElseNode> else_blk,
                int line = 0, int col = 0)
        : ASTNode(NodeType::ELSE_PART, line, col),
          elif_clauses(elifs), else_block(else_blk) {}

    std::string toString(int indent = 0) const override;
};

// While statement
class WhileNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> block;
    bool hasParentheses;

    // WhileNode(std::shared_ptr<ASTNode> cond, std::shared_ptr<ASTNode> b, int line, int col)
    //     : ASTNode(NodeType::WHILE_STMT, line, col), condition(cond), block(b) {}
    WhileNode(std::shared_ptr<ASTNode> condition, std::shared_ptr<BlockNode> block,
              int line_number, int column_number, bool hasParentheses = false)
        : ASTNode(NodeType::WHILE_STMT, line_number, column_number),
          condition(condition), block(block), hasParentheses(hasParentheses) {}

    std::string toString(int indent = 0) const override;
};

// For statement
class ForNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> target;
    std::shared_ptr<ASTNode> iterable;
    std::shared_ptr<ASTNode> block;
    bool hasParentheses;  // Add this field

    ForNode(std::shared_ptr<ASTNode> target, std::shared_ptr<ASTNode> iterable,
            std::shared_ptr<BlockNode> block, int line_number, int column_number,
            bool hasParentheses = false)
        : ASTNode(NodeType::FOR_STMT, line_number, column_number),
          target(target), iterable(iterable), block(block), hasParentheses(hasParentheses) {}


    // ForNode(std::shared_ptr<ASTNode> t, std::shared_ptr<ASTNode> iter, std::shared_ptr<ASTNode> b, int line, int col)
    //     : ASTNode(NodeType::FOR_STMT, line, col), target(t), iterable(iter), block(b) {}
    std::string toString(int indent = 0) const override;
};

// Function definition
// class FunctionDefNode : public ASTNode {
// public:
//     std::string name;
//     std::shared_ptr<ASTNode> params;
//     std::shared_ptr<ASTNode> body;
//
//     // Add these fields for the parentheses
//     std::shared_ptr<ASTNode> openParen;
//     std::shared_ptr<ASTNode> closeParen;
//     std::shared_ptr<ASTNode> colon;  // Also add the colon
//
//     FunctionDefNode(const std::string& n, std::shared_ptr<ASTNode> p, std::shared_ptr<ASTNode> b,
//                    int line, int col)
//         : ASTNode(NodeType::FUNC_DEF, line, col), name(n), params(p), body(b),
//           openParen(nullptr), closeParen(nullptr), colon(nullptr) {}
//
//     std::string toString(int indent = 0) const override;
// };

// Function definition
class FunctionDefNode : public ASTNode {
public:
    std::string name;
    std::shared_ptr<ASTNode> params;
    std::shared_ptr<ASTNode> body;

    // Terminal nodes for all parts of the function declaration syntax
    std::shared_ptr<ASTNode> defKeyword;    // 'def' keyword as a terminal node
    std::shared_ptr<ASTNode> nameNode;      // Function name as a terminal node
    std::shared_ptr<ASTNode> openParen;     // '(' after function name
    std::shared_ptr<ASTNode> closeParen;    // ')' after parameters
    std::shared_ptr<ASTNode> colon;         // ':' after parameters

    FunctionDefNode(const std::string& n, std::shared_ptr<ASTNode> p, std::shared_ptr<ASTNode> b,
                   int line, int col)
        : ASTNode(NodeType::FUNC_DEF, line, col), name(n), params(p), body(b),
          defKeyword(nullptr), nameNode(nullptr), openParen(nullptr),
          closeParen(nullptr), colon(nullptr) {}

    std::string toString(int indent = 0) const override;
};

// Return statement
class ReturnNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> expression;

    ReturnNode(std::shared_ptr<ASTNode> expr, int line, int col)
        : ASTNode(NodeType::RETURN_STMT, line, col), expression(expr) {}
    std::string toString(int indent = 0) const override;
};

// Import statement
class ImportNode : public ASTNode {
public:
    std::string module;
    std::string alias; // For "import x as y"

    ImportNode(const std::string& m, const std::string& a, int line, int col)
        : ASTNode(NodeType::IMPORT_STMT, line, col), module(m), alias(a) {}
    std::string toString(int indent = 0) const override;
};

// Binary expression
class BinaryExprNode : public ASTNode {
public:
    std::string op;
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;

    BinaryExprNode(const std::string& o, std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r, int line, int col)
        : ASTNode(NodeType::BINARY_EXPR, line, col), op(o), left(l), right(r) {}
    std::string toString(int indent = 0) const override;
};

// Unary expression
class UnaryExprNode : public ASTNode {
public:
    std::string op;
    std::shared_ptr<ASTNode> operand;

    UnaryExprNode(const std::string& o, std::shared_ptr<ASTNode> opnd, int line, int col)
        : ASTNode(NodeType::UNARY_EXPR, line, col), op(o), operand(opnd) {}
    std::string toString(int indent = 0) const override;
};


// Function call
class CallExprNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> function;
    std::shared_ptr<ASTNode> arguments;

    // Add these for parentheses visualization
    std::shared_ptr<ASTNode> openParen;
    std::shared_ptr<ASTNode> closeParen;

    CallExprNode(std::shared_ptr<ASTNode> func, std::shared_ptr<ASTNode> args, int line, int col)
        : ASTNode(NodeType::CALL_EXPR, line, col), function(func), arguments(args),
          openParen(nullptr), closeParen(nullptr) {}

    std::string toString(int indent = 0) const override;
};

// Subscript expression (list/dict indexing)
class SubscriptExprNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> container;
    std::shared_ptr<ASTNode> index;

    SubscriptExprNode(std::shared_ptr<ASTNode> c, std::shared_ptr<ASTNode> i, int line, int col)
        : ASTNode(NodeType::SUBSCRIPT_EXPR, line, col), container(c), index(i) {}
    std::string toString(int indent = 0) const override;
};

// Attribute reference (obj.attr)
class AttrRefNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> object;
    std::string attribute;

    AttrRefNode(std::shared_ptr<ASTNode> obj, const std::string& attr, int line, int col)
        : ASTNode(NodeType::ATTR_REF, line, col), object(obj), attribute(attr) {}
    std::string toString(int indent = 0) const override;
};

// Identifier
class IdentifierNode : public ASTNode {
public:
    std::string name;

    IdentifierNode(const std::string& n, int line, int col)
        : ASTNode(NodeType::IDENTIFIER, line, col), name(n) {}
    std::string toString(int indent = 0) const override;
};

// Literal (number, string, etc.)
class LiteralNode : public ASTNode {
public:
    std::string value;
    std::string type; // e.g. "int", "float", "string", "bool"

    LiteralNode(const std::string& v, const std::string& t, int line, int col)
        : ASTNode(NodeType::LITERAL, line, col), value(v), type(t) {}
    std::string toString(int indent = 0) const override;
};

// List literal
class ListNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> elements;

    ListNode(int line, int col) : ASTNode(NodeType::LIST_LITERAL, line, col) {}
    std::string toString(int indent = 0) const override;
};

// Dict literal
class DictNode : public ASTNode {
public:
    std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>>> items;

    DictNode(int line, int col) : ASTNode(NodeType::DICT_LITERAL, line, col) {}
    std::string toString(int indent = 0) const override;
};

// Parameter node for function parameters
class ParameterNode : public ASTNode {
public:
    std::string name;
    std::shared_ptr<ASTNode> default_value; // nullptr means no default value

    ParameterNode(const std::string& name, std::shared_ptr<ASTNode> default_value = nullptr,
                 int line = 0, int col = 0)
        : ASTNode(NodeType::PARAMETER_NODE, line, col),
          name(name), default_value(default_value) {}

    std::string toString(int indent = 0) const override;
};


// Parameter list for function definitions
class ParamListNode : public ASTNode {
public:
    // Change from vector<shared_ptr<IdentifierNode>> to vector<shared_ptr<ParameterNode>>
    std::vector<std::shared_ptr<ParameterNode>> parameters;

    ParamListNode() : ASTNode(NodeType::PARAM_LIST) {}
    std::string toString(int indent = 0) const override;
};

// Argument list for function calls
class ArgListNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> arguments;

    ArgListNode() : ASTNode(NodeType::ARG_LIST) {}
    std::string toString(int indent = 0) const override;
};

// New Condition Node class to represent the condition container
class ConditionNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> condition;

    ConditionNode(std::shared_ptr<ASTNode> cond, int line, int col)
        : ASTNode(NodeType::CONDITION_NODE, line, col), condition(cond) {}

    std::string toString(int indent = 0) const override;
};

// Error node for error recovery
class ErrorNode : public ASTNode {
public:
    std::string message;

    ErrorNode(const std::string& msg, int line, int col)
        : ASTNode(NodeType::ERROR_NODE, line, col), message(msg) {}
    std::string toString(int indent = 0) const override;
};

// Terminal node for symbols like '(', ')', '{', '}', '[', ']'
class TerminalNode : public ASTNode {
public:
    std::string value; // The value of the terminal (e.g., "(", "{", etc.)

    TerminalNode(const std::string& value, int line = 0, int col = 0)
        : ASTNode(NodeType::TERMINAL, line, col), value(value) {}

    std::string toString(int indent = 0) const override; // Declare the toString function
};

class StatementNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> statement;

    StatementNode(std::shared_ptr<ASTNode> stmt)
        : ASTNode(NodeType::STATEMENT), statement(stmt) {}

    std::string toString(int indent = 0) const override;
};


class ExpressionNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> expression;

    ExpressionNode(std::shared_ptr<ASTNode> expr)
        : ASTNode(NodeType::EXPRESSION, expr->line_number, expr->column_number),
          expression(expr) {}

    std::string toString(int indent = 0) const override;
};

class AssignStmtNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> assignment;

    AssignStmtNode(std::shared_ptr<ASTNode> assign)
        : ASTNode(NodeType::ASSIGNMENT_WRAPPER, assign->line_number, assign->column_number),
          assignment(assign) {}

    std::string toString(int indent = 0) const override;
};

class ComparisonExprNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> comparison;

    ComparisonExprNode(std::shared_ptr<ASTNode> comp)
        : ASTNode(NodeType::COMPARISON_WRAPPER, comp->line_number, comp->column_number),
          comparison(comp) {}

    // Fix: Replace with direct string creation like other inline toString methods
    std::string toString(int indent = 0) const override;
};

// The Parser class
class Parser {
private:
    Lexer& lexer;
    std::vector<Token>::iterator current_token;
    std::vector<std::string> errors;
    bool has_error;

    // Helper methods
    Token& peek();
    Token& consume();
    bool match(TokenType type);
    bool match(const std::string& lexeme);
    bool check(TokenType type);
    bool check(const std::string& lexeme);
    bool isAtEnd();

    bool isAssignmentOperator(const std::string &lexeme);

    bool isBuiltInFunction(const std::string& name); // Add this line
    void synchronize();
    void error(const std::string& message, int line, int column);

    // Recursive descent parsing methods
    // ...rest of the code
    // Recursive descent parsing methods
    std::shared_ptr<ProgramNode> parseProgram();
    std::shared_ptr<StatementListNode> parseStatementList();
    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<AssignmentNode> parseAssignment(std::shared_ptr<ASTNode> target);
    std::shared_ptr<IfNode> parseIfStatement();
    std::shared_ptr<ElifNode> parseElifClause();
    std::shared_ptr<ElseNode> parseElseClause();
    std::shared_ptr<WhileNode> parseWhileStatement();
    std::shared_ptr<ForNode> parseForStatement();
    std::shared_ptr<FunctionDefNode> parseFunctionDef();
    std::shared_ptr<ReturnNode> parseReturnStatement();
    std::shared_ptr<ImportNode> parseImportStatement();
    std::shared_ptr<BlockNode> parseBlock();

    std::shared_ptr<ASTNode> parseExpression();
    std::shared_ptr<ASTNode> parseOrExpr();
    std::shared_ptr<ASTNode> parseAndExpr();
    std::shared_ptr<ASTNode> parseNotExpr();
    std::shared_ptr<ASTNode> parseComparisonExpr();
    std::shared_ptr<ASTNode> parseArithmeticExpr();
    std::shared_ptr<ASTNode> parseTerm();
    std::shared_ptr<ASTNode> parseFactor();
    std::shared_ptr<ASTNode> parsePower();
    std::shared_ptr<ASTNode> parseUnary();

    void debugToken(const std::string &context);


    std::string tokenTypeToString(TokenType type);

    std::shared_ptr<ASTNode> parsePrimary();
    std::shared_ptr<ASTNode> parseAttributeReference(std::shared_ptr<ASTNode> object);
    std::shared_ptr<ASTNode> parseSubscript(std::shared_ptr<ASTNode> container);
    std::shared_ptr<ASTNode> parseCall(std::shared_ptr<ASTNode> function);
    std::shared_ptr<ArgListNode> parseArguments();
    std::shared_ptr<ParamListNode> parseParameters();
    std::shared_ptr<ListNode> parseListLiteral();
    std::shared_ptr<DictNode> parseDictLiteral();

public:
    Parser(Lexer& lexer);
    std::shared_ptr<ProgramNode> parse();
    void printParseTree(std::shared_ptr<ASTNode> root, int indent = 0);
    void printErrors();
    bool hasError() const { return !errors.empty(); }
    const std::vector<std::string>& getErrors() const { return errors; }
};

#endif // PARSER_H

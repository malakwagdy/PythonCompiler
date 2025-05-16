//parser.h

#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "lexer.h"
using namespace std;

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
    GROUP_EXPR,
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
    virtual string toString(int indent = 0) const = 0;
};

// Program node (root of AST)
class ProgramNode : public ASTNode {
public:
    vector<shared_ptr<ASTNode>> statements;

    ProgramNode() : ASTNode(NodeType::PROGRAM) {}
    string toString(int indent = 0) const override;
};

// Statement list node
class StatementListNode : public ASTNode {
public:
    vector<shared_ptr<ASTNode>> statements;

    StatementListNode() : ASTNode(NodeType::STATEMENT_LIST) {}
    string toString(int indent = 0) const override;
};

// Block of statements (indented code block)
class BlockNode : public ASTNode {
public:
    shared_ptr<ASTNode> statements;

    BlockNode() : ASTNode(NodeType::BLOCK) {}
    string toString(int indent = 0) const override;
};

// Assignment statement
class AssignmentNode : public ASTNode {
public:
    string op;                        // <- store the operator lexeme
    shared_ptr<ASTNode> target;
    shared_ptr<ASTNode> value;

    // Updated constructor:
    AssignmentNode(shared_ptr<ASTNode> t,
                   shared_ptr<ASTNode> v,
                   const string &op_,
                   int line,
                   int col)
        : ASTNode(NodeType::ASSIGNMENT_STMT, line, col)
        , op(op_)
        , target(move(t))
        , value(move(v))
    {}

    string toString(int indent = 0) const override;
};

// If statement
class IfNode : public ASTNode {
public:
    shared_ptr<ASTNode> condition;
    shared_ptr<ASTNode> if_block;
    // shared_ptr<ASTNode> elif_clauses;
    vector<shared_ptr<ElifNode>> elif_clauses;
    shared_ptr<ASTNode> else_block;
    bool hasParentheses; // New field to track parentheses

    IfNode(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> block,
            int line, int col, bool parentheses = false)
         : ASTNode(NodeType::IF_STMT, line, col), condition(cond), if_block(block),
           else_block(nullptr), hasParentheses(parentheses) {}

    string toString(int indent = 0) const override;
};


class ElifNode : public ASTNode {
public:
    shared_ptr<ASTNode> condition;
    shared_ptr<ASTNode> block;
    bool hasParentheses; // Add this field

    ElifNode(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> b, int line, int col, bool parentheses = false)
        : ASTNode(NodeType::ELIF_CLAUSE, line, col),
          condition(cond), block(b), hasParentheses(parentheses) {}
    string toString(int indent = 0) const override;
};
// Else clause
class ElseNode : public ASTNode {
public:
    shared_ptr<ASTNode> block;

    ElseNode(shared_ptr<ASTNode> b, int line, int col)
        : ASTNode(NodeType::ELSE_CLAUSE, line, col), block(b) {}
    string toString(int indent = 0) const override;
};
class ElsePartNode : public ASTNode {
public:
    vector<shared_ptr<ElifNode>> elif_clauses;
    shared_ptr<ElseNode> else_block;

    ElsePartNode(const vector<shared_ptr<ElifNode>>& elifs,
                shared_ptr<ElseNode> else_blk,
                int line = 0, int col = 0)
        : ASTNode(NodeType::ELSE_PART, line, col),
          elif_clauses(elifs), else_block(else_blk) {}

    string toString(int indent = 0) const override;
};

// While statement
class WhileNode : public ASTNode {
public:
    shared_ptr<ASTNode> condition;
    shared_ptr<ASTNode> block;
    bool hasParentheses;

    WhileNode(shared_ptr<ASTNode> condition, shared_ptr<BlockNode> block,
              int line_number, int column_number, bool hasParentheses = false)
        : ASTNode(NodeType::WHILE_STMT, line_number, column_number),
          condition(condition), block(block), hasParentheses(hasParentheses) {}

    string toString(int indent = 0) const override;
};

// For statement
class ForNode : public ASTNode {
public:
    shared_ptr<ASTNode> target;
    shared_ptr<ASTNode> iterable;
    shared_ptr<ASTNode> block;
    bool hasParentheses;  // Add this field

    ForNode(shared_ptr<ASTNode> target, shared_ptr<ASTNode> iterable,
            shared_ptr<BlockNode> block, int line_number, int column_number,
            bool hasParentheses = false)
        : ASTNode(NodeType::FOR_STMT, line_number, column_number),
          target(target), iterable(iterable), block(block), hasParentheses(hasParentheses) {}


    string toString(int indent = 0) const override;
};

// Function definition
class FunctionDefNode : public ASTNode {
public:
    string name;
    shared_ptr<ASTNode> params;
    shared_ptr<ASTNode> body;

    // Terminal nodes for all parts of the function declaration syntax
    shared_ptr<ASTNode> defKeyword;    // 'def' keyword as a terminal node
    shared_ptr<ASTNode> nameNode;      // Function name as a terminal node
    shared_ptr<ASTNode> openParen;     // '(' after function name
    shared_ptr<ASTNode> closeParen;    // ')' after parameters
    shared_ptr<ASTNode> colon;         // ':' after parameters

    FunctionDefNode(const string& n, shared_ptr<ASTNode> p, shared_ptr<ASTNode> b,
                   int line, int col)
        : ASTNode(NodeType::FUNC_DEF, line, col), name(n), params(p), body(b),
          defKeyword(nullptr), nameNode(nullptr), openParen(nullptr),
          closeParen(nullptr), colon(nullptr) {}

    string toString(int indent = 0) const override;
};

// Return statement
class ReturnNode : public ASTNode {
public:
    shared_ptr<ASTNode> expression;

    ReturnNode(shared_ptr<ASTNode> expr, int line, int col)
        : ASTNode(NodeType::RETURN_STMT, line, col), expression(expr) {}
    string toString(int indent = 0) const override;
};

// Import statement
class ImportNode : public ASTNode {
public:
    string module;
    string alias; // For "import x as y"

    ImportNode(const string& m, const string& a, int line, int col)
        : ASTNode(NodeType::IMPORT_STMT, line, col), module(m), alias(a) {}
    string toString(int indent = 0) const override;
};

// Binary expression
class BinaryExprNode : public ASTNode {
public:
    string op;
    shared_ptr<ASTNode> left;
    shared_ptr<ASTNode> right;

    BinaryExprNode(const string& o, shared_ptr<ASTNode> l, shared_ptr<ASTNode> r, int line, int col)
        : ASTNode(NodeType::BINARY_EXPR, line, col), op(o), left(l), right(r) {}
    string toString(int indent = 0) const override;
};

// Unary expression
class UnaryExprNode : public ASTNode {
public:
    string op;
    shared_ptr<ASTNode> operand;

    UnaryExprNode(const string& o, shared_ptr<ASTNode> opnd, int line, int col)
        : ASTNode(NodeType::UNARY_EXPR, line, col), op(o), operand(opnd) {}
    string toString(int indent = 0) const override;
};


// Function call
class CallExprNode : public ASTNode {
public:
    shared_ptr<ASTNode> function;
    shared_ptr<ASTNode> arguments;

    // Add these for parentheses visualization
    shared_ptr<ASTNode> openParen;
    shared_ptr<ASTNode> closeParen;

    CallExprNode(shared_ptr<ASTNode> func, shared_ptr<ASTNode> args, int line, int col)
        : ASTNode(NodeType::CALL_EXPR, line, col), function(func), arguments(args),
          openParen(nullptr), closeParen(nullptr) {}

    string toString(int indent = 0) const override;
};

// Subscript expression (list/dict indexing)
class SubscriptExprNode : public ASTNode {
public:
    shared_ptr<ASTNode> container;
    shared_ptr<ASTNode> index;

    SubscriptExprNode(shared_ptr<ASTNode> c, shared_ptr<ASTNode> i, int line, int col)
        : ASTNode(NodeType::SUBSCRIPT_EXPR, line, col), container(c), index(i) {}
    string toString(int indent = 0) const override;
};

// Attribute reference (obj.attr)
class AttrRefNode : public ASTNode {
public:
    shared_ptr<ASTNode> object;
    string attribute;

    AttrRefNode(shared_ptr<ASTNode> obj, const string& attr, int line, int col)
        : ASTNode(NodeType::ATTR_REF, line, col), object(obj), attribute(attr) {}
    string toString(int indent = 0) const override;
};

// Identifier
class IdentifierNode : public ASTNode {
public:
    string name;

    IdentifierNode(const string& n, int line, int col)
        : ASTNode(NodeType::IDENTIFIER, line, col), name(n) {}
    string toString(int indent = 0) const override;
};

// Literal (number, string, etc.)
class LiteralNode : public ASTNode {
public:
    string value;
    string type; // e.g. "int", "float", "string", "bool"

    LiteralNode(const string& v, const string& t, int line, int col)
        : ASTNode(NodeType::LITERAL, line, col), value(v), type(t) {}
    string toString(int indent = 0) const override;
};

// List literal
class ListNode : public ASTNode {
public:
    vector<shared_ptr<ASTNode>> elements;

    ListNode(int line, int col) : ASTNode(NodeType::LIST_LITERAL, line, col) {}
    string toString(int indent = 0) const override;
};

// Dict literal
class DictNode : public ASTNode {
public:
    vector<pair<shared_ptr<ASTNode>, shared_ptr<ASTNode>>> items;

    DictNode(int line, int col) : ASTNode(NodeType::DICT_LITERAL, line, col) {}
    string toString(int indent = 0) const override;
};

// Parameter node for function parameters
class ParameterNode : public ASTNode {
public:
    string name;
    shared_ptr<ASTNode> default_value; // nullptr means no default value

    ParameterNode(const string& name, shared_ptr<ASTNode> default_value = nullptr,
                 int line = 0, int col = 0)
        : ASTNode(NodeType::PARAMETER_NODE, line, col),
          name(name), default_value(default_value) {}

    string toString(int indent = 0) const override;
};


// Parameter list for function definitions
class ParamListNode : public ASTNode {
public:
    // Change from vector<shared_ptr<IdentifierNode>> to vector<shared_ptr<ParameterNode>>
    vector<shared_ptr<ParameterNode>> parameters;

    ParamListNode() : ASTNode(NodeType::PARAM_LIST) {}
    string toString(int indent = 0) const override;
};

// Argument list for function calls
class ArgListNode : public ASTNode {
public:
    vector<shared_ptr<ASTNode>> arguments;

    ArgListNode() : ASTNode(NodeType::ARG_LIST) {}
    string toString(int indent = 0) const override;
};

// New Condition Node class to represent the condition container
class ConditionNode : public ASTNode {
public:
    shared_ptr<ASTNode> condition;

    ConditionNode(shared_ptr<ASTNode> cond, int line, int col)
        : ASTNode(NodeType::CONDITION_NODE, line, col), condition(cond) {}

    string toString(int indent = 0) const override;
};

// Error node for error recovery
class ErrorNode : public ASTNode {
public:
    string message;

    ErrorNode(const string& msg, int line, int col)
        : ASTNode(NodeType::ERROR_NODE, line, col), message(msg) {}
    string toString(int indent = 0) const override;
};

// Terminal node for symbols like '(', ')', '{', '}', '[', ']'
class TerminalNode : public ASTNode {
public:
    string value; // The value of the terminal (e.g., "(", "{", etc.)

    TerminalNode(const string& value, int line = 0, int col = 0)
        : ASTNode(NodeType::TERMINAL, line, col), value(value) {}

    string toString(int indent = 0) const override; // Declare the toString function
};

class StatementNode : public ASTNode {
public:
    shared_ptr<ASTNode> statement;

    StatementNode(shared_ptr<ASTNode> stmt)
        : ASTNode(NodeType::STATEMENT), statement(stmt) {}

    string toString(int indent = 0) const override;
};


class ExpressionNode : public ASTNode {
public:
    shared_ptr<ASTNode> expression;

    ExpressionNode(shared_ptr<ASTNode> expr)
        : ASTNode(NodeType::EXPRESSION, expr->line_number, expr->column_number),
          expression(expr) {}

    string toString(int indent = 0) const override;
};

class AssignStmtNode : public ASTNode {
public:
    shared_ptr<ASTNode> assignment;

    AssignStmtNode(shared_ptr<ASTNode> assign)
        : ASTNode(NodeType::ASSIGNMENT_WRAPPER, assign->line_number, assign->column_number),
          assignment(assign) {}

    string toString(int indent = 0) const override;
};

class ComparisonExprNode : public ASTNode {
public:
    shared_ptr<ASTNode> comparison;

    ComparisonExprNode(shared_ptr<ASTNode> comp)
        : ASTNode(NodeType::COMPARISON_WRAPPER, comp->line_number, comp->column_number),
          comparison(comp) {}

    // Fix: Replace with direct string creation like other inline toString methods
    string toString(int indent = 0) const override;
};
// Group expression node for parenthesized expressions (e.g., (a + b))
class GroupExprNode : public ASTNode {
public:
    shared_ptr<ASTNode> expression;

    GroupExprNode(shared_ptr<ASTNode> expr, int line, int col)
        : ASTNode(NodeType::GROUP_EXPR, line, col), expression(expr) {}

    string toString(int indent = 0) const override;
};

// The Parser class
class Parser {
private:
    Lexer& lexer;
    vector<Token>::iterator current_token;
    vector<string> errors;
    bool has_error;

    // Helper methods
    Token& peek();
    Token& consume();
    bool match(TokenType type);
    bool match(const string& lexeme);
    bool check(TokenType type);
    bool check(const string& lexeme);
    bool isAtEnd();

    bool isAssignmentOperator(const string &lexeme);

    bool isBuiltInFunction(const string& name); // Add this line
    void synchronize();
    void error(const string& message, int line, int column);

    // Recursive descent parsing methods
    // ...rest of the code
    // Recursive descent parsing methods
    shared_ptr<ProgramNode> parseProgram();
    shared_ptr<StatementListNode> parseStatementList();
    shared_ptr<ASTNode> parseStatement();
    shared_ptr<AssignmentNode> parseAssignment(shared_ptr<ASTNode> target);
    shared_ptr<IfNode> parseIfStatement();
    shared_ptr<ElifNode> parseElifClause();
    shared_ptr<ElseNode> parseElseClause();
    shared_ptr<WhileNode> parseWhileStatement();
    shared_ptr<ForNode> parseForStatement();
    shared_ptr<FunctionDefNode> parseFunctionDef();
    shared_ptr<ReturnNode> parseReturnStatement();
    shared_ptr<ImportNode> parseImportStatement();
    shared_ptr<BlockNode> parseBlock();

    shared_ptr<ASTNode> parseExpression();
    shared_ptr<ASTNode> parseOrExpr();
    shared_ptr<ASTNode> parseAndExpr();
    shared_ptr<ASTNode> parseNotExpr();
    shared_ptr<ASTNode> parseComparisonExpr();
    shared_ptr<ASTNode> parseArithmeticExpr();
    shared_ptr<ASTNode> parseTerm();
    shared_ptr<ASTNode> parseFactor();
    shared_ptr<ASTNode> parsePower();
    shared_ptr<ASTNode> parseUnary();

    void debugToken(const string &context);


    string tokenTypeToString(TokenType type);

    shared_ptr<ASTNode> parsePrimary();
    shared_ptr<ASTNode> parseAttributeReference(shared_ptr<ASTNode> object);
    shared_ptr<ASTNode> parseSubscript(shared_ptr<ASTNode> container);
    shared_ptr<ASTNode> parseCall(shared_ptr<ASTNode> function);
    shared_ptr<ArgListNode> parseArguments();
    shared_ptr<ParamListNode> parseParameters();
    shared_ptr<ListNode> parseListLiteral();
    shared_ptr<DictNode> parseDictLiteral();

public:
    Parser(Lexer& lexer);
    shared_ptr<ProgramNode> parse();
    void printParseTree(shared_ptr<ASTNode> root, int indent = 0);
    void printErrors();
    bool hasError() const { return !errors.empty(); }
    const vector<string>& getErrors() const { return errors; }
};

#endif // PARSER_H

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

// AST Node Types
enum class NodeType {
    PROGRAM,
    STATEMENT_LIST,
    ASSIGNMENT_STMT,
    IF_STMT,
    ELIF_CLAUSE,
    ELSE_CLAUSE,
    WHILE_STMT,
    FOR_STMT,
    FUNC_DEF,
    RETURN_STMT,
    IMPORT_STMT,
    BINARY_EXPR,
    UNARY_EXPR,
    CALL_EXPR,
    SUBSCRIPT_EXPR,
    ATTR_REF,
    IDENTIFIER,
    LITERAL,
    LIST_LITERAL,
    DICT_LITERAL,
    PARAM_LIST,
    ARG_LIST,
    BLOCK,
    CONDITION_NODE,
    PARAMETER_NODE,
    ERROR_NODE
};

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

class ProgramNode : public ASTNode {
public:
    vector<shared_ptr<ASTNode>> statements;

    ProgramNode() : ASTNode(NodeType::PROGRAM) {}
    string toString(int indent = 0) const override;
};

class StatementListNode : public ASTNode {
public:
    vector<shared_ptr<ASTNode>> statements;

    StatementListNode() : ASTNode(NodeType::STATEMENT_LIST) {}
    string toString(int indent = 0) const override;
};

class BlockNode : public ASTNode {
public:
    shared_ptr<ASTNode> statements;

    BlockNode() : ASTNode(NodeType::BLOCK) {}
    string toString(int indent = 0) const override;
};

class AssignmentNode : public ASTNode {
public:
    string op;
    shared_ptr<ASTNode> target;
    shared_ptr<ASTNode> value;

    AssignmentNode(shared_ptr<ASTNode> t, shared_ptr<ASTNode> v,
                   const string &op_, int line, int col)
        : ASTNode(NodeType::ASSIGNMENT_STMT, line, col), op(op_), target(move(t)), value(move(v)) {}

    string toString(int indent = 0) const override;
};

class IfNode : public ASTNode {
public:
    shared_ptr<ASTNode> condition;
    shared_ptr<ASTNode> if_block;
    vector<shared_ptr<ASTNode>> elif_clauses;
    shared_ptr<ASTNode> else_block;

    IfNode(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> block, int line, int col)
        : ASTNode(NodeType::IF_STMT, line, col), condition(cond), if_block(block) {}
    string toString(int indent = 0) const override;
};

class ElifNode : public ASTNode {
public:
    shared_ptr<ASTNode> condition;
    shared_ptr<ASTNode> block;

    ElifNode(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> b, int line, int col)
        : ASTNode(NodeType::ELIF_CLAUSE, line, col), condition(cond), block(b) {}
    string toString(int indent = 0) const override;
};

class ElseNode : public ASTNode {
public:
    shared_ptr<ASTNode> block;

    ElseNode(shared_ptr<ASTNode> b, int line, int col)
        : ASTNode(NodeType::ELSE_CLAUSE, line, col), block(b) {}
    string toString(int indent = 0) const override;
};

class WhileNode : public ASTNode {
public:
    shared_ptr<ASTNode> condition;
    shared_ptr<ASTNode> block;

    WhileNode(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> b, int line, int col)
        : ASTNode(NodeType::WHILE_STMT, line, col), condition(cond), block(b) {}
    string toString(int indent = 0) const override;
};

class ForNode : public ASTNode {
public:
    shared_ptr<ASTNode> target;
    shared_ptr<ASTNode> iterable;
    shared_ptr<ASTNode> block;

    ForNode(shared_ptr<ASTNode> t, shared_ptr<ASTNode> iter, shared_ptr<ASTNode> b, int line, int col)
        : ASTNode(NodeType::FOR_STMT, line, col), target(t), iterable(iter), block(b) {}
    string toString(int indent = 0) const override;
};

class FunctionDefNode : public ASTNode {
public:
    string name;
    shared_ptr<ASTNode> params;
    shared_ptr<ASTNode> body;

    FunctionDefNode(const string& n, shared_ptr<ASTNode> p, shared_ptr<ASTNode> b, int line, int col)
        : ASTNode(NodeType::FUNC_DEF, line, col), name(n), params(p), body(b) {}
    string toString(int indent = 0) const override;
};

class ReturnNode : public ASTNode {
public:
    shared_ptr<ASTNode> expression;

    ReturnNode(shared_ptr<ASTNode> expr, int line, int col)
        : ASTNode(NodeType::RETURN_STMT, line, col), expression(expr) {}
    string toString(int indent = 0) const override;
};

class ImportNode : public ASTNode {
public:
    string module;
    string alias;

    ImportNode(const string& m, const string& a, int line, int col)
        : ASTNode(NodeType::IMPORT_STMT, line, col), module(m), alias(a) {}
    string toString(int indent = 0) const override;
};

class BinaryExprNode : public ASTNode {
public:
    string op;
    shared_ptr<ASTNode> left;
    shared_ptr<ASTNode> right;

    BinaryExprNode(const string& o, shared_ptr<ASTNode> l, shared_ptr<ASTNode> r, int line, int col)
        : ASTNode(NodeType::BINARY_EXPR, line, col), op(o), left(l), right(r) {}
    string toString(int indent = 0) const override;
};

class UnaryExprNode : public ASTNode {
public:
    string op;
    shared_ptr<ASTNode> operand;

    UnaryExprNode(const string& o, shared_ptr<ASTNode> opnd, int line, int col)
        : ASTNode(NodeType::UNARY_EXPR, line, col), op(o), operand(opnd) {}
    string toString(int indent = 0) const override;
};

class CallExprNode : public ASTNode {
public:
    shared_ptr<ASTNode> function;
    shared_ptr<ASTNode> arguments;

    CallExprNode(shared_ptr<ASTNode> func, shared_ptr<ASTNode> args, int line, int col)
        : ASTNode(NodeType::CALL_EXPR, line, col), function(func), arguments(args) {}
    string toString(int indent = 0) const override;
};

class SubscriptExprNode : public ASTNode {
public:
    shared_ptr<ASTNode> container;
    shared_ptr<ASTNode> index;

    SubscriptExprNode(shared_ptr<ASTNode> c, shared_ptr<ASTNode> i, int line, int col)
        : ASTNode(NodeType::SUBSCRIPT_EXPR, line, col), container(c), index(i) {}
    string toString(int indent = 0) const override;
};

class AttrRefNode : public ASTNode {
public:
    shared_ptr<ASTNode> object;
    string attribute;

    AttrRefNode(shared_ptr<ASTNode> obj, const string& attr, int line, int col)
        : ASTNode(NodeType::ATTR_REF, line, col), object(obj), attribute(attr) {}
    string toString(int indent = 0) const override;
};

class IdentifierNode : public ASTNode {
public:
    string name;

    IdentifierNode(const string& n, int line, int col)
        : ASTNode(NodeType::IDENTIFIER, line, col), name(n) {}
    string toString(int indent = 0) const override;
};

class LiteralNode : public ASTNode {
public:
    string value;
    string type;

    LiteralNode(const string& v, const string& t, int line, int col)
        : ASTNode(NodeType::LITERAL, line, col), value(v), type(t) {}
    string toString(int indent = 0) const override;
};

class ListNode : public ASTNode {
public:
    vector<shared_ptr<ASTNode>> elements;

    ListNode(int line, int col) : ASTNode(NodeType::LIST_LITERAL, line, col) {}
    string toString(int indent = 0) const override;
};

class DictNode : public ASTNode {
public:
    vector<pair<shared_ptr<ASTNode>, shared_ptr<ASTNode>>> items;

    DictNode(int line, int col) : ASTNode(NodeType::DICT_LITERAL, line, col) {}
    string toString(int indent = 0) const override;
};

class ParameterNode : public ASTNode {
public:
    string name;
    shared_ptr<ASTNode> default_value;

    ParameterNode(const string& name, shared_ptr<ASTNode> default_value = nullptr, int line = 0, int col = 0)
        : ASTNode(NodeType::PARAMETER_NODE, line, col), name(name), default_value(default_value) {}

    string toString(int indent = 0) const override;
};

class ParamListNode : public ASTNode {
public:
    vector<shared_ptr<ParameterNode>> parameters;

    ParamListNode() : ASTNode(NodeType::PARAM_LIST) {}
    string toString(int indent = 0) const override;
};

class ArgListNode : public ASTNode {
public:
    vector<shared_ptr<ASTNode>> arguments;

    ArgListNode() : ASTNode(NodeType::ARG_LIST) {}
    string toString(int indent = 0) const override;
};

class ConditionNode : public ASTNode {
public:
    shared_ptr<ASTNode> condition;

    ConditionNode(shared_ptr<ASTNode> cond, int line, int col)
        : ASTNode(NodeType::CONDITION_NODE, line, col), condition(cond) {}

    string toString(int indent = 0) const override;
};

class ErrorNode : public ASTNode {
public:
    string message;

    ErrorNode(const string& msg, int line, int col)
        : ASTNode(NodeType::ERROR_NODE, line, col), message(msg) {}
    string toString(int indent = 0) const override;
};

class Parser {
private:
    Lexer& lexer;
    vector<Token>::iterator current_token;
    vector<string> errors;
    bool has_error;

    Token& peek();
    Token& consume();
    bool match(TokenType type);
    bool match(const string& lexeme);
    bool check(TokenType type);
    bool check(const string& lexeme);
    bool isAtEnd();
    bool isAssignmentOperator(const string &lexeme);
    bool isBuiltInFunction(const string& name);
    void synchronize();
    void error(const string& message, int line, int column);

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
    shared_ptr<ASTNode> parsePrimary();
    shared_ptr<ASTNode> parseAttributeReference(shared_ptr<ASTNode> object);
    shared_ptr<ASTNode> parseSubscript(shared_ptr<ASTNode> container);
    shared_ptr<ASTNode> parseCall(shared_ptr<ASTNode> function);
    shared_ptr<ArgListNode> parseArguments();
    shared_ptr<ParamListNode> parseParameters();
    shared_ptr<ListNode> parseListLiteral();
    shared_ptr<DictNode> parseDictLiteral();

    void debugToken(const string &context);
    string tokenTypeToString(TokenType type);

public:
    Parser(Lexer& lexer);
    shared_ptr<ProgramNode> parse();
    void printParseTree(shared_ptr<ASTNode> root, int indent = 0);
    void printErrors();
    bool hasError() const { return !errors.empty(); }
    const vector<string>& getErrors() const { return errors; }
};

#endif // PARSER_H

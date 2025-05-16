// parser.cpp

#include "parser.h"
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

// Helper function to create indentation for pretty printing
string getIndentation(int indent) {
    return string(indent * 2, ' ');
}

// AST Node toString implementations
string ProgramNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Program" << endl;
    for (const auto& stmt : statements) {
        ss << stmt->toString(indent + 1);
    }
    return ss.str();
}

string StatementListNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "StatementList" << endl;
    for (const auto& stmt : statements) {
        ss << stmt->toString(indent + 1);
    }
    return ss.str();
}

string BlockNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Block" << endl;
    ss << statements->toString(indent + 1);
    return ss.str();
}

string AssignmentNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Assignment (line " << line_number << ")" << endl;
    ss << getIndentation(indent + 1) << "Target:" << endl;
    ss << target->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Value:" << endl;
    ss << value->toString(indent + 2);
    return ss.str();
}

string IfNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "If Statement (line " << line_number << ")" << endl;
    ss << getIndentation(indent + 1) << "Condition:" << endl;
    ss << condition->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Then:" << endl;
    ss << if_block->toString(indent + 2);

    if (!elif_clauses.empty()) {
        ss << getIndentation(indent + 1) << "Elif Clauses:" << endl;
        for (const auto& elif : elif_clauses) {
            ss << elif->toString(indent + 2);
        }
    }

    if (else_block) {
        ss << getIndentation(indent + 1) << "Else:" << endl;
        ss << else_block->toString(indent + 2);
    }

    return ss.str();
}

string ElifNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Elif Clause (line " << line_number << ")" << endl;
    ss << getIndentation(indent + 1) << "Condition:" << endl;
    ss << condition->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Then:" << endl;
    ss << block->toString(indent + 2);
    return ss.str();
}

string ElseNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Else Clause (line " << line_number << ")" << endl;
    ss << block->toString(indent + 1);
    return ss.str();
}

string WhileNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "While Statement (line " << line_number << ")" << endl;
    ss << getIndentation(indent + 1) << "Condition:" << endl;
    ss << condition->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Body:" << endl;
    ss << block->toString(indent + 2);
    return ss.str();
}

string ForNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "For Statement (line " << line_number << ")" << endl;
    ss << getIndentation(indent + 1) << "Target:" << endl;
    ss << target->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Iterable:" << endl;
    ss << iterable->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Body:" << endl;
    ss << block->toString(indent + 2);
    return ss.str();
}

string FunctionDefNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Function Definition: " << name << " (line " << line_number << ")" << endl;
    ss << getIndentation(indent + 1) << "Parameters:" << endl;
    ss << params->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Body:" << endl;
    ss << body->toString(indent + 2);
    return ss.str();
}

string ReturnNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Return Statement (line " << line_number << ")" << endl;
    if (expression) {
        ss << expression->toString(indent + 1);
    } else {
        ss << getIndentation(indent + 1) << "None" << endl;
    }
    return ss.str();
}

string ImportNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Import Statement (line " << line_number << ")" << endl;
    ss << getIndentation(indent + 1) << "Module: " << module << endl;
    if (!alias.empty()) {
        ss << getIndentation(indent + 1) << "As: " << alias << endl;
    }
    return ss.str();
}

string BinaryExprNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Binary Expression: " << op << " (line " << line_number << ")" << endl;
    ss << getIndentation(indent + 1) << "Left:" << endl;
    ss << left->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Right:" << endl;
    ss << right->toString(indent + 2);
    return ss.str();
}

string UnaryExprNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Unary Expression: " << op << " (line " << line_number << ")" << endl;
    ss << getIndentation(indent + 1) << "Operand:" << endl;
    ss << operand->toString(indent + 2);
    return ss.str();
}

string CallExprNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Function Call (line " << line_number << ")" << endl;
    ss << getIndentation(indent + 1) << "Function:" << endl;
    ss << function->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Arguments:" << endl;
    ss << arguments->toString(indent + 2);
    return ss.str();
}

string SubscriptExprNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Subscript Expression (line " << line_number << ")" << endl;
    ss << getIndentation(indent + 1) << "Container:" << endl;
    ss << container->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Index:" << endl;
    ss << index->toString(indent + 2);
    return ss.str();
}

string AttrRefNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Attribute Reference (line " << line_number << ")" << endl;
    ss << getIndentation(indent + 1) << "Object:" << endl;
    ss << object->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Attribute: " << attribute << endl;
    return ss.str();
}

string IdentifierNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Identifier: " << name << " (line " << line_number << ")" << endl;
    return ss.str();
}

string LiteralNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Literal: " << value << " (" << type << ") (line " << line_number << ")" << endl;
    return ss.str();
}

string ListNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "List (line " << line_number << ")" << endl;
    for (const auto& el : elements) {
        ss << el->toString(indent + 1);
    }
    return ss.str();
}

string DictNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Dictionary (line " << line_number << ")" << endl;
    for (const auto& item : items) {
        ss << getIndentation(indent + 1) << "Key:" << endl;
        ss << item.first->toString(indent + 2);
        ss << getIndentation(indent + 1) << "Value:" << endl;
        ss << item.second->toString(indent + 2);
    }
    return ss.str();
}

string ParameterNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Parameter: " << name;

    if (default_value) {
        ss << " (with default value)" << endl;
        ss << default_value->toString(indent + 1);
    } else {
        ss << " (no default)" << endl;
    }

    return ss.str();
}


string ParamListNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Parameters:" << endl;

    for (const auto& param : parameters) {
        ss << param->toString(indent + 1);
    }

    return ss.str();
}

string ArgListNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Argument List:" << endl;
    for (const auto& arg : arguments) {
        ss << arg->toString(indent + 1);
    }
    return ss.str();
}

// In parser.cpp:
string ConditionNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Condition:" << endl;
    if (condition) {
        ss << condition->toString(indent + 1);
    }
    return ss.str();
}

// Add this with your other toString implementations
string TerminalNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Terminal: " << value
       << " (line " << line_number << ", col " << column_number << ")\n";
    return ss.str();
}

string ErrorNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "ERROR (line " << line_number << "): " << message << endl;
    return ss.str();
}

string StatementNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Statement" << endl;
    ss << statement->toString(indent + 1);
    return ss.str();
}
string ElsePartNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "ElsePart" << endl;

    for (const auto& elif : elif_clauses) {
        ss << elif->toString(indent + 1);
    }

    if (else_block) {
        ss << else_block->toString(indent + 1);
    }

    return ss.str();
}
string ExpressionNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "Expression" << endl;
    if (expression) {
        ss << expression->toString(indent + 1);
    }
    return ss.str();
}

string AssignStmtNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "AssignmentStatement" << endl;
    if (assignment) {
        ss << assignment->toString(indent + 1);
    }
    return ss.str();
}
string ComparisonExprNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "ComparisonExpression" << endl;
    if (comparison) {
        ss << comparison->toString(indent + 1);
    }
    return ss.str();
}

string GroupExprNode::toString(int indent) const {
    stringstream ss;
    ss << getIndentation(indent) << "GroupExpr (line " << line_number << ")" << endl;
    ss << expression->toString(indent + 1);
    return ss.str();
}

// Parser implementation
Parser::Parser(Lexer& lexer) : lexer(lexer), has_error(false) {
    current_token = lexer.tokens.begin();
}

Token& Parser::peek() {
    return *current_token;
}

Token& Parser::consume() {
    if (!isAtEnd()) {
        return *(current_token++);
    }
    return *current_token; // Return EOF token
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        consume();
        return true;
    }
    return false;
}

bool Parser::match(const string& lexeme) {
    if (check(lexeme)) {
        consume();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return current_token->type == type;
}

bool Parser::check(const string& lexeme) {
    if (isAtEnd()) return false;
    return current_token->lexeme == lexeme;
}

bool Parser::isAtEnd() {
    return current_token == lexer.tokens.end() || current_token->type == END_OF_FILE;
}

bool Parser::isAssignmentOperator(const string &lexeme) {
    return
        lexeme == "="   ||
        lexeme == "+="  ||
        lexeme == "-="  ||
        lexeme == "*="  ||
        lexeme == "/="  ||
        lexeme == "%="  ||
        lexeme == "**=" ||
        lexeme == "//=" ||
        lexeme == "<<=" ||
        lexeme == ">>=" ||
        lexeme == "&="  ||
        lexeme == "|="  ||
        lexeme == "^=";
}


void Parser::error(const string &message, int line, int column)
{
    string error_msg = "Syntax Error at line " + to_string(line) + ", column "
                            + to_string(column) + ": " + message;
    errors.push_back(error_msg);
    has_error = true;
}

void Parser::synchronize() {
    consume(); // Skip the problematic token

    // Skip tokens until we find a statement boundary
    while (!isAtEnd()) {
        // Check if we've reached a semicolon, which marks a statement boundary
        if (peek().type == SYMBOL && peek().lexeme == ";") {
            consume(); // Consume the semicolon
            return;
        }

        // These tokens often indicate a statement boundary
        if (peek().type == KEYWORD &&
            (peek().lexeme == "if" || peek().lexeme == "while" ||
             peek().lexeme == "for" || peek().lexeme == "def" ||
             peek().lexeme == "return" || peek().lexeme == "import")) {
            return;
        }

        if (peek().type == INDENT || peek().type == DEDENT) {
            return;
        }

        consume();
    }
}

shared_ptr<ProgramNode> Parser::parse() {
    return parseProgram();
}

shared_ptr<ProgramNode> Parser::parseProgram() {
    auto program = make_shared<ProgramNode>();

    try {
        auto statements = parseStatementList();
        program->statements = statements->statements;
    } catch (const exception& e) {
        error(e.what(), peek().line_number, peek().column_number);
        synchronize();
    }

    return program;
}

shared_ptr<StatementListNode> Parser::parseStatementList() {
    auto statement_list = make_shared<StatementListNode>();

    while (!isAtEnd() && peek().type != DEDENT) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                statement_list->statements.push_back(stmt);
            }
        } catch (const exception& e) {
            error(e.what(), peek().line_number, peek().column_number);
            synchronize();
        }
    }

    return statement_list;
}

shared_ptr<ASTNode> Parser::parseStatement() {
    shared_ptr<ASTNode> statement;

    // Check for keywords that start specific statement types
    if (check(KEYWORD)) {
        if (check("if")) {
            statement = parseIfStatement();
        } else if (check("while")) {
            statement = parseWhileStatement();
        } else if (check("for")) {
            statement = parseForStatement();
        } else if (check("def")) {
            statement = parseFunctionDef();
        } else if (check("return")) {
            statement = parseReturnStatement();
        } else if (check("import")) {
            statement = parseImportStatement();
        }
    } else {
        // If not a special statement, it's an expression or assignment
        auto expr = parseExpression();

        // Check if this is an assignment (target = value)
        if (check(OPERATOR) && isAssignmentOperator(peek().lexeme)) {
            statement = parseAssignment(expr);
        } else {
            statement = expr;
        }
    }

    // Handle optional semicolon at the end of the statement
    if (check(SYMBOL) && peek().lexeme == ";") {
        consume(); // Consume the semicolon
    }

    return statement;
}

shared_ptr<AssignmentNode> Parser::parseAssignment(shared_ptr<ASTNode> target) {
    Token op = peek();
    consume(); // Consume = operator

    auto value = parseExpression();
    // you’ll need an AssignmentNode constructor that also stores op.lexeme
    return make_shared<AssignmentNode>(target,
                                            value,
                                            op.lexeme,
                                            op.line_number,
                                            op.column_number);
}

shared_ptr<IfNode> Parser::parseIfStatement() {
    Token if_token = consume(); // Consume 'if'

    bool hasParentheses = check(LPAREN);
    if (hasParentheses) consume(); // Consume '('

    auto condition = parseExpression();

    if (hasParentheses) {
        if (!check(RPAREN)) {
            error("Expected ')' after condition", peek().line_number, peek().column_number);
        } else {
            consume(); // Consume ')'
        }
    }

    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after if condition", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in if statement");
    }
    consume();

    auto if_block = parseBlock();
    // auto if_node = make_shared<IfNode>(condition, if_block, if_token.line_number, if_token.column_number);
    auto if_node = make_shared<IfNode>(
        condition, if_block, if_token.line_number, if_token.column_number, hasParentheses);
    // ...
    // Process elif clauses directly into the vector
    while (check(KEYWORD) && peek().lexeme == "elif") {
        if_node->elif_clauses.push_back(parseElifClause());
    }

    // Handle optional else clause
    if (check(KEYWORD) && peek().lexeme == "else") {
        if_node->else_block = parseElseClause();
    }

    return if_node;
}

shared_ptr<ElifNode> Parser::parseElifClause() {
    Token elif_token = consume(); // Consume 'elif'

    bool hasParentheses = check(LPAREN);
    if (hasParentheses) consume(); // Consume '('

    auto condition = parseExpression();

    if (hasParentheses) {
        if (!check(RPAREN)) {
            error("Expected ')' after condition", peek().line_number, peek().column_number);
        } else {
            consume(); // Consume ')'
        }
    }

    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after if condition", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in if statement");
    }
    consume();

    auto block = parseBlock();
    return make_shared<ElifNode>(condition, block, elif_token.line_number, elif_token.column_number, hasParentheses);
}

shared_ptr<ElseNode> Parser::parseElseClause() {
    Token else_token = consume(); // Consume 'else'

    // Check specifically for the colon
    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after else", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in else clause");
    }
    consume(); // Consume the colon - this line is essential

    auto block = parseBlock();
    return make_shared<ElseNode>(block, else_token.line_number, else_token.column_number);
}


shared_ptr<WhileNode> Parser::parseWhileStatement() {
    Token while_token = consume(); // Consume 'while'

    bool hasParentheses = check(LPAREN);
    if (hasParentheses) consume(); // Consume '('

    auto condition = parseExpression();

    if (hasParentheses) {
        if (!check(RPAREN)) {
            error("Expected ')' after condition", peek().line_number, peek().column_number);
        } else {
            consume(); // Consume ')'
        }
    }

    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after while condition", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in while statement");
    }
    consume();

    auto block = parseBlock();
    return make_shared<WhileNode>(condition, block, while_token.line_number, while_token.column_number, hasParentheses);
}

shared_ptr<ForNode> Parser::parseForStatement() {
    Token for_token = consume(); // Consume 'for'

    bool hasParentheses = check(LPAREN);
    if (hasParentheses) consume(); // Consume '('

    auto target = parseExpression();

    if (!check(KEYWORD) || peek().lexeme != "in") {
        error("Expected 'in' keyword in for loop", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in for statement");
    }

    // Create a terminal node for "in" keyword (we'll handle this in visualization)
    Token in_token = consume(); // Consume 'in'

    auto iterable = parseExpression();

    if (hasParentheses) {
        if (!check(RPAREN)) {
            error("Expected ')' after for loop iterable", peek().line_number, peek().column_number);
        } else {
            consume(); // Consume ')'
        }
    }

    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after for loop header", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in for statement");
    }
    consume(); // Consume ':'

    auto block = parseBlock();
    return make_shared<ForNode>(target, iterable, block, for_token.line_number, for_token.column_number, hasParentheses);
}



shared_ptr<FunctionDefNode> Parser::parseFunctionDef() {
    // Create a terminal node for the 'def' keyword
    Token def_token = peek();
    auto defKeyword = make_shared<TerminalNode>(def_token.lexeme, def_token.line_number, def_token.column_number);
    consume(); // Consume 'def'

    // Check for function name
    if (!check(IDENTIFIER) && !check(FUNCTION_IDENTIFIER)) {
        error("Expected function name after 'def'", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in function definition");
    }

    // Create a terminal node for the function name
    Token nameToken = peek();
    auto nameNode = make_shared<TerminalNode>(nameToken.lexeme, nameToken.line_number, nameToken.column_number);
    string name = consume().lexeme;

    // Check for opening parenthesis
    if (!check(LPAREN)) {
        error("Expected '(' after function name", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in function definition");
    }

    // Create a terminal node for the opening parenthesis
    Token openParen = peek();
    auto openParenNode = make_shared<TerminalNode>(openParen.lexeme, openParen.line_number, openParen.column_number);
    consume(); // Consume '('

    // Parse parameter list
    auto params = parseParameters();

    // Check for closing parenthesis
    if (!check(RPAREN)) {
        error("Expected ')' after function parameters", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in function definition");
    }

    // Create a terminal node for the closing parenthesis
    Token closeParen = peek();
    auto closeParenNode = make_shared<TerminalNode>(closeParen.lexeme, closeParen.line_number, closeParen.column_number);
    consume(); // Consume ')'

    // Check for colon
    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after function parameters", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in function definition");
    }

    // Create a terminal node for the colon
    Token colon = peek();
    auto colonNode = make_shared<TerminalNode>(colon.lexeme, colon.line_number, colon.column_number);
    consume(); // Consume ':'

    // Parse function body
    auto body = parseBlock();

    // Create the function definition node with the terminal nodes
    auto funcDef = make_shared<FunctionDefNode>(name, params, body, def_token.line_number, def_token.column_number);
    funcDef->defKeyword = defKeyword;   // Store the 'def' keyword
    funcDef->nameNode = nameNode;       // Store the function name
    funcDef->openParen = openParenNode;
    funcDef->closeParen = closeParenNode;
    funcDef->colon = colonNode;

    return funcDef;
}

shared_ptr<ReturnNode> Parser::parseReturnStatement() {
    Token return_token = consume(); // Consume 'return'

    // Return can be with or without an expression
    if (peek().type == SYMBOL && peek().lexeme == ":") {
        return make_shared<ReturnNode>(nullptr, return_token.line_number, return_token.column_number);
    }

    auto expr = parseExpression();
    return make_shared<ReturnNode>(expr, return_token.line_number, return_token.column_number);
}

shared_ptr<ImportNode> Parser::parseImportStatement() {
    Token import_token = consume(); // Consume 'import'

    if (!check(IDENTIFIER)) {
        error("Expected module name after 'import'", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in import statement");
    }

    string module = consume().lexeme;
    string alias;

    // Handle "import x as y" syntax
    if (check(KEYWORD) && peek().lexeme == "as") {
        consume(); // Consume 'as'

        if (!check(IDENTIFIER)) {
            error("Expected alias name after 'as'", peek().line_number, peek().column_number);
            throw runtime_error("Syntax error in import statement");
        }

        alias = consume().lexeme;
    }

    return make_shared<ImportNode>(module, alias, import_token.line_number, import_token.column_number);
}

shared_ptr<BlockNode> Parser::parseBlock() {
    auto block = make_shared<BlockNode>();

    // A block must start with an indent
    if (!match(INDENT)) {
        error("Expected indented block", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in code block");
    }

    block->statements = parseStatementList();

    // A block must end with a dedent (unless we're at EOF)
    if (!match(DEDENT) && !isAtEnd()) {
        error("Expected dedent at end of block", peek().line_number, peek().column_number);
    }

    return block;
}

shared_ptr<ASTNode> Parser::parseExpression() {
    return parseOrExpr();
}

shared_ptr<ASTNode> Parser::parseOrExpr() {
    auto left = parseAndExpr();

    while (check(KEYWORD) && peek().lexeme == "or") {
        Token op = consume();
        auto right = parseAndExpr();
        left = make_shared<BinaryExprNode>("or", left, right, op.line_number, op.column_number);
    }

    return left;
}

shared_ptr<ASTNode> Parser::parseAndExpr() {
    auto left = parseNotExpr();

    while (check(KEYWORD) && peek().lexeme == "and") {
        Token op = consume();
        auto right = parseNotExpr();
        left = make_shared<BinaryExprNode>("and", left, right, op.line_number, op.column_number);
    }

    return left;
}

shared_ptr<ASTNode> Parser::parseNotExpr() {
    // Special handling for 'not' keyword
    if (check(KEYWORD) && peek().lexeme == "not") {
        Token op = consume(); // Consume 'not'

        // DEBUG
        // cout << "Processing NOT operator at line " << op.line_number << ", column " << op.column_number << endl;

        // Parse the expression that follows the 'not'
        auto operand = parseComparisonExpr();

        return make_shared<UnaryExprNode>("not", operand, op.line_number, op.column_number);
    }

    return parseComparisonExpr();
}

shared_ptr<ASTNode> Parser::parseComparisonExpr() {
    auto left = parseArithmeticExpr();

    // Handle comparison operators: ==, !=, <, >, <=, >=
    if (check(OPERATOR) && (
        peek().lexeme == "==" || peek().lexeme == "!=" ||
        peek().lexeme == "<" || peek().lexeme == ">" ||
        peek().lexeme == "<=" || peek().lexeme == ">=")) {

        Token op = consume();
        auto right = parseArithmeticExpr();
        // left = make_shared<BinaryExprNode>(op.lexeme, left, right, op.line_number, op.column_number);
        // Create a BinaryExprNode first
        auto binary = make_shared<BinaryExprNode>(op.lexeme, left, right, op.line_number, op.column_number);

        // Wrap it in a ComparisonExprNode
        return make_shared<ComparisonExprNode>(binary);
    }

    return left;
}

shared_ptr<ASTNode> Parser::parseArithmeticExpr() {
    auto left = parseTerm();

    // Handle addition and subtraction
    while (check(OPERATOR) && (peek().lexeme == "+" || peek().lexeme == "-")) {
        Token op = consume();
        auto right = parseTerm();
        left = make_shared<BinaryExprNode>(op.lexeme, left, right, op.line_number, op.column_number);
    }

    return left;
}

shared_ptr<ASTNode> Parser::parseTerm() {
    auto left = parseFactor();

    // Handle multiplication, division, and modulo
    while (check(OPERATOR) && (
        peek().lexeme == "*" || peek().lexeme == "/" || peek().lexeme == "%")) {

        Token op = consume();
        auto right = parseFactor();
        left = make_shared<BinaryExprNode>(op.lexeme, left, right, op.line_number, op.column_number);
    }

    return left;
}

shared_ptr<ASTNode> Parser::parseFactor() {
    auto left = parsePower();

    // Handle exponentiation
    if (check(OPERATOR) && peek().lexeme == "**") {
        Token op = consume();
        auto right = parseFactor(); // Exponentiation is right-associative
        left = make_shared<BinaryExprNode>(op.lexeme, left, right, op.line_number, op.column_number);
    }

    return left;
}

shared_ptr<ASTNode> Parser::parsePower() {
    return parseUnary();
}


shared_ptr<ASTNode> Parser::parseUnary() {
    // Handle unary operators: +, -
    if (check(OPERATOR) && (peek().lexeme == "+" || peek().lexeme == "-")) {
        Token op = consume();

        // NEW: Check if the next token is also a unary operator
        if (check(OPERATOR) && (peek().lexeme == "+" || peek().lexeme == "-")) {
            // This is an error - adjacent unary operators without parentheses
            error("Invalid syntax: adjacent unary operators are not allowed without parentheses",
                  peek().line_number, peek().column_number);
            throw runtime_error("Syntax error in expression");
        }

        auto operand = parseUnary();
        return make_shared<UnaryExprNode>(op.lexeme, operand, op.line_number, op.column_number);
    }

    return parsePrimary();
}
//remove these two functions later
void Parser::debugToken(const string& context) {
    if (isAtEnd()) {
        cout << context << ": At end of token stream" << endl;
        return;
    }

    cout << context << ": Token = " << peek().lexeme
              << ", Type = " << tokenTypeToString(peek().type)
              << ", Line " << peek().line_number
              << ", Column " << peek().column_number << endl;
}

string Parser::tokenTypeToString(TokenType type) {
    // Add cases for each token type to debug
    switch(type) {
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case LBRACKET: return "LBRACKET";
        case RBRACKET: return "RBRACKET";
        // Add other token types as needed
        default: return "OTHER";
    }
}


shared_ptr<ASTNode> Parser::parsePrimary() {
    // Debug at the start to see what token we're starting with
    debugToken("parsePrimary start");

    // Skip any INDENT/DEDENT tokens that appear in expressions
    while (check(INDENT) || check(DEDENT)) {
        cout << "Skipping INDENT/DEDENT in primary expression" << endl;
        consume();
    }

    // Check for dictionary literals
    if (check(LBRACE)) {
        debugToken("Found LBRACE, about to parse dictionary");
        return parseDictLiteral();
    }

    // Debug before checking for identifiers
    debugToken("Checking for identifier");
    // Handle identifiers
    if (check(IDENTIFIER) || check(FUNCTION_IDENTIFIER)) {
        Token id = consume();
        auto node = make_shared<IdentifierNode>(id.lexeme, id.line_number, id.column_number);

        // Handle function calls: func()
        if (check(LPAREN)) {
            return parseCall(node);
        }

        // NEW: Check for built-in functions that should be called with parentheses
        if (isBuiltInFunction(id.lexeme) && (check(STRING) || check(IDENTIFIER) || check(NUMERIC))) {
            error("Missing parentheses in call to '" + id.lexeme + "'",
                  peek().line_number, peek().column_number);
            throw runtime_error("Syntax error in function call");
        }

        // Handle attribute access: obj.attr
        if (check(OPERATOR) && peek().lexeme == ".") {
            return parseAttributeReference(node);
        }

        // Handle subscripting: list[index]
        if (check(LBRACKET)) {
            return parseSubscript(node);
        }

        return node;
    }

    // Debug before checking for literals
    debugToken("Checking for literals");

    // Handle literals: numbers
    if (check(NUMERIC)) {
        Token num = consume();
        string type = num.lexeme.find('.') != string::npos ? "float" : "int";
        return make_shared<LiteralNode>(num.lexeme, type, num.line_number, num.column_number);
    }

    // Handle literals: strings
    if (check(STRING)) {
        Token str = consume();
        return make_shared<LiteralNode>(str.lexeme, "string", str.line_number, str.column_number);
    }

    // Handle literals: booleans and None
    if (check(DATA_TYPE)) {
        Token data = consume();
        string type = (data.lexeme == "True" || data.lexeme == "False") ? "bool" : "None";
        return make_shared<LiteralNode>(data.lexeme, type, data.line_number, data.column_number);
    }

    // Handle parenthesized expressions
    if (check(LPAREN)) {
        Token openParen = consume(); // Consume '('

        auto expr = parseExpression();

        if (!check(RPAREN)) {
            error("Expected ')' after expression", peek().line_number, peek().column_number);
            throw runtime_error("Syntax error in parenthesized expression");
        }
        Token closeParen = consume(); // Consume ')'

        // Create a GroupExprNode to represent the parenthesized expression
        return make_shared<GroupExprNode>(expr, openParen.line_number, openParen.column_number);
    }

    // Handle list literals
    if (check(LBRACKET)) {
        return parseListLiteral();
    }

    // Add final debug before error
    debugToken("No valid expression found");

    error("Expected expression", peek().line_number, peek().column_number);
    throw runtime_error("Unexpected token in expression");
}

shared_ptr<ASTNode> Parser::parseAttributeReference(shared_ptr<ASTNode> object) {
    consume(); // Consume '.'

    // Be more flexible with what we accept as an attribute name
    // Check for both IDENTIFIER and FUNCTION_IDENTIFIER
    if (!check(IDENTIFIER) && !check(FUNCTION_IDENTIFIER)) {
        error("Expected attribute name after '.'", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in attribute reference");
    }

    Token attr = consume();
    auto attr_ref = make_shared<AttrRefNode>(object, attr.lexeme, attr.line_number, attr.column_number);

    // Handle chained attribute access: obj.attr1.attr2
    if (check(OPERATOR) && peek().lexeme == ".") {
        return parseAttributeReference(attr_ref);
    }

    // Handle method calls: obj.method()
    if (check(LPAREN)) {
        return parseCall(attr_ref);
    }

    return attr_ref;
}

shared_ptr<ASTNode> Parser::parseSubscript(shared_ptr<ASTNode> container) {
    Token bracket = consume(); // Consume '['

    auto index = parseExpression();

    if (!match(RBRACKET)) {
        error("Expected ']' after subscript index", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in subscript expression");
    }

    auto subscript = make_shared<SubscriptExprNode>(container, index, bracket.line_number, bracket.column_number);

    // Handle chained subscripts: list[i][j]
    if (check(LBRACKET)) {
        return parseSubscript(subscript);
    }

    // Handle attribute access after subscript: list[i].append
    if (check(OPERATOR) && peek().lexeme == ".") {
        return parseAttributeReference(subscript);
    }

    return subscript;
}


shared_ptr<ASTNode> Parser::parseCall(shared_ptr<ASTNode> function) {
    // Create a terminal node for the opening parenthesis
    Token openParen = consume(); // Consume '('
    auto openParenNode = make_shared<TerminalNode>(openParen.lexeme, openParen.line_number, openParen.column_number);

    // Parse arguments (without including the parentheses)
    auto args = make_shared<ArgListNode>();

    if (!check(RPAREN)) {
        // Parse the arguments
        args->arguments.push_back(parseExpression());

        // Parse additional arguments if there are commas
        while (check(SYMBOL) && peek().lexeme == ",") {
            // Create a terminal node for the comma
            Token comma = consume(); // Consume the comma
            auto commaNode = make_shared<TerminalNode>(comma.lexeme, comma.line_number, comma.column_number);
            args->arguments.push_back(commaNode);

            args->arguments.push_back(parseExpression());
        }
    }

    // Check for the closing parenthesis
    if (!check(RPAREN)) {
        error("Expected ')' after function arguments", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in function call");
    }

    // Create a terminal node for the closing parenthesis
    Token closeParen = consume(); // Consume ')'
    auto closeParenNode = make_shared<TerminalNode>(closeParen.lexeme, closeParen.line_number, closeParen.column_number);

    // Create the call node with function, args, and both parentheses nodes
    auto call = make_shared<CallExprNode>(function, args, openParen.line_number, openParen.column_number);

    // Add the call node structure information to allow proper visualization
    call->openParen = openParenNode;
    call->closeParen = closeParenNode;

    return call;
}


shared_ptr<ArgListNode> Parser::parseArguments() {
    auto arg_list = make_shared<ArgListNode>();

    // If not immediately at closing parenthesis, parse arguments
    if (!check(RPAREN)) {
        // Parse first argument
        arg_list->arguments.push_back(parseExpression());

        // Parse remaining arguments
        while (check(SYMBOL) && peek().lexeme == ",") {
            consume(); // Explicitly consume the comma
            arg_list->arguments.push_back(parseExpression());
        }
    }

    return arg_list;
}


shared_ptr<ParamListNode> Parser::parseParameters() {
    auto param_list = make_shared<ParamListNode>();

    // If we're not immediately at the closing parenthesis, then parse parameters
    if (!check(RPAREN)) {
        // Parse first parameter
        if (!check(IDENTIFIER)) {
            error("Expected parameter name", peek().line_number, peek().column_number);
            throw runtime_error("Syntax error in parameter list");
        }

        // Get parameter name
        Token param_token = consume();
        shared_ptr<ASTNode> default_value = nullptr;

        // Check for default value (=)
        if (check(OPERATOR) && peek().lexeme == "=") {
            // Create a terminal node for the equals sign
            Token equals = consume(); // Consume the equals sign
            auto equalsNode = make_shared<TerminalNode>(equals.lexeme, equals.line_number, equals.column_number);

            // Parse and store the default value expression
            default_value = parseExpression();

            // Create a statement list to hold the equals and the default value
            auto defaultContainer = make_shared<StatementListNode>();
            defaultContainer->statements.push_back(equalsNode);
            defaultContainer->statements.push_back(default_value);
            default_value = defaultContainer;
        }

        // Create a ParameterNode for the parameter
        auto param = make_shared<ParameterNode>(
            param_token.lexeme,
            default_value,
            param_token.line_number,
            param_token.column_number
        );

        param_list->parameters.push_back(param);

        // Parse remaining parameters
        while (check(SYMBOL) && peek().lexeme == ",") {
            // Create a terminal node for the comma
            Token comma = consume(); // Explicitly consume the comma
            auto commaNode = make_shared<TerminalNode>(comma.lexeme, comma.line_number, comma.column_number);

            // Store the comma as a parameter with empty name
            auto commaParam = make_shared<ParameterNode>(
                ",", nullptr, comma.line_number, comma.column_number
            );
            param_list->parameters.push_back(commaParam);

            if (!check(IDENTIFIER)) {
                error("Expected parameter name after comma", peek().line_number, peek().column_number);
                throw runtime_error("Syntax error in parameter list");
            }

            // Get parameter name
            param_token = consume();
            default_value = nullptr;

            // Check for default value (=)
            if (check(OPERATOR) && peek().lexeme == "=") {
                // Create a terminal node for the equals sign
                Token equals = consume(); // Consume the equals sign
                auto equalsNode = make_shared<TerminalNode>(equals.lexeme, equals.line_number, equals.column_number);

                // Parse and store the default value expression
                default_value = parseExpression();

                // Create a statement list to hold the equals and the default value
                auto defaultContainer = make_shared<StatementListNode>();
                defaultContainer->statements.push_back(equalsNode);
                defaultContainer->statements.push_back(default_value);
                default_value = defaultContainer;
            }

            // Create a ParameterNode for each parameter
            auto param = make_shared<ParameterNode>(
                param_token.lexeme,
                default_value,
                param_token.line_number,
                param_token.column_number
            );

            param_list->parameters.push_back(param);
        }
    }

    return param_list;
}


shared_ptr<ListNode> Parser::parseListLiteral() {
    // Create a terminal node for the opening bracket
    Token openBracket = consume(); // Consume '['
    auto openBracketNode = make_shared<TerminalNode>(openBracket.lexeme, openBracket.line_number, openBracket.column_number);

    auto list = make_shared<ListNode>(openBracket.line_number, openBracket.column_number);

    // Add the opening bracket to the list
    list->elements.push_back(openBracketNode);

    // Skip any INDENT tokens within the list
    while (check(INDENT)) {
        consume(); // Skip the INDENT token
    }

    if (!check(RBRACKET)) {
        // Parse first element
        list->elements.push_back(parseExpression());

        // Parse remaining elements
        while (true) {
            // Skip any INDENT/DEDENT tokens between elements
            while (check(INDENT) || check(DEDENT)) {
                consume();
            }

            if (check(SYMBOL) && peek().lexeme == ",") {
                // Create a terminal node for the comma
                Token comma = consume(); // Consume the comma
                auto commaNode = make_shared<TerminalNode>(comma.lexeme, comma.line_number, comma.column_number);
                list->elements.push_back(commaNode);

                // Skip any INDENT/DEDENT tokens after comma
                while (check(INDENT) || check(DEDENT)) {
                    consume();
                }

                // If there's a trailing comma followed by closing bracket, break
                if (check(RBRACKET)) {
                    break;
                }

                list->elements.push_back(parseExpression());
            } else if (check(RBRACKET)) {
                break; // End of list
            } else {
                error("Expected ',' or ']' after list element", peek().line_number, peek().column_number);
                throw runtime_error("Syntax error in list literal");
            }
        }
    }

    if (!check(RBRACKET)) {
        error("Expected ']' after list elements", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in list literal");
    }

    // Create a terminal node for the closing bracket
    Token closeBracket = consume(); // Consume the closing bracket
    auto closeBracketNode = make_shared<TerminalNode>(closeBracket.lexeme, closeBracket.line_number, closeBracket.column_number);
    list->elements.push_back(closeBracketNode);

    return list;
}


shared_ptr<DictNode> Parser::parseDictLiteral() {
    cout << "Entering parseDictLiteral with token: " << peek().lexeme
              << " at line " << peek().line_number
              << ", column " << peek().column_number << endl;

    // Create a terminal node for the opening brace
    Token openBrace = consume(); // Consume '{'
    auto openBraceNode = make_shared<TerminalNode>(openBrace.lexeme, openBrace.line_number, openBrace.column_number);

    auto dict = make_shared<DictNode>(openBrace.line_number, openBrace.column_number);

    // Store opening brace as the first item (key with null value)
    dict->items.emplace_back(openBraceNode, nullptr);

    // Skip any INDENT tokens that appear within the dictionary
    while (check(INDENT)) {
        cout << "Skipping INDENT in dictionary" << endl;
        consume(); // Skip the INDENT token
    }

    // Check if we have an empty dictionary {}
    if (check(RBRACE)) {
        // Create a terminal node for the closing brace
        Token closeBrace = consume(); // Consume the closing brace
        auto closeBraceNode = make_shared<TerminalNode>(closeBrace.lexeme, closeBrace.line_number, closeBrace.column_number);

        // Store closing brace (null key with value)
        dict->items.emplace_back(nullptr, closeBraceNode);
        return dict;
    }

    // Parse first key-value pair
    auto key = parseExpression();

    // Skip any DEDENT tokens that might appear between key and colon
    while (check(DEDENT)) {
        cout << "Skipping DEDENT after key" << endl;
        consume();
    }

    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after dictionary key", peek().line_number, peek().column_number);
        throw runtime_error("Syntax error in dictionary literal");
    }

    // Create a terminal node for the colon
    Token colon = consume(); // Consume the colon
    auto colonNode = make_shared<TerminalNode>(colon.lexeme, colon.line_number, colon.column_number);

    // Store the key:colon pair
    dict->items.emplace_back(key, colonNode);

    auto value = parseExpression();
    dict->items.emplace_back(nullptr, value);

    // Parse remaining key-value pairs
    while (true) {
        // Skip any INDENT/DEDENT tokens between items
        while (check(INDENT) || check(DEDENT)) {
            cout << "Skipping INDENT/DEDENT between dict items" << endl;
            consume();
        }

        // Check for comma to continue or closing brace to end
        if (check(SYMBOL) && peek().lexeme == ",") {
            // Create a terminal node for the comma
            Token comma = consume(); // Consume the comma
            auto commaNode = make_shared<TerminalNode>(comma.lexeme, comma.line_number, comma.column_number);
            dict->items.emplace_back(nullptr, commaNode);

            // Skip any INDENT/DEDENT that might follow the comma
            while (check(INDENT) || check(DEDENT)) {
                cout << "Skipping INDENT/DEDENT after comma" << endl;
                consume();
            }

            // Handle trailing comma
            if (check(RBRACE)) {
                break;
            }

            key = parseExpression();

            // Skip any INDENT/DEDENT between key and colon
            while (check(INDENT) || check(DEDENT)) {
                cout << "Skipping INDENT/DEDENT between key and colon" << endl;
                consume();
            }

            if (!check(SYMBOL) || peek().lexeme != ":") {
                error("Expected ':' after dictionary key", peek().line_number, peek().column_number);
                throw runtime_error("Syntax error in dictionary literal");
            }

            // Create a terminal node for the colon
            Token colon = consume(); // Consume the colon
            auto colonNode = make_shared<TerminalNode>(colon.lexeme, colon.line_number, colon.column_number);

            // Store the key:colon pair
            dict->items.emplace_back(key, colonNode);

            value = parseExpression();
            dict->items.emplace_back(nullptr, value);
        } else if (check(RBRACE)) {
            break; // End of dictionary
        } else {
            error("Expected ',' or '}' after dictionary item", peek().line_number, peek().column_number);
            throw runtime_error("Syntax error in dictionary literal");
        }
    }

    // Create a terminal node for the closing brace
    Token closeBrace = consume(); // Consume the closing brace
    auto closeBraceNode = make_shared<TerminalNode>(closeBrace.lexeme, closeBrace.line_number, closeBrace.column_number);
    dict->items.emplace_back(nullptr, closeBraceNode);

    return dict;
}

void Parser::printParseTree(shared_ptr<ASTNode> root, int indent) {
    cout << root->toString(indent);
}

void Parser::printErrors() {
    if (!errors.empty()) {
        cout << "\n--- Parse Errors ---\n";
        for (const auto& error : errors) {
            cout << error << endl;
        }
        cout << "Total errors: " << errors.size() << endl;
    }
}

bool Parser::isBuiltInFunction(const string& name) {
    // List of common built-in functions in Python
    static const vector<string> builtins = {"print", "input",  "len",      "range",
                                                      "str",   "int",    "float",    "list",
                                                      "dict",  "set",    "tuple",    "sum",
                                                      "min",   "max",    "abs",      "all",
                                                      "any",   "sorted", "reversed", "enumerate",
                                                      "zip",   "open",   "type"};

    return find(builtins.begin(), builtins.end(), name) != builtins.end();
}
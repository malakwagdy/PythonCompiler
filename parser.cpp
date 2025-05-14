#include "parser.h"
#include <sstream>
#include <iostream>
#include <iomanip>

// Helper function to create indentation for pretty printing
std::string getIndentation(int indent) {
    return std::string(indent * 2, ' ');
}

// AST Node toString implementations
std::string ProgramNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Program" << std::endl;
    for (const auto& stmt : statements) {
        ss << stmt->toString(indent + 1);
    }
    return ss.str();
}

std::string StatementListNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "StatementList" << std::endl;
    for (const auto& stmt : statements) {
        ss << stmt->toString(indent + 1);
    }
    return ss.str();
}

std::string BlockNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Block" << std::endl;
    ss << statements->toString(indent + 1);
    return ss.str();
}

std::string AssignmentNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Assignment (line " << line_number << ")" << std::endl;
    ss << getIndentation(indent + 1) << "Target:" << std::endl;
    ss << target->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Value:" << std::endl;
    ss << value->toString(indent + 2);
    return ss.str();
}

std::string IfNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "If Statement (line " << line_number << ")" << std::endl;
    ss << getIndentation(indent + 1) << "Condition:" << std::endl;
    ss << condition->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Then:" << std::endl;
    ss << if_block->toString(indent + 2);

    for (const auto& elif : elif_clauses) {
        ss << elif->toString(indent + 1);
    }

    if (else_block) {
        ss << getIndentation(indent + 1) << "Else:" << std::endl;
        ss << else_block->toString(indent + 2);
    }

    return ss.str();
}

std::string ElifNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Elif Clause (line " << line_number << ")" << std::endl;
    ss << getIndentation(indent + 1) << "Condition:" << std::endl;
    ss << condition->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Then:" << std::endl;
    ss << block->toString(indent + 2);
    return ss.str();
}

std::string ElseNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Else Clause (line " << line_number << ")" << std::endl;
    ss << block->toString(indent + 1);
    return ss.str();
}

std::string WhileNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "While Statement (line " << line_number << ")" << std::endl;
    ss << getIndentation(indent + 1) << "Condition:" << std::endl;
    ss << condition->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Body:" << std::endl;
    ss << block->toString(indent + 2);
    return ss.str();
}

std::string ForNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "For Statement (line " << line_number << ")" << std::endl;
    ss << getIndentation(indent + 1) << "Target:" << std::endl;
    ss << target->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Iterable:" << std::endl;
    ss << iterable->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Body:" << std::endl;
    ss << block->toString(indent + 2);
    return ss.str();
}

std::string FunctionDefNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Function Definition: " << name << " (line " << line_number << ")" << std::endl;
    ss << getIndentation(indent + 1) << "Parameters:" << std::endl;
    ss << params->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Body:" << std::endl;
    ss << body->toString(indent + 2);
    return ss.str();
}

std::string ReturnNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Return Statement (line " << line_number << ")" << std::endl;
    if (expression) {
        ss << expression->toString(indent + 1);
    } else {
        ss << getIndentation(indent + 1) << "None" << std::endl;
    }
    return ss.str();
}

std::string ImportNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Import Statement (line " << line_number << ")" << std::endl;
    ss << getIndentation(indent + 1) << "Module: " << module << std::endl;
    if (!alias.empty()) {
        ss << getIndentation(indent + 1) << "As: " << alias << std::endl;
    }
    return ss.str();
}

std::string BinaryExprNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Binary Expression: " << op << " (line " << line_number << ")" << std::endl;
    ss << getIndentation(indent + 1) << "Left:" << std::endl;
    ss << left->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Right:" << std::endl;
    ss << right->toString(indent + 2);
    return ss.str();
}

std::string UnaryExprNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Unary Expression: " << op << " (line " << line_number << ")" << std::endl;
    ss << getIndentation(indent + 1) << "Operand:" << std::endl;
    ss << operand->toString(indent + 2);
    return ss.str();
}

std::string CallExprNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Function Call (line " << line_number << ")" << std::endl;
    ss << getIndentation(indent + 1) << "Function:" << std::endl;
    ss << function->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Arguments:" << std::endl;
    ss << arguments->toString(indent + 2);
    return ss.str();
}

std::string SubscriptExprNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Subscript Expression (line " << line_number << ")" << std::endl;
    ss << getIndentation(indent + 1) << "Container:" << std::endl;
    ss << container->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Index:" << std::endl;
    ss << index->toString(indent + 2);
    return ss.str();
}

std::string AttrRefNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Attribute Reference (line " << line_number << ")" << std::endl;
    ss << getIndentation(indent + 1) << "Object:" << std::endl;
    ss << object->toString(indent + 2);
    ss << getIndentation(indent + 1) << "Attribute: " << attribute << std::endl;
    return ss.str();
}

std::string IdentifierNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Identifier: " << name << " (line " << line_number << ")" << std::endl;
    return ss.str();
}

std::string LiteralNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Literal: " << value << " (" << type << ") (line " << line_number << ")" << std::endl;
    return ss.str();
}

std::string ListNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "List (line " << line_number << ")" << std::endl;
    for (const auto& el : elements) {
        ss << el->toString(indent + 1);
    }
    return ss.str();
}

std::string DictNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Dictionary (line " << line_number << ")" << std::endl;
    for (const auto& item : items) {
        ss << getIndentation(indent + 1) << "Key:" << std::endl;
        ss << item.first->toString(indent + 2);
        ss << getIndentation(indent + 1) << "Value:" << std::endl;
        ss << item.second->toString(indent + 2);
    }
    return ss.str();
}

// std::string ParamListNode::toString(int indent) const {
//     std::stringstream ss;
//     ss << getIndentation(indent) << "Parameter List:" << std::endl;
//     for (const auto& param : parameters) {
//         ss << getIndentation(indent + 1) << param << std::endl;
//     }
//     return ss.str();
// }

std::string ParamListNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Parameter List:" << std::endl;
    for (const auto& param : parameters) {
        // Call toString on each IdentifierNode parameter
        ss << param->toString(indent + 1);
    }
    return ss.str();
}

std::string ArgListNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Argument List:" << std::endl;
    for (const auto& arg : arguments) {
        ss << arg->toString(indent + 1);
    }
    return ss.str();
}

// In parser.cpp:
std::string ConditionNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "Condition:" << std::endl;
    if (condition) {
        ss << condition->toString(indent + 1);
    }
    return ss.str();
}

std::string ErrorNode::toString(int indent) const {
    std::stringstream ss;
    ss << getIndentation(indent) << "ERROR (line " << line_number << "): " << message << std::endl;
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

bool Parser::match(const std::string& lexeme) {
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

bool Parser::check(const std::string& lexeme) {
    if (isAtEnd()) return false;
    return current_token->lexeme == lexeme;
}

bool Parser::isAtEnd() {
    return current_token == lexer.tokens.end() || current_token->type == END_OF_FILE;
}

bool Parser::isAssignmentOperator(const std::string &lexeme) {
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


void Parser::error(const std::string &message, int line, int column)
{
    std::string error_msg = "Syntax Error at line " + std::to_string(line) + ", column "
                            + std::to_string(column) + ": " + message;
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

std::shared_ptr<ProgramNode> Parser::parse() {
    return parseProgram();
}

std::shared_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_shared<ProgramNode>();

    try {
        auto statements = parseStatementList();
        program->statements = statements->statements;
    } catch (const std::exception& e) {
        error(e.what(), peek().line_number, peek().column_number);
        synchronize();
    }

    return program;
}

std::shared_ptr<StatementListNode> Parser::parseStatementList() {
    auto statement_list = std::make_shared<StatementListNode>();

    while (!isAtEnd() && peek().type != DEDENT) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                statement_list->statements.push_back(stmt);
            }
        } catch (const std::exception& e) {
            error(e.what(), peek().line_number, peek().column_number);
            synchronize();
        }
    }

    return statement_list;
}

std::shared_ptr<ASTNode> Parser::parseStatement() {
    std::shared_ptr<ASTNode> statement;

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

std::shared_ptr<AssignmentNode> Parser::parseAssignment(std::shared_ptr<ASTNode> target) {
    Token op = peek();
    consume(); // Consume = operator

    auto value = parseExpression();
    // you’ll need an AssignmentNode constructor that also stores op.lexeme
    return std::make_shared<AssignmentNode>(target,
                                            value,
                                            op.lexeme,
                                            op.line_number,
                                            op.column_number);
}

std::shared_ptr<IfNode> Parser::parseIfStatement() {
    Token if_token = consume(); // Consume 'if'

    auto condition = parseExpression();

    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after if condition", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in if statement");
    }
    consume();

    auto if_block = parseBlock();
    auto if_node = std::make_shared<IfNode>(condition, if_block, if_token.line_number, if_token.column_number);

    // Handle any number of elif clauses
    while (check(KEYWORD) && peek().lexeme == "elif") {
        if_node->elif_clauses.push_back(parseElifClause());
    }

    // Handle optional else clause
    if (check(KEYWORD) && peek().lexeme == "else") {
        if_node->else_block = parseElseClause();
    }

    return if_node;
}

std::shared_ptr<ElifNode> Parser::parseElifClause() {
    Token elif_token = consume(); // Consume 'elif'

    auto condition = parseExpression();

    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after if condition", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in if statement");
    }
    consume();

    auto block = parseBlock();
    return std::make_shared<ElifNode>(condition, block, elif_token.line_number, elif_token.column_number);
}

std::shared_ptr<ElseNode> Parser::parseElseClause() {
    Token else_token = consume(); // Consume 'else'

    // Check specifically for the colon
    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after else", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in else clause");
    }
    consume(); // Consume the colon - this line is essential

    auto block = parseBlock();
    return std::make_shared<ElseNode>(block, else_token.line_number, else_token.column_number);
}

std::shared_ptr<WhileNode> Parser::parseWhileStatement() {
    Token while_token = consume(); // Consume 'while'

    auto condition = parseExpression();

    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after if condition", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in if statement");
    }
    consume();

    auto block = parseBlock();
    return std::make_shared<WhileNode>(condition, block, while_token.line_number, while_token.column_number);
}

std::shared_ptr<ForNode> Parser::parseForStatement() {
    Token for_token = consume(); // Consume 'for'

    auto target = parseExpression();

    if (!check(KEYWORD) || peek().lexeme != "in") {
        error("Expected 'in' keyword in for loop", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in for statement");
    }
    consume(); // Consume 'in'

    auto iterable = parseExpression();

    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after if condition", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in if statement");
    }
    consume();

    auto block = parseBlock();
    return std::make_shared<ForNode>(target, iterable, block, for_token.line_number, for_token.column_number);
}

// std::shared_ptr<FunctionDefNode> Parser::parseFunctionDef() {
//     Token def_token = consume(); // Consume 'def'
//
//     if (!check(IDENTIFIER) && !check(FUNCTION_IDENTIFIER)) {
//         error("Expected function name after 'def'", peek().line_number, peek().column_number);
//         throw std::runtime_error("Syntax error in function definition");
//     }
//
//     std::string name = consume().lexeme;
//
//     if (!match(LPAREN)) {
//         error("Expected '(' after function name", peek().line_number, peek().column_number);
//         throw std::runtime_error("Syntax error in function definition");
//     }
//
//     auto params = parseParameters();
//
//     if (!match(RPAREN)) {
//         error("Expected ')' after function parameters", peek().line_number, peek().column_number);
//         throw std::runtime_error("Syntax error in function definition");
//     }
//
//     if (!check(SYMBOL) || peek().lexeme != ":") {
//         error("Expected ':' after if condition", peek().line_number, peek().column_number);
//         throw std::runtime_error("Syntax error in if statement");
//     }
//     consume();
//
//     auto body = parseBlock();
//     return std::make_shared<FunctionDefNode>(name, params, body, def_token.line_number, def_token.column_number);
// }
std::shared_ptr<FunctionDefNode> Parser::parseFunctionDef() {
    Token def_token = consume(); // Consume 'def'

    // Check for function name
    if (!check(IDENTIFIER) && !check(FUNCTION_IDENTIFIER)) {
        error("Expected function name after 'def'", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in function definition");
    }
    std::string name = consume().lexeme;

    // Check for opening parenthesis
    if (!check(LPAREN)) {
        error("Expected '(' after function name", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in function definition");
    }
    consume(); // Consume '('

    // Parse parameter list
    auto params = parseParameters();

    // Check for closing parenthesis
    if (!check(RPAREN)) {
        error("Expected ')' after function parameters", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in function definition");
    }
    consume(); // Consume ')'

    // Check for colon
    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after function parameters", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in function definition");
    }
    consume(); // Consume ':'

    // Parse function body
    auto body = parseBlock();
    return std::make_shared<FunctionDefNode>(name, params, body, def_token.line_number, def_token.column_number);
}

std::shared_ptr<ReturnNode> Parser::parseReturnStatement() {
    Token return_token = consume(); // Consume 'return'

    // Return can be with or without an expression
    if (peek().type == SYMBOL && peek().lexeme == ":") {
        return std::make_shared<ReturnNode>(nullptr, return_token.line_number, return_token.column_number);
    }

    auto expr = parseExpression();
    return std::make_shared<ReturnNode>(expr, return_token.line_number, return_token.column_number);
}

std::shared_ptr<ImportNode> Parser::parseImportStatement() {
    Token import_token = consume(); // Consume 'import'

    if (!check(IDENTIFIER)) {
        error("Expected module name after 'import'", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in import statement");
    }

    std::string module = consume().lexeme;
    std::string alias;

    // Handle "import x as y" syntax
    if (check(KEYWORD) && peek().lexeme == "as") {
        consume(); // Consume 'as'

        if (!check(IDENTIFIER)) {
            error("Expected alias name after 'as'", peek().line_number, peek().column_number);
            throw std::runtime_error("Syntax error in import statement");
        }

        alias = consume().lexeme;
    }

    return std::make_shared<ImportNode>(module, alias, import_token.line_number, import_token.column_number);
}

std::shared_ptr<BlockNode> Parser::parseBlock() {
    auto block = std::make_shared<BlockNode>();

    // A block must start with an indent
    if (!match(INDENT)) {
        error("Expected indented block", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in code block");
    }

    block->statements = parseStatementList();

    // A block must end with a dedent (unless we're at EOF)
    if (!match(DEDENT) && !isAtEnd()) {
        error("Expected dedent at end of block", peek().line_number, peek().column_number);
    }

    return block;
}

std::shared_ptr<ASTNode> Parser::parseExpression() {
    return parseOrExpr();
}

std::shared_ptr<ASTNode> Parser::parseOrExpr() {
    auto left = parseAndExpr();

    while (check(KEYWORD) && peek().lexeme == "or") {
        Token op = consume();
        auto right = parseAndExpr();
        left = std::make_shared<BinaryExprNode>("or", left, right, op.line_number, op.column_number);
    }

    return left;
}

std::shared_ptr<ASTNode> Parser::parseAndExpr() {
    auto left = parseNotExpr();

    while (check(KEYWORD) && peek().lexeme == "and") {
        Token op = consume();
        auto right = parseNotExpr();
        left = std::make_shared<BinaryExprNode>("and", left, right, op.line_number, op.column_number);
    }

    return left;
}

// std::shared_ptr<ASTNode> Parser::parseNotExpr() {
//     if (check(KEYWORD) && peek().lexeme == "not") {
//         Token op = consume(); // Consume 'not'
//
//         // Parse the operand at the comparison level
//         auto operand = parseComparisonExpr();
//
//         return std::make_shared<UnaryExprNode>("not", operand, op.line_number, op.column_number);
//     }
//
//     return parseComparisonExpr();
// }

std::shared_ptr<ASTNode> Parser::parseNotExpr() {
    // Special handling for 'not' keyword
    if (check(KEYWORD) && peek().lexeme == "not") {
        Token op = consume(); // Consume 'not'

        // DEBUG
        // std::cout << "Processing NOT operator at line " << op.line_number << ", column " << op.column_number << std::endl;

        // Parse the expression that follows the 'not'
        auto operand = parseComparisonExpr();

        return std::make_shared<UnaryExprNode>("not", operand, op.line_number, op.column_number);
    }

    return parseComparisonExpr();
}

std::shared_ptr<ASTNode> Parser::parseComparisonExpr() {
    auto left = parseArithmeticExpr();

    // Handle comparison operators: ==, !=, <, >, <=, >=
    if (check(OPERATOR) && (
        peek().lexeme == "==" || peek().lexeme == "!=" ||
        peek().lexeme == "<" || peek().lexeme == ">" ||
        peek().lexeme == "<=" || peek().lexeme == ">=")) {

        Token op = consume();
        auto right = parseArithmeticExpr();
        left = std::make_shared<BinaryExprNode>(op.lexeme, left, right, op.line_number, op.column_number);
    }

    return left;
}

std::shared_ptr<ASTNode> Parser::parseArithmeticExpr() {
    auto left = parseTerm();

    // Handle addition and subtraction
    while (check(OPERATOR) && (peek().lexeme == "+" || peek().lexeme == "-")) {
        Token op = consume();
        auto right = parseTerm();
        left = std::make_shared<BinaryExprNode>(op.lexeme, left, right, op.line_number, op.column_number);
    }

    return left;
}

std::shared_ptr<ASTNode> Parser::parseTerm() {
    auto left = parseFactor();

    // Handle multiplication, division, and modulo
    while (check(OPERATOR) && (
        peek().lexeme == "*" || peek().lexeme == "/" || peek().lexeme == "%")) {

        Token op = consume();
        auto right = parseFactor();
        left = std::make_shared<BinaryExprNode>(op.lexeme, left, right, op.line_number, op.column_number);
    }

    return left;
}

std::shared_ptr<ASTNode> Parser::parseFactor() {
    auto left = parsePower();

    // Handle exponentiation
    if (check(OPERATOR) && peek().lexeme == "**") {
        Token op = consume();
        auto right = parseFactor(); // Exponentiation is right-associative
        left = std::make_shared<BinaryExprNode>(op.lexeme, left, right, op.line_number, op.column_number);
    }

    return left;
}

std::shared_ptr<ASTNode> Parser::parsePower() {
    return parseUnary();
}

std::shared_ptr<ASTNode> Parser::parseUnary() {
    // Handle unary operators: +, -
    if (check(OPERATOR) && (peek().lexeme == "+" || peek().lexeme == "-")) {
        Token op = consume();
        auto operand = parseUnary();
        return std::make_shared<UnaryExprNode>(op.lexeme, operand, op.line_number, op.column_number);
    }

    return parsePrimary();
}

//remove these two functions later
void Parser::debugToken(const std::string& context) {
    if (isAtEnd()) {
        std::cout << context << ": At end of token stream" << std::endl;
        return;
    }

    std::cout << context << ": Token = " << peek().lexeme
              << ", Type = " << tokenTypeToString(peek().type)
              << ", Line " << peek().line_number
              << ", Column " << peek().column_number << std::endl;
}

std::string Parser::tokenTypeToString(TokenType type) {
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

std::shared_ptr<ASTNode> Parser::parsePrimary() {
    // Add debug at the start to see what token we're starting with
    debugToken("parsePrimary start");

    // Skip any INDENT/DEDENT tokens that appear in expressions
    while (check(INDENT) || check(DEDENT)) {
        std::cout << "Skipping INDENT/DEDENT in primary expression" << std::endl;
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
        auto node = std::make_shared<IdentifierNode>(id.lexeme, id.line_number, id.column_number);

        // Handle function calls: func()
        if (check(LPAREN)) {
            return parseCall(node);
        }
        // Handle parenthesized expressions
        // if (check(LPAREN)) {
        //     consume(); // Consume '('
        //
        //     // This is the key part - parse the full expression
        //     auto expr = parseExpression();
        //
        //     if (!match(RPAREN)) {
        //         error("Expected ')' after expression", peek().line_number, peek().column_number);
        //         throw std::runtime_error("Syntax error in parenthesized expression");
        //     }
        //     consume();
        //     return expr;
        // }

        // NEW: Check for built-in functions that should be called with parentheses
        if (isBuiltInFunction(id.lexeme) && (check(STRING) || check(IDENTIFIER) || check(NUMERIC))) {
            error("Missing parentheses in call to '" + id.lexeme + "'",
                  peek().line_number, peek().column_number);
            throw std::runtime_error("Syntax error in function call");
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
        std::string type = num.lexeme.find('.') != std::string::npos ? "float" : "int";
        return std::make_shared<LiteralNode>(num.lexeme, type, num.line_number, num.column_number);
    }

    // Handle literals: strings
    if (check(STRING)) {
        Token str = consume();
        return std::make_shared<LiteralNode>(str.lexeme, "string", str.line_number, str.column_number);
    }

    // Handle literals: booleans and None
    if (check(DATA_TYPE)) {
        Token data = consume();
        std::string type = (data.lexeme == "True" || data.lexeme == "False") ? "bool" : "None";
        return std::make_shared<LiteralNode>(data.lexeme, type, data.line_number, data.column_number);
    }

    // Handle parenthesized expressions
    if (check(LPAREN)) {
        consume(); // Consume '('
        auto expr = parseExpression();

        if (!match(RPAREN)) {
            error("Expected ')' after expression", peek().line_number, peek().column_number);
            throw std::runtime_error("Syntax error in parenthesized expression");
        }

        return expr;
    }

    // Handle list literals
    if (check(LBRACKET)) {
        return parseListLiteral();
    }

    // // Handle dictionary literals
    // if (check(LBRACE)) {
    //     return parseDictLiteral();
    // }
    // Add final debug before error
    debugToken("No valid expression found");

    error("Expected expression", peek().line_number, peek().column_number);
    throw std::runtime_error("Unexpected token in expression");
}

std::shared_ptr<ASTNode> Parser::parseAttributeReference(std::shared_ptr<ASTNode> object) {
    consume(); // Consume '.'

    // Be more flexible with what we accept as an attribute name
    // Check for both IDENTIFIER and FUNCTION_IDENTIFIER
    if (!check(IDENTIFIER) && !check(FUNCTION_IDENTIFIER)) {
        error("Expected attribute name after '.'", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in attribute reference");
    }

    Token attr = consume();
    auto attr_ref = std::make_shared<AttrRefNode>(object, attr.lexeme, attr.line_number, attr.column_number);

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

// std::shared_ptr<ASTNode> Parser::parseAttributeReference(std::shared_ptr<ASTNode> object) {
//     consume(); // Consume '.'
//
//     // Be more flexible with what we accept as an attribute name
//     // Check for both IDENTIFIER and FUNCTION_IDENTIFIER
//     if (!check(IDENTIFIER) && !check(FUNCTION_IDENTIFIER)) {
//         error("Expected attribute name after '.'", peek().line_number, peek().column_number);
//         throw std::runtime_error("Syntax error in attribute reference");
//     }
//
//     Token attr = consume();
//     auto attr_ref = std::make_shared<AttrRefNode>(object, attr.lexeme, attr.line_number, attr.column_number);
//
//     // Handle chained attribute access: obj.attr1.attr2
//     if (check(OPERATOR) && peek().lexeme == ".") {
//         return parseAttributeReference(attr_ref);
//     }
//
//     // Handle method calls: obj.method()
//     if (check(LPAREN)) {
//         return parseCall(attr_ref);
//     }
//
//     return attr_ref;
// }

// std::shared_ptr<ASTNode> Parser::parseAttributeReference(std::shared_ptr<ASTNode> object) {
//     consume(); // Consume '.'
//
//     if (!check(IDENTIFIER)) {
//         error("Expected attribute name after '.'", peek().line_number, peek().column_number);
//         throw std::runtime_error("Syntax error in attribute reference");
//     }
//
//     Token attr = consume();
//     auto attr_ref = std::make_shared<AttrRefNode>(object, attr.lexeme, attr.line_number, attr.column_number);
//
//     // Handle chained attribute access: obj.attr1.attr2
//     if (check(OPERATOR) && peek().lexeme == ".") {
//         return parseAttributeReference(attr_ref);
//     }
//
//     // Handle method calls: obj.method()
//     if (check(LPAREN)) {
//         return parseCall(attr_ref);
//     }
//
//     return attr_ref;
// }

std::shared_ptr<ASTNode> Parser::parseSubscript(std::shared_ptr<ASTNode> container) {
    Token bracket = consume(); // Consume '['

    auto index = parseExpression();

    if (!match(RBRACKET)) {
        error("Expected ']' after subscript index", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in subscript expression");
    }

    auto subscript = std::make_shared<SubscriptExprNode>(container, index, bracket.line_number, bracket.column_number);

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

// std::shared_ptr<ASTNode> Parser::parseCall(std::shared_ptr<ASTNode> function) {
//     Token paren = consume(); // Consume '('
//
//     auto args = parseArguments();
//
//     if (!match(RPAREN)) {
//         error("Expected ')' after function arguments", peek().line_number, peek().column_number);
//         throw std::runtime_error("Syntax error in function call");
//     }
//
//     auto call = std::make_shared<CallExprNode>(function, args, paren.line_number, paren.column_number);
//
//     // Handle method chaining: obj.method1().method2()
//     if (check(OPERATOR) && peek().lexeme == ".") {
//         return parseAttributeReference(call);
//     }
//
//     // Handle subscripting after call: func()[i]
//     if (check(LBRACKET)) {
//         return parseSubscript(call);
//     }
//
//     return call;
// }
std::shared_ptr<ASTNode> Parser::parseCall(std::shared_ptr<ASTNode> function) {
    Token paren = consume(); // Consume '('

    // If there are no arguments (empty parentheses)
    auto args = std::make_shared<ArgListNode>();

    if (!check(RPAREN)) {
        // Parse the arguments
        args->arguments.push_back(parseExpression());

        // Parse additional arguments if there are commas
        while (check(SYMBOL) && peek().lexeme == ",") {
            consume(); // Consume the comma
            args->arguments.push_back(parseExpression());
        }
    }

    // Check for the closing parenthesis
    if (!check(RPAREN)) {
        error("Expected ')' after function arguments", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in function call");
    }

    consume(); // Consume ')'

    return std::make_shared<CallExprNode>(function, args, paren.line_number, paren.column_number);
}

// std::shared_ptr<ArgListNode> Parser::parseArguments() {
//     auto arg_list = std::make_shared<ArgListNode>();
//
//     if (!check(RPAREN)) {
//         // Parse first argument
//         arg_list->arguments.push_back(parseExpression());
//
//         // Parse remaining arguments
//         while (match(SYMBOL) && peek().lexeme == ",") {
//             arg_list->arguments.push_back(parseExpression());
//         }
//     }
//
//     return arg_list;
// }

std::shared_ptr<ArgListNode> Parser::parseArguments() {
    auto arg_list = std::make_shared<ArgListNode>();

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

// std::shared_ptr<ParamListNode> Parser::parseParameters() {
//     auto param_list = std::make_shared<ParamListNode>();
//
//     // If we're not immediately at the closing parenthesis, then parse parameters
//     if (!check(RPAREN)) {
//         // Parse first parameter
//         if (!check(IDENTIFIER)) {
//             error("Expected parameter name", peek().line_number, peek().column_number);
//             throw std::runtime_error("Syntax error in parameter list");
//         }
//
//         // Add first parameter
//         param_list->parameters.push_back(consume().lexeme);
//
//         // Parse remaining parameters
//         while (check(SYMBOL) && peek().lexeme == ",") {
//             consume(); // Explicitly consume the comma
//
//             if (!check(IDENTIFIER)) {
//                 error("Expected parameter name after comma", peek().line_number, peek().column_number);
//                 throw std::runtime_error("Syntax error in parameter list");
//             }
//
//             param_list->parameters.push_back(consume().lexeme);
//         }
//     }
//
//     return param_list;
// }

std::shared_ptr<ParamListNode> Parser::parseParameters() {
    auto param_list = std::make_shared<ParamListNode>();

    // If we're not immediately at the closing parenthesis, then parse parameters
    if (!check(RPAREN)) {
        // Parse first parameter
        if (!check(IDENTIFIER)) {
            error("Expected parameter name", peek().line_number, peek().column_number);
            throw std::runtime_error("Syntax error in parameter list");
        }

        // Create an IdentifierNode for the parameter
        Token param_token = consume();
        auto param = std::make_shared<IdentifierNode>(param_token.lexeme,
                                            param_token.line_number,
                                            param_token.column_number);

        // Check for default value (=)
        if (check(OPERATOR) && peek().lexeme == "=") {
            consume(); // Consume the equals sign

            // Parse the default value expression and ignore it for now
            // We're just recognizing it syntactically without storing it in the AST
            parseExpression();
        }

        param_list->parameters.push_back(param);

        // Parse remaining parameters
        while (check(SYMBOL) && peek().lexeme == ",") {
            consume(); // Explicitly consume the comma

            if (!check(IDENTIFIER)) {
                error("Expected parameter name after comma", peek().line_number, peek().column_number);
                throw std::runtime_error("Syntax error in parameter list");
            }

            // Create an IdentifierNode for each parameter
            param_token = consume();
            param = std::make_shared<IdentifierNode>(param_token.lexeme,
                                                param_token.line_number,
                                                param_token.column_number);

            // Check for default value (=)
            if (check(OPERATOR) && peek().lexeme == "=") {
                consume(); // Consume the equals sign

                // Parse the default value expression and ignore it for now
                parseExpression();
            }

            param_list->parameters.push_back(param);
        }
    }

    return param_list;
}

// std::shared_ptr<ListNode> Parser::parseListLiteral() {
//     Token bracket = consume(); // Consume '['
//     auto list = std::make_shared<ListNode>(bracket.line_number, bracket.column_number);
//
//     if (!check(RBRACKET)) {
//         // Parse first element
//         list->elements.push_back(parseExpression());
//
//         // Parse remaining elements
//         while (check(SYMBOL) && peek().lexeme == ",") {
//             consume(); // Explicitly consume the comma
//
//             // If there's a trailing comma followed by closing bracket, break
//             if (check(RBRACKET)) {
//                 break;
//             }
//
//             list->elements.push_back(parseExpression());
//         }
//     }
//
//     if (!match(RBRACKET)) {
//         error("Expected ']' after list elements", peek().line_number, peek().column_number);
//         throw std::runtime_error("Syntax error in list literal");
//     }
//
//     return list;
// }
std::shared_ptr<ListNode> Parser::parseListLiteral() {
    Token bracket = consume(); // Consume '['
    auto list = std::make_shared<ListNode>(bracket.line_number, bracket.column_number);

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
                consume(); // Consume the comma

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
                throw std::runtime_error("Syntax error in list literal");
            }
        }
    }

    if (!check(RBRACKET)) {
        error("Expected ']' after list elements", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in list literal");
    }
    consume(); // Consume the closing bracket

    return list;
}

std::shared_ptr<DictNode> Parser::parseDictLiteral() {
    std::cout << "Entering parseDictLiteral with token: " << peek().lexeme
              << " at line " << peek().line_number
              << ", column " << peek().column_number << std::endl;

    Token brace = consume(); // Consume '{'
    auto dict = std::make_shared<DictNode>(brace.line_number, brace.column_number);

    // Skip any INDENT tokens that appear within the dictionary
    while (check(INDENT)) {
        std::cout << "Skipping INDENT in dictionary" << std::endl;
        consume(); // Skip the INDENT token
    }

    // Check if we have an empty dictionary {}
    if (check(RBRACE)) {
        consume(); // Consume the closing brace
        return dict;
    }

    // Parse first key-value pair
    auto key = parseExpression();

    // Skip any DEDENT tokens that might appear between key and colon
    while (check(DEDENT)) {
        std::cout << "Skipping DEDENT after key" << std::endl;
        consume();
    }

    if (!check(SYMBOL) || peek().lexeme != ":") {
        error("Expected ':' after dictionary key", peek().line_number, peek().column_number);
        throw std::runtime_error("Syntax error in dictionary literal");
    }
    consume(); // Consume the colon

    auto value = parseExpression();
    dict->items.emplace_back(key, value);

    // Parse remaining key-value pairs
    while (true) {
        // Skip any INDENT/DEDENT tokens between items
        while (check(INDENT) || check(DEDENT)) {
            std::cout << "Skipping INDENT/DEDENT between dict items" << std::endl;
            consume();
        }

        // Check for comma to continue or closing brace to end
        if (check(SYMBOL) && peek().lexeme == ",") {
            consume(); // Consume the comma

            // Skip any INDENT/DEDENT that might follow the comma
            while (check(INDENT) || check(DEDENT)) {
                std::cout << "Skipping INDENT/DEDENT after comma" << std::endl;
                consume();
            }

            // Handle trailing comma
            if (check(RBRACE)) {
                break;
            }

            key = parseExpression();

            // Skip any INDENT/DEDENT between key and colon
            while (check(INDENT) || check(DEDENT)) {
                std::cout << "Skipping INDENT/DEDENT between key and colon" << std::endl;
                consume();
            }

            if (!check(SYMBOL) || peek().lexeme != ":") {
                error("Expected ':' after dictionary key", peek().line_number, peek().column_number);
                throw std::runtime_error("Syntax error in dictionary literal");
            }
            consume(); // Consume the colon

            value = parseExpression();
            dict->items.emplace_back(key, value);
        } else if (check(RBRACE)) {
            break; // End of dictionary
        } else {
            error("Expected ',' or '}' after dictionary item", peek().line_number, peek().column_number);
            throw std::runtime_error("Syntax error in dictionary literal");
        }
    }

    consume(); // Consume the closing brace
    return dict;
}
// std::shared_ptr<DictNode> Parser::parseDictLiteral() {
//     // Add debug output to trace what's happening
//     std::cout << "Entering parseDictLiteral with token: " << peek().lexeme
//               << " at line " << peek().line_number
//               << ", column " << peek().column_number << std::endl;
//
//     Token brace = consume(); // Consume '{'
//     auto dict = std::make_shared<DictNode>(brace.line_number, brace.column_number);
//
//     // Check if we have an empty dictionary {}
//     if (check(RBRACE)) {
//         consume(); // Consume the closing brace
//         return dict;
//     }
//
//     // Parse first key-value pair
//     auto key = parseExpression();
//
//     if (!check(SYMBOL) || peek().lexeme != ":") {
//         error("Expected ':' after dictionary key", peek().line_number, peek().column_number);
//         throw std::runtime_error("Syntax error in dictionary literal");
//     }
//     consume(); // Consume the colon
//
//     auto value = parseExpression();
//     dict->items.emplace_back(key, value);
//
//     // Parse remaining key-value pairs
//     while (check(SYMBOL) && peek().lexeme == ",") {
//         consume(); // Consume the comma
//
//         // Handle trailing comma
//         if (check(RBRACE)) {
//             break;
//         }
//
//         key = parseExpression();
//
//         if (!check(SYMBOL) || peek().lexeme != ":") {
//             error("Expected ':' after dictionary key", peek().line_number, peek().column_number);
//             throw std::runtime_error("Syntax error in dictionary literal");
//         }
//         consume(); // Consume the colon
//
//         value = parseExpression();
//         dict->items.emplace_back(key, value);
//     }
//
//     if (!check(RBRACE)) {
//         error("Expected '}' after dictionary items", peek().line_number, peek().column_number);
//         throw std::runtime_error("Syntax error in dictionary literal");
//     }
//     consume(); // Consume the closing brace
//
//     return dict;
// }

void Parser::printParseTree(std::shared_ptr<ASTNode> root, int indent) {
    std::cout << root->toString(indent);
}

void Parser::printErrors() {
    if (!errors.empty()) {
        std::cout << "\n--- Parse Errors ---\n";
        for (const auto& error : errors) {
            std::cout << error << std::endl;
        }
        std::cout << "Total errors: " << errors.size() << std::endl;
    }
}

bool Parser::isBuiltInFunction(const std::string& name) {
    // List of common built-in functions in Python
    static const std::vector<std::string> builtins = {"print", "input",  "len",      "range",
                                                      "str",   "int",    "float",    "list",
                                                      "dict",  "set",    "tuple",    "sum",
                                                      "min",   "max",    "abs",      "all",
                                                      "any",   "sorted", "reversed", "enumerate",
                                                      "zip",   "open",   "type"};

    return std::find(builtins.begin(), builtins.end(), name) != builtins.end();
}



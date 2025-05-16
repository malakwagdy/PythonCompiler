// lexer.h

#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#include <cctype>
#include <sstream>
#include <stack>
#include <iomanip>
#include <utility>
#include <map>
#include <algorithm>

// =====================
// Token Types
// =====================
enum TokenType {
    KEYWORD, IDENTIFIER, FUNCTION_IDENTIFIER, DATA_TYPE, OPERATOR,
    NUMERIC, STRING, LPAREN, RPAREN, LBRACKET, RBRACKET, LBRACE, RBRACE,
    SYMBOL, STATEMENT, INDENT, DEDENT, ERROR, END_OF_FILE
};

// =====================
// Token Structure
// =====================
struct Token {
    std::string lexeme;
    TokenType type;
    int line_number;
    int column_number;

    Token(std::string lexeme, TokenType type, int line_number, int column_number)
        : lexeme(std::move(lexeme)), type(type), line_number(line_number), column_number(column_number) {}
};

// =====================
// Lexer Class
// =====================
class Lexer {
public:
    Lexer(); // Constructor

    // Public methods
    void tokenize(const std::string& source_code);
    std::vector<Token> getTokens() const { return tokens; }
    std::vector<std::pair<std::string, std::pair<std::string, int>>> getSymbolTable() const { return symbol_table; }
    std::string getTokenTypeName(TokenType type);
    const std::map<std::string, std::string>& getOperatorDescriptions();

    void printTokens();
    void printSymbolTable();

    std::unordered_map<std::string, TokenType> keywords;
    std::vector<Token> tokens;
    std::vector<Token> buffer;
    std::vector<std::pair<std::string, std::pair<std::string, int>>> symbol_table;
    std::unordered_map<std::string, bool> symbol_presence;
    std::stack<int> indentation_levels;
    int line_number;

    // Helper methods
    bool isInteger(const std::string& str);
    bool isFloat(const std::string& str);
    bool isIdentifier(const std::string& str);
    bool isOperator(const std::string& str);
    bool isSymbol(const std::string& str);
    bool isIdentifierPosition(const std::string& token);

    void addToSymbolTable(const std::string& name, const std::string& type);
    void handleIndentation(const std::string& line);
    void analyzeBuffer();
    void tokenizeLine(const std::string& line);
    void tokenizeWord(const std::string& word, int start_column);
};

#endif // LEXER_H

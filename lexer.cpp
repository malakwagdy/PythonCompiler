//lexer.cpp

#include "lexer.h"
#include <algorithm> // For std::all_of
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map> // Added for operator descriptions map
#include <regex>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility> // Required for std::move
#include <vector>

using namespace std;

// Constructor implementation
Lexer::Lexer()
    : line_number(1)
{
    // Initialize keywords map
    // keywords = {
    //     {"if", KEYWORD}, {"else", KEYWORD}, {"elif", KEYWORD},
    //     {"while", KEYWORD}, {"for", KEYWORD}, {"def", KEYWORD},
    //     {"return", KEYWORD}, {"True", DATA_TYPE}, {"False", DATA_TYPE},
    //     {"None", DATA_TYPE}, {"in", KEYWORD}, {"import", KEYWORD}
    // };
    keywords = {
        {"if", KEYWORD},
        {"else", KEYWORD},
        {"elif", KEYWORD},
        {"while", KEYWORD},
        {"for", KEYWORD},
        {"def", KEYWORD},
        {"return", KEYWORD},
        {"True", DATA_TYPE},
        {"False", DATA_TYPE},
        {"None", DATA_TYPE},
        {"in", KEYWORD},
        {"import", KEYWORD},
        {"and", KEYWORD},
        {"or", KEYWORD},
        {"not", KEYWORD} // Add these boolean operators
    };

    // Initialize indentation levels
    indentation_levels = std::stack<int>();
}

// --- Helper Functions ---
bool Lexer::isInteger(const string &str)
{
    return regex_match(str, regex(R"(^[+-]?\d+$)"));
}

bool Lexer::isFloat(const string &str)
{
    return regex_match(str, regex(R"(^[+-]?(\d+\.\d*|\.\d+)$)"));
}

bool Lexer::isIdentifier(const string &str)
{
    return regex_match(str, regex("^[a-zA-Z_][a-zA-Z0-9_]*$"));
}

// bool Lexer::isOperator(const string& str) {
//     // Matches arithmetic, comparison, logical, assignment, exponentiation, attribute access
//     return regex_match(str, regex(R"(^([+\-*/%]|\*\*|==|!=|<=|>=|<|>|=|and|or|not|\.)$)"));
// }
bool Lexer::isOperator(const string &str) {
    // match *all* the compound operators, then fall back to any single-char operator
    static const regex op_re(R"(^(\+=|-=|\*=|/=|%=|\*\*=|//=|&=|\|=|\^=|<<=|>>=|!=|==|<=|>=|<<|>>|//|\*\*|[+\-*/%<>=&\|\^~\.])$)");
    return regex_match(str, op_re);
}

bool Lexer::isSymbol(const string &str)
{
    // Matches parentheses, brackets, braces, colon, comma, semicolon
    return regex_match(str, regex(R"([(){}\[\]:;,])"));
}

// --- Symbol Table Management ---
void Lexer::addToSymbolTable(const string &name, const string &type)
{
    // Add identifiers (not keywords or '_') or numbers if not already present
    if (keywords.find(name) == keywords.end() && symbol_presence.find(name) == symbol_presence.end()) {
        symbol_table.emplace_back(name, make_pair(type, line_number));
        symbol_presence[name] = true;
    }
}

// --- Indentation Handling ---
void Lexer::handleIndentation(const string &line)
{
    int indent_level = 0;
    int column = 1;
    // Calculate indentation level based on leading spaces/tabs
    for (char ch : line) {
        if (ch == ' ')
            indent_level++;
        else if (ch == '\t') {
            // Report error for tabs, but try to continue by assuming a width
            cerr << "Lexical Error: Tabs are not allowed for indentation at Line " << line_number
                 << ". Use spaces only.\n";
            tokens.emplace_back("TabError", ERROR, line_number, column);
            indent_level += 8; // Assume tab width (common but arbitrary)
        } else
            break; // Stop at first non-whitespace character
        column++;
    }

    // Determine if the line is blank or only contains a comment after whitespace
    size_t first_char_pos = line.find_first_not_of(" \t");
    bool is_blank_or_comment = (first_char_pos == string::npos
                                || (first_char_pos < line.length() && line[first_char_pos] == '#'));

    // Adjust indentation stack only for logical lines of code
    if (!is_blank_or_comment) {
        if (indentation_levels.empty()) {
            // Safety check, should be initialized with 0
            indentation_levels.push(0);
        }

        if (indent_level > indentation_levels.top()) {
            // Increase indentation level
            indentation_levels.push(indent_level);
            tokens.emplace_back("INDENT", INDENT, line_number, 1);
        } else if (indent_level < indentation_levels.top()) {
            // Decrease indentation level (handle multiple dedents)
            while (!indentation_levels.empty() && indent_level < indentation_levels.top()) {
                indentation_levels.pop();
                // Check for inconsistent dedent (level doesn't match any previous level)
                if (indentation_levels.empty() || indent_level > indentation_levels.top()) {
                    cerr << "Lexical Error at Line " << line_number
                         << ": Unindent does not match any outer indentation level.\n";
                    tokens.emplace_back("DedentError", ERROR, line_number, 1);
                    // Attempt recovery: Push the level found, even if incorrect, to avoid stack issues
                    indentation_levels.push(indent_level);
                    break; // Stop dedenting after error
                }
                tokens.emplace_back("DEDENT", DEDENT, line_number, 1);
            }
            // After dedenting, ensure the final level matches exactly
            if (!indentation_levels.empty() && indent_level != indentation_levels.top()) {
                cerr << "Lexical Error at Line " << line_number
                     << ": Inconsistent indentation level after dedent (final level mismatch).\n";
                tokens.emplace_back("IndentError", ERROR, line_number, 1);
            } else if (indentation_levels.empty() && indent_level != 0) {
                // Should be caught above, but as a safeguard:
                cerr << "Lexical Error at Line " << line_number
                     << ": Unindent error (level mismatch with base 0).\n";
                tokens.emplace_back("DedentError", ERROR, line_number, 1);
                indentation_levels.push(indent_level); // Recover by pushing level
            }
        }
        // If indent_level == indentation_levels.top(), indentation is consistent, do nothing.
    }
}

// --- Buffer Analysis (for lookahead) ---
void Lexer::analyzeBuffer()
{
    for (size_t i = 0; i < buffer.size(); ++i) {
        Token current = buffer[i];

        // Detect if a keyword is misused as an identifier (e.g., `if = 5`)
        if ((current.type == KEYWORD||current.type==DATA_TYPE) && i + 1 < buffer.size() && buffer[i + 1].type == OPERATOR
            && buffer[i + 1].lexeme == "=") {
            // Mark current as ERROR and skip assignment
            current.type = ERROR;
            cerr << "Lexical Error at Line " << current.line_number << ", Column "
                 << current.column_number << ": Reserved keyword '" << current.lexeme
                 << "' cannot be used as an identifier\n";

            tokens.emplace_back(current);
            continue; // Still emit '=' in next iteration
        }

        // Detect function identifiers
        if (current.type == IDENTIFIER && i + 1 < buffer.size() && buffer[i + 1].lexeme == "(") {
            current.type = FUNCTION_IDENTIFIER;
        }

        tokens.emplace_back(current);
    }

    buffer.clear();
}

// --- Line Tokenization ---
void Lexer::tokenizeLine(const string &line)
{
    string current_token;
    bool in_string = false;
    char string_delim = '\0';
    bool in_number = false; // New flag to track number parsing

    // List of multi-character operators
    vector<string> multi_char_operators = {
        "!=", "==", "<=", ">=",     // comparisons
        "**", "//",                 // power & floor-div
        "**=", "//=",               // compound versions
        "+=", "-=", "*=", "/=", "%=",  // arithmetic assignment
        "<<", ">>",                 // bit-shifts
        "<<=", ">>=",               // bit-shift assignment
        "&=", "|=", "^=",           // bitwise assignment
    };

    // Find start of actual content (skip leading whitespace)
    size_t first_char_pos = line.find_first_not_of(" \t");
    if (first_char_pos == string::npos)
        return;
    int start_column = first_char_pos + 1;

    string content_part = line.substr(first_char_pos);
    int current_token_start_col = start_column;

    for (size_t i = 0; i < content_part.size(); ++i) {
        char ch = content_part[i];
        int absolute_col = first_char_pos + i + 1;

        if (in_string) {
            current_token += ch;
            // Check for end of string literal, handling basic escapes
            if (ch == string_delim) {
                bool escaped = false;
                if (current_token.length() >= 2
                    && current_token[current_token.length() - 2] == '\\') {
                    // Check for double escape like \\"
                    if (current_token.length() >= 3
                        && current_token[current_token.length() - 3] == '\\') {
                        escaped = false; // \\" is not an escaped quote
                    } else {
                        escaped = true; // \" is an escaped quote
                    }
                }
                if (!escaped) {
                    // End of string found
                    buffer.emplace_back(current_token, STRING, line_number, current_token_start_col);
                    current_token.clear();
                    in_string = false;
                    current_token_start_col = absolute_col + 1; // Next token starts after quote
                }
            }
        } else {
            // Check for multi-character operators
            bool is_multi_char_op = false;
            for (const string &op : multi_char_operators) {
                if (content_part.substr(i, op.size()) == op) {
                    if (!current_token.empty()) {
                        tokenizeWord(current_token, current_token_start_col);
                        current_token.clear();
                    }
                    buffer.emplace_back(op, OPERATOR, line_number, absolute_col);
                    i += op.size() - 1; // Skip the length of the operator
                    is_multi_char_op = true;
                    break;
                }
            }
            if (is_multi_char_op) {
                continue;
            }

            if (ch == '"' || ch == '\'') { // Start of a new string
                // Tokenize any accumulated word before the string starts
                if (!current_token.empty()) {
                    tokenizeWord(current_token, current_token_start_col);
                    current_token.clear();
                }
                // Start the string token
                in_string = true;
                string_delim = ch;
                current_token += ch;
                current_token_start_col = absolute_col; // String starts at this quote
            } else if (isdigit(ch)
                       || (ch == '.' && !current_token.empty() && isdigit(current_token.back()))) {
                // Handle numbers (including decimal points)
                if (current_token.empty() && !isdigit(ch)) {
                    // Handle standalone decimal point or other symbols
                    tokenizeWord(string(1, ch), absolute_col);
                } else {
                    if (current_token.empty()) {
                        current_token_start_col = absolute_col;
                    }
                    current_token += ch;
                }
            } else if (isspace(ch)) {
                if (!current_token.empty()) {
                    tokenizeWord(current_token, current_token_start_col);
                    current_token.clear();
                }
                current_token_start_col = absolute_col + 1;
            } else if (isSymbol(string(1, ch)) || isOperator(string(1, ch))) {
                // Don't split if we're in the middle of a number with decimal point
                if (!current_token.empty() && (ch == '.' && isdigit(current_token.back()))) {
                    current_token += ch;
                } else {
                    if (!current_token.empty()) {
                        tokenizeWord(current_token, current_token_start_col);
                        current_token.clear();
                    }
                    tokenizeWord(string(1, ch), absolute_col);
                    current_token_start_col = absolute_col + 1;
                }
            } else {
                if (current_token.empty()) {
                    current_token_start_col = absolute_col;
                }
                current_token += ch;
            }
        }
    }

    // Handle any remaining token
    if (!current_token.empty()) {
        tokenizeWord(current_token, current_token_start_col);
    }
}

bool Lexer::isIdentifierPosition(const string &token)
{
    // Check current buffer for contextual clues
    if (!buffer.empty()) {
        const Token &last_token = buffer.back();
        // Check if previous token suggests identifier position
        if (last_token.lexeme == "=" || last_token.lexeme == "." || last_token.lexeme == "("
            || last_token.lexeme == "[" || last_token.lexeme == "as" || // Python-specific contexts
            last_token.lexeme == "from" || last_token.lexeme == "import") {
            return true;
        }
    }
    return false;
}

void Lexer::tokenizeWord(const string &word, int start_column)
{
    // === Check for invalid floats ===
    if (regex_match(word, regex(R"(^[+-]?\d*(\.\d+){2,}$)"))) {
        buffer.emplace_back(word, ERROR, line_number, start_column);
        cerr << "Lexical Error at Line " << line_number << ", Column " << start_column
             << ": Invalid float number: '" << word << "'\n";
        return;
    }
    // === Check if the token starts with digits followed by letters (e.g. 123abc) ===
    if (regex_match(word, regex(R"(^\d+[a-zA-Z_][a-zA-Z0-9_]*$)"))) {
        buffer.emplace_back(word, ERROR, line_number, start_column);
        cerr << "Lexical Error at Line " << line_number << ", Column " << start_column
             << ": Identifier cannot start with a digit: '" << word << "'\n";
        return;
    }

    // === Check for invalid characters in identifiers (e.g. @, #, etc.) ===
    if (regex_match(word, regex(R"(^[a-zA-Z_][a-zA-Z0-9_]*[^a-zA-Z0-9_\s]+[a-zA-Z0-9_]*$)"))) {
        buffer.emplace_back(word, ERROR, line_number, start_column);
        cerr << "Lexical Error at Line " << line_number << ", Column " << start_column
             << ": Invalid character in identifier: '" << word << "'\n";
        return;
    }

    // if (word == "_") {
    //     buffer.emplace_back(word, ERROR, line_number, start_column);
    //     cerr << "Lexical Error at Line " << line_number
    //          << ", Column " << start_column
    //          << ": underscore (‘_’) is not allowed as an identifier\n";
    //     return;
    // }

    static const std::regex splitter(
        R"(([-+]?\d*\.\d+|[-+]?\d+\.\d*|\d+|[A-Za-z_][A-Za-z0-9_]*|\+=|-=|\*=|/=|%=|\*\*=|//=|&=|\|=|\^=|<<=|>>=|!=|==|<=|>=|<<|>>|//|\*\*|and|or|not|[+\-*/%<>=&|\^~\.\(\)\{\}\[\]:;,])|.)");

    auto words_begin = sregex_iterator(word.begin(), word.end(), splitter);
    auto words_end = sregex_iterator();

    for (sregex_iterator it = words_begin; it != words_end; ++it) {
        smatch match = *it;
        string token = match.str(0);
        int token_start_col = start_column + match.position(0);

        if (token.empty() || all_of(token.begin(), token.end(), ::isspace)) {
            continue;
        }

        bool preceded_by_numeric_in_buffer = !buffer.empty() && buffer.back().type == NUMERIC;
        // *** NEW: Check if the last token in the line's buffer was an IDENTIFIER ***
        bool preceded_by_identifier_in_buffer = !buffer.empty() && buffer.back().type == IDENTIFIER;


        // === Reserved Keyword ===
        if (keywords.count(token)) {
            if (preceded_by_numeric_in_buffer) {
                bool is_allowed_after_numeric =
                    (token == "and" || token == "or" || token == "not" ||
                     token == "if" || token == "else" || token == "in");
                if (!is_allowed_after_numeric) {
                    buffer.emplace_back(token, ERROR, line_number, token_start_col);
                    cerr << "Lexical Error at Line " << line_number << ", Column " << token_start_col
                         << ": Keyword '" << token << "' cannot directly follow a numeric literal in this context.\n";
                    continue;
                }
            }


            bool is_assignment_context = false;
            bool is_boolean_operator = (token == "and" || token == "or" || token == "not");
            bool is_data_type = (keywords.at(token) == DATA_TYPE);

            if (!buffer.empty()) {
                string prev_lexeme_in_buffer = buffer.back().lexeme;
                TokenType prev_type_in_buffer = buffer.back().type;
                if ( (prev_type_in_buffer == OPERATOR && prev_lexeme_in_buffer == "=") ||
                    (prev_type_in_buffer == OPERATOR && prev_lexeme_in_buffer == ".") ||
                    prev_type_in_buffer == LPAREN ) {
                    if (!is_boolean_operator && !is_data_type) {
                        is_assignment_context = true;
                    // } else if (prev_lexeme_in_buffer == "=" && is_boolean_operator) {
                    //     is_assignment_context = true;
                    } else if ( (prev_lexeme_in_buffer == "." || prev_type_in_buffer == LPAREN) && is_data_type) {
                        if (prev_lexeme_in_buffer == "."){
                            is_assignment_context = true;
                        }
                    }
                }
            }

            if (is_assignment_context) {
                buffer.emplace_back(token, ERROR, line_number, token_start_col);
                cerr << "Lexical Error at Line " << line_number << ", Column " << token_start_col
                     << ": Reserved keyword '" << token << "' cannot be used as an identifier or in this context.\n";
            } else {
                buffer.emplace_back(token, keywords.at(token), line_number, token_start_col);
            }
        }
        // === Operator ===
        else if (isOperator(token)) {
            buffer.emplace_back(token, OPERATOR, line_number, token_start_col);
        }
        // === Symbols ===
        else if (token == "(") buffer.emplace_back(token, LPAREN, line_number, token_start_col);
        else if (token == ")") buffer.emplace_back(token, RPAREN, line_number, token_start_col);
        else if (token == "[") buffer.emplace_back(token, LBRACKET, line_number, token_start_col);
        else if (token == "]") buffer.emplace_back(token, RBRACKET, line_number, token_start_col);
        else if (token == "{") buffer.emplace_back(token, LBRACE, line_number, token_start_col);
        else if (token == "}") buffer.emplace_back(token, RBRACE, line_number, token_start_col);
        else if (token == ":" || token == "," || token == ";") {
            buffer.emplace_back(token, SYMBOL, line_number, token_start_col);
        }
        // === Float ===
        else if (isFloat(token)) {
            if (preceded_by_numeric_in_buffer) {
                buffer.emplace_back(token, ERROR, line_number, token_start_col);
                cerr << "Lexical Error at Line " << line_number << ", Column " << token_start_col
                     << ": Numeric literal '" << token << "' cannot directly follow another numeric literal without an operator.\n";
            } else if (preceded_by_identifier_in_buffer) { // e.g. `myVar 3.14`
                buffer.emplace_back(token, ERROR, line_number, token_start_col);
                cerr << "Lexical Error at Line " << line_number << ", Column " << token_start_col
                     << ": Numeric literal '" << token << "' cannot directly follow an identifier ('"
                     << buffer.back().lexeme << "') without an operator or separator.\n";
            }
            else {
                buffer.emplace_back(token, NUMERIC, line_number, token_start_col);
                // addToSymbolTable(token, "Numeric");
            }
        }
        // === Integer ===
        else if (isInteger(token)) {
            if (preceded_by_numeric_in_buffer) {
                buffer.emplace_back(token, ERROR, line_number, token_start_col);
                cerr << "Lexical Error at Line " << line_number << ", Column " << token_start_col
                     << ": Numeric literal '" << token << "' cannot directly follow another numeric literal without an operator.\n";
            } else if (preceded_by_identifier_in_buffer) { // e.g. `myVar 123`
                buffer.emplace_back(token, ERROR, line_number, token_start_col);
                cerr << "Lexical Error at Line " << line_number << ", Column " << token_start_col
                     << ": Numeric literal '" << token << "' cannot directly follow an identifier ('"
                     << buffer.back().lexeme << "') without an operator or separator.\n";
            }
            else {
                buffer.emplace_back(token, NUMERIC, line_number, token_start_col);
                // addToSymbolTable(token, "Numeric");
            }
        }
        // === Valid Identifier ===
        else if (isIdentifier(token)) {
            if (preceded_by_numeric_in_buffer) {
                buffer.emplace_back(token, ERROR, line_number, token_start_col);
                cerr << "Lexical Error at Line " << line_number << ", Column " << token_start_col
                     << ": Identifier '" << token << "' cannot directly follow a numeric literal without an operator.\n";
            }
            // *** THIS IS THE KEY CHANGE FOR "hello world = 1" ***
            else if (preceded_by_identifier_in_buffer) {
                buffer.emplace_back(token, ERROR, line_number, token_start_col);
                cerr << "Lexical Error at Line " << line_number << ", Column " << token_start_col
                     << ": Identifier '" << token << "' cannot directly follow another identifier ('"
                     << buffer.back().lexeme << "') without an operator or separator.\n";
            }
            else {
                buffer.emplace_back(token, IDENTIFIER, line_number, token_start_col);
                addToSymbolTable(token, "Identifier");
            }
        }
        // === Catch-all: unknown/illegal token ===
        else {
            buffer.emplace_back(token, ERROR, line_number, token_start_col);
            cerr << "Lexical Error at Line " << line_number << ", Column " << token_start_col
                 << ": Unknown or invalid token '" << token << "'\n";
        }
    }
}

// --- Get Basic Token Type Name ---
string Lexer::getTokenTypeName(TokenType type)
{
    switch (type) {
    case KEYWORD:
        return "KEYWORD";
    case IDENTIFIER:
        return "IDENTIFIER";
    case FUNCTION_IDENTIFIER:
        return "FUNCTION_IDENTIFIER";
    case DATA_TYPE:
        return "DATA_TYPE";
    case OPERATOR:
        return "OPERATOR";
    case NUMERIC:
        return "NUMERIC";
    case STRING:
        return "STRING";
    case SYMBOL:
        return "SYMBOL";
    case LPAREN:
        return "LPAREN";
    case RPAREN:
        return "RPAREN";
    case LBRACKET:
        return "LBRACKET";
    case RBRACKET:
        return "RBRACKET";
    case LBRACE:
        return "LBRACE";
    case RBRACE:
        return "RBRACE";
    case STATEMENT:
        return "STATEMENT";
    case INDENT:
        return "INDENT";
    case DEDENT:
        return "DEDENT";
    case ERROR:
        return "ERROR";
    case END_OF_FILE:
        return "END_OF_FILE";
    default:
        return "UNKNOWN";
    }
}

// --- Map for Operator Descriptions ---
const map<string, string> &Lexer::getOperatorDescriptions()
{
    // Static map initialized once
    static const map<string, string> descriptions = {
        {"=", "Assignment"},
        {"+", "Addition"},
        {"-", "Subtraction"},
        {"*", "Multiplication"},
        {"/", "Division"},
        {"%", "Modulo"},
        {"**", "Exponentiation"},
        {"==", "Equality"},
        {"!=", "Inequality"},
        {"<", "Less Than"},
        {">", "Greater Than"},
        {"<=", "Less Than/Equal"},
        {">=", "Greater Than/Equal"},
        {"and", "Logical AND"},
        {"or", "Logical OR"},
        {"not", "Logical NOT"},
        {"+=", "Add and assign"},
        {"-=", "Subtract and assign"},
        {"*=", "Multiply and assign"},
        {"/=", "Divide and assign"},
        {"%=", "Modulo and assign"},
        {"**=", "Exponentiation and assign"},
        {"//=", "Floor-division and assign"},
        {"&=", "Bitwise AND assign"},
        {"|=", "Bitwise OR assign"},
        {"^=", "Bitwise XOR assign"},
        {"<<", "Bitwise left shift"},
        {">>", "Bitwise right shift"},
        {"<<=", "Left shift and assign"},
        {">>=", "Right shift and assign"},
        {"//", "Floor division"},
        {".", "Attribute Access"} // Add compound assignment operators if needed: +=, -=, etc.
    };
    return descriptions;
}

// --- Main Tokenization Process ---
void Lexer::tokenize(const string &source_code)
{
    istringstream stream(source_code);
    string line;
    indentation_levels.push(0);        // Start at base indentation level 0
    bool in_multiline_comment = false; // Track if inside a multi-line comment
    string comment_delim = "\0";       // Track the delimiter for multi-line comments

    while (getline(stream, line)) {
        string original_line = line; // Keep for indentation calculation

        // Handle multi-line comments
        if (in_multiline_comment) {
            size_t end_pos = line.find(comment_delim + string(comment_delim) + comment_delim);
            if (end_pos != string::npos) {
                in_multiline_comment = false;    // End of multi-line comment
                line = line.substr(end_pos + 3); // Skip the comment
            } else {
                continue; // Skip the entire line if still inside the comment
            }
        }

        // Handle single-line comments and detect start of multi-line comments
        size_t comment_pos = line.find('#');
        size_t triple_single_pos = line.find("'''");
        size_t triple_double_pos = line.find("\"\"\"");

        if (triple_single_pos != string::npos || triple_double_pos != string::npos) {
            size_t start_pos = (triple_single_pos != string::npos) ? triple_single_pos
                                                                   : triple_double_pos;
            comment_delim = (triple_single_pos != string::npos) ? '\'' : '"';
            size_t end_pos = line.find(comment_delim + string(comment_delim) + comment_delim,
                                       start_pos + 3);

            if (end_pos != string::npos) {
                // Multi-line comment starts and ends on the same line
                line = line.substr(0, start_pos) + line.substr(end_pos + 3);
            } else {
                // Multi-line comment starts but doesn't end
                in_multiline_comment = true;
                line = line.substr(0, start_pos);
            }
        } else if (comment_pos != string::npos) {
            // Handle single-line comments
            line = line.substr(0, comment_pos);
        }

        // Trim trailing whitespace
        size_t last_char = line.find_last_not_of(" \t");
        if (string::npos == last_char)
            line.clear(); // Line is effectively empty
        else
            line = line.substr(0, last_char + 1);

        // Handle indentation
        handleIndentation(original_line);

        // Tokenize line content
        tokenizeLine(line);

        // Analyze buffer
        analyzeBuffer();

        line_number++; // Move to next line number
    }

    // Post-processing for dedents and EOF
    while (!indentation_levels.empty() && indentation_levels.top() > 0) {
        indentation_levels.pop();
        tokens.emplace_back("DEDENT", DEDENT, line_number, 1);
    }
    tokens.emplace_back("EOF", END_OF_FILE, line_number, 1);
}

// --- Output Functions ---
void Lexer::printTokens()
{
    cout << "\n--- Tokens --- \n";
    // Increased width for Type column to accommodate operator descriptions
    cout << left << setw(25) << "Lexeme" << left << setw(35) << "Type" << left << setw(10) << "Line"
         << left << setw(10) << "Column" << "\n";
    cout << string(80, '-') << "\n"; // Adjusted separator width

    const auto &op_descriptions = getOperatorDescriptions(); // Get the description map

    for (const auto &token : tokens) {
        string printable_lexeme = token.lexeme;
        // Basic escaping for common whitespace chars in output
        if (printable_lexeme == "\n")
            printable_lexeme = "\\n";
        else if (printable_lexeme == "\t")
            printable_lexeme = "\\t";

        string type_string;
        if (token.type == OPERATOR) {
            // If it's an operator, look up its description
            auto it = op_descriptions.find(token.lexeme);
            if (it != op_descriptions.end()) {
                type_string = "OPERATOR (" + it->second + ")"; // e.g., OPERATOR (Addition)
            } else {
                // Fallback if operator lexeme isn't in map (shouldn't happen with current regex)
                type_string = "OPERATOR (Unknown: " + token.lexeme + ")";
            }
        } else {
            // For all other types, use the standard name
            type_string = getTokenTypeName(token.type);
        }

        cout << left << setw(25) << ("'" + printable_lexeme + "'") // Show lexeme in quotes
             << left << setw(35) << type_string                    // Show detailed type
             << left << setw(10) << token.line_number << left << setw(10) << token.column_number
             << "\n";
    }
    cout << string(80, '-') << "\n"; // Adjusted separator width
}

void Lexer::printSymbolTable()
{
    cout << "\n--- Symbol Table --- \n";
    cout << left << setw(25) << "Name" << left << setw(20) << "Type" << left << setw(20)
         << "Declared at Line" << "\n";
    cout << string(65, '-') << "\n"; // Separator line
    for (const auto &entry : symbol_table) {
        cout << left << setw(25) << entry.first << left << setw(20)
        << entry.second.first                      // Type (Identifier, Integer, Float)
        << left << setw(20) << entry.second.second // Line number
        << "\n";
    }
    cout << string(65, '-') << "\n";
}

#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void processFile(const std::string& filename) {
    std::string source = readFile(filename);
    if (source.empty()) return;

    std::cout << "Processing file: " << filename << std::endl;

    // Phase 1: Lexical Analysis
    Lexer lexer;
    lexer.tokenize(source);
    lexer.printTokens();
    lexer.printSymbolTable();

    // Phase 2: Syntax Analysis
    Parser parser(lexer);
    auto ast = parser.parse();

    std::cout << "\n--- Parse Tree ---\n";
    parser.printParseTree(ast);

    if (parser.hasError()) {
        parser.printErrors();
    } else {
        std::cout << "\nParsing completed successfully!\n";
    }
}

void interactiveMode() {
    std::string line;
    std::string source;

    std::cout << "Python Parser Interactive Mode (Enter 'exit' to quit, 'run' to parse)\n";
    std::cout << "Enter Python code (multiple lines supported):\n";

    while (true) {
        std::cout << ">> ";
        std::getline(std::cin, line);

        if (line == "exit") {
            break;
        }

        if (line == "run") {
            // Parse the accumulated source code
            Lexer lexer;
            lexer.tokenize(source);
            lexer.printTokens();
            lexer.printSymbolTable();

            // Syntax Analysis
            Parser parser(lexer);
            auto ast = parser.parse();
            std::cout << "\n--- Parse Tree ---\n";
            parser.printParseTree(ast);

            if (parser.hasError()) {
                parser.printErrors();
            } else {
                std::cout << "\nParsing completed successfully!\n";
            }

            // Reset source for next input
            source.clear();
            continue;
        }

        // Add the line to the source code
        source += line + "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        // No arguments, run interactive mode
        interactiveMode();
    } else if (argc == 2) {
        // One argument, process as filename
        processFile(argv[1]);
    } else {
        std::cerr << "Usage: " << argv[0] << " [filename]" << std::endl;
        std::cerr << "Or run without arguments for interactive mode." << std::endl;
        return 1;
    }

    return 0;
}

























// #include "mainwindow.h"
// #include <QApplication>
// #include <QLocale>
// #include <QTranslator>
//
// int main(int argc, char *argv[])
// {
//     QApplication app(argc, argv);
//
//     // Set application info
//     QApplication::setApplicationName("Python Compiler");
//     QApplication::setApplicationVersion("1.0");
//     QApplication::setOrganizationName("Your Organization");
//
//     // Create and show the main window
//     MainWindow mainWindow;
//     mainWindow.show();
//
//     // Enter the event loop
//     return app.exec();
// }

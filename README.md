# Python Compiler

![Project Banner](https://via.placeholder.com/728x150.png?text=Python+Compiler+in+C%2B%2B+and+Qt)

A Python compiler built with C++ and the Qt framework, providing a feature-rich graphical interface to demonstrate the core stages of compilation. This application allows users to write code, run lexical and syntactical analysis, and view detailed outputs like tokens, errors, and the parse tree.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++ Version](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/std/status)
[![Qt Version](https://img.shields.io/badge/Qt-6-green.svg)](https://www.qt.io/)

---

## 📖 Table of Contents

-   [Features](#-features)
-   [Application GUI](#-application-gui)
-   [Architecture](#-architecture)
-   [Getting Started](#-getting-started)
    -   [Prerequisites](#prerequisites)
    -   [Building from Source](#building-from-source)
-   [How to Use](#-how-to-use)
-   [Contributing](#-contributing)
-   [Acknowledgments](#-acknowledgments)

---

## ✨ Features

*   **Integrated Code Editor**: A simple text area on the left to write or paste Python source code.
*   **File Management**: Easily **Open** and **Save** Python source files directly from the UI.
*   **Step-by-Step Compilation**: Execute individual compiler stages:
    *   **Run Lexer**: Performs lexical analysis to generate a stream of tokens from the source code.
    *   **Run Parser**: Performs syntax analysis to build a parse tree and detect syntax errors.
*   **Detailed Output Display**: A clean, tabbed view to inspect the results of the compilation:
    *   **Tokens Tab**: Displays the list of tokens identified by the lexer.
    *   **Symbol Table Tab**: Shows a table of identifiers and symbols found in the code.
    *   **Errors Tab**: Reports any syntax or lexical errors found during compilation.
    *   **Parse Tree Tab**: Provides a representation of the code's syntactic structure.
*   **Clear and Responsive UI**: A user-friendly interface built with Qt that neatly separates code input from compiler output.

---

## 🖥️ Application GUI

The main window is split into two primary sections: the **Source Code Editor** on the left and the **Compiler Output** on the right, which is organized into clear tabs for easy analysis.

![GUI Screenshot](https://raw.githubusercontent.com/malakwagdy/PythonCompiler/main/Screenshot.png)

---

## 🏗️ Architecture

This compiler is designed in distinct stages, which can be run independently through the UI:

1.  **Lexer (Lexical Analyzer)**: The `Run Lexer` function processes the raw source code and breaks it down into a series of tokens. The result is displayed in the **Tokens** tab.

2.  **Parser (Syntax Analyzer)**: The `Run Parser` function takes the list of tokens from the Lexer and builds a Parse Tree according to Python's grammar rules. It also populates the **Parse Tree** and **Errors** tabs.

3.  **Symbol Table**: As the code is processed, a symbol table is constructed to keep track of identifiers (variables, functions, etc.), which can be viewed in its dedicated tab.

4.  **GUI (Graphical User Interface)**: The interface, built with the **Qt Framework**, orchestrates the user interaction, triggers the compiler stages, and displays the results in an organized manner.

---

## 🚀 Getting Started

Follow these instructions to get a copy of the project up and running on your local machine.

### Prerequisites

You will need the following tools installed on your system:

*   **A C++ Compiler**: A modern C++ compiler that supports C++17 (e.g., GCC, Clang, MSVC).
*   **CMake**: Version 3.16 or higher.
*   **Qt Framework**: Qt version 6.2 or higher (installing Qt Creator is recommended). Make sure to install the components for your specific compiler.

### Building from Source

1.  **Clone the repository:**
    ```sh
    git clone https://github.com/malakwagdy/PythonCompiler.git
    cd PythonCompiler
    ```

2.  **Configure the project with CMake:**
    It's best practice to use a separate build directory.
    ```sh
    mkdir build && cd build
    ```
    Now run CMake. You may need to tell it where to find your Qt installation.
    ```sh
    # If Qt is in your system's PATH, this might be enough:
    cmake ..

    # If not, specify the path to Qt:
    cmake .. -DCMAKE_PREFIX_PATH=/path/to/your/Qt/version/platform
    ```

3.  **Build the project:**
    ```sh
    cmake --build .
    ```

4.  **Run the application:**
    The executable will be located in the `build` directory.
    ```sh
    ./PythonCompiler
    ```

---

## 📖 How to Use

1.  Launch the application.
2.  Write Python code in the **Source Code** editor on the left, or open a `.py` file using **File > Open**.
3.  Click **Run Lexer** to see the tokenized output in the **Tokens** tab.
4.  Click **Run Parser** to check for syntax errors and view the resulting **Parse Tree**.
5.  Use the **Symbol Table** and **Errors** tabs to further analyze the code.
6.  Click **Clear Output** to reset the output views for a new run.
7.  You can save your code using **File > Save**.

---

## 🤝 Contributing

Contributions, issues, and feature requests are welcome!

1.  **Fork** the project.
2.  Create your feature branch (`git checkout -b feature/NewFeature`).
3.  **Commit** your changes (`git commit -m 'Add some NewFeature'`).
4.  **Push** to the branch (`git push origin feature/NewFeature`).
5.  Open a **Pull Request**.

---

## 🙏 Acknowledgments

*   A big thanks to my project partners.

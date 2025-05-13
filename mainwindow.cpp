#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Ensure status bar is visible
    statusBar()->setVisible(true);

    // Default message
    statusBar()->showMessage("Ready");

    // Initialize table models
    setupTableModels();

    // Initialize parse tree scene (kept for backward compatibility)
    parseTreeScene = new QGraphicsScene(this);

    // Replace the QGraphicsView with our custom widget
    // First, get the layout of the parseTreeTab
    QLayout* layout = ui->verticalLayoutWidget_6->layout();

    // Remove the existing QGraphicsView if it's in the layout
    if (ui->parseTreeView->parent() == ui->verticalLayoutWidget_6) {
        layout->removeWidget(ui->parseTreeView);
    }

    // Create our custom widget for dragable/zoomable parse tree
    parseTreeWidget = new ParseTreeWidget(ui->verticalLayoutWidget_6);

    // Add it to the layout
    layout->addWidget(parseTreeWidget);

    // Hide the original graphics view
    ui->parseTreeView->hide();

    // Set window title
    setWindowTitle("Python Compiler");

    // Ensure the layout widgets expand properly
    ui->mainContainer->setLayout(ui->horizontalLayout);
    ui->leftPanel->setLayout(ui->verticalLayout_2);
    ui->rightPanel->setLayout(ui->verticalLayout_3);

    // Make error table read-only
    ui->errorTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Connect error table double-click
    connect(ui->errorTableView, &QTableView::doubleClicked, this, &MainWindow::onErrorTableDoubleClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete tokenTableModel;
    delete symbolTableModel;
    delete errorTableModel;
    delete parseTreeScene;
    // Don't delete parseTreeWidget, it's now owned by the layout
}

void MainWindow::setupTableModels()
{
    // Token table setup
    tokenTableModel = new QStandardItemModel(this);
    tokenTableModel->setHorizontalHeaderLabels({"Lexeme", "Type", "Line", "Column"});
    ui->tokenTableView->setModel(tokenTableModel);
    ui->tokenTableView->verticalHeader()->setVisible(false);

    // Symbol table setup
    symbolTableModel = new QStandardItemModel(this);
    symbolTableModel->setHorizontalHeaderLabels({"Name", "Type", "Declared at Line"});
    ui->symbolTableView->setModel(symbolTableModel);

    // Error table setup
    errorTableModel = new QStandardItemModel(this);
    errorTableModel->setHorizontalHeaderLabels({"Error Type", "Message", "Line", "Column"});
    ui->errorTableView->setModel(errorTableModel);
    ui->errorTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->errorTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Configure table views
    ui->tokenTableView->horizontalHeader()->setStretchLastSection(true);
    ui->tokenTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->symbolTableView->horizontalHeader()->setStretchLastSection(true);
    ui->symbolTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->errorTableView->horizontalHeader()->setStretchLastSection(true);
    ui->errorTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::on_actionOpen_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Python File", "", "Python Files (*.py);;All Files (*)");

    if (filePath.isEmpty())
        return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file: " + file.errorString());
        return;
    }

    QTextStream in(&file);
    ui->sourceEditor->setPlainText(in.readAll());
    currentFilePath = filePath;
    file.close();

    setWindowTitle("Python Compiler - " + QFileInfo(filePath).fileName());
    showStatusMessage(QString("File loaded: " + filePath), false);
}

void MainWindow::on_actionSave_triggered()
{
    if (currentFilePath.isEmpty()) {
        on_actionSave_As_triggered();
    } else {
        saveFile(currentFilePath);
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Save Python File", "", "Python Files (*.py);;All Files (*)");

    if (!filePath.isEmpty()) {
        if (saveFile(filePath)) {
            currentFilePath = filePath;
            setWindowTitle("Python Compiler - " + QFileInfo(filePath).fileName());
        }
    }
}

bool MainWindow::saveFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not save file: " + file.errorString());
        return false;
    }

    QTextStream out(&file);
    out << ui->sourceEditor->toPlainText();
    file.close();

    showStatusMessage(QString("File saved: " + filePath), false);
    return true;
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionRun_Lexer_triggered()
{
    // Clear previous outputs
    tokenTableModel->removeRows(0, tokenTableModel->rowCount());
    symbolTableModel->removeRows(0, symbolTableModel->rowCount());
    errorTableModel->removeRows(0, errorTableModel->rowCount());
    parseTreeScene->clear(); // Clear any previous parse trees
    parseTreeWidget->setParseTree(nullptr); // Clear the custom parse tree widget

    QString sourceCode = ui->sourceEditor->toPlainText();
    if (sourceCode.isEmpty()) {
        showStatusMessage("No source code to analyze!", true);
        return;
    }

    // Create and run the lexer
    Lexer lexer;
    try {
        lexer.tokenize(sourceCode.toStdString());
        updateTokenTable(lexer.getTokens());
        updateSymbolTable(lexer.getSymbolTable());
        updateErrorTable(lexer.getTokens());
        highlightErrors(lexer.getTokens()); // Add error highlighting

        // Count errors for status message
        int errorCount = 0;
        for (const auto& token : lexer.getTokens()) {
            if (token.type == ERROR) errorCount++;
        }

        if (errorCount > 0) {
            showStatusMessage(QString("Lexer completed with " + QString::number(errorCount) + " errors"), true);
            ui->outputTabs->setCurrentIndex(2); // Switch to errors tab
        } else {
            showStatusMessage("Lexer completed successfully", false);
            ui->outputTabs->setCurrentIndex(0); // Switch to tokens tab
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Lexer Error", "An exception occurred during lexical analysis: " + QString(e.what()));
        showStatusMessage("Lexer failed with an exception", true);
    }
}

void MainWindow::on_actionRun_Parser_triggered()
{
    // First run the lexer to ensure we have tokens
    on_actionRun_Lexer_triggered();

    // Check if lexer found errors
    int lexerErrorCount = 0;
    for (int i = 0; i < errorTableModel->rowCount(); i++) {
        if (errorTableModel->item(i, 0)->text() == "Lexical Error") {
            lexerErrorCount++;
        }
    }

    // If lexer found errors, don't run the parser
    if (lexerErrorCount > 0) {
        showStatusMessage("Cannot parse: Lexical errors must be fixed first", true);
        return;
    }

    // Clear parse tree and parser errors
    parseTreeScene->clear();
    parseTreeWidget->setParseTree(nullptr);

    // Create the lexer and parser
    Lexer lexer;
    QString sourceCode = ui->sourceEditor->toPlainText();
    lexer.tokenize(sourceCode.toStdString());

    Parser parser(lexer);

    try {
        // Run the parser
        std::shared_ptr<ProgramNode> ast = parser.parse();

        // Handle parser errors
        if (parser.hasError()) {
            updateParserErrorTable(parser.getErrors());
            showStatusMessage("Parser completed with errors", true);
            ui->outputTabs->setCurrentIndex(2); // Switch to errors tab
        } else {
            showStatusMessage("Parser completed successfully", false);
            // Visualize the parse tree
            visualizeParseTree(ast);
            ui->outputTabs->setCurrentIndex(3); // Switch to parse tree tab

            // Make sure the tree is centered properly after resizing
            QTimer::singleShot(100, this, [this]() {
                if (parseTreeWidget) {
                    parseTreeWidget->update();
                }
            });
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Parser Error", "An exception occurred during parsing: " + QString(e.what()));
        showStatusMessage("Parser failed with an exception", true);
    }
}

void MainWindow::updateTokenTable(const std::vector<Token> &tokens)
{
    // Configure the token table with the results
    tokenTableModel->setRowCount(0); // Clear previous data

    // Map for operator descriptions
    const std::map<std::string, std::string>& opDescriptions = Lexer().getOperatorDescriptions();

    for (const auto& token : tokens) {
        QList<QStandardItem*> row;

        // Format the lexeme for display
        QString lexeme = QString::fromStdString(token.lexeme);
        if (lexeme == "\n") lexeme = "\\n";
        if (lexeme == "\t") lexeme = "\\t";

        // Add lexeme
        row.append(new QStandardItem(lexeme));

        // Add token type with description for operators
        QString typeStr = QString::fromStdString(Lexer().getTokenTypeName(token.type));
        if (token.type == OPERATOR) {
            auto it = opDescriptions.find(token.lexeme);
            if (it != opDescriptions.end()) {
                typeStr += " (" + QString::fromStdString(it->second) + ")";
            }
        }
        row.append(new QStandardItem(typeStr));

        // Add line and column
        row.append(new QStandardItem(QString::number(token.line_number)));
        row.append(new QStandardItem(QString::number(token.column_number)));

        // Add the row
        tokenTableModel->appendRow(row);
    }

    // Resize columns to content
    ui->tokenTableView->resizeColumnsToContents();
}

void MainWindow::updateSymbolTable(const std::vector<std::pair<std::string, std::pair<std::string, int>>> &symbolTable)
{
    symbolTableModel->setRowCount(0); // Clear previous data

    for (const auto& symbol : symbolTable) {
        QList<QStandardItem*> row;

        // Add symbol name
        row.append(new QStandardItem(QString::fromStdString(symbol.first)));

        // Add symbol type
        row.append(new QStandardItem(QString::fromStdString(symbol.second.first)));

        // Add declaration line
        row.append(new QStandardItem(QString::number(symbol.second.second)));

        symbolTableModel->appendRow(row);
    }

    // Resize columns to content
    ui->symbolTableView->resizeColumnsToContents();
}


void MainWindow::updateErrorTable(const std::vector<Token> &tokens)
{
    errorTableModel->setRowCount(0); // Clear previous data

    for (const auto& token : tokens) {
        if (token.type == ERROR) {
            QList<QStandardItem*> row;

            // Add error type
            row.append(new QStandardItem("Lexical Error"));

            // Generate detailed error message based on lexeme and context
            QString errorMsg;
            QString lexeme = QString::fromStdString(token.lexeme);

            // Check for specific error patterns
            if (lexeme.contains("TabError")) {
                errorMsg = "Tabs are not allowed for indentation. Use spaces only.";
            }
            else if (lexeme.contains("DedentError")) {
                errorMsg = "Unindent does not match any outer indentation level.";
            }
            else if (lexeme.contains("IndentError")) {
                errorMsg = "Inconsistent indentation level.";
            }
            // Check for numeric identifier (identifier starting with digit)
            else if (QRegularExpression("^\\d+[a-zA-Z_][a-zA-Z0-9_]*$").match(lexeme).hasMatch()) {
                errorMsg = QString("Identifier cannot start with a digit: '%1'").arg(lexeme);
            }
            // Check for keyword misused as identifier
            else if (lexeme == "if" || lexeme == "else" || lexeme == "elif" ||
                     lexeme == "while" || lexeme == "for" || lexeme == "def" ||
                     lexeme == "return" || lexeme == "True" || lexeme == "False" ||
                     lexeme == "None" || lexeme == "in" || lexeme == "import") {
                errorMsg = QString("Reserved keyword '%1' cannot be used as an identifier").arg(lexeme);
            }
            // Check for invalid characters in identifiers
            else if (QRegularExpression("[a-zA-Z_][a-zA-Z0-9_]*[@#$%^&*!~`]+[a-zA-Z0-9_]*").match(lexeme).hasMatch()) {
                errorMsg = QString("Invalid character in identifier: '%1'").arg(lexeme);
            }
            // Check for multiple decimal points
            else if (QRegularExpression("^[+-]?\\d*(\\.\\d+){2,}$").match(lexeme).hasMatch()) {
                errorMsg = QString("Invalid float number: '%1'").arg(lexeme);
            }
            // Default error message
            else {
                errorMsg = QString("Unknown or invalid token '%1'").arg(lexeme);
            }

            row.append(new QStandardItem(errorMsg));

            // Add line and column
            row.append(new QStandardItem(QString::number(token.line_number)));
            row.append(new QStandardItem(QString::number(token.column_number)));

            errorTableModel->appendRow(row);
        }
    }

    // If errors were found, switch to the errors tab
    if (errorTableModel->rowCount() > 0) {
        ui->outputTabs->setCurrentIndex(2); // Index 2 is the errors tab
    }

    // Resize columns to content
    ui->errorTableView->resizeColumnsToContents();
}

void MainWindow::updateParserErrorTable(const std::vector<std::string> &errors)
{
    // Add parser errors to the error table
    for (const auto& error : errors) {
        QList<QStandardItem*> row;

        // Add error type
        row.append(new QStandardItem("Syntax Error"));

        // Extract line and column from error message if available
        QString errorMsg = QString::fromStdString(error);
        int line = 0;
        int column = 0;

        QRegularExpression lineColRegex("line (\\d+), column (\\d+)");
        QRegularExpressionMatch match = lineColRegex.match(errorMsg);

        if (match.hasMatch()) {
            line = match.captured(1).toInt();
            column = match.captured(2).toInt();
        }

        // Add message, line, column
        row.append(new QStandardItem(errorMsg));
        row.append(new QStandardItem(line > 0 ? QString::number(line) : ""));
        row.append(new QStandardItem(column > 0 ? QString::number(column) : ""));

        errorTableModel->appendRow(row);
    }

    // Resize columns to content
    ui->errorTableView->resizeColumnsToContents();
}

void MainWindow::onErrorTableDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    // Get the row of the clicked error
    int row = index.row();

    // Get line and column from the model
    QModelIndex lineModelIndex = errorTableModel->index(row, 2);  // Line column
    QModelIndex columnModelIndex = errorTableModel->index(row, 3); // Column column

    if (!lineModelIndex.isValid() || !columnModelIndex.isValid())
        return;

    int lineNumber = errorTableModel->data(lineModelIndex).toInt();
    int columnNumber = errorTableModel->data(columnModelIndex).toInt();

    // Skip if we don't have line info
    if (lineNumber <= 0)
        return;

    // Find the text block for this line (0-based line index)
    QTextBlock block = ui->sourceEditor->document()->findBlockByLineNumber(lineNumber - 1);
    if (!block.isValid())
        return;

    // Get the text of the line
    QString lineText = block.text();

    // Calculate column index (0-based)
    int columnPos = qMax(0, columnNumber - 1);

    // Calculate position in document
    int position = block.position() + columnPos;

    // Create a cursor at the error position
    QTextCursor cursor(ui->sourceEditor->document());
    cursor.setPosition(position);

    // Set cursor in text editor
    ui->sourceEditor->setTextCursor(cursor);

    // Ensure the cursor is visible
    ui->sourceEditor->ensureCursorVisible();

    // Give focus to the editor
    ui->sourceEditor->setFocus();

    // Highlight the current line with light gray
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;

    // Light gray color for line highlighting
    QColor lineColor = QColor(211, 211, 211); // Light gray RGB

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = ui->sourceEditor->textCursor();
    selection.cursor.clearSelection();

    extraSelections.append(selection);
    ui->sourceEditor->setExtraSelections(extraSelections);

    // Update status bar with error information
    QModelIndex msgIndex = errorTableModel->index(row, 1); // Message column
    QString errorMessage = errorTableModel->data(msgIndex).toString();
    showStatusMessage(QString("Line %1, Column %2: %3").arg(lineNumber).arg(columnNumber).arg(errorMessage), true, 10000);
}

void MainWindow::highlightErrors(const std::vector<Token> &tokens)
{
    // Get the document from the text editor
    QTextDocument* document = ui->sourceEditor->document();

    // Create a text cursor for manipulating the document
    QTextCursor cursor(document);

    // Create a format for highlighting errors
    QTextCharFormat errorFormat;
    errorFormat.setBackground(Qt::red);
    errorFormat.setForeground(Qt::white);

    // Clear existing highlighting by resetting the document's format
    cursor.select(QTextCursor::Document);
    QTextCharFormat defaultFormat;
    cursor.setCharFormat(defaultFormat);

    // Apply highlighting for each error
    for (const auto& token : tokens) {
        if (token.type == ERROR) {
            // Go to the line and column of the error
            int line = token.line_number - 1; // 0-based line index
            int column = token.column_number - 1; // 0-based column index

            // Find the block (line) where the error is
            QTextBlock block = document->findBlockByLineNumber(line);
            if (!block.isValid()) continue;

            // Create a cursor at the position of the error
            cursor = QTextCursor(block);
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);

            // Select the error token
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, token.lexeme.length());

            // Apply the error format
            cursor.setCharFormat(errorFormat);
        }
    }
}

// New implementation of visualizeParseTree that uses the custom widget
void MainWindow::visualizeParseTree(std::shared_ptr<ASTNode> root) {
    if (!root) {
        return;
    }

    // Use our custom widget to display the tree
    updateParseTree(root);
}

// Implementation of updateParseTree
void MainWindow::updateParseTree(std::shared_ptr<ASTNode> root) {
    parseTreeWidget->setParseTree(root);
}

// Keep these legacy methods for compatibility
void MainWindow::layoutParseTreeNode(std::shared_ptr<ASTNode> node, qreal x, qreal y, qreal width, int depth, QMap<std::shared_ptr<ASTNode>, QPointF> &nodePositions)
{
    if (!node) return;

    // Store this node's position
    nodePositions[node] = QPointF(x, y);

    // Get node's children
    std::vector<std::shared_ptr<ASTNode>> children;
    getChildren(node, children);

    int childCount = children.size();
    if (childCount > 0) {
        qreal childWidth = width / childCount;
        qreal startX = x - (width / 2) + (childWidth / 2);

        for (int i = 0; i < childCount; i++) {
            qreal childX = startX + i * childWidth;
            layoutParseTreeNode(children[i], childX, y + levelHeight, childWidth, depth + 1, nodePositions);
        }
    }
}

void MainWindow::createNodeVisual(std::shared_ptr<ASTNode> node, const QPointF &position)
{
    // Create ellipse for the node
    QGraphicsEllipseItem *ellipse = parseTreeScene->addEllipse(
        position.x() - nodeRadius,
        position.y() - nodeRadius,
        nodeRadius * 2,
        nodeRadius * 2
        );

    // Different colors for different node types
    QColor nodeColor;
    switch (node->type) {
    case NodeType::PROGRAM:
        nodeColor = QColor(173, 216, 230); // Light blue
        break;
    case NodeType::BINARY_EXPR:
        nodeColor = QColor(144, 238, 144); // Light green
        break;
    case NodeType::ASSIGNMENT_STMT:
        nodeColor = QColor(255, 182, 193); // Light pink
        break;
    case NodeType::IDENTIFIER:
        nodeColor = QColor(255, 255, 224); // Light yellow
        break;
    case NodeType::LITERAL:
        nodeColor = QColor(221, 160, 221); // Plum
        break;
    case NodeType::CALL_EXPR:
        nodeColor = QColor(255, 222, 173); // Navajo white
        break;
    default:
        nodeColor = QColor(173, 216, 230); // Default light blue
    }

    ellipse->setBrush(QBrush(nodeColor));
    ellipse->setPen(QPen(Qt::black));

    // Add text label
    QString label = getNodeLabel(node);
    QGraphicsTextItem *textItem = parseTreeScene->addText(label);
    textItem->setDefaultTextColor(Qt::black);

    // Center the text in the ellipse
    QRectF textRect = textItem->boundingRect();
    textItem->setPos(
        position.x() - textRect.width()/2,
        position.y() - textRect.height()/2
        );
}

void MainWindow::createEdges(std::shared_ptr<ASTNode> parent, const QMap<std::shared_ptr<ASTNode>, QPointF> &nodePositions)
{
    if (!parent || !nodePositions.contains(parent)) return;

    QPointF parentPos = nodePositions[parent];

    // Get children
    std::vector<std::shared_ptr<ASTNode>> children;
    getChildren(parent, children);

    // Create edges to each child
    for (auto& child : children) {
        if (child && nodePositions.contains(child)) {
            QPointF childPos = nodePositions[child];
            QPen edgePen(Qt::black);
            edgePen.setWidth(1);

            // Draw the edge from parent to child
            parseTreeScene->addLine(
                parentPos.x(),
                parentPos.y() + nodeRadius,
                childPos.x(),
                childPos.y() - nodeRadius,
                edgePen
                );

            // Recursive call for this child's children
            createEdges(child, nodePositions);
        }
    }
}

QString MainWindow::getNodeLabel(std::shared_ptr<ASTNode> node)
{
    // Return a concise label based on node type
    switch (node->type) {
    case NodeType::PROGRAM:
        return "Program";
    case NodeType::STATEMENT_LIST:
        return "Stmts";
    case NodeType::BLOCK:
        return "Block";
    case NodeType::ASSIGNMENT_STMT:
        return "=";
    case NodeType::IF_STMT:
        return "if";
    case NodeType::ELIF_CLAUSE:
        return "elif";
    case NodeType::ELSE_CLAUSE:
        return "else";
    case NodeType::WHILE_STMT:
        return "while";
    case NodeType::FOR_STMT:
        return "for";
    case NodeType::FUNC_DEF: {
        auto funcDef = std::static_pointer_cast<FunctionDefNode>(node);
        return QString("def %1").arg(funcDef->name.c_str());
    }
    case NodeType::RETURN_STMT:
        return "return";
    case NodeType::IMPORT_STMT: {
        auto import = std::static_pointer_cast<ImportNode>(node);
        return QString("%1").arg(import->module.c_str());
    }
    case NodeType::BINARY_EXPR: {
        auto binary = std::static_pointer_cast<BinaryExprNode>(node);
        return QString::fromStdString(binary->op);
    }
    case NodeType::UNARY_EXPR: {
        auto unary = std::static_pointer_cast<UnaryExprNode>(node);
        return QString::fromStdString(unary->op);
    }
    case NodeType::CALL_EXPR:
        return "call";
    case NodeType::SUBSCRIPT_EXPR:
        return "[]";
    case NodeType::ATTR_REF: {
        auto attr = std::static_pointer_cast<AttrRefNode>(node);
        return QString(".%1").arg(attr->attribute.c_str());
    }
    case NodeType::IDENTIFIER: {
        auto id = std::static_pointer_cast<IdentifierNode>(node);
        return QString::fromStdString(id->name);
    }
    case NodeType::LITERAL: {
        auto literal = std::static_pointer_cast<LiteralNode>(node);
        QString value = QString::fromStdString(literal->value);
        // Truncate long values
        if (value.length() > 8) {
            value = value.left(6) + "...";
        }
        return value;
    }
    case NodeType::LIST_LITERAL:
        return "[ ]";
    case NodeType::DICT_LITERAL:
        return "{ }";
    case NodeType::PARAM_LIST:
        return "params";
    case NodeType::ARG_LIST:
        return "args";
    case NodeType::ERROR_NODE: {
        auto error = std::static_pointer_cast<ErrorNode>(node);
        return "ERROR";
    }
    default:
        return QString("#%1").arg(static_cast<int>(node->type));
    }
}

void MainWindow::on_actionClear_Output_triggered()
{
    // Clear all tables
    tokenTableModel->removeRows(0, tokenTableModel->rowCount());
    symbolTableModel->removeRows(0, symbolTableModel->rowCount());
    errorTableModel->removeRows(0, errorTableModel->rowCount());

    // Clear parse tree visualizations
    parseTreeScene->clear();
    parseTreeWidget->setParseTree(nullptr);

    // Clear any error highlighting in the source editor
    QTextCursor cursor(ui->sourceEditor->document());
    cursor.select(QTextCursor::Document);
    QTextCharFormat defaultFormat;
    cursor.setCharFormat(defaultFormat);

    // Clear any extra selections (like the current line highlight)
    ui->sourceEditor->setExtraSelections(QList<QTextEdit::ExtraSelection>());

    showStatusMessage("Output cleared", false, 2000);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About Python Compiler",
                       "Python Compiler\n\n"
                       "A lexer and parser for Python language implemented in C++ with Qt GUI.\n\n"
                       "© 2025 - This is an educational project");
}

void MainWindow::showStatusMessage(const QString& message, bool isError, int messageTimeout, int styleTimeout)
{
    // Set message in status bar (0 duration = stays until changed)
    statusBar()->showMessage(message, messageTimeout);

    // Apply appropriate style based on message type
    if (isError) {
        statusBar()->setStyleSheet("background-color: #F8D7DA; color: #721C24;");
    } else {
        statusBar()->setStyleSheet("background-color: #D4EDDA; color: #155724;");
    }

    // Set a timer to reset the style after specified timeout
    QTimer::singleShot(styleTimeout, this, [this]() {
        statusBar()->setStyleSheet(""); // Reset to default style
    });
}

int MainWindow::countChildren(std::shared_ptr<ASTNode> node)
{
    if (!node) return 0;

    switch (node->type) {
    case NodeType::PROGRAM:
        return static_cast<ProgramNode*>(node.get())->statements.size();

    case NodeType::STATEMENT_LIST:
        return static_cast<StatementListNode*>(node.get())->statements.size();

    case NodeType::BLOCK:
        return 1; // The statement list

    case NodeType::ASSIGNMENT_STMT:
        return 2; // Target and value

    case NodeType::IF_STMT: {
        auto ifNode = static_cast<IfNode*>(node.get());
        return 2 + ifNode->elif_clauses.size() + (ifNode->else_block ? 1 : 0);
    }

    case NodeType::ELIF_CLAUSE:
    case NodeType::WHILE_STMT:
        return 2; // Condition and block

    case NodeType::ELSE_CLAUSE:
        return 1; // Block

    case NodeType::FOR_STMT:
        return 3; // Target, iterable, and block

    case NodeType::FUNC_DEF:
        return 2; // Params and body

    case NodeType::RETURN_STMT:
        return node->type == NodeType::RETURN_STMT &&
                       static_cast<ReturnNode*>(node.get())->expression ? 1 : 0;

    case NodeType::IMPORT_STMT:
        return 0; // No child nodes

    case NodeType::BINARY_EXPR:
        return 2; // Left and right

    case NodeType::UNARY_EXPR:
        return 1; // Operand

    case NodeType::CALL_EXPR:
        return 2; // Function and arguments

    case NodeType::SUBSCRIPT_EXPR:
        return 2; // Container and index

    case NodeType::ATTR_REF:
        return 1; // Object

    case NodeType::IDENTIFIER:
    case NodeType::LITERAL:
    case NodeType::ERROR_NODE:
        return 0; // No child nodes

    case NodeType::LIST_LITERAL:
        return static_cast<ListNode*>(node.get())->elements.size();

    case NodeType::DICT_LITERAL:
        return 2 * static_cast<DictNode*>(node.get())->items.size(); // Key and value pairs

    case NodeType::PARAM_LIST:
        return 0; // Parameters are not nodes

    case NodeType::ARG_LIST:
        return static_cast<ArgListNode*>(node.get())->arguments.size();

    default:
        return 0;
    }
}

void MainWindow::getChildren(std::shared_ptr<ASTNode> node, std::vector<std::shared_ptr<ASTNode>>& children)
{
    children.clear();
    if (!node) return;

    switch (node->type) {
    case NodeType::PROGRAM:
        for (auto& stmt : static_cast<ProgramNode*>(node.get())->statements) {
            children.push_back(stmt);
        }
        break;

    case NodeType::STATEMENT_LIST:
        for (auto& stmt : static_cast<StatementListNode*>(node.get())->statements) {
            children.push_back(stmt);
        }
        break;

    case NodeType::BLOCK:
        children.push_back(static_cast<BlockNode*>(node.get())->statements);
        break;

    case NodeType::ASSIGNMENT_STMT: {
        auto assignment = static_cast<AssignmentNode*>(node.get());
        children.push_back(assignment->target);
        children.push_back(assignment->value);
        break;
    }

    case NodeType::BINARY_EXPR: {
        auto binary = static_cast<BinaryExprNode*>(node.get());
        children.push_back(binary->left);
        children.push_back(binary->right);
        break;
    }

    case NodeType::IF_STMT: {
        auto ifNode = static_cast<IfNode*>(node.get());
        children.push_back(ifNode->condition);
        children.push_back(ifNode->if_block);
        for (auto& elif : ifNode->elif_clauses) {
            children.push_back(elif);
        }
        if (ifNode->else_block) {
            children.push_back(ifNode->else_block);
        }
        break;
    }

    case NodeType::ELIF_CLAUSE: {
        auto elif = static_cast<ElifNode*>(node.get());
        children.push_back(elif->condition);
        children.push_back(elif->block);
        break;
    }

    case NodeType::ELSE_CLAUSE:
        children.push_back(static_cast<ElseNode*>(node.get())->block);
        break;

    case NodeType::WHILE_STMT: {
        auto whileNode = static_cast<WhileNode*>(node.get());
        children.push_back(whileNode->condition);
        children.push_back(whileNode->block);
        break;
    }

    case NodeType::FOR_STMT: {
        auto forNode = static_cast<ForNode*>(node.get());
        children.push_back(forNode->target);
        children.push_back(forNode->iterable);
        children.push_back(forNode->block);
        break;
    }

    case NodeType::FUNC_DEF: {
        auto funcDef = static_cast<FunctionDefNode*>(node.get());
        children.push_back(funcDef->params);
        children.push_back(funcDef->body);
        break;
    }

    case NodeType::RETURN_STMT: {
        auto ret = static_cast<ReturnNode*>(node.get());
        if (ret->expression) {
            children.push_back(ret->expression);
        }
        break;
    }

    case NodeType::UNARY_EXPR:
        children.push_back(static_cast<UnaryExprNode*>(node.get())->operand);
        break;

    case NodeType::CALL_EXPR: {
        auto call = static_cast<CallExprNode*>(node.get());
        children.push_back(call->function);
        children.push_back(call->arguments);
        break;
    }

    case NodeType::SUBSCRIPT_EXPR: {
        auto subscript = static_cast<SubscriptExprNode*>(node.get());
        children.push_back(subscript->container);
        children.push_back(subscript->index);
        break;
    }

    case NodeType::ATTR_REF:
        children.push_back(static_cast<AttrRefNode*>(node.get())->object);
        break;

    case NodeType::LIST_LITERAL:
        for (auto& elem : static_cast<ListNode*>(node.get())->elements) {
            children.push_back(elem);
        }
        break;

    case NodeType::DICT_LITERAL:
        for (auto& item : static_cast<DictNode*>(node.get())->items) {
            children.push_back(item.first);
            children.push_back(item.second);
        }
        break;

    case NodeType::ARG_LIST:
        for (auto& arg : static_cast<ArgListNode*>(node.get())->arguments) {
            children.push_back(arg);
        }
        break;

    default:
        break;
    }
}

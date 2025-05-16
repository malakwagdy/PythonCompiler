#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTextStream>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextCursor>
#include <QRegularExpression>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include "lexer.h"
#include "parser.h"
#include "ParseTreeWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // File menu actions
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionExit_triggered();

    // Compiler menu actions
    void on_actionRun_Lexer_triggered();
    void on_actionRun_Parser_triggered(); // Add parser action
    void on_actionClear_Output_triggered();

    // Help menu action
    void on_actionAbout_triggered();

    void showStatusMessage(const QString& message, bool isError = false, int messageTimeout = 0, int styleTimeout = 5000);

private:
    Ui::MainWindow *ui;
    QString currentFilePath;

    // Models for table views
    QStandardItemModel *tokenTableModel;
    QStandardItemModel *symbolTableModel;
    QStandardItemModel *errorTableModel;

    // For parse tree visualization
    QGraphicsScene *parseTreeScene;
    qreal nodeRadius = 20.0;
    qreal levelHeight = 80.0;
    qreal horizontalSpacing = 60.0;

    // Custom parse tree widget
    ParseTreeWidget *parseTreeWidget;

    // Initialize the table models
    void setupTableModels();

    // Update the token table with lexer output
    void updateTokenTable(const std::vector<Token> &tokens);

    // Update the symbol table with lexer output
    void updateSymbolTable(const std::vector<std::pair<std::string, std::pair<std::string, int>>> &symbolTable);

    // Update the error table with lexer/parser errors
    void updateErrorTable(const std::vector<Token> &tokens);
    void updateParserErrorTable(const std::vector<std::string> &errors);

    // Helper functions for error highlighting in source code
    void highlightErrors(const std::vector<Token> &tokens);
    void onErrorTableDoubleClicked(const QModelIndex &index);

    // Parse tree visualization method
    void visualizeParseTree(std::shared_ptr<ASTNode> root);

    // Save file function used by both save actions
    bool saveFile(const QString &filePath);

};
#endif // MAINWINDOW_H

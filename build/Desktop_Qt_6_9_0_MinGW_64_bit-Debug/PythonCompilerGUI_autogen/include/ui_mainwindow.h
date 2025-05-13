/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionSave_As;
    QAction *actionExit;
    QAction *actionRun_Lexer;
    QAction *actionClear_Output;
    QAction *actionAbout;
    QWidget *centralWidget;
    QWidget *mainContainer;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QWidget *leftPanel;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout_2;
    QLabel *sourceHeaderLabel;
    QPlainTextEdit *sourceEditor;
    QWidget *rightPanel;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_3;
    QLabel *outputHeaderLabel;
    QTabWidget *outputTabs;
    QWidget *tokensTab;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *verticalLayout_4;
    QTableView *tokenTableView;
    QWidget *symbolsTab;
    QWidget *verticalLayoutWidget_4;
    QVBoxLayout *verticalLayout_5;
    QTableView *symbolTableView;
    QWidget *errorsTab;
    QWidget *verticalLayoutWidget_5;
    QVBoxLayout *verticalLayout_7;
    QTableView *errorTableView;
    QStatusBar *statusbar;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuCompile;
    QMenu *menuHelp;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1020, 650);
        MainWindow->setMinimumSize(QSize(800, 600));
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName("actionOpen");
        actionOpen->setMenuRole(QAction::MenuRole::NoRole);
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName("actionSave");
        actionSave->setMenuRole(QAction::MenuRole::NoRole);
        actionSave_As = new QAction(MainWindow);
        actionSave_As->setObjectName("actionSave_As");
        actionSave_As->setMenuRole(QAction::MenuRole::NoRole);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        actionExit->setMenuRole(QAction::MenuRole::NoRole);
        actionRun_Lexer = new QAction(MainWindow);
        actionRun_Lexer->setObjectName("actionRun_Lexer");
        actionRun_Lexer->setMenuRole(QAction::MenuRole::NoRole);
        actionClear_Output = new QAction(MainWindow);
        actionClear_Output->setObjectName("actionClear_Output");
        actionClear_Output->setMenuRole(QAction::MenuRole::NoRole);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName("actionAbout");
        actionAbout->setMenuRole(QAction::MenuRole::NoRole);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        mainContainer = new QWidget(centralWidget);
        mainContainer->setObjectName("mainContainer");
        mainContainer->setGeometry(QRect(0, 0, 1000, 600));
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(mainContainer->sizePolicy().hasHeightForWidth());
        mainContainer->setSizePolicy(sizePolicy1);
        mainContainer->setMinimumSize(QSize(600, 400));
        horizontalLayoutWidget = new QWidget(mainContainer);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(0, 0, 1001, 601));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        leftPanel = new QWidget(horizontalLayoutWidget);
        leftPanel->setObjectName("leftPanel");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(4);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(leftPanel->sizePolicy().hasHeightForWidth());
        leftPanel->setSizePolicy(sizePolicy2);
        verticalLayoutWidget = new QWidget(leftPanel);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(0, 0, 401, 601));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        sourceHeaderLabel = new QLabel(verticalLayoutWidget);
        sourceHeaderLabel->setObjectName("sourceHeaderLabel");
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        sourceHeaderLabel->setFont(font);
        sourceHeaderLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_2->addWidget(sourceHeaderLabel);

        sourceEditor = new QPlainTextEdit(verticalLayoutWidget);
        sourceEditor->setObjectName("sourceEditor");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Courier New")});
        font1.setPointSize(10);
        sourceEditor->setFont(font1);
        sourceEditor->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);

        verticalLayout_2->addWidget(sourceEditor);


        horizontalLayout->addWidget(leftPanel);

        rightPanel = new QWidget(horizontalLayoutWidget);
        rightPanel->setObjectName("rightPanel");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy3.setHorizontalStretch(6);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(rightPanel->sizePolicy().hasHeightForWidth());
        rightPanel->setSizePolicy(sizePolicy3);
        verticalLayoutWidget_2 = new QWidget(rightPanel);
        verticalLayoutWidget_2->setObjectName("verticalLayoutWidget_2");
        verticalLayoutWidget_2->setGeometry(QRect(0, 0, 601, 601));
        verticalLayout_3 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        outputHeaderLabel = new QLabel(verticalLayoutWidget_2);
        outputHeaderLabel->setObjectName("outputHeaderLabel");
        outputHeaderLabel->setFont(font);
        outputHeaderLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_3->addWidget(outputHeaderLabel);

        outputTabs = new QTabWidget(verticalLayoutWidget_2);
        outputTabs->setObjectName("outputTabs");
        tokensTab = new QWidget();
        tokensTab->setObjectName("tokensTab");
        sizePolicy.setHeightForWidth(tokensTab->sizePolicy().hasHeightForWidth());
        tokensTab->setSizePolicy(sizePolicy);
        verticalLayoutWidget_3 = new QWidget(tokensTab);
        verticalLayoutWidget_3->setObjectName("verticalLayoutWidget_3");
        verticalLayoutWidget_3->setGeometry(QRect(0, 0, 591, 531));
        verticalLayout_4 = new QVBoxLayout(verticalLayoutWidget_3);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        tokenTableView = new QTableView(verticalLayoutWidget_3);
        tokenTableView->setObjectName("tokenTableView");
        tokenTableView->setAlternatingRowColors(true);
        tokenTableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        tokenTableView->setSortingEnabled(false);

        verticalLayout_4->addWidget(tokenTableView);

        outputTabs->addTab(tokensTab, QString());
        symbolsTab = new QWidget();
        symbolsTab->setObjectName("symbolsTab");
        sizePolicy.setHeightForWidth(symbolsTab->sizePolicy().hasHeightForWidth());
        symbolsTab->setSizePolicy(sizePolicy);
        verticalLayoutWidget_4 = new QWidget(symbolsTab);
        verticalLayoutWidget_4->setObjectName("verticalLayoutWidget_4");
        verticalLayoutWidget_4->setGeometry(QRect(-1, 9, 591, 521));
        verticalLayout_5 = new QVBoxLayout(verticalLayoutWidget_4);
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        symbolTableView = new QTableView(verticalLayoutWidget_4);
        symbolTableView->setObjectName("symbolTableView");
        symbolTableView->setAlternatingRowColors(true);
        symbolTableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        symbolTableView->setSortingEnabled(false);

        verticalLayout_5->addWidget(symbolTableView);

        outputTabs->addTab(symbolsTab, QString());
        errorsTab = new QWidget();
        errorsTab->setObjectName("errorsTab");
        sizePolicy.setHeightForWidth(errorsTab->sizePolicy().hasHeightForWidth());
        errorsTab->setSizePolicy(sizePolicy);
        verticalLayoutWidget_5 = new QWidget(errorsTab);
        verticalLayoutWidget_5->setObjectName("verticalLayoutWidget_5");
        verticalLayoutWidget_5->setGeometry(QRect(-1, 9, 591, 521));
        verticalLayout_7 = new QVBoxLayout(verticalLayoutWidget_5);
        verticalLayout_7->setObjectName("verticalLayout_7");
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        errorTableView = new QTableView(verticalLayoutWidget_5);
        errorTableView->setObjectName("errorTableView");
        errorTableView->setAlternatingRowColors(true);
        errorTableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        verticalLayout_7->addWidget(errorTableView);

        outputTabs->addTab(errorsTab, QString());

        verticalLayout_3->addWidget(outputTabs);


        horizontalLayout->addWidget(rightPanel);

        MainWindow->setCentralWidget(centralWidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1020, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuCompile = new QMenu(menubar);
        menuCompile->setObjectName("menuCompile");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menubar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName("mainToolBar");
        MainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, mainToolBar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuCompile->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSave_As);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuCompile->addAction(actionRun_Lexer);
        menuCompile->addAction(actionClear_Output);
        menuHelp->addAction(actionAbout);
        mainToolBar->addAction(actionOpen);
        mainToolBar->addAction(actionSave);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionRun_Lexer);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionClear_Output);

        retranslateUi(MainWindow);

        outputTabs->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Python Compiler", nullptr));
        actionOpen->setText(QCoreApplication::translate("MainWindow", "&Open", nullptr));
#if QT_CONFIG(shortcut)
        actionOpen->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave->setText(QCoreApplication::translate("MainWindow", "&Save", nullptr));
#if QT_CONFIG(shortcut)
        actionSave->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave_As->setText(QCoreApplication::translate("MainWindow", "Save &As", nullptr));
#if QT_CONFIG(shortcut)
        actionSave_As->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionExit->setText(QCoreApplication::translate("MainWindow", "E&xit", nullptr));
#if QT_CONFIG(shortcut)
        actionExit->setShortcut(QCoreApplication::translate("MainWindow", "Shift+E", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRun_Lexer->setText(QCoreApplication::translate("MainWindow", "Run &Lexer", nullptr));
#if QT_CONFIG(shortcut)
        actionRun_Lexer->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+L", nullptr));
#endif // QT_CONFIG(shortcut)
        actionClear_Output->setText(QCoreApplication::translate("MainWindow", "&Clear Output", nullptr));
#if QT_CONFIG(shortcut)
        actionClear_Output->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+K", nullptr));
#endif // QT_CONFIG(shortcut)
        actionAbout->setText(QCoreApplication::translate("MainWindow", "&About", nullptr));
        sourceHeaderLabel->setText(QCoreApplication::translate("MainWindow", "Source Code", nullptr));
        sourceEditor->setPlaceholderText(QCoreApplication::translate("MainWindow", "Enter Python code here...", nullptr));
        outputHeaderLabel->setText(QCoreApplication::translate("MainWindow", "Compiler Output", nullptr));
        outputTabs->setTabText(outputTabs->indexOf(tokensTab), QCoreApplication::translate("MainWindow", "Tokens", nullptr));
        outputTabs->setTabText(outputTabs->indexOf(symbolsTab), QCoreApplication::translate("MainWindow", "Symbol Table", nullptr));
        outputTabs->setTabText(outputTabs->indexOf(errorsTab), QCoreApplication::translate("MainWindow", "Errors", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "&File", nullptr));
        menuCompile->setTitle(QCoreApplication::translate("MainWindow", "&Compile", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "&Help", nullptr));
        mainToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

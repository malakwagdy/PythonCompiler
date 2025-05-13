#ifndef PARSETREEWIDGET_H
#define PARSETREEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <memory>
#include "parser.h"

class ParseTreeWidget : public QWidget {
    Q_OBJECT

public:
    explicit ParseTreeWidget(QWidget *parent = nullptr);
    void setParseTree(std::shared_ptr<ASTNode> root);
    std::shared_ptr<ASTNode> getCurrentTree() const { return root; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    std::shared_ptr<ASTNode> root;
    double scale;
    QPoint offset;
    QPoint dragStart;
    bool isDragging;

    void drawNode(QPainter &painter, std::shared_ptr<ASTNode> node, int x, int y, int level);
    std::vector<std::shared_ptr<ASTNode>> getNodeChildren(std::shared_ptr<ASTNode> node);
    QString getNodeLabel(std::shared_ptr<ASTNode> node);
    int calculateNodeWidth(const QString &text);
    int calculateSubtreeWidth(std::shared_ptr<ASTNode> node, int level);
};

#endif // PARSETREEWIDGET_H

#include "ParseTreeWidget.h"
#include <QFontMetrics>
#include <algorithm>

ParseTreeWidget::ParseTreeWidget(QWidget *parent) :
    QWidget(parent), scale(0.4), offset(0, 0), isDragging(false) {  // Even more zoomed out for taller tree
    // Enable mouse tracking for drag operations
    setMouseTracking(true);
    // Set focus policy to accept wheel events
    setFocusPolicy(Qt::WheelFocus);
    // Set minimum size
    setMinimumSize(400, 300);
    // Set cursor to indicate draggability
    setCursor(Qt::OpenHandCursor);
}

void ParseTreeWidget::setParseTree(std::shared_ptr<ASTNode> root) {
    this->root = root;
    // Reset view with a better default scale
    scale = 0.4;  // Even more zoomed out for taller tree
    offset = QPoint(width() / 2, 30);  // Move up slightly
    update();
}

void ParseTreeWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    // Fill background
    painter.fillRect(rect(), Qt::white);

    // Apply transformations for zoom and pan
    painter.translate(offset);
    painter.scale(scale, scale);

    // Draw the tree if we have one
    if (root) {
        drawNode(painter, root, 0, 0, 0);
    } else {
        // Draw a message if no tree is available
        painter.drawText(rect(), Qt::AlignCenter, "No parse tree to display");
    }
}

int ParseTreeWidget::calculateSubtreeWidth(std::shared_ptr<ASTNode> node, int level) {
    if (!node) return 0;

    // Get the width needed for this node itself
    QString label = getNodeLabel(node);
    int nodeWidth = calculateNodeWidth(label);

    // Get children
    std::vector<std::shared_ptr<ASTNode>> children = getNodeChildren(node);
    if (children.empty()) {
        // Leaf node - needs its own width plus padding
        return nodeWidth + 20; // Further reduced padding from 30 to 20
    }

    // Calculate total width needed by all children
    int totalChildrenWidth = 0;
    // Further reduced from 120 to 90, still adaptive
    const int horizontalSpacingForLevel = 90 - level * 10;

    for (const auto& child : children) {
        totalChildrenWidth += calculateSubtreeWidth(child, level + 1);
    }

    // Add spacing between children
    if (children.size() > 1) {
        totalChildrenWidth += horizontalSpacingForLevel * (children.size() - 1);
    }

    // Return the max of node's width and the width of all its children
    return std::max(nodeWidth, totalChildrenWidth);
}

void ParseTreeWidget::drawNode(QPainter &painter, std::shared_ptr<ASTNode> node,
                               int x, int y, int level) {
    if (!node) return;

    const int nodeHeight = 35; // Reduced from 40 to 35
    const int verticalSpacing = 160; // INCREASED from 120 to 160
    // Further reduced from 120 to 90, still adaptive
    const int horizontalSpacing = 90 - level * 10;

    // Get node label and calculate width
    QString label = getNodeLabel(node);
    int nodeWidth = calculateNodeWidth(label);

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

    // Draw node ellipse
    painter.setPen(Qt::black);
    painter.setBrush(QBrush(nodeColor));
    QRect nodeRect(x - nodeWidth/2, y, nodeWidth, nodeHeight);
    painter.drawRoundedRect(nodeRect, 5, 5);

    // Draw the node text
    painter.drawText(nodeRect, Qt::AlignCenter, label);

    // Get children nodes
    std::vector<std::shared_ptr<ASTNode>> children = getNodeChildren(node);

    if (!children.empty()) {
        // Calculate the total width needed for children
        std::vector<int> childrenWidths;
        int totalChildrenWidth = 0;

        // Calculate width for each child's subtree
        for (const auto& child : children) {
            int width = calculateSubtreeWidth(child, level + 1);
            childrenWidths.push_back(width);
            totalChildrenWidth += width;
        }

        // Add spacing between children
        totalChildrenWidth += horizontalSpacing * (children.size() - 1);

        // Calculate starting X position for first child
        int startX = x - (totalChildrenWidth / 2);

        // Draw each child with appropriate spacing
        int currentX = startX;
        for (size_t i = 0; i < children.size(); i++) {
            const auto& child = children[i];
            int childWidth = childrenWidths[i];

            // Position the child at the center of its allocated space
            int childX = currentX + childWidth / 2;
            int childY = y + nodeHeight + verticalSpacing;

            // Draw line to child
            painter.drawLine(x, y + nodeHeight, childX, childY);

            // Recursively draw the child
            drawNode(painter, child, childX, childY, level + 1);

            // Move to next child position
            currentX += childWidth + horizontalSpacing;
        }
    }
}

std::vector<std::shared_ptr<ASTNode>> ParseTreeWidget::getNodeChildren(std::shared_ptr<ASTNode> node) {
    std::vector<std::shared_ptr<ASTNode>> children;
    if (!node) return children;

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

    return children;
}

QString ParseTreeWidget::getNodeLabel(std::shared_ptr<ASTNode> node) {
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
    case NodeType::ERROR_NODE:
        return "ERROR";
    default:
        return QString("#%1").arg(static_cast<int>(node->type));
    }
}

int ParseTreeWidget::calculateNodeWidth(const QString &text) {
    QFontMetrics fm(font());
    // Further reduced minimum width from 70 to 60, and padding from 40 to 30
    return std::max(60, fm.horizontalAdvance(text) + 30);
}

void ParseTreeWidget::wheelEvent(QWheelEvent *event) {
    // Zoom with mouse wheel
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numDegrees.isNull()) {
        double zoomFactor = 1.0 + (numDegrees.y() / 120.0) * 0.1;
        // Limit zoom range
        double newScale = qBound(0.1, scale * zoomFactor, 5.0);

        // Get mouse position before zoom
        QPointF mousePos = event->position();
        QPointF scenePos = (mousePos - QPointF(offset)) / scale;

        // Apply new scale
        scale = newScale;

        // Adjust offset to keep mouse position fixed
        offset = (mousePos - scenePos * scale).toPoint();

        update();
    }

    event->accept();
}

void ParseTreeWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        dragStart = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void ParseTreeWidget::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        offset += event->pos() - dragStart;
        dragStart = event->pos();
        update();
    }
}

void ParseTreeWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && isDragging) {
        isDragging = false;
        setCursor(Qt::OpenHandCursor);
    }
}

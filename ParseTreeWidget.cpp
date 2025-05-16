// ParseTreeWidget.cpp

#include "ParseTreeWidget.h"
#include <QFontMetrics>
#include <algorithm>
using namespace std;

bool isComparisonOperator(const string& op) {
    return op == ">" || op == "<" || op == ">=" || op == "<=" ||
           op == "==" || op == "!=" || op == "in" || op == "not in" ||
           op == "is" || op == "is not";
}


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

void ParseTreeWidget::setParseTree(shared_ptr<ASTNode> root) {
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
        painter.setPen(Qt::black);
        painter.drawText(rect(), Qt::AlignCenter, "No parse tree to display");
    }
}

int ParseTreeWidget::calculateSubtreeWidth(shared_ptr<ASTNode> node, int level) {
    if (!node) return 0;

    // Get the width needed for this node itself
    QString label = getNodeLabel(node);
    int nodeWidth = calculateNodeWidth(label);

    // Get children
    vector<shared_ptr<ASTNode>> children = getNodeChildren(node);
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
    return max(nodeWidth, totalChildrenWidth);
}

void ParseTreeWidget::drawNode(QPainter &painter, shared_ptr<ASTNode> node,
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
    case NodeType::CONDITION_NODE:
        nodeColor = QColor(135, 206, 235); // Sky blue
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
    vector<shared_ptr<ASTNode>> children = getNodeChildren(node);

    if (!children.empty()) {
        // Calculate the total width needed for children
        vector<int> childrenWidths;
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

vector<shared_ptr<ASTNode>> ParseTreeWidget::getNodeChildren(shared_ptr<ASTNode> node) {
    vector<shared_ptr<ASTNode>> children;
    if (!node) return children;

    switch (node->type) {
    case NodeType::PROGRAM: {
        for (auto& stmt : static_cast<ProgramNode*>(node.get())->statements) {
            // Check if this is an expression or assignment used as a statement
            if (stmt->type == NodeType::BINARY_EXPR ||
                stmt->type == NodeType::CALL_EXPR ||
                stmt->type == NodeType::ASSIGNMENT_STMT ||
                stmt->type == NodeType::UNARY_EXPR||
                stmt->type == NodeType::RETURN_STMT||
                stmt->type == NodeType::IF_STMT ||     // Added IF_STMT
                stmt->type == NodeType::WHILE_STMT ||  // Added WHILE_STMT
                stmt->type == NodeType::FOR_STMT) {

                // Create a statement wrapper node
                auto stmtNode = make_shared<StatementNode>(stmt);
                children.push_back(stmtNode);
                } else {
                    // Other statement types (if, while, def, etc.) are already clearly statements
                    // Other statement types (def, import, etc.)
                    children.push_back(stmt);
                }
        }
        break;
    }
    case NodeType::STATEMENT_LIST: {
        for (auto& stmt : static_cast<StatementListNode*>(node.get())->statements) {
            // Same logic as above
            if (stmt->type == NodeType::BINARY_EXPR ||
                stmt->type == NodeType::CALL_EXPR ||
                stmt->type == NodeType::ASSIGNMENT_STMT ||
                stmt->type == NodeType::UNARY_EXPR||
                stmt->type == NodeType::RETURN_STMT||
                stmt->type == NodeType::IF_STMT ||     // Added IF_STMT
                stmt->type == NodeType::WHILE_STMT ||  // Added WHILE_STMT
                stmt->type == NodeType::FOR_STMT) {


                auto stmtNode = make_shared<StatementNode>(stmt);
                children.push_back(stmtNode);
                } else {
                    children.push_back(stmt);
                }
        }
        break;
    }

    case NodeType::STATEMENT: {
        auto stmtNode = static_cast<StatementNode*>(node.get());

        // Special handling for return statements
        if (stmtNode->statement->type == NodeType::RETURN_STMT) {
            auto returnNode = static_pointer_cast<ReturnNode>(stmtNode->statement);

            // Add "return" keyword as a terminal node
            auto returnKeyword = make_shared<TerminalNode>("return",
                returnNode->line_number, returnNode->column_number);
            children.push_back(returnKeyword);

            // Add the expression directly (if it exists)
            if (returnNode->expression) {
                children.push_back(returnNode->expression);
            }
        }
        // Add back special handling for assignment statements
        else if (stmtNode->statement->type == NodeType::ASSIGNMENT_STMT) {
            auto assignNode = static_pointer_cast<AssignmentNode>(stmtNode->statement);
            auto assignStmtNode = make_shared<AssignStmtNode>(stmtNode->statement);
            children.push_back(assignStmtNode);
        }
        else {
            // For other statement types, continue as before
            children.push_back(stmtNode->statement);
        }
        break;
    }

        // Add this new case
    case NodeType::ASSIGNMENT_WRAPPER: {
        auto assignWrapper = static_cast<AssignStmtNode*>(node.get());
        auto assignNode = static_pointer_cast<AssignmentNode>(assignWrapper->assignment);

        // Add left side (target) as first child
        children.push_back(assignNode->target);

        // Add equals sign as second child
        auto equalsNode = make_shared<TerminalNode>(assignNode->op, assignNode->line_number, assignNode->column_number);
        children.push_back(equalsNode);

        // Add expression wrapper as third child
        auto expNode = make_shared<ExpressionNode>(assignNode->value);
        children.push_back(expNode);
        break;
    }

    case NodeType::EXPRESSION: {
        auto expNode = static_cast<ExpressionNode*>(node.get());

            if (expNode->expression->type == NodeType::UNARY_EXPR) {
                auto unary = static_pointer_cast<UnaryExprNode>(expNode->expression);

                // Handle ALL unary minus expressions, not just for literals
                if (unary->op == "-") {
                    auto minusNode = make_shared<TerminalNode>("-",
                        unary->line_number, unary->column_number);
                    children.push_back(minusNode);
                    children.push_back(unary->operand);
                    break;
                } else if (unary->op == "not") {
                    // Keep existing handling for "not" operator
                    auto notNode = make_shared<TerminalNode>("not",
                        unary->line_number, unary->column_number);
                    children.push_back(notNode);
                    children.push_back(unary->operand);
                    break;
                }
        }

        // Handle parenthesized expression
        else if (expNode->expression->type == NodeType::GROUP_EXPR) {
            auto groupExpr = static_pointer_cast<GroupExprNode>(expNode->expression);

            // Add opening parenthesis
            auto leftParenNode = make_shared<TerminalNode>("(",
                groupExpr->line_number, groupExpr->column_number);
            children.push_back(leftParenNode);

            // Add the expression itself, wrapped in an ExpressionNode
            auto innerExpNode = make_shared<ExpressionNode>(groupExpr->expression);
            children.push_back(innerExpNode);

            // Add closing parenthesis
            auto rightParenNode = make_shared<TerminalNode>(")",
                groupExpr->line_number, groupExpr->column_number);
            children.push_back(rightParenNode);

            break;
        }

        // Existing code for binary expressions and other types
        else if (expNode->expression->type == NodeType::BINARY_EXPR) {
            auto binary = static_cast<BinaryExprNode*>(expNode->expression.get());

            // Check if left side is also a binary expression or a group expression
            if (binary->left->type == NodeType::BINARY_EXPR ||
                binary->left->type == NodeType::GROUP_EXPR) {
                // Wrap in an ExpressionNode to maintain hierarchy
                auto leftExpNode = make_shared<ExpressionNode>(binary->left);
                children.push_back(leftExpNode);
            } else {
                // Add left operand directly
                children.push_back(binary->left);
            }

            // Add the operator as a terminal node
            auto opNode = make_shared<TerminalNode>(binary->op, binary->line_number, binary->column_number);
            children.push_back(opNode);

            // Check if right side is also a binary expression or a group expression
            if (binary->right->type == NodeType::BINARY_EXPR ||
                binary->right->type == NodeType::GROUP_EXPR) {
                // Wrap in an ExpressionNode to maintain hierarchy
                auto rightExpNode = make_shared<ExpressionNode>(binary->right);
                children.push_back(rightExpNode);
            } else {
                // Add right operand directly
                children.push_back(binary->right);
            }
        } // Handle subscript expressions (bypass the "subscript" node)
        else if (expNode->expression->type == NodeType::SUBSCRIPT_EXPR) {
            auto subscript = static_pointer_cast<SubscriptExprNode>(expNode->expression);

            // Add container directly
            children.push_back(subscript->container);

            // Add opening bracket
            auto leftBracketNode = make_shared<TerminalNode>("[",
                subscript->line_number, subscript->column_number);
            children.push_back(leftBracketNode);

            // Add index
            children.push_back(subscript->index);

            // Add closing bracket
            auto rightBracketNode = make_shared<TerminalNode>("]",
                subscript->line_number, subscript->column_number);
            children.push_back(rightBracketNode);
        } else {
            // For non-binary and non-unary-minus expressions, just add directly
            children.push_back(expNode->expression);
        }
        break;
    }

    case NodeType::GROUP_EXPR: {
        auto groupExpr = static_cast<GroupExprNode*>(node.get());

        // Add opening parenthesis as terminal node
        auto leftParenNode = make_shared<TerminalNode>("(",
            groupExpr->line_number, groupExpr->column_number);
        children.push_back(leftParenNode);

        // Add the expression itself, wrapped in an ExpressionNode
        auto innerExpNode = make_shared<ExpressionNode>(groupExpr->expression);
        children.push_back(innerExpNode);

        // Add closing parenthesis as terminal node
        auto rightParenNode = make_shared<TerminalNode>(")",
            groupExpr->line_number, groupExpr->column_number);
        children.push_back(rightParenNode);

        break;
    }

    case NodeType::COMPARISON_WRAPPER: {
        auto compWrapper = static_cast<ComparisonExprNode*>(node.get());
        auto binaryNode = static_pointer_cast<BinaryExprNode>(compWrapper->comparison);

        // Add left operand
        children.push_back(binaryNode->left);

        // Add operator as a terminal node
        auto opNode = make_shared<TerminalNode>(binaryNode->op, binaryNode->line_number, binaryNode->column_number);
        children.push_back(opNode);

        // Check if right side is a binary expression
        if (binaryNode->right->type == NodeType::BINARY_EXPR) {
            // Wrap the right binary expression in an ExpressionNode
            auto rightExpNode = make_shared<ExpressionNode>(binaryNode->right);
            children.push_back(rightExpNode);
        } else {
            // For non-binary expressions, add directly
            children.push_back(binaryNode->right);
        }
        break;
    }

    case NodeType::BLOCK: {
        auto blockNode = static_cast<BlockNode*>(node.get());

        // Check if blockNode->statements is a StatementListNode
        if (blockNode->statements && blockNode->statements->type == NodeType::STATEMENT_LIST) {
            // Bypass the StatementList and add its children directly
            auto stmtList = static_cast<StatementListNode*>(blockNode->statements.get());
            for (auto& stmt : stmtList->statements) {
                // Keep the same logic for statements that need wrappers
                if (stmt->type == NodeType::BINARY_EXPR ||
                    stmt->type == NodeType::CALL_EXPR ||
                    stmt->type == NodeType::ASSIGNMENT_STMT ||
                    stmt->type == NodeType::UNARY_EXPR ||
                    stmt->type == NodeType::RETURN_STMT ||
                    stmt->type == NodeType::IF_STMT ||
                    stmt->type == NodeType::WHILE_STMT ||
                    stmt->type == NodeType::FOR_STMT) {

                    auto stmtNode = make_shared<StatementNode>(stmt);
                    children.push_back(stmtNode);
                    } else {
                        children.push_back(stmt);
                    }
            }
        } else {
            // If it's not a StatementListNode, add it directly (fallback)
            children.push_back(blockNode->statements);
        }
        break;
    }

        // This is what you should use - it matches your NodeType enum
    case NodeType::ASSIGNMENT_STMT: {
        auto assignWrapper = static_cast<AssignStmtNode*>(node.get());
        auto assignNode = static_pointer_cast<AssignmentNode>(assignWrapper->assignment);

        // Add left side (target) as first child
        children.push_back(assignNode->target);

        // Add the actual operator (=, +=, -=, etc.) as second child
        auto opNode = make_shared<TerminalNode>(assignNode->op, assignNode->line_number, assignNode->column_number);
        children.push_back(opNode);

        // Add expression wrapper as third child
        auto expNode = make_shared<ExpressionNode>(assignNode->value);
        children.push_back(expNode);
        break;
    }

    case NodeType::BINARY_EXPR: {
        auto binary = static_cast<BinaryExprNode*>(node.get());

        // Check if this is a comparison operator
        if (isComparisonOperator(binary->op)) {
            // Don't add the compNode to children - instead convert directly to the flattened representation
            // Add left operand
            children.push_back(binary->left);

            // Add operator as a terminal node
            auto opNode = make_shared<TerminalNode>(binary->op, binary->line_number, binary->column_number);
            children.push_back(opNode);

            // Add right operand
            children.push_back(binary->right);
        } else {
            // Regular binary expression - keep as is
            children.push_back(binary->left);
            children.push_back(binary->right);
        }
        break;
    }

    case NodeType::IF_STMT: {
        auto ifNode = static_cast<IfNode*>(node.get());

        // Create a terminal node for "if" keyword
        auto ifKeywordNode = make_shared<TerminalNode>("if", ifNode->line_number, ifNode->column_number);
        children.push_back(ifKeywordNode);

        // Add left parenthesis if present
        if (ifNode->hasParentheses) {
            auto leftParenNode = make_shared<TerminalNode>("(", ifNode->line_number, ifNode->column_number);
            children.push_back(leftParenNode);
        }

        // Create a condition wrapper node that will have the actual condition as its child
        auto conditionNode = make_shared<ConditionNode>(
            ifNode->condition, ifNode->line_number, ifNode->column_number);
        children.push_back(conditionNode);

        // Add right parenthesis if present
        if (ifNode->hasParentheses) {
            auto rightParenNode = make_shared<TerminalNode>(")", ifNode->line_number, ifNode->column_number);
            children.push_back(rightParenNode);
        }

        // Add colon
        auto colonNode = make_shared<TerminalNode>(":", ifNode->line_number, ifNode->column_number);
        children.push_back(colonNode);

        // Add the if block
        children.push_back(ifNode->if_block);

        // Create an else-part node if we have any elif or else clauses
        if (!ifNode->elif_clauses.empty() || ifNode->else_block) {
            auto elsePartNode = make_shared<ElsePartNode>(
                ifNode->elif_clauses,
                dynamic_pointer_cast<ElseNode>(ifNode->else_block),
                ifNode->line_number, ifNode->column_number);
            children.push_back(elsePartNode);
        }
        break;
    }

    case NodeType::ELSE_PART: {
        auto elsePart = static_cast<ElsePartNode*>(node.get());

        // If we have elif clauses, handle them specially
        if (!elsePart->elif_clauses.empty()) {
            // Get the first elif clause
            auto& firstElif = elsePart->elif_clauses[0];

            // Create a terminal node for "elif" keyword
            auto elifKeywordNode = make_shared<TerminalNode>("elif", firstElif->line_number, firstElif->column_number);
            children.push_back(elifKeywordNode);

            // Add left parenthesis if present
            if (firstElif->hasParentheses) {
                auto leftParenNode = make_shared<TerminalNode>("(", firstElif->line_number, firstElif->column_number);
                children.push_back(leftParenNode);
            }

            // Create a condition wrapper node for the elif
            auto conditionNode = make_shared<ConditionNode>(
                firstElif->condition, firstElif->line_number, firstElif->column_number);
            children.push_back(conditionNode);

            // Add right parenthesis if present
            if (firstElif->hasParentheses) {
                auto rightParenNode = make_shared<TerminalNode>(")", firstElif->line_number, firstElif->column_number);
                children.push_back(rightParenNode);
            }

            // Add colon
            auto colonNode = make_shared<TerminalNode>(":", firstElif->line_number, firstElif->column_number);
            children.push_back(colonNode);

            // Add the elif block
            children.push_back(firstElif->block);

            // If there are more elifs or an else, create a new ElsePartNode for them
            if (elsePart->elif_clauses.size() > 1 || elsePart->else_block) {
                // Create a vector with remaining elifs (skipping the first one)
                vector<shared_ptr<ElifNode>> remainingElifs;
                for (size_t i = 1; i < elsePart->elif_clauses.size(); i++) {
                    remainingElifs.push_back(elsePart->elif_clauses[i]);
                }

                // Create a new ElsePartNode for the remaining elifs and the else block
                auto nestedElsePart = make_shared<ElsePartNode>(
                    remainingElifs,
                    elsePart->else_block,
                    firstElif->line_number,
                    firstElif->column_number);

                children.push_back(nestedElsePart);
            }
        }
        // If no elif clauses but we have an else, add "else" keyword and block separately
        else if (elsePart->else_block) {
            // Create a terminal node for "else" keyword
            auto elseKeywordNode = make_shared<TerminalNode>("else",
                elsePart->else_block->line_number, elsePart->else_block->column_number);
            children.push_back(elseKeywordNode);

            // Create a terminal node for the colon
            auto colonNode = make_shared<TerminalNode>(":",
                elsePart->else_block->line_number, elsePart->else_block->column_number);
            children.push_back(colonNode);

            // Add the block separately
            children.push_back(elsePart->else_block->block);
        }

        break;
    }

    case NodeType::CONDITION_NODE: {
        auto condNode = static_cast<ConditionNode*>(node.get());
        if (condNode->condition) {
            // Check if this is a binary expression with logical operators (and, or)
            if (condNode->condition->type == NodeType::BINARY_EXPR) {
                auto binary = static_cast<BinaryExprNode*>(condNode->condition.get());

                // Check if operator is a logical operator (and, or)
                if (binary->op == "and" || binary->op == "or") {
                    // Flatten the structure - add left operand directly
                    children.push_back(binary->left);

                    // Add the operator as a terminal node
                    auto opNode = make_shared<TerminalNode>(binary->op, binary->line_number, binary->column_number);
                    children.push_back(opNode);

                    // Add right operand directly
                    children.push_back(binary->right);
                }
                // Keep existing comparison operator handling
                else if (isComparisonOperator(binary->op)) {
                    // Wrap it in a ComparisonExprNode (same as your existing code)
                    auto compNode = make_shared<ComparisonExprNode>(
                        make_shared<BinaryExprNode>(*binary));
                    children.push_back(compNode);
                } else {
                    // Regular binary expression (same as your existing code)
                    children.push_back(condNode->condition);
                }
            }
            // Handle unary "not" operator
            else if (condNode->condition->type == NodeType::UNARY_EXPR) {
                auto unary = static_cast<UnaryExprNode*>(condNode->condition.get());

                // Check if this is the "not" operator
                if (unary->op == "not") {
                    // Add the "not" operator as a terminal node
                    auto notNode = make_shared<TerminalNode>("not", unary->line_number, unary->column_number);
                    children.push_back(notNode);

                    // Add the operand directly
                    children.push_back(unary->operand);
                } else {
                    // Other unary expressions
                    children.push_back(condNode->condition);
                }
            }
            else {
                // Not a binary or unary expression
                children.push_back(condNode->condition);
            }
        }
        break;
    }

    case NodeType::ELIF_CLAUSE: {
        auto elif = static_cast<ElifNode*>(node.get());

        // Create a virtual condition node for elif as well
        auto conditionNode = make_shared<ConditionNode>(
            elif->condition, elif->line_number, elif->column_number);

        children.push_back(conditionNode);
        children.push_back(elif->block);
        break;
    }

    case NodeType::ELSE_CLAUSE:
        children.push_back(static_cast<ElseNode*>(node.get())->block);
        break;

    case NodeType::WHILE_STMT: {
        auto whileNode = static_cast<WhileNode*>(node.get());

        // Create a terminal node for "while" keyword
        auto whileKeywordNode = make_shared<TerminalNode>("while", whileNode->line_number, whileNode->column_number);
        children.push_back(whileKeywordNode);

        // Add left parenthesis if present
        if (whileNode->hasParentheses) {
            auto leftParenNode = make_shared<TerminalNode>("(", whileNode->line_number, whileNode->column_number);
            children.push_back(leftParenNode);
        }

        // Create a condition wrapper node
        auto conditionNode = make_shared<ConditionNode>(
            whileNode->condition, whileNode->line_number, whileNode->column_number);
        children.push_back(conditionNode);

        // Add right parenthesis if present
        if (whileNode->hasParentheses) {
            auto rightParenNode = make_shared<TerminalNode>(")", whileNode->line_number, whileNode->column_number);
            children.push_back(rightParenNode);
        }

        // Add colon
        auto colonNode = make_shared<TerminalNode>(":", whileNode->line_number, whileNode->column_number);
        children.push_back(colonNode);

        // Add the block
        children.push_back(whileNode->block);
        break;
    }

    case NodeType::FOR_STMT: {
        auto forNode = static_cast<ForNode*>(node.get());

        // Create a terminal node for "for" keyword
        auto forKeywordNode = make_shared<TerminalNode>("for", forNode->line_number, forNode->column_number);
        children.push_back(forKeywordNode);

        // Add left parenthesis if present
        if (forNode->hasParentheses) {
            auto leftParenNode = make_shared<TerminalNode>("(", forNode->line_number, forNode->column_number);
            children.push_back(leftParenNode);
        }

        // Add target (the variable that iterates)
        children.push_back(forNode->target);

        // Add "in" keyword
        auto inKeywordNode = make_shared<TerminalNode>("in", forNode->line_number, forNode->column_number);
        children.push_back(inKeywordNode);

        // Add iterable expression
        children.push_back(forNode->iterable);

        // Add right parenthesis if present
        if (forNode->hasParentheses) {
            auto rightParenNode = make_shared<TerminalNode>(")", forNode->line_number, forNode->column_number);
            children.push_back(rightParenNode);
        }

        // Add colon
        auto colonNode = make_shared<TerminalNode>(":", forNode->line_number, forNode->column_number);
        children.push_back(colonNode);

        // Add the block
        children.push_back(forNode->block);
        break;
    }

        // Update the FUNC_DEF case in getNodeChildren
        case NodeType::FUNC_DEF: {
        auto funcDef = static_cast<FunctionDefNode*>(node.get());

        // First show the 'def' keyword terminal
        if (funcDef->defKeyword) {
            children.push_back(funcDef->defKeyword);
        }

        // Then the function name terminal
        if (funcDef->nameNode) {
            children.push_back(funcDef->nameNode);
        }

        // Then open parenthesis
        if (funcDef->openParen) {
            children.push_back(funcDef->openParen);
        }

        // Then params
        children.push_back(funcDef->params);

        // Then close parenthesis
        if (funcDef->closeParen) {
            children.push_back(funcDef->closeParen);
        }

        // Then colon
        if (funcDef->colon) {
            children.push_back(funcDef->colon);
        }

        // Then body
        children.push_back(funcDef->body);
        break;
        }

    case NodeType::PARAM_LIST: {
        auto paramList = static_cast<ParamListNode*>(node.get());
        for (const auto& param : paramList->parameters) {
            children.push_back(param);
        }
        break;
    }

    case NodeType::RETURN_STMT: {
        // auto ret = static_cast<ReturnNode*>(node.get());
        // if (ret->expression) {
        //     children.push_back(ret->expression);
        // }
        break;
    }

    case NodeType::UNARY_EXPR:
        children.push_back(static_cast<UnaryExprNode*>(node.get())->operand);
        break;

    case NodeType::CALL_EXPR: {
        auto call = static_cast<CallExprNode*>(node.get());

        // Check if the function is an attribute reference (method call)
        if (call->function->type == NodeType::ATTR_REF) {
            auto attrRef = static_pointer_cast<AttrRefNode>(call->function);

            // Add the object directly (e.g., "results")
            children.push_back(attrRef->object);

            // Add the attribute as a terminal node (e.g., ".append")
            auto methodNode = make_shared<TerminalNode>("." + attrRef->attribute,
                call->line_number, call->column_number);
            children.push_back(methodNode);
        }
        else {
            // For regular function calls (not methods), add the function directly
            children.push_back(call->function);
        }

        // Then continue with parentheses and arguments as before
        if (call->openParen) {
            children.push_back(call->openParen);
        }
        // Arguments
        children.push_back(call->arguments);
        // Close parenthesis
        if (call->closeParen) {
            children.push_back(call->closeParen);
        }
        break;
    }

    case NodeType::SUBSCRIPT_EXPR: {
        auto subscript = static_cast<SubscriptExprNode*>(node.get());

        // Add the container (e.g., "my_list")
        children.push_back(subscript->container);

        // Add opening bracket as a terminal node
        auto leftBracketNode = make_shared<TerminalNode>("[",
            subscript->line_number, subscript->column_number);
        children.push_back(leftBracketNode);

        // Add the index/key
        children.push_back(subscript->index);

        // Add closing bracket as a terminal node
        auto rightBracketNode = make_shared<TerminalNode>("]",
            subscript->line_number, subscript->column_number);
        children.push_back(rightBracketNode);

        break;
    }

    case NodeType::ATTR_REF:
        children.push_back(static_cast<AttrRefNode*>(node.get())->object);
        break;

    case NodeType::LIST_LITERAL: {
            auto listNode = static_cast<ListNode*>(node.get());
            for (auto& elem : listNode->elements) {
                // Check if this is a unary minus expression
                if (elem->type == NodeType::UNARY_EXPR) {
                    auto unary = static_pointer_cast<UnaryExprNode>(elem);

                    // Handle all unary minus expressions, not just on literals
                    if (unary->op == "-") {
                        // Add the minus sign as a separate terminal node
                        auto minusNode = make_shared<TerminalNode>("-",
                            unary->line_number, unary->column_number);
                        children.push_back(minusNode);

                        // Add the operand directly
                        children.push_back(unary->operand);
                    } else {
                        // For other unary expressions, add normally
                        children.push_back(elem);
                    }
                } else {
                    // Regular element, add as normal
                    children.push_back(elem);
                }
            }
            break;
    }

    case NodeType::DICT_LITERAL: {
            auto dictNode = static_cast<DictNode*>(node.get());

            for (const auto& item : dictNode->items) {
                // Process key
                if (item.first) {
                    if (item.first->type == NodeType::UNARY_EXPR) {
                        auto unary = static_pointer_cast<UnaryExprNode>(item.first);

                        if (unary->op == "-") {
                            // Add minus sign
                            auto minusNode = make_shared<TerminalNode>("-",
                                unary->line_number, unary->column_number);
                            children.push_back(minusNode);

                            // Add the operand
                            children.push_back(unary->operand);
                        } else {
                            children.push_back(item.first);
                        }
                    } else {
                        children.push_back(item.first);
                    }
                }

                // Process value
                if (item.second) {
                    if (item.second->type == NodeType::UNARY_EXPR) {
                        auto unary = static_pointer_cast<UnaryExprNode>(item.second);

                        if (unary->op == "-") {
                            // Add minus sign
                            auto minusNode = make_shared<TerminalNode>("-",
                                unary->line_number, unary->column_number);
                            children.push_back(minusNode);

                            // Add the operand
                            children.push_back(unary->operand);
                        } else {
                            children.push_back(item.second);
                        }
                    } else {
                        children.push_back(item.second);
                    }
                }
            }
            break;
    }

    case NodeType::ARG_LIST: {
            auto argList = static_cast<ArgListNode*>(node.get());
            for (auto& arg : argList->arguments) {
                // Check if this is a unary minus expression
                if (arg->type == NodeType::UNARY_EXPR) {
                    auto unary = static_pointer_cast<UnaryExprNode>(arg);

                    // Handle all unary minus expressions, not just on literals
                    if (unary->op == "-") {
                        // Add the minus sign as a separate terminal node
                        auto minusNode = make_shared<TerminalNode>("-",
                            unary->line_number, unary->column_number);
                        children.push_back(minusNode);

                        // Add the operand directly
                        children.push_back(unary->operand);
                    } else {
                        // For other unary expressions, add normally
                        children.push_back(arg);
                    }
                } else {
                    // Regular argument, add as normal
                    children.push_back(arg);
                }
            }
            break;
    }

    case NodeType::PARAMETER_NODE: {
        auto param = static_cast<ParameterNode*>(node.get());

        // If this is a comma, don't add any children
        if (param->name == ",") {
            // Explicitly return an empty vector for commas
            break;
        }

        // For regular parameters without default values, just add name as child
        if (!param->default_value) {
            auto nameNode = make_shared<IdentifierNode>(param->name, param->line_number, param->column_number);
            children.push_back(nameNode);
            break;
        }

        // For parameters with default values, create the name-equals-value structure
        auto nameNode = make_shared<IdentifierNode>(param->name, param->line_number, param->column_number);
        children.push_back(nameNode);

        auto equalsNode = make_shared<TerminalNode>("=", param->line_number, param->column_number);
        children.push_back(equalsNode);

        // Add the default value, bypassing any StatementList wrapper
        if (param->default_value->type == NodeType::STATEMENT_LIST) {
            auto stmtList = static_cast<StatementListNode*>(param->default_value.get());
            if (stmtList->statements.size() > 1) {
                children.push_back(stmtList->statements[1]);
            }
        } else {
            children.push_back(param->default_value);
        }
        break;
    }

    default:
        break;
    }

    return children;
}

QString ParseTreeWidget::getNodeLabel(shared_ptr<ASTNode> node) {
    // Return a concise label based on node type
    switch (node->type) {
    case NodeType::PROGRAM:
        return "Program";
    case NodeType::STATEMENT_LIST:
        return "Stmts";
    case NodeType::STATEMENT:
        return "Stmt";
    case NodeType::BLOCK:
        return "Block";
    case NodeType::ASSIGNMENT_STMT:
        return "=";
    case NodeType::IF_STMT:
        return "if-stmt";
    case NodeType::ELIF_CLAUSE:
        return "elif";
    case NodeType::ELSE_PART:
        return "Else-part";
    case NodeType::ELSE_CLAUSE:
        return "else";
    case NodeType::WHILE_STMT:
        return "while-stmt";
    case NodeType::FOR_STMT:
        return "for-stmt";
    case NodeType::CONDITION_NODE:
        return "Condition";
    case NodeType::FUNC_DEF:
        return "Function";
    case NodeType::RETURN_STMT:
        return "return";
    case NodeType::IMPORT_STMT: {
        auto import = static_pointer_cast<ImportNode>(node);
        return QString("%1").arg(import->module.c_str());
    }
    case NodeType::BINARY_EXPR: {
        auto binary = static_pointer_cast<BinaryExprNode>(node);
        return QString::fromStdString(binary->op);
    }
    case NodeType::UNARY_EXPR: {
        auto unary = static_pointer_cast<UnaryExprNode>(node);
        return QString::fromStdString(unary->op);
    }
    case NodeType::GROUP_EXPR:
        return "group";
    case NodeType::CALL_EXPR:
        return "call";
    case NodeType::SUBSCRIPT_EXPR:
        return "subscript";
    case NodeType::ATTR_REF: {
        auto attr = static_pointer_cast<AttrRefNode>(node);
        return QString(".%1").arg(attr->attribute.c_str());
    }
    case NodeType::IDENTIFIER: {
        auto id = static_pointer_cast<IdentifierNode>(node);
        return QString::fromStdString(id->name);
    }
    case NodeType::LITERAL: {
        auto literal = static_pointer_cast<LiteralNode>(node);
        QString value = QString::fromStdString(literal->value);
        // Truncate long values
        if (value.length() > 8) {
            value = value.left(6) + "...";
        }
        return value;
    }

    case NodeType::TERMINAL: {
    auto terminal = static_pointer_cast<TerminalNode>(node);
    return QString::fromStdString(terminal->value);
    }

    case NodeType::PARAMETER_NODE: {
        auto param = static_cast<ParameterNode*>(node.get());

        // Check if this is a comma (special case in your parser)
        if (param->name == ",") {
            return QString(",");  // Just show the comma as is
        }

        // For actual parameters, show "param"
        return QString("param");

    }
    case NodeType::LIST_LITERAL:
        return "List"; // Change from "[ ]" to "List"
    case NodeType::DICT_LITERAL:
        return "Dictionary";
    case NodeType::PARAM_LIST:
        return "params";
    case NodeType::ARG_LIST:
        return "args";
    case NodeType::ERROR_NODE:
        return "ERROR";
    case NodeType::ASSIGNMENT_WRAPPER:
        return "assign-stmt";
    case NodeType::EXPRESSION:
        return "exp";
    case NodeType::COMPARISON_WRAPPER:
        return "comp-expr";
    default:
        return QString("#%1").arg(static_cast<int>(node->type));
    }
}

int ParseTreeWidget::calculateNodeWidth(const QString &text) {
    QFontMetrics fm(font());
    // Further reduced minimum width from 70 to 60, and padding from 40 to 30
    return max(60, fm.horizontalAdvance(text) + 30);
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
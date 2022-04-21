#ifndef LAB3_TREE_H
#define LAB3_TREE_H

/// token 类别
enum token {
    TOK_INT,
    TOK_FLOAT,
    TOK_STRUCT,
    TOK_RETURN,
    TOK_IF,
    TOK_ELSE,
    TOK_WHILE,
    TOK_TYPE,
    TOK_ID,
    TOK_SEMI,
    TOK_COMMA,
    TOK_ASSIGNOP,
    TOK_RELOP,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_DIV,
    TOK_AND,
    TOK_OR,
    TOK_DOT,
    TOK_NOT,
    TOK_LP,
    TOK_RP,
    TOK_LB,
    TOK_RB,
    TOK_LC,
    TOK_RC,

    NOT_TOKEN,
};

/// 语法树节点
struct node {
    int          lineno;        // 节点对应 token 所在的行号
    enum token   token;         // token 类别
    char         name[32];      // token 展示的名称
    struct node* left_child;    // 节点的最左子节点
    struct node* right_sibling; // 节点的右兄弟节点
};

/// 新建树节点，
/// \p lineno 当前 token 节点对应在文件中的行号
/// \p token 当前 token 种类
/// \p name 当前 token 展示的鸣唱
/// \p argc 当前节点的子节点数量
/// \p va_list 子节点列表
struct node* new_node(int lineno, enum token token, char name[], int argc, ...);

/// 从根节点 \p root 前序遍历一棵树，
/// \p height 是 \p root 节点所在的层数
void pre_order_traversal(struct node* root, int height);

#endif

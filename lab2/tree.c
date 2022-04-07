#include "tree.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node* new_node(int lineno, enum token token, char name[], int argc,
                      ...) {
    struct node* node   = (struct node*)malloc(sizeof(struct node));
    node->lineno        = lineno;
    node->token         = token;
    node->left_child    = NULL;
    node->right_sibling = NULL;
    strcpy(node->name, name);

    if (argc > 0) {
        va_list vl;
        va_start(vl, argc);
        struct node* left_child_node = va_arg(vl, struct node*);
        node->left_child             = left_child_node;
        // 链式记录最左子节点的右兄弟节点，从而记录当前节点的所有子节点
        for (int i = 1; i < argc; i++) {
            left_child_node->right_sibling = va_arg(vl, struct node*);
            if (left_child_node->right_sibling != NULL) {
                left_child_node = left_child_node->right_sibling;
            }
        }
        va_end(vl);
    }

    return node;
}

void pre_order_traversal(struct node* root, int height) {
    if (root == NULL) {
        return;
    }

    // 依据层数打印缩进
    for (int i = 0; i < height; i++) {
        printf("  ");
    }

    if (root->token == TOK_ID) {
        printf("ID: %s\n", root->name);
    } else if (root->token == TOK_INT) {
        printf("INT: %d\n", atoi(root->name));
    } else if (root->token == TOK_FLOAT) {
        printf("FLOAT: %f\n", atof(root->name));
    } else if (root->token == TOK_TYPE) {
        printf("TYPE: %s\n", root->name);
    } else if (root->token == NOT_TOKEN) {
        printf("%s (%d)\n", root->name, root->lineno);
    } else {
        printf("%s\n", root->name);
    }

    pre_order_traversal(root->left_child, height + 1);
    pre_order_traversal(root->right_sibling, height);
}

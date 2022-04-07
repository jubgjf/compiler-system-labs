#include "semantic.h"
#include <stdlib.h>
#include <string.h>

/// 符号表
struct sym_table* table;

char* copy_str(char* src) {
    if (src == NULL) {
        return NULL;
    }

    int   len = strlen(src) + 1;
    char* cpy = (char*)malloc(sizeof(char) * len);
    strncpy(cpy, src, len);

    return cpy;
}

void print_error(enum error_type type, int line, char* msg) {
    printf("Error type %d at Line %d: %s\n", type, line, msg);
}

void print_func_error(struct node* node, struct table_item* func_item) {
    printf("Error type %d at Line %d: ", FUNC_AGRC_MISMATCH, node->lineno);

    printf("Function \"%s(", func_item->field->name);
    struct field* field_temp = func_item->field;
    while (field_temp) {
        print_kind(field_temp->type->u.function.argv->type);
        field_temp = field_temp->next;
    }
    printf(")\" is not applicable for arguments \"(");

    struct node* args_node_temp = node;
    while (args_node_temp->left_child->right_sibling) {
        struct type* args_type_temp = Exp(args_node_temp->left_child);
        print_kind(args_type_temp);
        printf(", ");

        // args_node_temp->left_child->right_sibling
        //                                   ^
        //                                   |
        //                                 COMMA
        args_node_temp =
            args_node_temp->left_child->right_sibling->right_sibling;
    }
    struct type* args_type_temp = Exp(args_node_temp->left_child);
    print_kind(args_type_temp);
    printf(")\".\n");
}

void print_kind(struct type* type) {
    switch (type->kind) {
    case BASIC:
        if (type->u.basic == INT_TYPE) {
            printf("int");
        } else {
            printf("float");
        }
        break;
    case ARRAY:
        print_kind(type->u.array.element_type);
        break;
    case STRUCTURE:
        printf("struct %s", type->u.structure.name);
        break;
    case FUNCTION:
        printf("function");
        break;
    };
}

void semantic_analysis(struct node* node) {
    if (node == NULL)
        return;

    if (!strcmp(node->name, "ExtDef"))
        ExtDef(node);

    semantic_analysis(node->left_child);
    semantic_analysis(node->right_sibling);
}

struct type* new_type(enum kind kind, ...) {
    struct type* type = (struct type*)malloc(sizeof(struct type));

    type->kind = kind;

    va_list vl;

    switch (kind) {
    case BASIC:
        va_start(vl, 1);
        type->u.basic = va_arg(vl, enum basic_type);
        va_end(vl);
        break;
    case ARRAY:
        va_start(vl, 2);
        type->u.array.element_type = va_arg(vl, struct type*);
        type->u.array.size         = va_arg(vl, int);
        va_end(vl);
        break;
    case STRUCTURE:
        va_start(vl, 2);
        type->u.structure.name  = va_arg(vl, char*);
        type->u.structure.field = va_arg(vl, struct field*);
        va_end(vl);
        break;
    case FUNCTION:
        va_start(vl, 3);
        type->u.function.argc        = va_arg(vl, int);
        type->u.function.argv        = va_arg(vl, struct field*);
        type->u.function.return_type = va_arg(vl, struct type*);
        va_end(vl);
        break;
    }

    return type;
}

struct type* copy_type(struct type* src) {
    if (src == NULL) {
        return NULL;
    }

    struct type* type = (struct type*)malloc(sizeof(struct type));

    type->kind = src->kind;

    switch (type->kind) {
    case BASIC:
        type->u.basic = src->u.basic;
        break;
    case ARRAY:
        type->u.array.element_type = copy_type(src->u.array.element_type);
        type->u.array.size         = src->u.array.size;
        break;
    case STRUCTURE:
        type->u.structure.name  = copy_str(src->u.structure.name);
        type->u.structure.field = copy_field(src->u.structure.field);
        break;
    case FUNCTION:
        type->u.function.argc        = src->u.function.argc;
        type->u.function.argv        = copy_field(src->u.function.argv);
        type->u.function.return_type = copy_type(src->u.function.return_type);
        break;
    }

    return type;
}

int is_type_same(struct type* type1, struct type* type2) {
    if (type1 == NULL || type2 == NULL)
        return 1;
    if (type1->kind == FUNCTION || type2->kind == FUNCTION)
        return 0;
    if (type1->kind != type2->kind)
        return 0;
    else {
        switch (type1->kind) {
        case BASIC:
            return type1->u.basic == type2->u.basic;
        case ARRAY:
            return is_type_same(type1->u.array.element_type,
                                type2->u.array.element_type);
        case STRUCTURE:
            return !strcmp(type1->u.structure.name, type2->u.structure.name);
        case FUNCTION:
            // 这个分支已经被前边的 if 覆盖到了，所以一定不会走到这里
            return 0;
        }
    }
}

struct field* new_field(char* name, struct type* type) {
    struct field* field = (struct field*)malloc(sizeof(struct field));

    field->name = copy_str(name);
    field->type = type;
    field->next = NULL;

    return field;
}

struct field* copy_field(struct field* src) {
    struct field* head = NULL;
    struct field* curr = NULL;
    struct field* copy = src;

    while (copy) {
        if (!head) {
            head = new_field(copy->name, copy_type(copy->type));
            curr = head;
            copy = copy->next;
        } else {
            curr->next = new_field(copy->name, copy_type(copy->type));
            curr       = curr->next;
            copy       = copy->next;
        }
    }

    return head;
}

void set_field_name(struct field* field, char* name) {
    if (field->name != NULL) {
        free(field->name);
    }

    field->name = copy_str(name);
}

struct table_item* new_table_item(struct field* field) {
    struct table_item* item =
        (struct table_item*)malloc(sizeof(struct table_item));

    item->field = field;
    item->next  = NULL;

    return item;
}

int is_struct(struct table_item* item) {
    if (item == NULL)
        return 0;
    if (item->field->type->kind != STRUCTURE)
        return 0;
    if (item->field->type->u.structure.name)
        return 0;
    return 1;
}

struct sym_table* init_table() {
    struct sym_table* table =
        (struct sym_table*)malloc(sizeof(struct sym_table));

    table->hash_array = (struct table_item**)malloc(sizeof(struct table_item*) *
                                                    SYM_TABLE_SIZE);
    for (int i = 0; i < SYM_TABLE_SIZE; i++) {
        table->hash_array[i] = NULL;
    }

    return table;
};

unsigned get_hash_code(char* str) {
    unsigned hash = 0;

    for (; *str; ++str) {
        hash = (hash << 2) + *str;

        unsigned i;
        if ((i = hash & ~SYM_TABLE_SIZE))
            hash = (hash ^ (i >> 12)) & SYM_TABLE_SIZE;
    }

    return hash;
}

struct table_item* get_table_item(struct sym_table* table, char* name) {
    unsigned           hash_code = get_hash_code(name);
    struct table_item* item_head = table->hash_array[hash_code];

    while (item_head) {
        if (!strcmp(item_head->field->name, name)) {
            return item_head;
        }

        item_head = item_head->next;
    }

    return NULL;
}

int is_table_item_redefined(struct sym_table* table, struct table_item* item) {
    struct table_item* temp = get_table_item(table, item->field->name);

    while (temp) {
        if (!strcmp(temp->field->name, item->field->name)) {
            return 1;
        }

        temp = temp->next;
    }

    return 0;
}

void add_table_item(struct sym_table* table, struct table_item* item) {
    unsigned hash_code = get_hash_code(item->field->name);

    item->next                   = table->hash_array[hash_code];
    table->hash_array[hash_code] = item;
}

void ExtDef(struct node* node) {
    // ExtDef : Specifier ExtDecList SEMI
    //        | Specifier SEMI
    //        | Specifier FunDec CompSt

    struct type* specifierType     = Specifier(node->left_child);
    char*        second_child_name = node->left_child->right_sibling->name;

    if (!strcmp(second_child_name, "ExtDecList")) {
        // ExtDef : Specifier ExtDecList SEMI

        ExtDecList(node->left_child->right_sibling, specifierType);
    } else if (!strcmp(second_child_name, "FunDec")) {
        // ExtDef : Specifier FunDec CompSt

        FunDec(node->left_child->right_sibling, specifierType);
        CompSt(node->left_child->right_sibling->right_sibling, specifierType);
    }
}

void ExtDecList(struct node* node, struct type* specifier) {
    // ExtDecList : VarDec
    //            | VarDec COMMA ExtDecList

    struct node* temp = node;
    while (temp) {
        struct table_item* item = VarDec(temp->left_child, specifier);

        if (is_table_item_redefined(table, item)) {
            char msg[100] = {0};
            sprintf(msg, "Redefined variable \"%s\".", item->field->name);
            print_error(REDEFINE_VAR, temp->lineno, msg);
        } else {
            add_table_item(table, item);
        }

        if (temp->left_child->right_sibling) {
            // ExtDecList
            //     |
            //     v
            //     VarDec COMMA ExtDecList
            //     (temp)           |
            //                      v
            //                      VarDec COMMA ExtDecList
            //                    (new temp)
            temp = temp->right_sibling->right_sibling->left_child;
        } else {
            break;
        }
    }
}

struct type* Specifier(struct node* node) {
    // Specifier : TYPE
    //           | StructSpecifier

    struct node* lc = node->left_child;

    if (lc->token == TOK_TYPE) {
        // Specifier : TYPE

        if (!strcmp(lc->name, "float")) {
            return new_type(BASIC, FLOAT_TYPE);
        } else {
            return new_type(BASIC, INT_TYPE);
        }
    } else {
        // Specifier : StructSpecifier

        return StructSpecifier(lc);
    }
}

struct type* StructSpecifier(struct node* node) {
    // StructSpecifier : STRUCT OptTag LC DefList RC
    //                 | STRUCT Tag

    struct node* second_child = node->left_child->right_sibling;
    struct type* return_type  = NULL;

    if (strcmp(second_child->name, "Tag")) {
        // StructSpecifier : STRUCT OptTag LC DefList RC

        struct table_item* struct_item =
            new_table_item(new_field("", new_type(STRUCTURE, NULL, NULL)));

        // OptTag -> ID | empty
        // Tag -> ID

        if (!strcmp(second_child->name, "OptTag")) {
            set_field_name(struct_item->field, second_child->left_child->name);
            second_child = second_child->right_sibling;
        }

        if (!strcmp(second_child->right_sibling->name, "DefList")) {
            DefList(second_child->right_sibling, struct_item);
        }

        if (is_table_item_redefined(table, struct_item)) {
            char msg[100] = {0};
            sprintf(msg, "Duplicated name \"%s\".", struct_item->field->name);
            print_error(DUPLICATED_NAME, node->lineno, msg);
        } else {
            return_type = new_type(
                STRUCTURE, copy_str(struct_item->field->name),
                copy_field(struct_item->field->type->u.structure.field));

            if (!strcmp(node->left_child->right_sibling->name, "OptTag")) {
                add_table_item(table, struct_item);
            }
        }
    } else {
        // StructSpecifier : STRUCT Tag

        struct table_item* struct_item =
            get_table_item(table, second_child->left_child->name);

        if (struct_item == NULL || !is_struct(struct_item)) {
            char msg[100] = {0};
            sprintf(msg, "Undefined structure \"%s\".",
                    second_child->left_child->name);
            print_error(UNDEFINE_STRUCT, node->lineno, msg);
        } else {
            return_type = new_type(
                STRUCTURE, copy_str(struct_item->field->name),
                copy_field(struct_item->field->type->u.structure.field));
        }
    }

    return return_type;
}

struct table_item* VarDec(struct node* node, struct type* specifier) {
    // VarDec : ID
    //        | VarDec LB INT RB

    // 获取到 ID
    struct node* id = node;
    while (id->left_child) {
        id = id->left_child;
    }

    struct table_item* item = new_table_item(new_field(id->name, NULL));

    if (node->left_child->token == TOK_ID) {
        // VarDec : ID

        item->field->type = copy_type(specifier);
    } else {
        // VarDec : VarDec LB INT RB

        struct node* vardec = node->left_child;
        struct type* temp   = specifier;

        while (vardec->right_sibling) {
            item->field->type =
                new_type(ARRAY, copy_type(temp),
                         atoi(vardec->right_sibling->right_sibling->name));
            temp   = item->field->type;
            vardec = vardec->left_child;
        }
    }

    return item;
}

void FunDec(struct node* node, struct type* return_type) {
    // FunDec : ID LP VarList RP
    //        | ID LP RP

    struct table_item* item = new_table_item(
        new_field(node->left_child->name,
                  new_type(FUNCTION, 0, NULL, copy_type(return_type))));

    if (!strcmp(node->left_child->right_sibling->right_sibling->name,
                "VarList")) {
        // FunDec : ID LP VarList RP

        VarList(node->left_child->right_sibling->right_sibling, item);
    }

    // FunDec : ID LP RP 不需要处理

    // 检查重定义
    if (is_table_item_redefined(table, item)) {
        char msg[100] = {0};
        sprintf(msg, "Redefined function \"%s\".", item->field->name);
        print_error(REDEFINE_FUNC, node->lineno, msg);
        item = NULL;
    } else {
        add_table_item(table, item);
    }
}

void VarList(struct node* node, struct table_item* func_item) {
    // VarList : ParamDec COMMA VarList
    //         | ParamDec

    int           argc = 0;
    struct node*  lc   = node->left_child;
    struct field* curr = NULL;

    // VarList : ParamDec
    struct field* paramdec                  = ParamDec(lc);
    func_item->field->type->u.function.argv = copy_field(paramdec);
    curr = func_item->field->type->u.function.argv;
    argc++;

    // VarList : ParamDec COMMA VarList
    while (lc->right_sibling) {
        lc       = lc->right_sibling->right_sibling->left_child;
        paramdec = ParamDec(lc);

        if (paramdec) {
            curr->next = copy_field(paramdec);
            curr       = curr->next;
            argc++;
        }
    }

    func_item->field->type->u.function.argc = argc;
}

struct field* ParamDec(struct node* node) {
    // ParamDec : Specifier VarDec

    struct type*       specifier_type = Specifier(node->left_child);
    struct table_item* item =
        VarDec(node->left_child->right_sibling, specifier_type);

    if (is_table_item_redefined(table, item)) {
        char msg[100] = {0};
        sprintf(msg, "Redefined variable \"%s\".", item->field->name);
        print_error(REDEFINE_VAR, node->lineno, msg);
        return NULL;
    } else {
        add_table_item(table, item);
        return item->field;
    }
}

void CompSt(struct node* node, struct type* return_type) {
    // CompSt : LC DefList StmtList RC

    struct node* temp = node->left_child->right_sibling;

    if (!strcmp(temp->name, "DefList")) {
        DefList(temp, NULL);
        temp = temp->right_sibling;
    }

    if (!strcmp(temp->name, "StmtList")) {
        StmtList(temp, return_type);
    }
}

void StmtList(struct node* node, struct type* return_type) {
    // StmtList : Stmt StmtList
    //          | empty

    while (node) {
        Stmt(node->left_child, return_type);
        node = node->left_child->right_sibling;
    }
}

void Stmt(struct node* node, struct type* return_type) {
    // Stmt : Exp SEMI
    //      | CompSt
    //      | RETURN Exp SEMI
    //      | IF LP Exp RP Stmt
    //      | IF LP Exp RP Stmt ELSE Stmt
    //      | WHILE LP Exp RP Stmt

    struct type* exp_type = NULL;

    if (!strcmp(node->left_child->name, "Exp")) {
        // Stmt : Exp SEMI

        exp_type = Exp(node->left_child);
    } else if (!strcmp(node->left_child->name, "CompSt")) {
        // Stmt : CompSt

        CompSt(node->left_child, return_type);
    } else if (!strcmp(node->left_child->name, "RETURN")) {
        // Stmt : RETURN Exp SEMI

        exp_type = Exp(node->left_child->right_sibling);

        if (!is_type_same(return_type, exp_type)) {
            print_error(TYPE_MISMATCH_RETURN, node->lineno,
                        "Type mismatched for return.");
        }
    } else if (!strcmp(node->left_child->name, "IF")) {
        // Stmt : IF LP Exp RP Stmt

        struct node* stmt = node->left_child->right_sibling->right_sibling
                                ->right_sibling->right_sibling;
        exp_type = Exp(node->left_child->right_sibling->right_sibling);
        Stmt(stmt, return_type);

        if (stmt->right_sibling != NULL) {
            // Stmt : IF LP Exp RP Stmt ELSE Stmt

            Stmt(stmt->right_sibling->right_sibling, return_type);
        }
    } else if (!strcmp(node->left_child->name, "WHILE")) {
        // Stmt : WHILE LP Exp RP Stmt

        exp_type = Exp(node->left_child->right_sibling->right_sibling);
        Stmt(node->left_child->right_sibling->right_sibling->right_sibling
                 ->right_sibling,
             return_type);
    }
}

void DefList(struct node* node, struct table_item* struct_item) {
    // DefList : Def DefList
    //         | empty

    while (node) {
        Def(node->left_child, struct_item);
        node = node->left_child->right_sibling;
    }
}

void Def(struct node* node, struct table_item* struct_item) {
    // Def : Specifier DecList SEMI

    struct type* dectype = Specifier(node->left_child);
    DecList(node->left_child->right_sibling, dectype, struct_item);
}

void DecList(struct node* node, struct type* specifier,
             struct table_item* struct_item) {
    // DecList : Dec
    //         | Dec COMMA DecList

    struct node* temp = node;
    while (temp) {
        Dec(temp->left_child, specifier, struct_item);
        if (temp->left_child->right_sibling) {
            temp = temp->left_child->right_sibling->right_sibling;
        } else {
            break;
        }
    }
}

void Dec(struct node* node, struct type* specifier,
         struct table_item* struct_info) {
    // Dec : VarDec
    //     | VarDec ASSIGNOP Exp

    if (!node->left_child->right_sibling) {
        // Dec : VarDec

        if (struct_info != NULL) {
            struct table_item* dec_item = VarDec(node->left_child, specifier);
            struct field*      payload  = dec_item->field;
            struct field*      struct_field =
                struct_info->field->type->u.structure.field;
            struct field* last = NULL;

            while (struct_field) {
                if (!strcmp(payload->name, struct_field->name)) {
                    //出现重定义，报错
                    char msg[100] = {0};
                    sprintf(msg, "Redefined field \"%s\".",
                            dec_item->field->name);
                    print_error(REDEFINE_FEILD, node->lineno, msg);
                    return;
                } else {
                    last         = struct_field;
                    struct_field = struct_field->next;
                }
            }

            if (last == NULL) {
                struct_info->field->type->u.structure.field =
                    copy_field(dec_item->field);
            } else {
                last->next = copy_field(dec_item->field);
            }
        } else {
            struct table_item* decitem = VarDec(node->left_child, specifier);
            if (is_table_item_redefined(table, decitem)) {
                // 出现冲突，报错
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->field->name);
                print_error(REDEFINE_VAR, node->lineno, msg);
            } else {
                add_table_item(table, decitem);
            }
        }
    } else {
        // Dec : VarDec ASSIGNOP Exp

        if (struct_info != NULL) {
            // 结构体内不能赋值，报错
            print_error(REDEFINE_FEILD, node->lineno,
                        "Illegal initialize variable in struct.");
        } else {
            // 判断赋值类型是否相符
            // 如果成功，注册该符号
            struct table_item* dec_item = VarDec(node->left_child, specifier);
            struct type*       exp_type =
                Exp(node->left_child->right_sibling->right_sibling);
            if (is_table_item_redefined(table, dec_item)) {
                // 出现冲突，报错
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        dec_item->field->name);
                print_error(REDEFINE_VAR, node->lineno, msg);
            }
            if (!is_type_same(dec_item->field->type, exp_type)) {
                // 类型不相符，报错
                print_error(TYPE_MISMATCH_ASSIGN, node->lineno,
                            "struct type mis matched for assignment.");
            }
            if (dec_item->field->type && dec_item->field->type->kind == ARRAY) {
                // 报错，对非 BASIC 类型赋值
                print_error(TYPE_MISMATCH_ASSIGN, node->lineno,
                            "Illegal initialize variable.");
            } else {
                add_table_item(table, dec_item);
            }
        }
    }
}

struct type* Exp(struct node* node) {
    // Exp : Exp ASSIGNOP Exp
    //     | Exp AND Exp
    //     | Exp OR Exp
    //     | Exp RELOP Exp
    //     | Exp PLUS Exp
    //     | Exp MINUS Exp
    //     | Exp STAR Exp
    //     | Exp DIV Exp
    //     | Exp LB Exp RB
    //     | Exp DOT ID
    //     | LP Exp RP
    //     | MINUS Exp
    //     | NOT Exp
    //     | ID LP Args RP
    //     | ID LP RP
    //     | ID
    //     | INT
    //     | FLOAT

    struct node* lc = node->left_child;

    if (!strcmp(lc->name, "Exp")) {
        // Exp : Exp ASSIGNOP Exp
        //     | Exp AND Exp
        //     | Exp OR Exp
        //     | Exp RELOP Exp
        //     | Exp PLUS Exp
        //     | Exp MINUS Exp
        //     | Exp STAR Exp
        //     | Exp DIV Exp
        //     | Exp LB Exp RB
        //     | Exp DOT ID

        if (strcmp(lc->right_sibling->name, "LB") &&
            strcmp(lc->right_sibling->name, "DOT")) {
            // Exp : Exp ASSIGNOP Exp
            //     | Exp AND Exp
            //     | Exp OR Exp
            //     | Exp RELOP Exp
            //     | Exp PLUS Exp
            //     | Exp MINUS Exp
            //     | Exp STAR Exp
            //     | Exp DIV Exp

            struct type* expl        = Exp(lc);
            struct type* expr        = Exp(lc->right_sibling->right_sibling);
            struct type* return_type = NULL;

            if (!strcmp(lc->right_sibling->name, "ASSIGNOP")) {
                // Exp : Exp ASSIGNOP Exp

                // 检查左值
                struct node* lc_lc = lc->left_child;
                if (lc_lc->token == TOK_FLOAT || lc_lc->token == TOK_INT) {
                    // 报错，左值
                    print_error(LEFT_VAR_ASSIGN, lc->lineno,
                                "The left-hand side of an assignment must be "
                                "a variable.");
                } else if (lc_lc->token == TOK_ID ||
                           !strcmp(lc_lc->right_sibling->name, "LB") ||
                           !strcmp(lc_lc->right_sibling->name, "DOT")) {
                    if (!is_type_same(expl, expr)) {
                        // 报错，类型不匹配
                        print_error(TYPE_MISMATCH_ASSIGN, lc->lineno,
                                    "Type mismatched for assignment.");
                    } else
                        return_type = copy_type(expl);
                } else {
                    //报错，左值
                    print_error(LEFT_VAR_ASSIGN, lc->lineno,
                                "The left-hand side of an assignment must be "
                                "a variable.");
                }
            } else {
                // Exp : Exp AND Exp
                //     | Exp OR Exp
                //     | Exp RELOP Exp
                //     | Exp PLUS Exp
                //     | Exp MINUS Exp
                //     | Exp STAR Exp
                //     | Exp DIV Exp

                if (expl && expr &&
                    (expl->kind == ARRAY || expr->kind == ARRAY)) {
                    // 报错，数组，结构体运算
                    print_error(TYPE_MISMATCH_OP, lc->lineno,
                                "Type mismatched for operands.");
                } else if (!is_type_same(expl, expr)) {
                    // 报错，类型不匹配
                    print_error(TYPE_MISMATCH_OP, lc->lineno,
                                "Type mismatched for operands.");
                } else {
                    if (expl && expr) {
                        return_type = copy_type(expl);
                    }
                }
            }

            return return_type;
        } else {
            if (!strcmp(lc->right_sibling->name, "LB")) {
                // Exp -> Exp LB Exp RB

                struct type* arr_name  = Exp(lc);
                struct type* arr_index = Exp(lc->right_sibling->right_sibling);
                struct type* return_type = NULL;

                if (arr_name && arr_name->kind != ARRAY) {
                    // 报错，非数组使用 [] 运算符
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an array.",
                            lc->left_child->name);
                    print_error(NOT_A_ARRAY, lc->lineno, msg);
                } else if (!arr_index || arr_index->kind != BASIC ||
                           arr_index->u.basic != INT_TYPE) {
                    // 报错，不用 int 索引 []
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an integer.",
                            lc->right_sibling->right_sibling->left_child->name);
                    print_error(NOT_A_INT, lc->lineno, msg);
                } else {
                    return_type = copy_type(arr_name->u.array.element_type);
                }

                return return_type;
            } else {
                // Exp -> Exp DOT ID

                struct type* struct_type = Exp(lc);
                struct type* return_type = NULL;

                if (!struct_type || struct_type->kind != STRUCTURE ||
                    !struct_type->u.structure.name) {
                    // 报错，对非结构体使用 "." 运算符
                    print_error(ILLEGAL_USE_DOT, lc->lineno,
                                "Illegal use of \".\".");
                } else {
                    struct node* struct_member =
                        lc->right_sibling->right_sibling;
                    struct field* struct_field = struct_type->u.structure.field;

                    while (struct_field != NULL) {
                        if (!strcmp(struct_field->name, struct_member->name)) {
                            break;
                        }
                        struct_field = struct_field->next;
                    }
                    if (struct_field == NULL) {
                        // 报错，没有可以匹配的域
                        char msg[100] = {0};
                        sprintf(msg, "Non-existent field \"%s\".",
                                struct_member->name);
                        print_error(NONEXISTFIELD, lc->lineno, msg);
                    } else {
                        return_type = copy_type(struct_field->type);
                    }
                }

                return return_type;
            }
        }
    } else if (!strcmp(lc->name, "MINUS") || !strcmp(lc->name, "NOT")) {
        // Exp : MINUS Exp
        //     | NOT Exp

        struct type* simgle_exp  = Exp(lc->right_sibling);
        struct type* return_type = NULL;

        if (!simgle_exp || simgle_exp->kind != BASIC) {
            // 报错，数组，结构体运算
            printf("Error type %d at Line %d: %s.\n", 7, lc->lineno,
                   "TYPE_MISMATCH_OP");
        } else {
            return_type = copy_type(simgle_exp);
        }

        return return_type;
    } else if (!strcmp(lc->name, "LP")) {
        // Exp : LP Exp RP
        return Exp(lc->right_sibling);
    } else if (lc->token == TOK_ID && lc->right_sibling) {
        // Exp : ID LP Args RP
        //     | ID LP RP

        struct table_item* func_item = get_table_item(table, lc->name);

        // 没有这个函数
        if (func_item == NULL) {
            char msg[100] = {0};
            sprintf(msg, "Undefined function \"%s\".", lc->name);
            print_error(UNDEFINE_FUNC, node->lineno, msg);
            return NULL;
        } else if (func_item->field->type->kind != FUNCTION) {
            char msg[100] = {0};
            sprintf(msg, "\"i\" is not a function.", lc->name);
            print_error(NOT_A_FUNC, node->lineno, msg);
            return NULL;
        } else if (!strcmp(lc->right_sibling->right_sibling->name, "Args")) {
            // Exp : ID LP Args RP

            Args(lc->right_sibling->right_sibling, func_item);
            return copy_type(func_item->field->type->u.function.return_type);
        } else {
            // Exp : ID LP RP

            if (func_item->field->type->u.function.argc != 0) {
                char msg[100] = {0};
                sprintf(msg,
                        "too few arguments to function \"%s\", except %d args.",
                        func_item->field->name,
                        func_item->field->type->u.function.argc);
                print_error(FUNC_AGRC_MISMATCH, node->lineno, msg);
            }
            return copy_type(func_item->field->type->u.function.return_type);
        }
    } else if (lc->token == TOK_ID) {
        // Exp : ID

        struct table_item* item = get_table_item(table, lc->name);
        if (item == NULL || is_struct(item)) {
            char msg[100] = {0};
            sprintf(msg, "Undefined variable \"%s\".", lc->name);
            print_error(UNDEFINE_VAR, lc->lineno, msg);
            return NULL;
        } else {
            return copy_type(item->field->type);
        }
    } else {
        if (lc->token == TOK_FLOAT) {
            // Exp : FLOAT

            return new_type(BASIC, FLOAT_TYPE);
        } else {
            // Exp : INT

            return new_type(BASIC, INT_TYPE);
        }
    }
}

void Args(struct node* node, struct table_item* func_item) {
    // Args : Exp COMMA Args
    //      | Exp

    struct node*  temp = node;
    struct field* argv = func_item->field->type->u.function.argv;

    while (temp) {
        if (argv == NULL) {
            print_func_error(node, func_item);
            break;
        }

        struct type* arg_type = Exp(temp->left_child);

        if (!is_type_same(arg_type, argv->type)) {
            char msg[100] = {0};
            sprintf(msg, "Function \"%s\" is not applicable for arguments.",
                    func_item->field->name);
            print_error(FUNC_AGRC_MISMATCH, node->lineno, msg);
            return;
        }

        argv = argv->next;
        if (temp->left_child->right_sibling) {
            // (temp)
            //  Args
            //   |
            //   v       (new temp)
            //  Exp COMMA   Args
            //               |
            //               v
            //              ...
            temp = temp->left_child->right_sibling->right_sibling;
        } else {
            break;
        }
    }

    if (argv != NULL) {
        print_func_error(node, func_item);
    }
}

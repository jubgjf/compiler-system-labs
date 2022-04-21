#include "inter.h"
#include "tree.h"
#include <stdlib.h>

/// 中间代码生成是否出现错误
int error_iter = 0;

/// 中间代码链表
struct inter_code_list* inter_code_list;

extern struct sym_table* table;

struct operand* new_operand(int kind, ...) {
    struct operand* op = (struct operand*)malloc(sizeof(struct operand));

    op->kind = kind;

    va_list vl;
    va_start(vl, 1);
    switch (kind) {
    case OPA_CONSTANT:
        op->u.value = va_arg(vl, int);
        break;
    case OPA_VARIABLE:
    case OPA_ADDRESS:
    case OPA_LABEL:
    case OPA_FUNCTION:
    case OPA_RELOP:
        op->u.name = va_arg(vl, char*);
        break;
    }

    return op;
}

void set_operand(struct operand* operand, int kind, void* v) {
    operand->kind = kind;

    switch (kind) {
    case OPA_CONSTANT:
        operand->u.value = *((int*)v);
        break;
    case OPA_VARIABLE:
    case OPA_ADDRESS:
    case OPA_LABEL:
    case OPA_FUNCTION:
    case OPA_RELOP:
        operand->u.name = (char*)v;
        break;
    }
}

void print_operand(FILE* fp, struct operand* operand) {
    switch (operand->kind) {
    case OPA_CONSTANT:
        fprintf(fp, "#%d", operand->u.value);
        break;
    case OPA_VARIABLE:
    case OPA_ADDRESS:
    case OPA_LABEL:
    case OPA_FUNCTION:
    case OPA_RELOP:
        fprintf(fp, "%s", operand->u.name);
        break;
    }
}

struct inter_code* new_iter_code(int kind, ...) {
    struct inter_code* ir = malloc(sizeof(struct inter_code));

    ir->kind = kind;

    va_list vl;
    switch (kind) {
        // 只有一个操作数
    case IR_LABEL:
    case IR_FUNCTION:
    case IR_GOTO:
    case IR_RETURN:
    case IR_ARG:
    case IR_PARAM:
    case IR_READ:
    case IR_WRITE:
        va_start(vl, 1);
        ir->u.one_op.op = va_arg(vl, struct operand*);
        break;

        // 赋值语句
    case IR_ASSIGN:
    case IR_GET_ADDR:
    case IR_READ_ADDR:
    case IR_WRITE_ADDR:
    case IR_CALL:
        va_start(vl, 2);
        ir->u.assign.left  = va_arg(vl, struct operand*);
        ir->u.assign.right = va_arg(vl, struct operand*);
        break;

        // 两个操作数，一个结果
    case IR_ADD:
    case IR_SUB:
    case IR_MUL:
    case IR_DIV:
        va_start(vl, 3);
        ir->u.bin_op.result = va_arg(vl, struct operand*);
        ir->u.bin_op.op1    = va_arg(vl, struct operand*);
        ir->u.bin_op.op2    = va_arg(vl, struct operand*);
        break;

        // 条件跳转
    case IR_IF_GOTO:
        va_start(vl, 4);
        ir->u.cjmp.op1    = va_arg(vl, struct operand*);
        ir->u.cjmp.relop  = va_arg(vl, struct operand*);
        ir->u.cjmp.op2    = va_arg(vl, struct operand*);
        ir->u.cjmp.target = va_arg(vl, struct operand*);
        break;

        // 数组及其字节长度
    case IR_DEC:
        va_start(vl, 2);
        ir->u.dec.op   = va_arg(vl, struct operand*);
        ir->u.dec.size = va_arg(vl, int);
        break;
    }

    return ir;
}

void print_iter_code(FILE* fp, struct inter_code_list* inter_code_list) {
    for (struct inter_codes* cur = inter_code_list->head; cur != NULL;
         // TODO fix type
         cur = cur->next) {
        switch (cur->code->kind) {
        case IR_LABEL:
            fprintf(fp, "LABEL ");
            print_operand(fp, cur->code->u.one_op.op);
            fprintf(fp, " :");
            break;
        case IR_FUNCTION:
            fprintf(fp, "FUNCTION ");
            print_operand(fp, cur->code->u.one_op.op);
            fprintf(fp, " :");
            break;
        case IR_ASSIGN:
            print_operand(fp, cur->code->u.assign.left);
            fprintf(fp, " := ");
            print_operand(fp, cur->code->u.assign.right);
            break;
        case IR_ADD:
            print_operand(fp, cur->code->u.bin_op.result);
            fprintf(fp, " := ");
            print_operand(fp, cur->code->u.bin_op.op1);
            fprintf(fp, " + ");
            print_operand(fp, cur->code->u.bin_op.op2);
            break;
        case IR_SUB:
            print_operand(fp, cur->code->u.bin_op.result);
            fprintf(fp, " := ");
            print_operand(fp, cur->code->u.bin_op.op1);
            fprintf(fp, " - ");
            print_operand(fp, cur->code->u.bin_op.op2);
            break;
        case IR_MUL:
            print_operand(fp, cur->code->u.bin_op.result);
            fprintf(fp, " := ");
            print_operand(fp, cur->code->u.bin_op.op1);
            fprintf(fp, " * ");
            print_operand(fp, cur->code->u.bin_op.op2);
            break;
        case IR_DIV:
            print_operand(fp, cur->code->u.bin_op.result);
            fprintf(fp, " := ");
            print_operand(fp, cur->code->u.bin_op.op1);
            fprintf(fp, " / ");
            print_operand(fp, cur->code->u.bin_op.op2);
            break;
        case IR_GET_ADDR:
            print_operand(fp, cur->code->u.assign.left);
            fprintf(fp, " := &");
            print_operand(fp, cur->code->u.assign.right);
            break;
        case IR_READ_ADDR:
            print_operand(fp, cur->code->u.assign.left);
            fprintf(fp, " := *");
            print_operand(fp, cur->code->u.assign.right);
            break;
        case IR_WRITE_ADDR:
            fprintf(fp, "*");
            print_operand(fp, cur->code->u.assign.left);
            fprintf(fp, " := ");
            print_operand(fp, cur->code->u.assign.right);
            break;
        case IR_GOTO:
            fprintf(fp, "GOTO ");
            print_operand(fp, cur->code->u.one_op.op);
            break;
        case IR_IF_GOTO:
            fprintf(fp, "IF ");
            print_operand(fp, cur->code->u.cjmp.op1);
            fprintf(fp, " ");
            print_operand(fp, cur->code->u.cjmp.relop);
            fprintf(fp, " ");
            print_operand(fp, cur->code->u.cjmp.op2);
            fprintf(fp, " GOTO ");
            print_operand(fp, cur->code->u.cjmp.target);
            break;
        case IR_RETURN:
            fprintf(fp, "RETURN ");
            print_operand(fp, cur->code->u.one_op.op);
            break;
        case IR_DEC:
            fprintf(fp, "DEC ");
            print_operand(fp, cur->code->u.dec.op);
            fprintf(fp, " ");
            fprintf(fp, "%d", cur->code->u.dec.size);
            break;
        case IR_ARG:
            fprintf(fp, "ARG ");
            print_operand(fp, cur->code->u.one_op.op);
            break;
        case IR_CALL:
            print_operand(fp, cur->code->u.assign.left);
            fprintf(fp, " := CALL ");
            print_operand(fp, cur->code->u.assign.right);
            break;
        case IR_PARAM:
            fprintf(fp, "PARAM ");
            print_operand(fp, cur->code->u.one_op.op);
            break;
        case IR_READ:
            fprintf(fp, "READ ");
            print_operand(fp, cur->code->u.one_op.op);
            break;
        case IR_WRITE:
            fprintf(fp, "WRITE ");
            print_operand(fp, cur->code->u.one_op.op);
            break;
        }

        fprintf(fp, "\n");
    }
}

struct inter_codes* new_iter_codes(struct inter_code* inter_code) {
    struct inter_codes* node = malloc(sizeof(struct inter_codes));

    node->code = inter_code;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

struct arg* new_arg(struct operand* operand) {
    struct arg* node = (struct arg*)malloc(sizeof(struct arg));

    node->op   = operand;
    node->next = NULL;

    return node;
}

struct arg_list* new_arg_list() {
    struct arg_list* node = (struct arg_list*)malloc(sizeof(struct arg_list));

    node->head = NULL;
    node->tail = NULL;

    return node;
}

void add_arg(struct arg_list* arg_list, struct arg* arg) {
    if (arg_list->head == NULL) {
        arg_list->head = arg;
        arg_list->tail = arg;
    } else {
        arg_list->tail->next = arg;
        arg_list->tail       = arg;
    }
}

struct inter_code_list* new_inter_code_list() {
    struct inter_code_list* node = malloc(sizeof(struct inter_code_list));

    node->head            = NULL;
    node->tail            = NULL;
    node->last_array_name = NULL;
    node->temp_var_num    = 1;
    node->label_num       = 1;

    return node;
}

void add_inter_code(struct inter_code_list* inter_code_list,
                    struct inter_codes*     inter_codes) {
    if (inter_code_list->head == NULL) {
        // 链表为空
        inter_code_list->head = inter_codes;
        inter_code_list->tail = inter_codes;
    } else {
        // 链表非空，插入到末尾
        // TODO fix type
        inter_code_list->tail->next = inter_codes;
        inter_codes->prev           = inter_code_list->tail;
        inter_code_list->tail       = inter_codes;
    }
}

struct operand* new_tmp_var() {
    char t_name[10] = {0};
    sprintf(t_name, "t%d", inter_code_list->temp_var_num);
    inter_code_list->temp_var_num++;
    struct operand* temp = new_operand(OPA_VARIABLE, copy_str(t_name));

    return temp;
}

struct operand* new_label() {
    char table_name[10] = {0};
    sprintf(table_name, "label%d", inter_code_list->label_num);
    inter_code_list->label_num++;
    struct operand* temp = new_operand(OPA_LABEL, copy_str(table_name));

    return temp;
}

int get_size(struct type* type) {
    if (type == NULL) {
        return 0;
    } else if (type->kind == BASIC) {
        return 4;
    } else if (type->kind == ARRAY) {
        return type->u.array.size * get_size(type->u.array.element_type);
    } else if (type->kind == STRUCTURE) {
        int           size = 0;
        struct field* temp = type->u.structure.field;
        while (temp) {
            size += get_size(temp->type);
            temp = temp->right_sibling;
        }

        return size;
    } else {
        return 0;
    }
}

void gen_inter_codes(struct node* node) {
    if (node == NULL) {
        return;
    }

    if (!strcmp(node->name, "ExtDefList")) {
        translate_ExtDefList(node);
    } else {
        gen_inter_codes(node->left_child);
        gen_inter_codes(node->right_sibling);
    }
}

void gen_inter_code(int kind, ...) {
    struct operand* result = NULL;
    struct operand* op1    = NULL;
    struct operand* op2    = NULL;
    struct operand* relop  = NULL;

    struct operand*     tmp_var    = NULL;
    struct inter_codes* iter_codes = NULL;

    va_list vl;

    switch (kind) {
        // 只有一个操作数
    case IR_LABEL:
    case IR_FUNCTION:
    case IR_GOTO:
    case IR_RETURN:
    case IR_ARG:
    case IR_PARAM:
    case IR_READ:
    case IR_WRITE:
        va_start(vl, 1);
        op1 = va_arg(vl, struct operand*);
        if (op1->kind == OPA_ADDRESS) {
            tmp_var = new_tmp_var();
            gen_inter_code(IR_READ_ADDR, tmp_var, op1);
            op1 = tmp_var;
        }
        iter_codes = new_iter_codes(new_iter_code(kind, op1));
        add_inter_code(inter_code_list, iter_codes);
        break;

        // 赋值语句
    case IR_ASSIGN:
    case IR_GET_ADDR:
    case IR_READ_ADDR:
    case IR_WRITE_ADDR:
    case IR_CALL:
        va_start(vl, 2);
        op1 = va_arg(vl, struct operand*);
        op2 = va_arg(vl, struct operand*);
        if (kind == IR_ASSIGN &&
            (op1->kind == OPA_ADDRESS || op2->kind == OPA_ADDRESS)) {
            if (op1->kind == OPA_ADDRESS && op2->kind != OPA_ADDRESS)
                gen_inter_code(IR_WRITE_ADDR, op1, op2);
            else if (op2->kind == OPA_ADDRESS && op1->kind != OPA_ADDRESS)
                gen_inter_code(IR_READ_ADDR, op1, op2);
            else {
                tmp_var = new_tmp_var();
                gen_inter_code(IR_READ_ADDR, tmp_var, op2);
                gen_inter_code(IR_WRITE_ADDR, op1, tmp_var);
            }
        } else {
            iter_codes = new_iter_codes(new_iter_code(kind, op1, op2));
            add_inter_code(inter_code_list, iter_codes);
        }
        break;

        // 两个操作数，一个结果
    case IR_ADD:
    case IR_SUB:
    case IR_MUL:
    case IR_DIV:
        va_start(vl, 3);
        result = va_arg(vl, struct operand*);
        op1    = va_arg(vl, struct operand*);
        op2    = va_arg(vl, struct operand*);
        if (op1->kind == OPA_ADDRESS) {
            tmp_var = new_tmp_var();
            gen_inter_code(IR_READ_ADDR, tmp_var, op1);
            op1 = tmp_var;
        }
        if (op2->kind == OPA_ADDRESS) {
            tmp_var = new_tmp_var();
            gen_inter_code(IR_READ_ADDR, tmp_var, op2);
            op2 = tmp_var;
        }
        iter_codes = new_iter_codes(new_iter_code(kind, result, op1, op2));
        add_inter_code(inter_code_list, iter_codes);
        break;

        // 数组
    case IR_IF_GOTO:
        va_start(vl, 4);
        result = va_arg(vl, struct operand*);
        relop  = va_arg(vl, struct operand*);
        op1    = va_arg(vl, struct operand*);
        op2    = va_arg(vl, struct operand*);
        iter_codes =
            new_iter_codes(new_iter_code(kind, result, relop, op1, op2));
        add_inter_code(inter_code_list, iter_codes);
        break;

        // 条件跳转
    case IR_DEC:
        va_start(vl, 2);
        op1        = va_arg(vl, struct operand*);
        int size   = va_arg(vl, int);
        iter_codes = new_iter_codes(new_iter_code(kind, op1, size));
        add_inter_code(inter_code_list, iter_codes);
        break;
    }
}

void translate_Exp(struct node* node, struct operand* place) {
    if (error_iter) {
        return;
    }

    // Exp : Exp ASSIGNOP Exp
    //     | Exp AND Exp
    //     | Exp OR Exp
    //     | Exp RELOP Exp
    //     | Exp PLUS Exp
    //     | Exp MINUS Exp
    //     | Exp STAR Exp
    //     | Exp DIV Exp
    //     | MINUS Exp
    //     | NOT Exp
    //     | ID LP Args RP
    //     | ID LP RP
    //     | Exp LB Exp RB
    //     | Exp DOT ID
    //     | ID
    //     | INT
    //     | FLOAT

    if (!strcmp(node->left_child->name, "LP")) {
        // Exp -> LP Exp RP

        translate_Exp(node->left_child->right_sibling, place);
    } else if (!strcmp(node->left_child->name, "Exp") ||
               !strcmp(node->left_child->name, "NOT")) {
        if (strcmp(node->left_child->right_sibling->name, "LB") &&
            strcmp(node->left_child->right_sibling->name, "DOT")) {
            if (!strcmp(node->left_child->right_sibling->name, "AND") ||
                !strcmp(node->left_child->right_sibling->name, "OR") ||
                !strcmp(node->left_child->name, "NOT") ||
                node->left_child->right_sibling->token == TOK_RELOP) {
                // Exp : Exp AND Exp
                //     | Exp OR Exp
                //     | Exp RELOP Exp
                //     | NOT Exp

                struct operand* label1    = new_label();
                struct operand* label2    = new_label();
                struct operand* true_num  = new_operand(OPA_CONSTANT, 1);
                struct operand* false_num = new_operand(OPA_CONSTANT, 0);
                gen_inter_code(IR_ASSIGN, place, false_num);
                translate_Cond(node, label1, label2);
                gen_inter_code(IR_LABEL, label1);
                gen_inter_code(IR_ASSIGN, place, true_num);
            } else {
                if (!strcmp(node->left_child->right_sibling->name,
                            "ASSIGNOP")) {
                    // Exp -> Exp ASSIGNOP Exp

                    struct operand* t2 = new_tmp_var();
                    translate_Exp(
                        node->left_child->right_sibling->right_sibling, t2);
                    struct operand* t1 = new_tmp_var();
                    translate_Exp(node->left_child, t1);
                    gen_inter_code(IR_ASSIGN, t1, t2);
                } else {
                    struct operand* t1 = new_tmp_var();
                    translate_Exp(node->left_child, t1);
                    struct operand* t2 = new_tmp_var();
                    translate_Exp(
                        node->left_child->right_sibling->right_sibling, t2);
                    if (!strcmp(node->left_child->right_sibling->name,
                                "PLUS")) {
                        // Exp -> Exp PLUS Exp

                        gen_inter_code(IR_ADD, place, t1, t2);
                    } else if (!strcmp(node->left_child->right_sibling->name,
                                       "MINUS")) {
                        // Exp -> Exp MINUS Exp

                        gen_inter_code(IR_SUB, place, t1, t2);
                    } else if (!strcmp(node->left_child->right_sibling->name,
                                       "STAR")) {
                        // Exp -> Exp STAR Exp

                        gen_inter_code(IR_MUL, place, t1, t2);
                    } else if (!strcmp(node->left_child->right_sibling->name,
                                       "DIV")) {
                        // Exp -> Exp DIV Exp

                        gen_inter_code(IR_DIV, place, t1, t2);
                    }
                }
            }
        } else {
            if (!strcmp(node->left_child->right_sibling->name, "LB")) {
                // Exp -> Exp LB Exp RB

                if (node->left_child->left_child->right_sibling &&
                    !strcmp(node->left_child->left_child->right_sibling,
                            "LB")) {
                    // TODO fix
                    //多维数组，报错
                    error_iter = 1;
                    printf(
                        "Cannot translate: Code containsvariables of "
                        "multi-dimensional array type or parameters of array "
                        "type.\n");
                    return;
                } else {
                    struct operand* idx = new_tmp_var();
                    translate_Exp(
                        node->left_child->right_sibling->right_sibling, idx);
                    struct operand* base = new_tmp_var();
                    translate_Exp(node->left_child, base);

                    struct operand*    width;
                    struct operand*    offset = new_tmp_var();
                    struct operand*    target;
                    struct table_item* item =
                        get_table_item(table, base->u.name);

                    width = new_operand(
                        OPA_CONSTANT,
                        get_size(item->field->type->u.array.element_type));
                    gen_inter_code(IR_MUL, offset, idx, width);
                    if (base->kind == OPA_VARIABLE) {
                        target = new_tmp_var();
                        gen_inter_code(IR_GET_ADDR, target, base);
                    } else {
                        target = base;
                    }
                    gen_inter_code(IR_ADD, place, target, offset);
                    place->kind                      = OPA_ADDRESS;
                    inter_code_list->last_array_name = base->u.name;
                }
            } else {
                // Exp -> Exp DOT ID

                struct operand* temp = new_tmp_var();
                translate_Exp(node->left_child, temp);
                struct operand* target;

                if (temp->kind == OPA_ADDRESS) {
                    target = new_operand(temp->kind, temp->u.name);
                } else {
                    target = new_tmp_var();
                    gen_inter_code(IR_GET_ADDR, target, temp);
                }

                struct operand* id = new_operand(
                    OPA_VARIABLE,
                    copy_str(
                        node->left_child->right_sibling->right_sibling->name));
                int                offset = 0;
                struct table_item* item   = get_table_item(table, temp->u.name);
                if (item == NULL) {
                    item =
                        get_table_item(table, inter_code_list->last_array_name);
                }

                struct field* tmp;
                if (item->field->type->kind == ARRAY) {
                    tmp = item->field->type->u.array.element_type->u.structure
                              .field;
                } else {
                    tmp = item->field->type->u.structure.field;
                }

                while (tmp) {
                    if (!strcmp(tmp->name, id->u.name))
                        break;
                    offset += get_size(tmp->type);
                    tmp = tmp->right_sibling;
                }

                struct operand* tOffset = new_operand(OPA_CONSTANT, offset);
                if (place) {
                    gen_inter_code(IR_ADD, place, target, tOffset);
                    set_operand(place, OPA_ADDRESS,
                                (void*)copy_str(id->u.name));
                }
            }
        }
    } else if (!strcmp(node->left_child->name, "MINUS")) {
        // Exp -> MINUS Exp

        struct operand* t1 = new_tmp_var();
        translate_Exp(node->left_child->right_sibling, t1);
        struct operand* zero = new_operand(OPA_CONSTANT, 0);
        gen_inter_code(IR_SUB, place, zero, t1);
    } else if (node->left_child->token == TOK_ID &&
               node->left_child->right_sibling) {
        // Exp : ID LP Args RP
        //     | ID LP RP

        struct operand* func_temp =
            new_operand(OPA_FUNCTION, copy_str(node->left_child->name));

        if (!strcmp(node->left_child->right_sibling->right_sibling->name,
                    "Args")) {
            // Exp -> ID LP Args RP

            struct arg_list* arg_list = new_arg_list();
            translate_Args(node->left_child->right_sibling->right_sibling,
                           arg_list);
            if (!strcmp(node->left_child->name, "write")) {
                gen_inter_code(IR_WRITE, arg_list->head->op);
            } else {
                struct arg* argTemp = arg_list->head;
                while (argTemp) {
                    if (argTemp->op == OPA_VARIABLE) {
                        // TODO fix
                        struct table_item* item =
                            get_table_item(table, argTemp->op->u.name);

                        // 结构体作为参数需要传址
                        if (item && item->field->type->kind == STRUCTURE) {
                            struct operand* varTemp = new_tmp_var();
                            gen_inter_code(IR_GET_ADDR, varTemp, argTemp->op);
                            struct operand* varTempCopy =
                                new_operand(OPA_ADDRESS, varTemp->u.name);
                            gen_inter_code(IR_ARG, varTempCopy);
                        }
                    } else {
                        // 一般参数直接传值
                        gen_inter_code(IR_ARG, argTemp->op);
                    }
                    argTemp = argTemp->next;
                }
                if (place) {
                    gen_inter_code(IR_CALL, place, func_temp);
                } else {
                    struct operand* temp = new_tmp_var();
                    gen_inter_code(IR_CALL, temp, func_temp);
                }
            }
        } else {
            // Exp -> ID LP RP

            if (!strcmp(node->left_child->name, "read")) {
                gen_inter_code(IR_READ, place);
            } else {
                if (place) {
                    gen_inter_code(IR_CALL, place, func_temp);
                } else {
                    struct operand* temp = new_tmp_var();
                    gen_inter_code(IR_CALL, temp, func_temp);
                }
            }
        }
    } else if (node->left_child->token == TOK_ID) {
        // Exp -> ID

        struct table_item* item = get_table_item(table, node->left_child->name);
        // 结构体不允许传值而应该是传址
        inter_code_list->temp_var_num--;
        if (item->field->type->kind == STRUCTURE) {
            set_operand(place, OPA_ADDRESS,
                        (void*)copy_str(node->left_child->name));
        } else {
            set_operand(place, OPA_VARIABLE,
                        (void*)copy_str(node->left_child->name));
        }
    } else {
        // Exp -> INT

        inter_code_list->temp_var_num--;
        int* v = malloc(sizeof(int));
        *v     = atoi(node->left_child->name);
        set_operand(place, OPA_CONSTANT, (void*)v);
    }
}

void translate_Args(struct node* node, struct arg_list* arg_list) {
    if (error_iter) {
        return;
    }

    // Args : Exp COMMA Args
    //      | Exp

    // Args -> Exp
    struct arg* temp = new_arg(new_tmp_var());
    translate_Exp(node->left_child, temp->op);

    if (temp->op->kind == OPA_VARIABLE) {
        struct table_item* item = get_table_item(table, temp->op->u.name);
        if (item && item->field->type->kind == ARRAY) {
            error_iter = 1;
            printf("Cannot translate: Code containsvariables of "
                   "multi-dimensional array type or parameters of array "
                   "type.\n");
            return;
        }
    }
    add_arg(arg_list, temp);

    // Args -> Exp COMMA Args
    if (node->left_child->right_sibling != NULL) {
        translate_Args(node->left_child->right_sibling->right_sibling,
                       arg_list);
    }
}

void translate_Cond(struct node* node, struct operand* label_true,
                    struct operand* label_false) {
    if (error_iter) {
        return;
    }

    // Exp : Exp AND Exp
    //     | Exp OR Exp
    //     | Exp RELOP Exp
    //     | NOT Exp

    if (!strcmp(node->left_child->name, "NOT")) {
        // Exp -> NOT Exp

        translate_Cond(node->left_child->right_sibling, label_false,
                       label_true);
    } else if (node->left_child->right_sibling->token == TOK_RELOP) {
        // Exp -> Exp RELOP Exp

        struct operand* t1 = new_tmp_var();
        struct operand* t2 = new_tmp_var();
        translate_Exp(node->left_child, t1);
        translate_Exp(node->left_child->right_sibling->right_sibling, t2);

        struct operand* relop = new_operand(
            OPA_RELOP, copy_str(node->left_child->right_sibling->name));

        if (t1->kind == OPA_ADDRESS) {
            struct operand* temp = new_tmp_var();
            gen_inter_code(IR_READ_ADDR, temp, t1);
            t1 = temp;
        }
        if (t2->kind == OPA_ADDRESS) {
            struct operand* temp = new_tmp_var();
            gen_inter_code(IR_READ_ADDR, temp, t2);
            t2 = temp;
        }

        gen_inter_code(IR_IF_GOTO, t1, relop, t2, label_true);
        gen_inter_code(IR_GOTO, label_false);
    } else if (!strcmp(node->left_child->right_sibling->name, "AND")) {
        // Exp -> Exp AND Exp

        struct operand* label1 = new_label();
        translate_Cond(node->left_child, label1, label_false);
        gen_inter_code(IR_LABEL, label1);
        translate_Cond(node->left_child->right_sibling->right_sibling,
                       label_true, label_false);
    } else if (!strcmp(node->left_child->right_sibling->name, "OR")) {
        // Exp -> Exp OR Exp

        struct operand* label1 = new_label();
        translate_Cond(node->left_child, label_true, label1);
        gen_inter_code(IR_LABEL, label1);
        translate_Cond(node->left_child->right_sibling->right_sibling,
                       label_true, label_false);
    } else {
        struct operand* t1 = new_tmp_var();
        translate_Exp(node, t1);
        struct operand* t2    = new_operand(OPA_CONSTANT, 0);
        struct operand* relop = new_operand(OPA_RELOP, copy_str("!="));

        if (t1->kind == OPA_ADDRESS) {
            struct operand* temp = new_tmp_var();
            gen_inter_code(IR_READ_ADDR, temp, t1);
            t1 = temp;
        }
        gen_inter_code(IR_IF_GOTO, t1, relop, t2, label_true);
        gen_inter_code(IR_GOTO, label_false);
    }
}

void translate_VarDec(struct node* node, struct operand* place) {
    if (error_iter) {
        return;
    }

    // VarDec : ID
    //        | VarDec LB INT RB

    if (node->left_child->token == TOK_ID) {
        struct table_item* temp = get_table_item(table, node->left_child->name);
        struct type*       type = temp->field->type;
        if (type->kind == BASIC) {
            if (place) {
                inter_code_list->temp_var_num--;
                set_operand(place, OPA_VARIABLE,
                            (void*)copy_str(temp->field->name));
            }
        } else if (type->kind == ARRAY) {
            if (type->u.array.element_type->kind == ARRAY) {
                // 不允许出现高维数组
                error_iter = 1;
                printf("Cannot translate: Code containsvariables of "
                       "multi-dimensional array type or parameters of array "
                       "type.\n");
                return;
            } else {
                gen_inter_code(
                    IR_DEC,
                    new_operand(OPA_VARIABLE, copy_str(temp->field->name)),
                    get_size(type));
            }
        } else if (type->kind == STRUCTURE) {
            gen_inter_code(
                IR_DEC, new_operand(OPA_VARIABLE, copy_str(temp->field->name)),
                get_size(type));
        }
    } else {
        translate_VarDec(node->left_child, place);
    }
}

void translate_Dec(struct node* node) {
    if (error_iter) {
        return;
    }

    // Dec : VarDec
    //     | VarDec ASSIGNOP Exp

    if (node->left_child->right_sibling == NULL) {
        // Dec -> VarDec
        translate_VarDec(node->left_child, NULL);
    } else {
        // Dec -> VarDec ASSIGNOP Exp
        struct operand* t1 = new_tmp_var();
        translate_VarDec(node->left_child, t1);
        struct operand* t2 = new_tmp_var();
        translate_Exp(node->left_child->right_sibling->right_sibling, t2);
        gen_inter_code(IR_ASSIGN, t1, t2);
    }
}

void translate_DecList(struct node* node) {
    if (error_iter) {
        return;
    }

    // DecList : Dec
    //         | Dec COMMA DecList

    struct node* temp = node;
    while (temp) {
        translate_Dec(temp->left_child);
        if (temp->left_child->right_sibling) {
            temp = temp->left_child->right_sibling->right_sibling;
        } else {
            break;
        }
    }
}

void translate_Def(struct node* node) {
    if (error_iter) {
        return;
    }

    // Def : Specifier DecList SEMI
    translate_DecList(node->left_child->right_sibling);
}

void translate_DefList(struct node* node) {
    if (error_iter) {
        return;
    }

    // DefList : Def DefList
    //         | empty

    while (node) {
        translate_Def(node->left_child);
        node = node->left_child->right_sibling;
    }
}

void translate_CompSt(struct node* node) {
    if (error_iter) {
        return;
    }

    // CompSt : LC DefList StmtList RC

    struct node* temp = node->left_child->right_sibling;
    if (!strcmp(temp->name, "DefList")) {
        translate_DefList(temp);
        temp = temp->right_sibling;
    }
    if (!strcmp(temp->name, "StmtList")) {
        translate_StmtList(temp);
    }
}

void translate_Stmt(struct node* node) {
    if (error_iter) {
        return;
    }

    // Stmt : Exp SEMI
    //      | CompSt
    //      | RETURN Exp SEMI
    //      | IF LP Exp RP Stmt
    //      | IF LP Exp RP Stmt ELSE Stmt
    //      | WHILE LP Exp RP Stmt

    if (!strcmp(node->left_child->name, "Exp")) {
        // Stmt -> Exp SEMI
        translate_Exp(node->left_child, NULL);
    } else if (!strcmp(node->left_child->name, "CompSt")) {
        // Stmt -> CompSt
        translate_CompSt(node->left_child);
    } else if (!strcmp(node->left_child->name, "RETURN")) {
        // Stmt -> RETURN Exp SEMI
        struct operand* t1 = new_tmp_var();
        translate_Exp(node->left_child->right_sibling, t1);
        gen_inter_code(IR_RETURN, t1);
    } else if (!strcmp(node->left_child->name, "IF")) {
        // Stmt -> IF LP Exp RP Stmt
        struct node*    exp    = node->left_child->right_sibling->right_sibling;
        struct node*    stmt   = exp->right_sibling->right_sibling;
        struct operand* label1 = new_label();
        struct operand* label2 = new_label();

        translate_Cond(exp, label1, label2);
        gen_inter_code(IR_LABEL, label1);
        translate_Stmt(stmt);
        if (stmt->right_sibling == NULL) {
            gen_inter_code(IR_LABEL, label2);
        } else {
            // Stmt -> IF LP Exp RP Stmt ELSE Stmt
            struct operand* label3 = new_label();
            gen_inter_code(IR_GOTO, label3);
            gen_inter_code(IR_LABEL, label2);
            translate_Stmt(stmt->right_sibling->right_sibling);
            gen_inter_code(IR_LABEL, label3);
        }
    } else if (!strcmp(node->left_child->name, "WHILE")) {
        // Stmt -> WHILE LP Exp RP Stmt
        struct operand* label1 = new_label();
        struct operand* label2 = new_label();
        struct operand* label3 = new_label();

        gen_inter_code(IR_LABEL, label1);
        translate_Cond(node->left_child->right_sibling->right_sibling, label2,
                       label3);
        gen_inter_code(IR_LABEL, label2);
        translate_Stmt(node->left_child->right_sibling->right_sibling
                           ->right_sibling->right_sibling);
        gen_inter_code(IR_GOTO, label1);
        gen_inter_code(IR_LABEL, label3);
    }
}

void translate_StmtList(struct node* node) {
    if (error_iter) {
        return;
    }

    // StmtList : Stmt StmtList
    //          | empty

    while (node) {
        translate_Stmt(node->left_child);
        node = node->left_child->right_sibling;
    }
}

void translate_FunDec(struct node* node) {
    if (error_iter) {
        return;
    }

    // FunDec : ID LP VarList RP
    //        | ID LP RP

    // 生成函数名
    gen_inter_code(IR_FUNCTION,
                   new_operand(OPA_FUNCTION, copy_str(node->left_child->name)));

    struct table_item* func_item =
        get_table_item(table, node->left_child->name);

    // 生成参数中间代码
    struct field* temp = func_item->field->type->u.function.argv;
    while (temp) {
        gen_inter_code(IR_PARAM,
                       new_operand(OPA_VARIABLE, copy_str(temp->name)));
        temp = temp->right_sibling;
    }
}

void translate_ExtDef(struct node* node) {
    if (error_iter) {
        return;
    }

    // ExtDef : Specifier ExtDecList SEMI
    //        | Specifier SEMI
    //        | Specifier FunDec CompSt

    // 由于假设了没有全局变量，因此
    // ExtDef : Specifier ExtDecList SEMI
    // ExtDef : Specifier SEMI
    // 两种情况均不存在，只需处理
    // ExtDef : Specifier FunDec CompSt
    // 即可
    if (!strcmp(node->left_child->right_sibling->name, "FunDec")) {
        translate_FunDec(node->left_child->right_sibling);
        translate_CompSt(node->left_child->right_sibling->right_sibling);
    }
}

void translate_ExtDefList(struct node* node) {
    // ExtDefList : ExtDef ExtDefList
    //            | empty

    while (node) {
        translate_ExtDef(node->left_child);
        node = node->left_child->right_sibling;
    }
}

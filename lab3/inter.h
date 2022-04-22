#ifndef LAB3_INTER_H
#define LAB3_INTER_H

#include "semantic.h"
#include "tree.h"

/// 操作数
struct operand {
    /// 操作数类型
    enum {
        OPA_CONSTANT, // 常量
        OPA_VARIABLE, // 变量
        OPA_ADDRESS,  // 地址
        OPA_LABEL,    // 标签
        OPA_FUNCTION, // 函数
        OPA_RELOP,    // 比较运算
    } kind;

    /// 操作数具体值
    union {
        int   value; // kind == OPA_CONSTANT 时，这个常量的值
        char* name;  // kind == _            时，对应的变量/函数名/...
    } u;
};

/// 单条中间代码
struct inter_code {
    /// 中间代码类型
    enum {
        IR_LABEL,      // 标签
        IR_FUNCTION,   // 函数
        IR_ASSIGN,     // 赋值
        IR_ADD,        // 加
        IR_SUB,        // 减
        IR_MUL,        // 乘
        IR_DIV,        // 除
        IR_GET_ADDR,   // 取地址 x = &y
        IR_READ_ADDR,  // 解引用赋值 x = *y
        IR_WRITE_ADDR, // 写入地址 *x = y
        IR_GOTO,       // 无条件跳转
        IR_IF_GOTO,    // 有条件跳转
        IR_RETURN,     // 返回
        IR_DEC,        // 数组字节长度声明
        IR_ARG,        // 调用函数前给定函数参数
        IR_CALL,       // 函数调用
        IR_PARAM,      // 函数内接收函数参数
        IR_READ,       // 内置 read 函数
        IR_WRITE,      // 内置 write 函数
    } kind;

    /// 中间代码操作，选择 union 的哪一个由 kind 决定
    union {
        /// 只有一个操作数
        struct {
            struct operand* op;
        } one_op;

        /// 赋值语句
        struct {
            struct operand* right;
            struct operand* left;
        } assign;

        /// 两个操作数，一个结果
        struct {
            struct operand* result;
            struct operand* op1;
            struct operand* op2;
        } bin_op;

        /// 条件跳转
        struct {
            struct operand* op1;
            struct operand* relop;
            struct operand* op2;
            struct operand* target;
        } cjmp;

        /// 数组及其字节长度
        struct {
            struct operand* op;
            int             size;
        } dec;
    } u;
};

/// 所有中间代码组成的双向链表的一个节点
struct inter_codes {
    struct inter_code*  code;
    struct inter_codes* prev;
    struct inter_codes* next;
};

/// 函数参数链表的一个节点
struct arg {
    struct operand* op;
    struct arg*     next;
};

/// 函数参数双向链表
struct arg_list {
    struct arg* head;
    struct arg* tail;
};

/// 所有中间代码组成的双向链表
struct inter_code_list {
    struct inter_codes* head;
    struct inter_codes* tail;
    char* last_array_name; // 针对结构体数组，因为需要数组名查表
    int   temp_var_num;
    int   label_num;
};

/// 新建一个 \p kind 类型的操作数
struct operand* new_operand(int kind, ...);

/// 根据 \p kind 和 \p v 设置 \p operand 的值
/// \p v 可能是 char* name 或 int* value，需要进行类型转换
void set_operand(struct operand* operand, int kind, void* v);

/// 输出操作符 \p operand 到 \p fp 中
void print_operand(FILE* fp, struct operand* operand);

/// 新建一个 \p kind 操作的中间代码
struct inter_code* new_iter_code(int kind, ...);

/// 输出中间代码列表 \p inter_code_list 到 \p fp 中
void print_iter_code(FILE* fp, struct inter_code_list* inter_code_list);

/// 新建一个中间代码为 \p inter_code 的链表节点
struct inter_codes* new_iter_codes(struct inter_code* inter_code);

/// 新建一个参数节点
struct arg* new_arg(struct operand* operand);

/// 新建一个参数双向链表
struct arg_list* new_arg_list();

/// 向双向链表 \p arg_list 中添加一个节点 \p arg
void add_arg(struct arg_list* arg_list, struct arg* arg);

/// 新建一个用于存放所有中间代码的双向链表
struct inter_code_list* new_inter_code_list();

/// 向 \p inter_code_list 链表中插入一个中间代码 \p inter_codes
void add_inter_code(struct inter_code_list* inter_code_list,
                    struct inter_codes*     inter_codes);

/// 新建一个用于中间代码的临时变量
struct operand* new_tmp_var();

/// 新建一个用于跳转的标签
struct operand* new_label();

/// 获取类型 \p type 占用的内存大小
int get_size(struct type* type);

/// 基于语法分析树 \p node 递归生成中间代码
void gen_inter_codes(struct node* node);

/// 根据中间代码类型 \p kind 生成中间代码
/// 其他参数是 \p kind 决定的各个操作数
void gen_inter_code(int kind, ...);

// ========== 递归生成中间代码 ==========

void translate_Exp(struct node* node, struct operand* place);
void translate_Args(struct node* node, struct arg_list* arg_list);
void translate_Cond(struct node* node, struct operand* label_true,
                    struct operand* label_false);
void translate_VarDec(struct node* node, struct operand* place);
void translate_Dec(struct node* node);
void translate_DecList(struct node* node);
void translate_Def(struct node* node);
void translate_DefList(struct node* node);
void translate_CompSt(struct node* node);
void translate_Stmt(struct node* node);
void translate_StmtList(struct node* node);
void translate_FunDec(struct node* node);
void translate_ExtDef(struct node* node);
void translate_ExtDefList(struct node* node);

#endif

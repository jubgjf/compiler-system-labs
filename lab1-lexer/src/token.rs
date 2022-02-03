use crate::util;
use std::collections::HashMap;
use std::fmt::{Display, Formatter};

/// C 语言 token
pub enum Token {
    // ========== 标识符 ==========
    Identifier(String),

    // ========== 关键字 ==========
    Break,
    Case,
    Char,
    Const,
    Continue,
    Default,
    Do,
    Double,
    Else,
    Enum,
    Float,
    For,
    Goto,
    If,
    Int,
    Long,
    Return,
    Short,
    Signed,
    Sizeof,
    Static,
    Struct,
    Switch,
    Typedef,
    Union,
    Unsigned,
    Void,
    While,

    // ========== 界限符 ==========
    Semicolon,      // 分号 ;
    Comma,          // 逗号 ,
    LRoundBracket,  // 左小括号 (
    RRoundBracket,  // 右小括号 )
    LSquareBracket, // 左中括号 [
    RSquareBracket, // 右中括号 ]
    LCurlyBracket,  // 左大括号 {
    RCurlyBracket,  // 右大括号 }

    // ========== 运算符 ==========
    // 算数运算
    And,    // 逻辑与 &&
    Or,     // 逻辑或 ||
    Not,    // 非 !
    Xor,    // 异或 ^
    BitAnd, // 按位与 &
    BitOr,  // 按位或 |
    // 关系运算
    Add,       // 加法 +
    Subtract,  // 减法 -
    Multipliy, // 乘法 *
    Division,  // 除法 /
    Mod,       // 模 %
    Increase,  // 自增 ++
    Decrease,  // 自减 --
    // 逻辑运算
    Equal,        // 等于 ==
    Notequal,     // 不等于 !=
    Greater,      // 大于 >
    Less,         // 小于 <
    GreaterEqual, // 大于等于 >=
    LessEqual,    // 小于等于 <=
    // 赋值
    Assign, // 赋值 =

    // ========== 字面值 ==========
    LiteralInt10(i32), // 十进制数字
    LiteralInt8(i32),  // 八进制数字
    LiteralInt16(i32), // 十六进制数字
    LiteralFloat(f32), // 浮点数

    // ========== 注释 ==========
    BlockComment(String), // 块注释 /* ... */
    LineComment(String),  // 行注释 // ...

    // ========== 未知 ==========
    Unknown,
}

impl Display for Token {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            Token::Identifier(ide) => write!(f, "<IDENTIFIER, {}>", ide),
            Token::Break => write!(f, "<BREAK, _>"),
            Token::Case => write!(f, "<CASE, _>"),
            Token::Char => write!(f, "<CHAR, _>"),
            Token::Const => write!(f, "<CONST, _>"),
            Token::Continue => write!(f, "<CONTINUE, _>"),
            Token::Default => write!(f, "<DEFAULT, _>"),
            Token::Do => write!(f, "<DO, _>"),
            Token::Double => write!(f, "<DOUBLE, _>"),
            Token::Else => write!(f, "<ELSE, _>"),
            Token::Enum => write!(f, "<ENUM, _>"),
            Token::Float => write!(f, "<FLOAT, _>"),
            Token::For => write!(f, "<FOR, _>"),
            Token::Goto => write!(f, "<GOTO, _>"),
            Token::If => write!(f, "<IF, _>"),
            Token::Int => write!(f, "<INT, _>"),
            Token::Long => write!(f, "<LONG, _>"),
            Token::Return => write!(f, "<RETURN, _>"),
            Token::Short => write!(f, "<SHORT, _>"),
            Token::Signed => write!(f, "<SIGNED, _>"),
            Token::Sizeof => write!(f, "<SIZEOF, _>"),
            Token::Static => write!(f, "<STATIC, _>"),
            Token::Struct => write!(f, "<STRUCT, _>"),
            Token::Switch => write!(f, "<SWITCH, _>"),
            Token::Typedef => write!(f, "<TYPEDEF, _>"),
            Token::Union => write!(f, "<UNION, _>"),
            Token::Unsigned => write!(f, "<UNSIGNED, _>"),
            Token::Void => write!(f, "<VOID, _>"),
            Token::While => write!(f, "<WHILE, _>"),
            Token::Semicolon => write!(f, "<SEMICOLON, _>"),
            Token::Comma => write!(f, "<COMMA, _>"),
            Token::LRoundBracket => write!(f, "<LROUNDBRACKET, _>"),
            Token::RRoundBracket => write!(f, "<RROUNDBRACKET, _>"),
            Token::LSquareBracket => write!(f, "<LSQUAREBRACKET, _>"),
            Token::RSquareBracket => write!(f, "<RSQUAREBRACKET, _>"),
            Token::LCurlyBracket => write!(f, "<LCURLYBRACKET, _>"),
            Token::RCurlyBracket => write!(f, "<RCURLYBRACKET, _>"),
            Token::And => write!(f, "<AND, _>"),
            Token::Or => write!(f, "<OR, _>"),
            Token::Not => write!(f, "<NOT, _>"),
            Token::Xor => write!(f, "<XOR, _>"),
            Token::BitAnd => write!(f, "<BITAND, _>"),
            Token::BitOr => write!(f, "<BITOR, _>"),
            Token::Add => write!(f, "<ADD, _>"),
            Token::Subtract => write!(f, "<SUBTRACT, _>"),
            Token::Multipliy => write!(f, "<MULTIPLIY, _>"),
            Token::Division => write!(f, "<DIVISION, _>"),
            Token::Mod => write!(f, "<MOD, _>"),
            Token::Increase => write!(f, "<INCREASE, _>"),
            Token::Decrease => write!(f, "<DECREASE, _>"),
            Token::Equal => write!(f, "<EQUAL, _>"),
            Token::Notequal => write!(f, "<NOTEQUAL, _>"),
            Token::Greater => write!(f, "<GREATER, _>"),
            Token::Less => write!(f, "<LESS, _>"),
            Token::GreaterEqual => write!(f, "<GREATEREQUAL, _>"),
            Token::LessEqual => write!(f, "<LESSEQUAL, _>"),
            Token::Assign => write!(f, "<ASSIGN, _>"),
            Token::LiteralInt10(num) => write!(f, "<LITERALINT10, {}>,", num),
            Token::LiteralInt8(num) => write!(f, "<LITERALINT8, {}>,", num),
            Token::LiteralInt16(num) => write!(f, "<LITERALINT16, {}>,", num),
            Token::LiteralFloat(num) => write!(f, "<LITERALFLOAT, {}>,", num),
            Token::BlockComment(cmt) => write!(f, "<BLOCKCOMMENT, {}>", cmt),
            Token::LineComment(cmt) => write!(f, "<LINECOMMENT, {}>", cmt),
            Token::Unknown => write!(f, "<UNKNOWN, _>"),
        }
    }
}

/// 在 `target` 序列中，从下标为 `index` 处识别出标识符 token；
///
/// 识别出的有效词存在 `word` 中
pub fn lex_identifier(target: &[char], index: &mut usize, word: &mut String) -> Token {
    let mut keyword_map = HashMap::from([
        ("break", Token::Break),
        ("case", Token::Case),
        ("char", Token::Char),
        ("const", Token::Const),
        ("continue", Token::Continue),
        ("default", Token::Default),
        ("do", Token::Do),
        ("double", Token::Double),
        ("else", Token::Else),
        ("enum", Token::Enum),
        ("float", Token::Float),
        ("for", Token::For),
        ("goto", Token::Goto),
        ("if", Token::If),
        ("int", Token::Int),
        ("long", Token::Long),
        ("return", Token::Return),
        ("short", Token::Short),
        ("signed", Token::Signed),
        ("sizeof", Token::Sizeof),
        ("static", Token::Static),
        ("struct", Token::Struct),
        ("switch", Token::Switch),
        ("typedef", Token::Typedef),
        ("union", Token::Union),
        ("unsigned", Token::Unsigned),
        ("void", Token::Void),
        ("while", Token::While),
    ]);

    // 先识别关键字
    let target_head = String::from_iter(target[*index..].iter());
    let mut match_keyword = false;
    for key in keyword_map.keys() {
        let expected_keyword = key.to_string() + " ";
        if target_head.starts_with(&expected_keyword) {
            *word = key.to_string();
            match_keyword = true;
        }
    }
    if match_keyword {
        *index += word.len() - 1;
        return keyword_map.remove(word.as_str()).unwrap();
    }

    // 没有识别到关键字，再去识别标识符
    let mut curr_index = *index + 1;
    let mut curr_ch = util::nth(target, curr_index);

    while curr_ch.is_ascii_digit() || curr_ch.is_ascii_alphabetic() || curr_ch == '_' {
        curr_index += 1;
        curr_ch = util::nth(target, curr_index);
    }

    *word = String::from_iter(target[*index..curr_index].iter());
    *index = curr_index - 1;
    Token::Identifier(word.to_string())
}

/// 在 `target` 序列中，从下标为 `index` 处识别出界限符 token；
///
/// 识别出的有效词存在 `word` 中
pub fn lex_separator(target: &[char], index: &mut usize, word: &mut String) -> Token {
    *word = String::from(target[*index]);

    match util::nth(target, *index) {
        ';' => Token::Semicolon,
        ',' => Token::Comma,
        '(' => Token::LRoundBracket,
        ')' => Token::RRoundBracket,
        '[' => Token::LSquareBracket,
        ']' => Token::RSquareBracket,
        '{' => Token::LCurlyBracket,
        '}' => Token::RCurlyBracket,
        _ => Token::Unknown,
    }
}

/// 在 `target` 序列中，从下标为 `index` 处识别出注释 token；
///
/// 识别出的有效词存在 `word` 中
pub fn lex_comment(target: &[char], index: &mut usize, word: &mut String) -> Token {
    let mut curr_index = *index + 2;

    match util::nth(target, curr_index - 1) {
        // 块注释 /**/
        '*' => {
            let mut state = 14;

            loop {
                let curr_ch = util::nth(target, curr_index);

                match state {
                    14 => {
                        if curr_ch == '*' {
                            curr_index += 1;
                            state = 15;
                        } else if curr_ch == '\0' {
                            backtrack();

                            *word = String::from_iter(target[*index..curr_index].iter());
                            *index = curr_index;

                            return Token::Unknown;
                        } else {
                            curr_index += 1;
                            state = 14;
                        }
                    }
                    15 => {
                        if curr_ch == '*' {
                            curr_index += 1;
                            state = 15;
                        } else if curr_ch == '/' {
                            *word = String::from_iter(target[*index..curr_index + 1].iter());
                            *index = curr_index;

                            return Token::BlockComment(word.replace("\n", "\\n"));
                        } else if curr_ch == '\0' {
                            backtrack();

                            *word = String::from_iter(target[*index..curr_index].iter());
                            *index = curr_index;

                            return Token::Unknown;
                        } else {
                            curr_index += 1;
                            state = 14;
                        }
                    }
                    _ => {
                        panic!("[Error] Invalid state");
                    }
                }
            }
        }

        // 行注释 //
        '/' => loop {
            let curr_ch = util::nth(target, curr_index);

            match curr_ch {
                '\n' | '\0' => {
                    *word = String::from_iter(target[*index..curr_index].iter());
                    *index = curr_index;

                    return Token::LineComment(word.to_string());
                }
                _ => {
                    curr_index += 1;
                }
            }
        },

        // 错误
        _ => {
            panic!("[Error] Invalid state");
        }
    }
}

/// 在 `target` 序列中，从下标为 `index` 处识别出运算符 token；
///
/// 识别出的有效词存在 `word` 中
pub fn lex_operator(target: &[char], index: &mut usize, word: &mut String) -> Token {
    let map = HashMap::from([
        ('+', '+'),
        ('-', '-'),
        ('!', '='),
        ('<', '='),
        ('>', '='),
        ('=', '='),
        ('&', '&'),
        ('|', '|'),
    ]);

    let curr_ch = util::nth(target, *index);
    let next_ch = util::nth(target, *index + 1);

    *word = match curr_ch {
        c if map.contains_key(&c) => {
            if next_ch == *map.get(&curr_ch).unwrap() {
                *index += 1;
                String::from_iter(target[*index - 1..*index + 1].iter())
            } else {
                String::from(target[*index])
            }
        }
        _ => String::from(target[*index]),
    };

    token_map(word)
}

/// 在 `target` 序列中，从下标为 `index` 处识别出字面值 token；
///
/// 识别出的有效词存在 `word` 中
pub fn lex_literal(target: &[char], index: &mut usize, word: &mut String) -> Token {
    let mut curr_index = *index;
    let mut curr_ch = util::nth(target, curr_index);

    let mut state = if curr_ch == '0' { 8 } else { 2 };

    curr_index += 1;
    curr_ch = util::nth(target, curr_index);

    loop {
        match state {
            2 => {
                if curr_ch.is_ascii_digit() {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 2;
                } else if curr_ch == '.' {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 3;
                } else if curr_ch == 'E' || curr_ch == 'e' {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 5;
                } else {
                    *word = String::from_iter(target[*index..curr_index].iter());
                    *index = curr_index - 1;
                    return Token::LiteralInt10(word.parse().unwrap());
                }
            }
            3 => {
                if curr_ch.is_ascii_digit() {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 4;
                } else {
                    backtrack();

                    *word = String::from_iter(target[*index..curr_index].iter());
                    *index = curr_index - 1;
                    return Token::Unknown;
                }
            }
            4 => {
                if curr_ch.is_ascii_digit() {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 4;
                } else if curr_ch == 'E' || curr_ch == 'e' {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 5;
                } else {
                    *word = String::from_iter(target[*index..curr_index].iter());
                    *index = curr_index - 1;
                    return Token::LiteralFloat(word.parse().unwrap());
                }
            }
            5 => {
                if curr_ch.is_ascii_digit() {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 7;
                } else if curr_ch == '+' || curr_ch == '-' {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 6;
                } else {
                    backtrack();

                    *word = String::from_iter(target[*index..curr_index].iter());
                    *index = curr_index - 1;
                    return Token::Unknown;
                }
            }
            6 => {
                if curr_ch.is_ascii_digit() {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 7;
                } else {
                    backtrack();

                    *word = String::from_iter(target[*index..curr_index].iter());
                    *index = curr_index - 1;
                    return Token::Unknown;
                }
            }
            7 => {
                if curr_ch.is_ascii_digit() {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 7;
                } else {
                    *word = String::from_iter(target[*index..curr_index].iter());
                    *index = curr_index - 1;
                    return Token::LiteralFloat(word.parse().unwrap());
                }
            }
            8 => {
                if curr_ch.is_ascii_digit() {
                    state = if curr_ch == '8' || curr_ch == '9' {
                        10
                    } else {
                        9
                    };
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                } else if curr_ch == 'x' {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 11;
                } else {
                    *word = String::from_iter(target[*index..curr_index].iter());
                    *index = curr_index - 1;
                    return Token::LiteralInt10(0);
                }
            }
            9 => {
                if curr_ch.is_ascii_digit() {
                    state = if curr_ch == '8' || curr_ch == '9' {
                        10
                    } else {
                        9
                    };
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                } else if curr_ch == '.' {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 3;
                } else {
                    *word = String::from_iter(target[*index..curr_index].iter());
                    *index = curr_index - 1;
                    return Token::LiteralInt8(i32::from_str_radix(&word[1..], 8).unwrap());
                }
            }
            10 => {
                if curr_ch == '8' || curr_ch == '9' {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 10;
                } else if curr_ch == '.' {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 3;
                } else {
                    backtrack();

                    *word = String::from_iter(target[*index..curr_index].iter());
                    *index = curr_index - 1;
                    return Token::Unknown;
                }
            }
            11 => {
                if curr_ch.is_ascii_digit()
                    || curr_ch == 'a'
                    || curr_ch == 'b'
                    || curr_ch == 'c'
                    || curr_ch == 'd'
                    || curr_ch == 'e'
                    || curr_ch == 'f'
                    || curr_ch == 'A'
                    || curr_ch == 'B'
                    || curr_ch == 'C'
                    || curr_ch == 'D'
                    || curr_ch == 'E'
                    || curr_ch == 'F'
                {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 12;
                } else {
                    backtrack();

                    *word = String::from_iter(target[*index..curr_index].iter());
                    *index = curr_index - 1;
                    return Token::Unknown;
                }
            }
            12 => {
                if curr_ch.is_ascii_digit()
                    || curr_ch == 'a'
                    || curr_ch == 'b'
                    || curr_ch == 'c'
                    || curr_ch == 'd'
                    || curr_ch == 'e'
                    || curr_ch == 'f'
                    || curr_ch == 'A'
                    || curr_ch == 'B'
                    || curr_ch == 'C'
                    || curr_ch == 'D'
                    || curr_ch == 'E'
                    || curr_ch == 'F'
                {
                    curr_index += 1;
                    curr_ch = util::nth(target, curr_index);
                    state = 12;
                } else {
                    *word = String::from_iter(target[*index..curr_index].iter());
                    *index = curr_index - 1;
                    return Token::LiteralInt16(i32::from_str_radix(&word[2..], 16).unwrap());
                }
            }
            _ => {
                panic!("[Error] Invalid state");
            }
        }
    }
}

/// 判断字符 `ch` 是否是某个标识符 token 的首字符
pub fn is_identifier_pred(ch: char) -> bool {
    ch.is_ascii_alphabetic() || ch == '_'
}

/// 判断字符 `ch` 是否是某个界限符 token 的首字符
pub fn is_separator_pred(ch: char) -> bool {
    ch == ';'
        || ch == ','
        || ch == '('
        || ch == ')'
        || ch == '['
        || ch == ']'
        || ch == '{'
        || ch == '}'
}

/// 判断字符 `ch` 和 `ch_next` 是否是某个注释 token 的前两个字符
pub fn is_comment_pred(ch: char, ch_next: char) -> bool {
    ch == '/' && (ch_next == '/' || ch_next == '*')
}

/// 判断字符 `ch` 是否是某个运算符 token 的首字符
pub fn is_operator_pred(ch: char) -> bool {
    ch == '+'
        || ch == '-'
        || ch == '*'
        || ch == '/'
        || ch == '%'
        || ch == '!'
        || ch == '<'
        || ch == '>'
        || ch == '='
        || ch == '&'
        || ch == '|'
}

/// 出现词法分析错误时的处理
pub fn backtrack() {
    // TODO backtrack
    print!("Err>> ");
}

/// 给定字符串 `word`，返回其对应的 token
///
/// 若不存在对应的 token，则返回 [`Token::Unknown`]
pub fn token_map(word: &str) -> Token {
    match word {
        // 算数运算
        "&&" => Token::And,   // 逻辑与 &&
        "||" => Token::Or,    // 逻辑或 ||
        "!" => Token::Not,    // 非 !
        "^" => Token::Xor,    // 异或 ^
        "&" => Token::BitAnd, // 按位与 &
        "|" => Token::BitOr,  // 按位或 |
        // 关系运算
        "+" => Token::Add,       // 加法 +
        "-" => Token::Subtract,  // 减法 -
        "*" => Token::Multipliy, // 乘法 *
        "/" => Token::Division,  // 除法 /
        "%" => Token::Mod,       // 模 %
        "++" => Token::Increase, // 自增 ++
        "--" => Token::Decrease, // 自减 --
        // 逻辑运算
        "==" => Token::Equal,        // 等于 ==
        "!=" => Token::Notequal,     // 不等于 !=
        ">" => Token::Greater,       // 大于 >
        "<" => Token::Less,          // 小于 <
        ">=" => Token::GreaterEqual, // 大于等于 >=
        "<=" => Token::LessEqual,    // 小于等于 <=
        // 赋值
        "=" => Token::Assign, // 赋值 =

        // 意外情况
        _ => Token::Unknown,
    }
}

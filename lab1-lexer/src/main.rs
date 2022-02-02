use crate::token::*;

mod token;
mod util;

fn main() {
    let file_content = util::read_file("lab1-lexer/test/right.c");
    // let file_content = util::read_file("lab1-lexer/test/error.c");
    // let file_content = String::from("defaults");
    let file_content_vec: Vec<char> = file_content.chars().collect();

    // i: 索引 file_content_vec 的下标
    let mut i = 0;
    let len = file_content_vec.len();

    while i < len {
        // token 对应的单词
        let mut word = String::new();

        // ch: 当前字符
        let ch = util::nth(&file_content_vec, i);
        let ch_next = util::nth(&file_content_vec, i + 1);

        let token = match ch {
            // ========== 空字符 ==========
            c if char::is_whitespace(c) => {
                i += 1;
                continue;
            }

            // ========== 标识符或关键字 ==========
            c if is_identifier_pred(c) => lex_identifier(&file_content_vec, &mut i, &mut word),

            // ========== 界限符 ==========
            c if is_separator_pred(c) => lex_separator(&file_content_vec, &mut i, &mut word),

            // ========== 注释 ==========
            // 由于注释的关键字 "/*" "*/" "//" 以运算符中的除法 "/" 开头，
            // 因此匹配注释应该在匹配运算符之前
            c if is_comment_pred(c, ch_next) => lex_comment(&file_content_vec, &mut i, &mut word),

            // ========== 运算符 ==========
            c if is_operator_pred(c) => lex_operator(&file_content_vec, &mut i, &mut word),

            // ========== 字面值 ==========
            c if c.is_ascii_digit() => lex_literal(&file_content_vec, &mut i, &mut word),

            // ========== 未知或错误 ==========
            _ => {
                word = String::from(file_content_vec[i]);
                Token::Unknown
            }
        };

        i += 1;

        println!("word = {:<8}, token = {}", word.replace("\n", "\\n"), token);
    }
}

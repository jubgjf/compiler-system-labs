use crate::token::*;

mod token;
mod util;

/// 对 `content` 中的内容进行词法分析，返回 Token 序列
pub fn lexer(content: String) -> Vec<String> {
    let content_vec: Vec<char> = content.chars().collect();
    let mut result: Vec<String> = vec![];

    // i: 索引 file_content_vec 的下标
    let mut i = 0;
    let len = content_vec.len();

    while i < len {
        // token 对应的单词
        let mut word = String::new();

        // ch: 当前字符
        let ch = util::nth(&content_vec, i);
        let ch_next = util::nth(&content_vec, i + 1);

        let token = match ch {
            // ========== 空字符 ==========
            c if char::is_whitespace(c) => {
                i += 1;
                continue;
            }

            // ========== 标识符或关键字 ==========
            c if is_identifier_pred(c) => lex_identifier(&content_vec, &mut i, &mut word),

            // ========== 界限符 ==========
            c if is_separator_pred(c) => lex_separator(&content_vec, &mut i, &mut word),

            // ========== 注释 ==========
            // 由于注释的关键字 "/*" "*/" "//" 以运算符中的除法 "/" 开头，
            // 因此匹配注释应该在匹配运算符之前
            c if is_comment_pred(c, ch_next) => lex_comment(&content_vec, &mut i, &mut word),

            // ========== 运算符 ==========
            c if is_operator_pred(c) => lex_operator(&content_vec, &mut i, &mut word),

            // ========== 字面值 ==========
            c if c.is_ascii_digit() => lex_literal(&content_vec, &mut i, &mut word),

            // ========== 未知或错误 ==========
            _ => {
                word = String::from(content_vec[i]);
                Token::Unknown
            }
        };

        i += 1;

        println!("word = {:<8}, token = {}", word.replace("\n", "\\n"), token);
        result.push(format!(
            "\"word\": \"{}\", \"token\": \"{}\"",
            word.replace("\n", "\\n").replace("\"", "\\\""),
            token
        ));
    }

    result
}

/// 获取 `target` 中第 `index` 个元素；
///
/// 若 `index` 超出范围，则返回 '\0'
pub fn nth(target: &[char], index: usize) -> char {
    let len = target.len();

    if index >= len {
        '\0'
    } else {
        target[index]
    }
}

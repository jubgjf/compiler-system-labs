use std::fs::File;
use std::io::Read;
use std::path::Path;

/// 读取并返回文件内容
///
/// 发生错误时会 panic
pub fn read_file(path: &str) -> String {
    let path = Path::new(path);
    let display = path.display();

    let mut file = match File::open(&path) {
        Err(why) => panic!("couldn't open {}: {}", display, why),
        Ok(file) => file,
    };

    let mut s = String::new();
    if let Err(why) = file.read_to_string(&mut s) {
        panic!("couldn't read {}: {}", display, why)
    }

    s
}

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

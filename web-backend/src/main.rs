#[macro_use]
extern crate rocket;

use lab1_lexer::lexer;
use rocket::fairing::{Fairing, Info, Kind};
use rocket::http::Header;
use rocket::{Request, Response};

/// 对前端发送的 `data` 进行词法分析，返回 JSON 格式的 Token 序列
#[post("/lexer", format = "text/plain", data = "<data>")]
fn call_lexer(data: String) -> String {
    let lexer_result = lexer(data);

    let mut lexer_result_json = String::new();
    lexer_result_json.push('[');
    for word_token in lexer_result.iter() {
        lexer_result_json.push('{');
        lexer_result_json.push_str(word_token.as_str());
        lexer_result_json.push_str("},");
    }
    lexer_result_json.pop();
    lexer_result_json.push(']');

    lexer_result_json
}

#[launch]
fn rocket() -> _ {
    rocket::build().mount("/", routes![call_lexer]).attach(CORS)
}

pub struct CORS;

#[rocket::async_trait]
impl Fairing for CORS {
    fn info(&self) -> Info {
        Info {
            name: "Add CORS headers to responses",
            kind: Kind::Response,
        }
    }

    async fn on_response<'r>(&self, _request: &'r Request<'_>, response: &mut Response<'r>) {
        response.set_header(Header::new("Access-Control-Allow-Origin", "*"));
        response.set_header(Header::new(
            "Access-Control-Allow-Methods",
            "POST, GET, PATCH, OPTIONS",
        ));
        response.set_header(Header::new("Access-Control-Allow-Headers", "*"));
        response.set_header(Header::new("Access-Control-Allow-Credentials", "true"));
    }
}

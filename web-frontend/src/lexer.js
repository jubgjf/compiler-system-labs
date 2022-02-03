import {Form, Button, Table, Row, Col} from "antd";
import React from "react";
import "antd/dist/antd.css";
import TextArea from "antd/es/input/TextArea";
import {DeleteOutlined, SendOutlined} from "@ant-design/icons";

// 词法分析器组件
export default class LexerBlock extends React.Component {
    state = {
        editor_text: "", // 编辑器文本内容
        tokens: [] // Token 列表
    };

    // 向后端发送编辑器内容，进行词法分析
    // 获得的 Token 列表存入 this.state
    handleSubmit = async () => {
        if (!this.state.editor_text) {
            return;
        }

        await fetch("http://localhost:8000/lexer", {
            method: "POST",
            headers: {
                Accept: "text/plain",
                "Content-Type": "text/plain"
            },
            body: this.state.editor_text
        }).then(async (response) => {
            let lexer_result = await response.text();

            console.log(lexer_result)

            this.setState({
                tokens: JSON.parse(lexer_result)
            });
        }).catch((error) => {
            console.error(error);
        });
    };

    // 随时更新编辑器文本内容到 this.state 中
    handleChange = e => {
        this.setState({
            editor_text: e.target.value,
        });
    };

    // 清空文本编辑器内容
    handleClear = () => {
        this.setState({
            editor_text: "",
            tokens: []
        });
    };

    render() {
        // Token 表格标题
        const table_columns = [
            {
                title: "关键字",
                dataIndex: "word",
                key: "word",
            },
            {
                title: "Token",
                dataIndex: "token",
                key: "token",
            }
        ];

        return (
            <>
                <Row style={{paddingLeft: "25px"}}>
                    <Button onClick={this.handleSubmit} shape="circle" icon={<SendOutlined/>}
                            style={{marginRight: "10px"}}/>
                    <Button onClick={this.handleClear} shape="circle" icon={<DeleteOutlined/>}
                            style={{marginRight: "10px"}}/>
                </Row>
                <Row>
                    <Col span={12} style={{padding: "10px 25px"}}>
                        <Editor
                            onChange={this.handleChange}
                            value={this.state.editor_text}
                        />
                    </Col>
                    <Col span={12} style={{padding: "10px 25px"}}>
                        <TokenTable
                            columns={table_columns}
                            data={this.state.tokens}
                        />
                    </Col>
                </Row>
            </>
        )
    }
}

// 代码编辑器
const Editor = ({onChange, value}) => (
    <Form.Item>
        <TextArea rows={25} onChange={onChange} value={value}/>
    </Form.Item>
);

// 显示 Token 的表格
const TokenTable = ({columns, data}) => (
    <Table columns={columns} dataSource={data}/>
);

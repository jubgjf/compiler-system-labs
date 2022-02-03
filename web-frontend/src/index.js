import React from "react";
import ReactDOM from "react-dom";
import LexerBlock from "./lexer"
import {HashRouter, Link, Navigate, Route, Routes} from "react-router-dom";
import {Content, Header} from "antd/es/layout/layout";
import {Layout, Menu} from "antd";

ReactDOM.render(
    <HashRouter>
        <Layout>
            <Header>
                <Menu theme="dark" mode="horizontal" defaultSelectedKeys={["1"]}>
                    <Menu.Item key={1}>
                        <Link to="/lexer">词法分析</Link>
                    </Menu.Item>
                    <Menu.Item key={2}>
                        <Link to="/todo">语法分析</Link>
                    </Menu.Item>
                    <Menu.Item key={3}>
                        <Link to="/todo">语义分析</Link>
                    </Menu.Item>
                </Menu>
            </Header>
            <Content style={{padding: "10px 50px 50px"}}>
                <Routes>
                    <Route path="/" element={<Navigate replace to="/lexer"/>}/>
                    <Route path="/lexer" element={<LexerBlock/>}/>
                    <Route path="/todo" element={<h1>Todo</h1>}/>
                </Routes>
            </Content>
        </Layout>
    </HashRouter>,
    document.getElementById("root")
);

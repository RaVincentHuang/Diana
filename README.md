# SysY-Compiler

## 介绍 
SysY语言编译器

## 软件架构
本编译器采用了前端ANTLR+后端MLIR的总体架构，当前完成了前端到AST的生成，具体文件结构如下。

```shell
sys-y-compiler
├── LICENSE
├── Makefile
├── README.en.md
├── README.md
├── build
├── doc
│   ├── SysY语言定义.pdf
│   └── SysY运行时库.pdf
├── include
│   └── sysy
│       ├── Frontend
│       │   ├── Ast.h
│       │   ├── AstTramsform.h
│       │   ├── AstType.h
│       │   ├── Frontend.h
│       │   ├── Location.h
│       │   ├── SysYLexer.h
│       │   ├── SysYOp.h
│       │   ├── SysYParser.h
│       │   ├── SysYParserBaseVisitor.h
│       │   ├── SysYParserVisitor.h
│       │   ├── SysYVisitor.h
│       │   └── SysyContext.h
│       └── Support
│           ├── CommandLine.h
│           ├── common.h
│           └── debug.h
├── lib
│   └── Frontend
│       ├── Ast.cpp
│       ├── AstTramsform.cpp
│       ├── Frontend.cpp
│       ├── Location.cpp
│       ├── SysYLexer.cpp
│       ├── SysYLexer.g4
│       ├── SysYParser.cpp
│       ├── SysYParser.g4
│       ├── SysYParserBaseVisitor.cpp
│       ├── SysYParserVisitor.cpp
│       ├── SysYVisitor.cpp
│       └── SysyContext.cpp
├── scripts
│   └── config.mk
├── sysyc
├── sysyc.cpp
└── test.sy
```

## 安装说明
编译依赖于LLVM和ANTLR

### LLVM
我们需要修改scripts/config.mk 中的变量LLVM_CONFIG为您系统中正确的CONIG脚本名。

### ANTLR4 
我们需要安装ANTLR作为依赖文件。
```shell
cd <Where/you/want/to/install/Antlr4>
wget https://www.antlr.org/download/antlr-4.9.3-complete.jar
export CLASSPATH=".:<Where/you/want/to/install/Antlr4>/antlr-4.9.3-complete.jar:$CLASSPATH"
alias antlr4='java -jar <Where/you/want/to/install/Antlr4>/antlr-4.9.3-complete.jar'
alias grun='java org.antlr.v4.gui.TestRig'
```

我们还需要安装ANTLR的C++运行时环境
```shell
git clone https://github.com/antlr/antlr4.git
cd antlr4/runtime/Cpp
mkdir build && mkdir run && cd build
cmake .. -DANTLR_JAR_LOCATION=<Where/you/want/to/install/Antlr4>/antlr-4.9.3-complete.jar -DWITH_DEMO=True
make
sudo make install
```

后我们需要修改scripts/config.mk的ANTLR相关的变量
```makefile
ANTLR4 = java -jar <Where/you/want/to/install/Antlr4>/antlr-4.9.2-complete.jar -Dlanguage=Cpp -no-listener -visitor
```

可以使用发行版

## 软件使用
```shell
$ sysyc <filename> [opt, ..]

$ sysyc <filename> -o outName

# 打印源代码
$ sysyc <filename> --gen=src

# 打印CST
$ sysyc <filename> --gen=cst

# 打印AST
$ sysyc <filename> --gen=ast

# 打印llvm-ir
$ sysyc <filename> --gen=llvm-ir
```
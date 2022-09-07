# Diana

**A SysY Compiler**
```
██████╗ ██╗ █████╗ ███╗   ██╗ █████╗     
██╔══██╗██║██╔══██╗████╗  ██║██╔══██╗    
██║  ██║██║███████║██╔██╗ ██║███████║    
██║  ██║██║██╔══██║██║╚██╗██║██╔══██║    
██████╔╝██║██║  ██║██║ ╚████║██║  ██║    
╚═════╝ ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝    
```

---
## Introduction
An open source compiler for the SysY language. Developed from the 2022 Huawei Bisheng Cup entries. Note that this is the interface documentation inside the project. The project address is <https://github.com/RaVincentHuang/Diana>.

---
## Contributor
+ **Zixiao Huang** From the School of Computer Science, National University of Defense Technology. Maintainer of the Diana project, contributing front-end and mid-end code for Diana. <huangzixiaopaz@nudt.edu.cn>, <https://github.com/RaVincentHuang>
+ **Simin Xiong** From the School of Computer Science, National University of Defense Technology. Developer of the Diana project, Contributed backend instruction selection and backend optimization for Diana.
+ **Ziran He** From the School of Computer Science, National University of Defense Technology. Developer of the Diana project, Contributed backend register allocation for Diana.
+ **Zihao Li** From the School of Computer Science, National University of Defense Technology. Developer of the Diana project, Contributed backend register allocation for Diana. Contributed Diana's automated test scripts.

## Build the Project
```shell
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_CXX_COMPILER=clang++
ninja
```

---
## TODOList
- Refactor the compiler frontend.
- Refactor the compiler backend. 
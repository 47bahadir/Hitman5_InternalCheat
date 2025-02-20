
# Hitman5_InternalCheat
杀手5内部作弊程序 dll注入文件

---

## 代码分析  

 
### 1. DLL注入初始化

在dll注入时需要在程序上附加时，在`DLL_PROCESS_ATTACH`阶段通过`CreateThread`函数创建一个线程。再通过`CloseHandle`函数把`CreateThread`函数创建的句柄关闭，这样就留下了一个干净的线程。

### 2. 汇编函数Hook基础

最开始定义的两个汇编函数是用于Hook游戏的部分函数。在内部外挂当中可以直接通过`GetModuleHandle`函数来获取基础地址。

### 3. Hook函数实现细节

`myHook`函数接受三个参数：目标地址（`dst`）、汇编函数地址（`ourAddy`）和Hook目标地址的长度（`len`）。

1.  **长度验证** 首先判断目标地址的长度是否大于五，因为在X86架构中跳转指令的汇编代码最少占5个字节。如果小于五则直接返回`false`表示函数失败。
    
2.  **内存权限修改** 使用`VirtualProtect`将目标地址的权限设置为`PAGE_EXECUTE_READWRITE`（可读写）。
    
3.  **NOP填充** 通过`memset`将目标地址的前`len`个字节全部设置为`0x90`（NOP指令）。这样做的目的是将除了JMP指令之外的地址用NOP填充，防止程序执行流程出错。
    
4.  **相对地址计算**
    
    cpp
    
    ```cpp
    DWORD relativeAddy = ((DWORD)ourAddy - (DWORD)dst) - 5;
    
    ```
    
    内存布局说明：
    
    ```none
    +----------------+     +----------------+
    | dst (跳转起点) | --> | ourAddy (目标) |
    +----------------+     +----------------+
            |                     ▲
            | 相对跳转偏移          |
            +---------------------+
    
    ```
    
    执行JMP指令后，CPU的EIP寄存器会指向dst + 5（因为E9跳转指令占5字节） 因此实际偏移量需要计算：ourAddy - (dst + 5)
    
5.  **写入JMP指令**
    
    -   将目标地址的第一个字节修改为机器码0xE9（JMP指令）
    -   后续4字节写入计算出的相对偏移地址
6.  **恢复内存权限** 最后再次调用VirtualProtect将内存权限恢复为原始值。
    

### 4. 内部作弊和外部作弊实现对比

-   外部作弊：
    
    -   必须使用WriteProcessMemory进行内存写入
    -   指针解引用需要逐级调用ReadProcessMemory
-   内部作弊：
    
    -   可直接使用memcpy或memset操作内存
    -   可直接通过指针解引用（例如*(DWORD*)address）
    -   权限修改只需执行一次（无需频繁切换）

### 5. 关闭Hook的恢复逻辑

关闭外挂恢复原始状态时需要定义writePatch函数，其原理与外部作弊的patchEx函数类似：

-   备份原始指令
-   通过直接内存操作还原被修改的代码

### 6. 线程安全退出

使用FreeLibraryAndExitThread函数彻底释放DLL并退出线程：

cpp

```cpp
FreeLibraryAndExitThread(hModule, 0);  // hModule为DLL模块句柄
```
---

## Legal Disclaimer 法律声明  

### 1. Purpose Limitation 使用目的限制  
This project is intended **SOLELY** for the following purposes:  
本项目仅限用于以下用途：  
- Academic research on software reverse engineering techniques  
  软件逆向工程技术学术研究  
- Educational demonstration of memory analysis methods  
  内存分析方法的数学证明与教学演示  
- Non-commercial study of game system design patterns  
  非商业性质的游戏系统设计模式研究  

### 2. Copyright Notice 版权声明  
All game-related assets including but not limited to:  
包含但不限于以下游戏相关资产：  
- Game client binaries 游戏客户端二进制文件  
- Original artwork/resources 原始美术资源  
- Trademarked logos 注册商标标识  
are property of their respective copyright holders.  
均归属其合法版权方所有。  

### 3. User Responsibility 使用者责任  
Any usage beyond the scope defined in Section 1 shall be deemed as:  
超出第1条定义范围的使用行为将被视为：  
- Violation of the Digital Millennium Copyright Act (DMCA) in the US  
  违反美国《数字千年版权法案》(DMCA)  
- Infringement of Copyright Law Article 47 in China  
  触犯中国《著作权法》第47条  
- Breach of EU Directive 2001/29/EC in Europe  
  违背欧盟《2001/29/EC指令》  

### 4. Technical Restrictions 技术限制  
This repository EXPLICITLY EXCLUDES:  
本仓库明确不包含：  
- Pre-modified game executables 预修改的游戏可执行文件  
- Copyrighted resource files 受版权保护的资源文件  
- Activation bypass mechanisms 激活验证绕过机制  

### 5. Liability Exclusion 责任免除  
The author shall not be liable for:  
作者不对以下情况承担责任：  
- Damages caused by misuse of the code 代码滥用造成的损害  
- Legal consequences from unauthorized modifications 未经授权的修改导致的法务后果  
- Violation of EULAs by end users 终端用户违反最终用户许可协议(EULA)  

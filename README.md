# Hitman5_InternalCheat
杀手5内部作弊程序 dll注入文件

---

## 代码分析  

1.在dll注入时需要在程序上附加时在DLL_PROCESS_ATTACH处通过CreateThread函数创建一个线程。再通过CloseHandle函数把CreateThread函数创建的句柄给关闭，这样就留下了一个干净的线程。

2.最开始定义的两个汇编函数是用于Hook游戏的部分函数。在内部外挂当中可以直接通过GetModuleHandle函数来获取基础地址。

3.myHook函数接受三个参数分别是目标地址，汇编函数地址，Hook目标地址的长度。首先判断目标地址的长度是否大于五，因为在X86当中跳转指令的汇编代码最少占5个字节。
  小于五的话直接返回0表示函数失败。下一步是将目标地址的权限设置为可读写，然后把输入长度的所有字节设置为0x90。这么做的目的是除了jmp指令之外的地址变成Nop指令防止程序出错。
  之后从我们的函数地址减去目标地址再减去5就能得到需要跳转的相对地址。
  
  内存地址布局（低地址 → 高地址）：
+----------------+     +----------------+
| dst (跳转起点) | --> | ourAddy (目标) |
+----------------+     +----------------+
        |                     ▲
        | 相对跳转偏移          |
        +---------------------+

 ourAddy（目标地址）  
▲  
│  
│ 相对偏移 = ourAddy - (dst + 5)  
│ （因为执行完JMP指令后，EIP会指向 dst + 5）  
│  
▼  
dst（原代码地址） → E9 XX XX XX XX（占5字节）  

得到偏移地址之后就需要把目标地址的第一个指令改成机器码0xE9，后面则附带刚计算出来的偏移地址。最后需要把目标地址读写权限改回原来的权限。

4.启用外挂功能时直接调用前面定义的myHook函数即可,那么关闭外挂回复原来的状态时需要定义一个新的函数叫writePatch。这个函数的原理跟外部作弊的patchEx函数一样。
  在外部作弊当中不能直接使用memcpy或memset函数所以只能使用WindowsAPI当中的WriteProcessMemory函数来代替。但是在内部作弊中我们可以不用调用WriteProcessMemory函数而是直接使用memcpy或memset函数。
  这就是内部作弊对比外部作弊的方便之处，在指针解引方面也可以直接解引不需要特此调用ReadProcessMemory函数。

5.在我们定义的汇编函数当中需要指定跳转地址为原始指令的后续。因此先定义一个地址变量再把原代码地址加上下个原代码地址的长度就可以计算出跳转地址了。

6.最后想彻底关闭线程的话就可以用FreeLibraryAndExitThread函数来释放线程。

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

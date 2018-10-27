## Instruction Selection

[+] mipscodegen.c

[+] mipsframe.c

### Summary

这个部分的目标是将canon tree转换为assembly code，这里我选择的是mips32汇编语言作为最终生成目标。

关于mips32的内容，可以通过google搜索。

我提供一个mips32的emulator: [SPIM](http://spimsimulator.sourceforge.net)

这里注意一个问题，就是为了使这部分和寄存器分配的部分分离开来，所以生成的汇编代码使用的寄存器不是mips32里面严格规定的寄存器，而是使用了一些临时变量、临时寄存器的名字来取代，因此生成的汇编代码并不是运行，而只是将canon tree的内容以代码的形式表示出来。

而且汇编代码的具体结构，包括prolog、epilog、main函数声明、.data声明都没用加进去。这些内容将在之后的章节完善。



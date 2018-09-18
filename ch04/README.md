## 抽象语法树
抽象语法树貌似没有什么可讲的...

因为和类型检查是分开来做的，所以生产抽象语法树也就是对每个语法规则调用一下构造函数...

你可能想知道 absyn_root 是干嘛的...

但其实你看下 parse.c 你就知道这其实是树的根...

现在的代码里有 lextest.c parsetest.c absyntest.c 前面两个和以前的作用一样

absyntest.c 则是将树打印出来的 driver 

parse.[ch] 是一个 parse 函数，和 parsetest.c 里的稍有不同

哦对了，我还为每个头文件都加了guard!

貌似没什么可交待了的

## Canon

惊喜的发现这部分作者已经写好了，不过我们在拿来用的同时，不妨看看这样做的原因是什么。

### 使用

1. C_linearize()

2. C_basicBlocks()

3. C_traceSchedule()

将stmlist依此传入canon的三个接口即可获得干净的canon tree.

### 探索

一棵ir tree包含了很多类似ESEQ CJUMP等结构，我们拿CJUMP来说，CJUMP有两个跳转地址，这两个地址是可以指定的。
但是在汇编里，是不可能允许一个判断语句、一个跳转语句有两个跳转地址的，只能有一个，比如为真跳转至某个地址，为假
跳转至下一个指令。因此我们的ir tree就非常不适合转化为汇编语言，因此解决方案就是把两个地址变成一个地址，也就是
将为假的地址变成下一个statement的地址。因此我们需要让这个ir tree重构，变成一棵干净的canon tree.

那么如何进行这样的转化呢？这里提出了一个Basic Blocks的概念，将一连串只含有一个跳转指令的statements合成一个
Block，在形成这些Block之后，我们就可以重新排序这些Blocks的位置，这样就解决了CJUMP的问题了。


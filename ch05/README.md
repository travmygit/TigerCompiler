类型检查真是写得头晕。。。

建议先把几个工具函数写了：

actual_ty() 把 Ty_name 转为 built-in type

actual_eq() 类型检查经常用到的判断两种类型是否相等

判断相等需要注意：

1. 内置的类型只有简单的几个：Ty_int Ty_string Ty_void Ty_nil
2. 稍微复杂的类型：Ty_record Ty_array
3. 比较Ty_record Ty_array需要判断两者的 reference 是否相同的

比较值得学习的点：

1. S_table 符号表是一个hash表，用S_symbol这个结构指针的指针值(4B)作KEY

修改的地方：

1. tiger.y 把所有的左递归变成了右递归，这是为了让parse解析的顺序与程序写的顺序一致，如果不这样干，可能会导致程序后面定义的变量会被安排到前面，会造成许多困扰

建议多看看附录吧
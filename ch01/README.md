## Programming style in C (代码风格)
1. Trees are described by a grammar.
1. 树描述语法
2. A tree is described by one or more typedefs. corresponding to a symbol in the grammar.
2. 树由一个或多个typedef描述
3. Each typedef defines a pointer to a corresponding struct. The struct name, which ends in an underscore, is never used anywhere except in the declaration of the typedef and the definition of the struct itself.
3. 每个typedef定义一个结构指针，结构的名字不会在程序其他地方使用，只在声明的时候使用，跟结构体有关的地方只使用指针，不直接使用结构体
4. Each struct contains a kind field, which is an enum showing different variants, one for each grammar rule; and a u field, which is a union.
4. 结构体都包含一个枚举的kind，一个联合的u，kind表示语法规则，u表示语法里的元素
5. If there is more than one nontrivial symbol in the right-hand side of a rule, the union will have a component that is itself a struct comprising these values.
5. 如果一个语法规则包含了两个以上元素，那么上面提到的对应的u将会是一个struct
6. If there is only one nontrivial symbol in the right-hand side of a rule, the union will have a component that is the value.
6. 如果一个语法规则只包含了一个元素，那么上面提到的对应的u将会是一个值
7. Every class will have a constructor function that initializes all the fields. The malloc function shall never be called directly, except in these constructor functions.
7. 每个类都有构造函数，可以在构造函数里直接使用malloc，其他地方不能直接使用malloc
8. Each module shall have a prefix unique to that module.
8. 每个模块的程序都有一个特殊的前缀，比如A_
9. Typedef names(after the prefix) shall start with lowercase letters; constructor functions(after the prefix) with uppercase; enumeration atoms(after the prefix) with lowercase; and union variants(which have no prefix) with lowercase.
9. 选取名字的时候，typedef以小写开头，构造函数以大写开头，枚举以小写开头，联合以小写开头

## Modularity principles for C programs. (模块化的约定)
A compiler can be a big program; careful attention to modules and interfaces prevents chaos.
编译器将会是一个很大的程序，仔细的定义好模块化和结构的规则可以避免代码混乱
1. Each phase or module of the compiler belongs in its own ".c" file, which will have a corresponding ".h" file.
1. 每个模块的程序有自己对应的.c/.h文件
2. Each module shall have a prefix unique to that module. All global names exported by the module shall start with the prefix. Then the human reader of a file will not have to look outside that file to determine where a name comes from.
2. 每个模块的都有自己的前缀，从一个模块产出的全局变量名、函数名等都要加上自己的前缀
3. All functions shall have prototypes, and the C compiler shall be told to warn about uses of functions without prototypes.
3. 每个函数都要有函数原型
4. We will #include "util.h" in each file:
The inclusion of assert.h encourages the liberal use of assertion by the C programmer.
4. 在每个文件中都 #include "util.h"，鼓励大家在编码的时候多多使用assertion
5. The string type means a heap-allocated string that will not be modified after its initial creation. The String function builds a heap-allocated string from a C-type character pointer(just like the standard C library function strdup). Functions that take strings as arguments assume that the contents will never change.
5. string类型的字符串意味着这是个分配在堆上的字符串，是类似cpp的不可变字符串，所有把string当作参数的函数都不会修改string的内容
6. C's malloc function return NULL if there is no memory left. The Tiger compiler will not have sophisticated memory management to deal with this problem. Instead, it will never call malloc directly, but call only our own function, checked_malloc, which guarantees never to return NULL:
    void *checked_malloc(int len) {
      void *p = malloc(len);
      if (!p) {
        fprintf(stderr,"\nRan out of memory!\n");
        exit(1);
      }
      return p;
    }
6. 在Tiger编译器里，没有复杂的内存管理，我们不会直接调用malloc，而是使用checked_malloc，这个函数不会返回NULL
7. We will never call free. Of course, a production-quality compiler must free its unused data in order to avoid wasting memory. The best way to do this is to use an automatic garbage collector, as described in Chapter 13. Without a garbage collector, the programmer must carefully free(p) when the structure p is about to become inaccessible not too late, or the pointer p will be lost, but not too soon, or else still-useful data may be freed. In order to be able to concentrate more on compiling techniques than on memory deallocation techniques, we can simply neglect to do any freeing.
7. 在程序设计的过程中，我们不会使用free来释放内存，尽管一个好的产品应该注意内存管理，但是我们希望更多的关注编译原理，而不是内存管理，所以这个编译器里没有free

## Basic

### lvalue
    variable -> a
    array -> a[2] b[2][2]
    record -> c.child.childchild.childchildchild

### NIL
NIL

### LPAREN RPAREN
just ()

### INT
10086 :)

### STRING
"fuck the ungreatfulness"

### UMINUS
-666

### func_call
    procedure -> foo()
    function with parameter -> bar("fuck the rubbish", 999)

### arith_exp
1+2

### cmp_exp
1>2

### bool_exp
1<>2 2=2

### record_create
rectype {name="Nobody", age=1000}

### array_create
arrtype [10] of 0

### assignment
a := 2

### IF exp THEN exp ELSE exp
    then-else with the same type: if 2 then 3 else 3
    if-then with no value:        if 2 then 

### WHILE exp DO exp
while 2>1 do ..

### FOR ID ASSIGN exp TO exp DO exp
for i := 1 to 10 do ..

### break
break must inside for loop, but it will be done in the type-check module

### LET decs IN expseq END
    type decl:
        let
            type rectype = {name:string, age:int}
            type arrtype = array of int
            type myint = int
        in
            1+1
        end

    variable decl:
        let
            type rectype = {name:string, age:int}
            type arrtype = array of int
            type myint = int
            var rec1:rectype := rectype {name="Nobody", age=1000}
            var arr1:arrtype := arrtype [10] of 0
            var int1:myint := 2
        in
            1+1
        end

    function decl:
        let
            function do_nothing(a: int, b: string):int = ( a := 1; 0 )
        in
            1+1
        end

### LPAREN expseq RPAREN
(1) (1;2) (1;2;3)

tips: remember the value of expseq is the value of last element

## Conflict

### lvalue
为什么我使用这样的语法? ->

    lvalue: ID lvalue_extension

    lvalue_extension:
                    | DOT ID lvalue_extension
                    | LBRACK exp RBRACK lvalue_extension

因为合在一起写会出现这样的情况：

    lvalue: ID
          | ID LBRACK ...

    ID .
    ID . LBRACK

那么这时候应该reduce shift? 所以写一个没有二义的语法

### if
怎么解决这个问题的？

    if exp then exp
    if exp then exp else exp

看看状态图去兄弟：

    then exp .
    then exp . else exp

大家应该都知道这时候要比较的是then和else的优先级吧？那么优先级给改一改不久行了么？

    %nonassoc THEN
    %nonassoc ELSE

### 书上的问题
    varable[exp]
    type-id[exp] of exp

应该都知道这是数组表达式和数组创建的conflict吧？

看看状态图

    RBRACK .
    RBRACK . OF

这里遇到的问题是不是shift reduce啊？而且这个shift reduce好像和其他的不同，因为我已经说过了，这是数组表达式和数组创建的conflict，两个根本不是一个路子的表达式，为啥要写在一起呢？

我的意思也就是说，让数组表达式自己归到lvalue这个河水，而数组创建自己一个井水，然后他们就...

看一下下面的状态图你应该知道了

    lvalue_extension: LBRACK exp RBRACK . lvalue_extension
    array_create: ID LBRACK exp RBRACK . OF exp

    LBRACK  shift, and go to state 89
    DOT     shift, and go to state 25
    OF      shift, and go to state 100

    $default  reduce using rule 23 (lvalue_extension)

tips: $default $表示EOF

### 你想知道的
为什么要 %nonassoc DO OF，我们一个一个来，一共有三个

    exp: WHILE exp DO exp .
    arith_exp: exp . PLUS exp

这时候应该怎么办？说白了就是这样 while 1=1 do 1+1 ，我们当然希望是shift了，让他加完嘛，所以DO的优先级是不是要低一些呢？ 可能吧:)

    array_create: ID LBRACK exp RBRACK OF exp .
    arith_exp: exp . PLUS exp

这个是不是一样的情况？还有一种我也不想解释了，自己看吧

    exp: FOR ID ASSIGN exp TO exp DO exp .
    arith_exp: exp . PLUS exp

> 也许你可以给一个star?

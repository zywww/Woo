# Woo

一个简单的编译器原型，语法规则继承了pascal和python的风格。

学习过程分享：

* 先写了一个简单的正则引擎，然后基于这个正则引擎快速写出词法分析器，参考[轮子哥的博客](http://www.cppblog.com/vczh/)。
* 语法分析部分就是经典的递归下降方法生成AST，codgen过程用虚函数，真的很简洁也利于理解，参考[llvm tutorial](http://llvm.org/docs/tutorial/index.html)。
* 虚拟机部分关键是理解工作原理，可以参考知乎上相关答案。

# Supports
* 语法
	* 类型：整数，字符串
	* 运算：算数表达式计算
	* 控制结构：while 
	* 条件表达式：`>`,`<`,`>=`,`<=`
	* 函数：参数列表，递归调用
	* 内置函数：print
* 编译器功能
	* 编译错误提示
	* 编译成功后，输出中间代码文件，需要配合Wvm解释中间代码。

# Syntax
语法特点：

* 每行代码以换行结束
* 变量不需要定义，直接使用，因为默认初始化为0了。
* 函数定义使用`def` 关键字，后面接一对圆括号，括号内为参数列表，函数以`end`结束。

	```
	def func()
		. . .
		代码
		. . . 
	end
	
	```

* `while`也需要以`end`结束

	```
	while(a>b)
		...
		...
	end
	
	```
* 内置输出函数`print(参数列表)`括号里可以是表达式或者字符串，也可以接收多个参数。
	
	```
	a = 1
	b = "hello"
	print(a,b)
	
	```
	
# Example

* 编译错误提示
![img](http://7xkpdt.com1.z0.glb.clouddn.com/16-2-27/3315691.jpg)

* 打印一个心型图案（其实包括Woo这个语言的命名，都是受[Leviathan1995](https://github.com/Leviathan1995)的鼓舞，haha）[源代码](https://github.com/TanSilver/Woo/blob/master/Debug/main.txt)

	![img](http://7xkpdt.com1.z0.glb.clouddn.com/16-2-27/11291333.jpg)




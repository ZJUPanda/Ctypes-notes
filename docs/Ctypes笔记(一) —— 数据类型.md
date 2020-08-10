# Ctypes笔记(一) —— 数据类型

`ctypes`是Python的外部函数库。它提供了与C兼容的数据类型，并允许调用DLL/SO或共享库中的函数。可使用该模块以纯Python形式对这些库进行封装。  

本文重点介绍使用`ctypes`过程中涉及到的Python和C之间各种数据类型的映射，关于具体的使用方法和操作流程可参考[Ctypes笔记(二) —— 操作流程]( \.\/Ctypes笔记\(二\)\ ——\ 操作流程\.md )。

---
## 函数参数传递

C函数`test_func()`的函数声明如下，该函数接受4个参数（`int`型变量`a`、float型变量`b`、指向`int`的指针变量`p`和`int`型一维数组`arr`），返回值类型为`float`。
```C
float test_func(int a, float b, int* p, int arr[5]);
```
在Python中该函数的步骤如下，需通过函数对象的`argtypes`属性设定函数的输入参数类型，函数返回类型默认为`c_int`，可通过设置函数对象的`restype`属性修改。
```python
# 1. 载入动态链接库
test_lib = ctypes.CDLL("./libtest.so")
# 2. 导入动态链接库中的函数
test_func = test_lib.test_func
# 3. 指定函数的参数类型与返回类型
test_func.argtypes = [ctypes.c_int, ctypes.c_float, ctypes.POINTER(ctypes.c_int), ctypes.c_int*5]
test_func.restype = ctypes.c_float
# 4. 调用函数
a = ctypes.c_int(1)
b = ctypes.c_float(2.0)
p = ctypes.pointer(ctypes.c_int(3))
arr = (ctypes.c_int*5)()
return_value = test_func(a, b, p, arr)
```

---
## ctypes数据类型
### 1. 基础数据类型
`ctypes`定义了一些和C兼容的基础数据类型：

| ctypes类型 | C类型 | Python类型 |
| --- | --- | --- |
| c_bool | _Bool | bool |
| c_char | char | 单字符字节对象 |
| c_wchar | wchar_t | 单字符字符串 |
| c_byte | char | int |
| c_ubyte | unsigned char | int |
| c_short | short | int |
| c_ushort | unsigned short | int |
| **c_int** | **int** | **int** |
| c_uint | unsigned int | int |
| c_long | long | int |
| c_ulong | unsigned long | int |
| c_longlong | __int64 或 long long | int |
| c_ulonglong | unsigned __int64 或 unsigned long long | int |
| c_size_t | size_t | int |
| c_ssize_t | ssize_t 或 Py_ssize_t | int |
| **c_float** | **float** | **float** |
| **c_double** | **double** | **float** |
| c_longdouble | long double | float |
| c_char_p | char * (以 NUL 结尾) | 字节串对象或 None |
| c_wchar_p | wchar_t * (以 NUL 结尾) | 字符串或 None |
| c_void_p | void * | int 或 None |

这些类型的使用方式非常简单，举例：
``` python
>>> i = c_int(42)  # 创建ctypes.c_int类型对象i
>>> print(i)  #  在sizeof(int) == sizeof(long)的系统中，c_int用c_long表示
c_long(42)
>>> print(i.value)  # 打印i的value属性
42
>>> i.value = -99  # 修改i的value属性
>>> print(i.value)
-99

>>> c = c_char_p(b"abc")  # python3默认宽字符编码（对应c_wchar_p类型），需转化为byte编码
>>> print(c)
c_char_p(1891469824720)
>>> print(c.value)
b'abc'
```

### 2. 数组
数组是一个序列，包含指定个数元素，且必须类型相同。  
#### 2.1 枚举法创建ctypes数组对象
首先构建与希望创建的数组对象相匹配的数组类型，使用枚举法，列出数组初始元素即可成功创建数组对象。
```python
>>> int_array_1d = c_int * 5  # 创建一个长度为5的一维int型数组类型
>>> array1 = int_array_1d(0,1,2,3,4)  # 构造一个int_array_1d对象array1，初值为{0, 1, 2, 3, 4}
>>> print(array1)
<__main__.c_long_Array_5 object at 0x...>
>>> print(array1[2])
2
>>> array1[2] = 0
>>> print(array1[2])
0

>>> int_array_2d = (c_int * 4) * 3  # 创建一个3×4的二维int型数组类型
>>> array2 = int_array_2d((0,0,0,0),(1,1,1,1),(2,2,2,2))  # 构造一个int_array_2d对象array2
>>> print(array2)
<__main__.c_long_Array_4_Array_3 object at 0x...>
>>> print(array2[1][2])
1
>>> array1[1][2] = 0
>>> print(array1[2])
0
```
#### 2.2 从列表创建ctypes数组对象


#### 2.3 从numpy数组创建ctypes数组对象
`numpy`提供了`ctypes`的相关模块`numpy.ctypeslib`，该模块提供了一系列方法用于`ctypes`对象和`numpy`对象间的转换。
|方法|功能|
|-|-|
|`as_array(obj, shape=None)`|Create a numpy array from a ctypes array or POINTER|
|`as_ctypes(obj)`|Create and return a ctypes object from a numpy array|

```
>>> array = np.array([[0,0,0,0],[1,1,1,1],[2,2,2,2]])
>>> array_c = np.ctypeslib.as_ctypes(array)
>>> print(array_c)
<c_long_Array_4_Array_3 object at 0x...>
>>> print('np: ', array[1][1], '\tctypes: ', array_c[1][1])
np:  1  ctypes:  1
>>> array[1][1] = 0
>>> print('np: ', array[1][1], '\tctypes: ', array_c[1][1])
np:  0  ctypes:  0
```

通过`numpy.ctypeslib.ndpointer(dtype=None, ndim=None, shape=None, flags=None)`方法，可直接将`numpy`数组传入调用的C函数中。  
该方法有四个可选参数，用于对将要传入的`ndarray`对象加以限制：
|类型|描述|
|-|-|
|dtype|数组数据类型|
|ndim|数组维数|
|shape|数组形状|
|flags|/|

`ndpointer`方法用于规范C函数的输入参数(`argtypes`))与返回值(`restypes`)   
假设调用的C函数声明如下，该函数有两个输入参数，3×4的`int`型数组`a`和`int`型变量`b`：
```C
void testfunc_array(int a[3][4], int b);
```
则需相应地定义`argstype = [numpy.ctypeslib.ndpointer(dtype=np.intc, ndim=2, shape=(3, 4), flags='C'), ctypes.c_int]`  
在调用C函数时，直接将对应的ndarray对象作为参数传入即可。

### 3. 结构体
结构体必须继承自`ctypes`模块中的`Structure`。子类必须定义`_fields_`属性。`_fields_`是一个二元组列表，二元组中包含`field name`和`field type`。  
`type`字段必须是一个`ctypes`类型，比如`c_int`，或者其他ctypes类型: 结构体、联合、数组、指针。  
我们在C中构造了一个结构体`test_struct`，该结构体包含一个`int`型变量`a`、一个`int`型数组变量`b`、一个`Point`型结构体变量`point`和一个指向自身类型的结构体指针变量`p_struct`。
```C
typedef struct Point {
    float x;
    float y;
} Point;

struct test_struct {
    int a;
    int b[5];
    Point point;
    struct test_struct* p_struct;
}
```
在Python中，如需向调用的C函数传递上述结构体类型变量，则需定义好对应的`ctypes.Structure`子类：
```python
class Point(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_float),
        ("y", ctypes.c_float)
    ]

# test_struct类的_fields_属性涉及到对自身的引用，可通过以下方式定义_fields_属性
class test_struct(ctypes.Structure): 
    pass
test_struct._fields_ = [
    ("a", ctypes.c_int),
    ("b", ctypes.c_int * 5),
    ("point", Point),
    ("p_struct", ctypes.POINTER(test_struct))
]
```
结构体对象的构造初始化方法如下：
```
>>> point = Point(1,2)
>>> point
<__main__.Point object at 0x...>
>>> print(point.x, point.y)
1.0 2.0
>>> 
>>> struct = test_struct(a=1, b = (ctypes.c_int*5)(1,2,3,4,5), point=Point(1,2), p_struct = None)
>>> struct
<__main__.test_struct object at 0x...>
>>> struct.b
<__main__.c_long_Array_5 object at 0x...>
>>> struct.p_struct
<__main__.LP_test_struct object at 0x...>
```

### 4. 指针
#### 创建指针对象
`ctypes`提供了`pointer()`和`POINTER()`两种方法创建指针，区别在于：`pointer()`用于将对象转化为指针，而`POINTER()`用于定义某个类型的指针。  
指针实例的`contents`属性存储了指针指向的真实对象。通过整数下标或`contents`属性均可访问或修改指针指向的真实对象内容。
```python
# pointer()
>>> int_obj = ctypes.c_int(3)
>>> int_p_obj = ctypes.pointer(int_obj)
>>> print(int_p_obj)
<ctypes.wintypes.LP_c_long object at 0x...>
>>> print(int_p_obj.contents)  # 使用contents方法访问指针
c_long(3)
>>> print(int_p_obj[0])  # 获取指针指向的值
3
# POINTER()
>>> int_p = ctypes.POINTER(ctypes.c_int)
>>> int_obj = ctypes.c_int(4)
>>> int_p_obj = int_p(int_obj)
>>> print(int_p_obj)
<ctypes.wintypes.LP_c_long object at 0x...>
>>> print(int_p_obj.contents)
c_long(4)
>>> print(int_p_obj[0])
4
```
#### `NULL`指针
在C语言中，指针操作通常需要用到空指针`NULL`，例如创建链表时。在`ctypes`中，通过无参调用指针类型可以创建一个`NULL`指针，`NULL`指针的布尔值是`False`.
```
>>> nullptr = ctypes.POINTER(ctypes.c_int) ()
>>> print(nullptr)
<ctypes.wintypes.LP_c_long object at 0x...>
>>> print(bool(nullptr))
False
```
#### 多级指针 
`ctypes`支持二级以上的指针类型

```
>>> pptr = ctypes.POINTER(ctypes.POINTER(ctypes.c_int))
>>> pptr_obj = pptr(ctypes.pointer(ctypes.c_int(2)))
>>> print(pptr_obj)
<__main__.LP_LP_c_long object at 0x...>
>>> print(pptr_obj.contents.contents)
c_long(2)
```

#### 传递指针对象
`ptr`为`ctypes`的一个指向`c_int`对象`a`( `c_int(2)` )的一级指针实例。

若所调用的C函数入参为一个指向`int`类型的一级指针变量，则直接将`ptr`作为实参传入，并在`argstype`的对应位置规范类型为：`ctypes.POINTER(ctypes.c_int)`

若所调用的C函数入参为一个指向`int`类型的二级指针变量，则需在`argstype`的对应位置规范类型为：`ctypes.POINTER(ctypes.POINTER(ctypes.c_int))`，而实参有以下两种传递方式( `pointer()`和`byref()` )。  
第一种，将`ctypes.pointer(ptr)`作为实参传入，相当于创建了一个指向`ptr`的指针对象，该对象所指的内容可变；  
第二种，将`ctypes.byref(ptr)`作为实参传入，该方式通过引用传递参数，不会创造新的指针对象。  
在Python代码本身不需要使用指针对象的情况下，使用 `byref()`的效率更高。


---
## 参考资料
1. [ctypes -- A foreign function library for Python](https://docs.python.org/3/library/ctypes.html#module-ctypes)
2. [C-Types Foreign Function Interface (numpy.ctypeslib)](https://numpy.org/doc/stable/reference/routines.ctypeslib.html)
3. [Python--ctypes(数据类型详细踩坑指南）](https://zhuanlan.zhihu.com/p/145165873)
4. [Python和C语言交互--ctypes，struct
](https://zhuanlan.zhihu.com/p/143026186)
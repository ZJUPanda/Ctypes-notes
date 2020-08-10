import ctypes

# interface
class Point(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_float),
        ("y", ctypes.c_float)
    ]


class Node(ctypes.Structure):
    pass


Node._fields_ = [
    ("i", ctypes.c_int),
    ("arr", ctypes.c_int*3),
    ("point", Point),
    ("next", ctypes.POINTER(Node))
]


class test:
    def __init__(self):
        test_lib = ctypes.CDLL("./libtest.so")
        self.insert_node = test_lib.insert_node
        self.delete_node = test_lib.delete_node
        self.print_list = test_lib.print_list
        self.insert_node.argtypes = [ctypes.POINTER(ctypes.POINTER(Node)), ctypes.c_int, ctypes.c_int*3, Point]
        self.delete_node.argtypes = [ctypes.POINTER(ctypes.POINTER(Node)), ctypes.c_int]
        self.print_list.argtypes = [ctypes.POINTER(Node)]
    def run(self):
        linkp = ctypes.POINTER(Node)()
        arr = (ctypes.c_int*3)(0, 1, 2)
        point = Point(3.0, 4.0)
        for i in range(3):
            self.insert_node(ctypes.byref(linkp), i, arr, point)
        self.print_list(linkp)
        print('deleting node 1...\n')
        self.delete_node(ctypes.byref(linkp), 1)
        self.print_list(linkp)


if __name__ == '__main__':
    test = test()
    test.run()
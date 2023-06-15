\$@ 表示 目标文件
\$^ 表示所有不重复的依赖的文件
\$< 表示第一个依赖的文件

%  通配符的作用：

与匹配模式一起使用，用于匹配文件名中的任意字符串

```makefile
%.o: %.c
equal to
f1.o: f1.c
f2.o: f2.c
```

通用样例

e.g.

```makefile
CXX = gcc
TARGET = main
OBJ = ./test/main.o ./test/tool.o

CXXFLAGS = -c -Wall
$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

%.o: %.c
	$(CXX) $(CXXFLAGS) $< -o $@ 

```

### 伪目标

e.g.

```makefile
.PHONY:clean
	#s生命该伪目标后，make将不会判断目标是否存在或者是否需要更新
```


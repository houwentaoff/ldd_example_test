default:all

all: test_aio

test_aio:test_app.o
	gcc $< -o $@ -laio

%.o:%.c
	gcc -c -fPIC $<  -o $@ 
	-@# -fPIC 参数 便于生成动态库.so,否则会出现 can not be used when making a shared object; recompile with -fPIC 这种错误提示

.PHONY:clean                                                                                                                                                      
clean:     
	-rm *.o  *.so *.a  -rf test_aio



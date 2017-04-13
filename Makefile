all: tvm379.o

CC_Object = gcc -w

tvm379.o : tvm379.c
	$(CC_Object) tvm379.c -lm -o tvm379

clean: 
	rm *.o -rf $(MAKE) clean

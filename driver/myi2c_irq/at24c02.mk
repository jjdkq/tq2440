CROSS=arm-linux-
OBJ=at24c02
$(OBJ): at24c02_test.c
	$(CROSS)gcc $^ -o $@ -lm   #-lm:链接使用数学库
	$(CROSS)strip $@
	cp $@ /work/nfs_tq/
clean:
	rm -rf *.o $(OBJ)
.PHONY:clean $(OBJ)

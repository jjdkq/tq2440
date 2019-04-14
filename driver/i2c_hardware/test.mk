CROSS=arm-linux-
#OBJ=at24c02
OBJ=bmp180
$(OBJ):main.c
	$(CROSS)gcc $^ -o $@ -lm   #-lm:链接使用数学库
	$(CROSS)strip $@
	cp $@ /work/nfs_tq/
clean:
	rm -rf *.o $(OBJ)
.PHONY:clean $(OBJ)

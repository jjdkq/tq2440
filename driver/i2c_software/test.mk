CROSS=arm-linux-
OBJ=bmp180
$(OBJ):test.c
	$(CROSS)gcc $^ -o $@ -lm  #-lm:链接使用数学库
	$(CROSS)strip $@
	cp $@ /work/nfs_tq/sbin
clean:
	rm -rf *.o $(OBJ)
.PHONY:clean $(OBJ)

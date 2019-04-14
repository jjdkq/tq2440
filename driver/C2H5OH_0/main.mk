CROSS:=arm-linux-
OBJ=C2H5OH_m
$(OBJ):main.c thread_0.c thread_2.c
	$(CROSS)gcc $^ -o $@ -lm -lpthread
	$(CROSS)strip $@
	cp $@ /work/nfs_tq/sbin
clean:
	rm -rf *.o $(OBJ)
.PHONY:clean $(OBJ)

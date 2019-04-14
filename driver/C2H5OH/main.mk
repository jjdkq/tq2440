CROSS:=arm-linux-
OBJ=C2H5OH
$(OBJ):main.c
	$(CROSS)gcc $^ -o $@ -lm
	$(CROSS)strip $@
	cp $@ /work/nfs_tq/sbin
clean:
	rm -rf *.o $(OBJ)
.PHONY:clean $(OBJ)

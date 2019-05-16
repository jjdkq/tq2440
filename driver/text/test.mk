CROSS=arm-linux-
OBJ=text
$(OBJ):main.c
	$(CROSS)gcc $^ -o $@
	$(CROSS)strip $@
	cp $(OBJ) /work/nfs_tq/sbin
clean:
	rm -rf *.o $(OBJ)
.PHONY:clean $(OBJ)


CROSS:=arm-linux-
OBJ=key
$(OBJ):main.c
	$(CROSS)gcc $^ -o $@
	$(CROSS)strip $@
	cp $@ /work/nfs_tq/sbin
clean:
	rm -rf $(OBJ) *.o
.PHONY:$(OBJ) clean

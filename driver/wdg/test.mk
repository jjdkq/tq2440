CROSS := arm-linux-
OBJ := wdg
$(OBJ): main.c
	$(CROSS)gcc $^ -o $@
	cp $@ /work/nfs_tq/sbin
clean:
	rm -r $(OBJ) *.o
.PHONY:$(OBJ) clean

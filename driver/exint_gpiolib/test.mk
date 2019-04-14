CROSS := arm-linux-
OBJ := exint
COPYDIR := /work/nfs_tq/sbin
$(OBJ): test.c
	$(CROSS)gcc $^ -o $@
	$(CROSS)strip $@
	cp $@ $(COPYDIR)
clean:
	rm *.o $(OBJ)
.PHONY: $(OBJ) clean

CROSS := arm-linux-
OBJ = w25q64
INSTALLDIR = /work/nfs_tq/sbin
$(OBJ): test.c
	$(CROSS)gcc $^ -o $@
	$(CROSS)strip $@
	cp $@ $(INSTALLDIR)
clean:
	rm -rf *.o $(OBJ)
.PHONY: clean $(OBJ)


CROSS := arm-linux-
OBJ = esp8266_test
INSTALLDIR = /work/nfs_tq/sbin
$(OBJ): *.c
	$(CROSS)gcc $^ -o $@
	$(CROSS)strip $@
	cp $@ $(INSTALLDIR)
clean:
	rm -rf *.o $(OBJ)
.PHONY: clean $(OBJ)


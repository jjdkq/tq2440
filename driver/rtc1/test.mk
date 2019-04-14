OBJ:=rtc
CROSS=arm-linux-
$(OBJ):rtc_driver_test.c
	$(CROSS)gcc $^ -o $@
	$(CROSS)strip $@
	cp $@ /work/nfs_tq/sbin
clean:
	rm -rf *.o $(OBJ)
.PHONY:$(OBJ) clean


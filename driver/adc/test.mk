CROSS:=arm-linux-
OBJ=adc
$(OBJ):test.c
	$(CROSS)gcc $^ -o $@
	$(CROSS)strip $@
	cp $@ /work/nfs_tq/sbin
clean:
	rm -rf *.o $(OBJ)
.PHONY:clean $(OBJ)

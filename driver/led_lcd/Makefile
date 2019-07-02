CROSS:=arm-linux-
OBJ=led_lcd
$(OBJ):*.c *.h
	$(CROSS)gcc $^ -o $@ -lpthread
	$(CROSS)strip $@
	cp $@ /work/nfs_tq/sbin
clean:
	rm -rf $(OBJ) *.o
.PHONY:$(OBJ) clean

CROSS:=arm-linux-
OBJ=lcd
$(OBJ):lcd_main.c lcd_test.c F*.c pic*.c *.h
	$(CROSS)gcc $^ -o $@
	$(CROSS)strip $@
	cp $@ /work/nfs_tq/sbin
clean:
	rm -rf $(OBJ) *.o
.PHONY:$(OBJ) clean

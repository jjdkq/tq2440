CROSS:=arm-linux-
led12:test.c
	$(CROSS)gcc $^ -o $@
	$(CROSS)strip $@
	cp $@ /work/nfs_tq/sbin
clean:
	rm -rf led12 *.o
.PHONY:leds clean

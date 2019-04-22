
.PHONY:all
all:test
test : serial.o test.o stm_cmd.o stm_reboot.o


clean:
	rm -rf ./*.o
	rm -rf ./test

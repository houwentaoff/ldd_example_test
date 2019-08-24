default:all

clean:
	for i in `ls`; do [ -d $$i ] && [ -f $$i/Makefile ] && $(MAKE) -C $$i clean; done

all:
	for i in `ls`; do [ -d $$i ] && [ -f $$i/Makefile ] && $(MAKE) -C $$i ; done

.PHONY:clean
	

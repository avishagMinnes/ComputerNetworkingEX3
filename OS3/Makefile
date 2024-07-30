SUBDIRS = ex1 ex2 ex3 ex4 ex5 ex6 ex7 ex8 ex9 ex10

.PHONY: all clean

all:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done


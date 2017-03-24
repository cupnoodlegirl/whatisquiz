CC := gcc
CFLAGS= -std=c99
LDLIBS :=  -lgdbm -lreadline -lm

whatisquiz: whatisquiz.c
        $(CC) whatisquiz.c -o whatisquiz $(LDLIBS) $(CFLAGS)

.PHONY: clean
clean:
        rm *.o

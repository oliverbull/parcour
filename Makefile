IDIR =./include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=./obj
LDIR=./lib
SDIR=./src

LIBS=-lpthread
DEBUG=-g3

_DEPS = logging.h worker1.h worker2.h sync.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o logging.o worker1.o worker2.o sync.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) $(DEBUG) -c -o $@ $< $(CFLAGS)

parcur: $(OBJ)
	$(CC) $(DEBUG) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o parcur

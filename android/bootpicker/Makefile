CC = agcc
CFLAGS=-c
LDFLAGS=-lcutils
SRC=picker.c
OBJ=$(SRC:.c=.o)
OUT=picker

all: $(SRC) $(OUT)
	
$(OUT): $(OBJ) 
	$(CC) $(LDFLAGS) $(OBJ) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
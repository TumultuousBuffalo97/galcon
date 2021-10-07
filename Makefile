# Make ADV

CC=cc
CFLAGS = -O
LDFLAGS = 

GALCON = galcon.o

all: 	galcon

galcon: $(GALCON)
	$(CC) $(LDFLAGS) $(GALCON) -o  galcon 

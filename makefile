OPTIONS = -Wall -pedantic -std=c99
OPT = -O0

all: processTurn.c
	cc $^ -o bin $(OPTIONS) $(OPT)

run:
	./bin "                                                                                                                A                                                                                                                " "AABBCCDDDDEEEEEEEEEEEEFFGGGHHIIIIIIIIIJKLLLLMMNNNNNNOOOOOOOOPPQRRRRRRSSSSTTTTTTUUUUVVWWXYYZ  " "AVOCADO"  

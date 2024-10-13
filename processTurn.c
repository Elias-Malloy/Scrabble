/*
created by eli malloy
march 31, 2024
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define EMPTY '.'

#define LOG

const int LETTER_VALUE[256] = {
	['A']=1, ['B']=1, ['C']=3, ['D']=2, ['E']=1, ['F']=4, ['G']=2,
	['H']=4, ['I']=1, ['J']=8, ['K']=5, ['L']=1, ['M']=3, ['N']=1,
	['O']=1, ['P']=3, ['Q']=10,['R']=1, ['S']=1, ['T']=1, ['U']=1,
	['V']=4, ['W']=4, ['X']=8, ['Y']=4, ['Z']=10
};

const char sortedBag[101] = "AAAAAAAAABBCCDDDDEEEEEEEEEEEEFFGGGHHIIIIIIIIIJKLLLLMMNNNNNNOOOOOOOOPPQRRRRRRSSSSTTTTTTUUUUVVWWXYYZ  ";

typedef enum SquareType {
	NORMAL = 0,
	DOUBLE_WORD = 1,
	TRIPLE_WORD = 2,
	DOUBLE_LETTER = 3,
	TRIPLE_LETTER = 4,
} SquareType;

const SquareType SQUARE_BONUSES[225] = {
	[0]=TRIPLE_WORD, [3]=DOUBLE_LETTER, [7]=TRIPLE_WORD, [11]=DOUBLE_LETTER, [14]=TRIPLE_WORD,
	[16]=DOUBLE_WORD, [20]=TRIPLE_LETTER, [24]=TRIPLE_LETTER, [28]=DOUBLE_WORD,
	[32]=DOUBLE_WORD, [36]=DOUBLE_LETTER, [38]=DOUBLE_LETTER, [42]=DOUBLE_WORD,
	[45]=DOUBLE_LETTER, [48]=DOUBLE_WORD, [52]=DOUBLE_LETTER, [56]=DOUBLE_WORD, [59]=DOUBLE_LETTER,
	[64]=DOUBLE_WORD, [70]=DOUBLE_WORD,
	[76]=TRIPLE_LETTER, [80]=TRIPLE_LETTER, [84]=TRIPLE_LETTER, [88]=TRIPLE_LETTER,
	[92]=DOUBLE_LETTER, [96]=DOUBLE_LETTER, [98]=DOUBLE_LETTER, [102]=DOUBLE_LETTER,
	[105]=TRIPLE_WORD, [108]=DOUBLE_LETTER, [112]=DOUBLE_WORD, [116]=DOUBLE_LETTER, [119]=TRIPLE_WORD,
	[122]=DOUBLE_LETTER, [126]=DOUBLE_LETTER, [128]=DOUBLE_LETTER, [132]=DOUBLE_LETTER,
	[136]=TRIPLE_LETTER, [140]=TRIPLE_LETTER, [144]=TRIPLE_LETTER, [148]=TRIPLE_LETTER,
	[154]=DOUBLE_WORD, [160]=DOUBLE_WORD,
	[165]=DOUBLE_LETTER, [168]=DOUBLE_WORD, [172]=DOUBLE_LETTER, [176]=DOUBLE_WORD, [179]=DOUBLE_LETTER,
	[182]=DOUBLE_WORD, [186]=DOUBLE_LETTER, [188]=DOUBLE_LETTER, [192]=DOUBLE_WORD,
	[196]=DOUBLE_WORD, [200]=TRIPLE_LETTER, [204]=TRIPLE_LETTER, [208]=DOUBLE_WORD,
	[210]=TRIPLE_WORD, [213]=DOUBLE_LETTER, [217]=TRIPLE_WORD, [221]=DOUBLE_LETTER, [224]=TRIPLE_WORD
};

// square index <-> x, y coordinate conversions
#define X(i) ((i) % 15)
#define Y(i) ((i) / 15)
#define I(x, y) (15 * (y) + (x))

/*
called from php
return value:
 0 on success
-1 on failure

case 1:
	argv[1] is the string "create-new"	

case 2:
	argv[1] is a 225 len string listing the characters on the board (from db)
	argv[2] is a 0-100 len string listing the characters in the bag (from db)
	argv[3] is a 0-7 len string listing the tiles in player1's hand (from db)
	argv[4] is a 0-7 len string listing the tiles in player2's hand (from db)
	argv[5] is a string encoding player1's score (from db)
	argv[6] is a string encoding player2's score (from db)
	argv[7] is a length 1 string encoding the player to move (0=player1, 1=player2) (from db)
	argv[8] is a string of (letter, index) pairs of tiles the user placed (from client)
			indexes are encoded as text numbers not binary numbers
on success, print the updated values
	board string
	bag string
	hand1 string
	hand2 string
	score1 string
	score2 string
	player1ToMove string
	winner string
*/
typedef struct LetterIndex {
	char letter;
	int index;
} LetterIndex;

int isScrabbleWord(const char *word);

int main(int argc, char **argv) {
	int i, j, k;

#ifdef LOG
	FILE *log = fopen("log.txt", "a");
	fprintf(log, "---args---\n");
	for (i = 1; i < argc; i++)
		fprintf(log, "%d: %s\n", i, argv[i]);
#endif		
	
	if (argc < 2)
		return -1;


	if (strcmp(argv[1], "create-new") == 0) {
		// print out new game
		
		// print empty board
		for (i = 0; i < 225; i++)
			putchar(EMPTY);
		putchar('\n');
		
		char shuffledBag[101];
		strcpy(shuffledBag, sortedBag);
		
		int r, temp;
		srand(time(NULL));
		// shuffle bag
		for (i = 0; i < 20; i++) {
			for (j = 0; j < 100; j++) {
				r = rand() % 100;
				temp = shuffledBag[j];
				shuffledBag[j] = shuffledBag[r];
				shuffledBag[r] = temp;
			}
		}
		// print bag
		printf("%s\n", shuffledBag + 14);
		
		// print hand1
		for (i = 0; i < 7; i++) {
			putchar(shuffledBag[i]);
		}
		putchar('\n');
		
		// print hand2
		for (i = 7; i < 14; i++) {
			putchar(shuffledBag[i]);
		}
		putchar('\n');
	
		// print player1 score, player2 score
		printf("0\n0\n");
		
		// print player1ToMove (0 or 1)
		printf("%d\n", rand() % 2);

		// print winner (null)
		putchar('\n');

		return 0;
	}

	if (argc < 9)
		return -1;

	// read command line arguments
	char *board = argv[1];
	char *bag 	= argv[2];
	char *hand1	= argv[3];
	char *hand2	= argv[4];
	int score1, score2, player1ToMove;
	sscanf(argv[5], "%d", &score1);
	sscanf(argv[6], "%d", &score2);
	sscanf(argv[7], "%d", &player1ToMove);

	char *input	= argv[8];

	if (strlen(board) != 225)
		return -1;

	if (strlen(bag) > 100)
		return -1;

	if (strlen(hand1) > 7)
		return -1;
	
	if (strlen(hand2) > 7)
		return -1;
	
	char *hand;
	if (player1ToMove) {
		hand = hand1;
	} else {
		hand = hand2;
	}

	LetterIndex placed[7];
	i = 0;
	j = 0;
	// parse input string of placed tiles
	// ex: E16L17I18 -> (E, 16), (L, 17), (I, 18)
	while (input[i]) {
		if (isalpha(input[i])) {
			// store letter
			placed[j].letter = input[i++];
			// store number
			if (isdigit(input[i]))
				sscanf(input + i, "%d", &placed[j].index);
			else
				return -1;

			j++;
			if (j >= 7)
				break;
			
			// skip the number characters	
			while (isdigit(input[i]))
				i++;
		} else {
			return -1;
		}
	}

	int min, numPlaced;
	LetterIndex temp;

	// save the number of placed tiles	
	numPlaced = j;

	// verify that at least one tile has been placed
	if (numPlaced == 0) {
		printf("You must place at least one tile to play a word\n");
		return -1;
	}

	// sort the (letter, index) pairs by index using selection sort
	for (i = 0; i < numPlaced - 1; i++) {
		min = i;
		for (j = i + 1; j < numPlaced; j++) {
			if (placed[j].index < placed[min].index)
				min = j;
		}
		temp = placed[min];
		placed[min] = placed[i];
		placed[i] = temp;
	}
	
	// verify that all placed tiles are from the player's hand
	int found;
	char handCopy[8];
	
	strncpy(handCopy, hand, 8);
	for (i = 0; i < numPlaced; i++) {
		found = 0;
		for (j = 0; j < 7; j++) {
			if (handCopy[j] == placed[i].letter ||
			   (islower(placed[i].letter) && handCopy[j] == ' ')) {	
				handCopy[j] = '!';
				found = 1;
				break;
			}
		}
		if (!found) {
			printf("Placed tiles must be from your deck\n");
			return -1;
		}
	}

	// verify that all tile indexes are in the range 0-224
	for (i = 0; i < numPlaced; i++) {
		if (placed[i].index < 0 || placed[i].index > 224) {
			printf("Placed tiles must be on the board\n");
			return -1;
		}
	}

	// verify that multiple tiles aren't placed on the same square
	for (i = 0; i < numPlaced; i++) {
		if (board[placed[i].index] != EMPTY) {
			printf("Two tiles may not be placed on the same square\n");
			return -1;
		}

		for (j = 0; j < numPlaced; j++) {
			if (i != j && placed[i].index == placed[j].index) {
				printf("Two tiles may not be placed on the same square\n");
				return -1;
			}
		}
	}

	// verify that placed tiles are in a connected line
	int horizontal = 1;
	int gap;
	if (numPlaced > 1) {
		horizontal = Y(placed[0].index) == Y(placed[1].index);
		if (horizontal) {
			for (i = 0; i < numPlaced - 1; i++) {
				if (Y(placed[i + 1].index) != Y(placed[0].index))
					return -1;
				
				gap = 1;
				while (board[placed[i].index + gap] != EMPTY)
					gap++;
				
				if (placed[i + 1].index - placed[i].index != gap)
					return -1;
			}
		} else {
			for (i = 0; i < numPlaced - 1; i++) {
				if (X(placed[i + 1].index) != X(placed[0].index)) {
					printf("Placed tiles must be in a connected line\n");
					return -1;
				}
				
				gap = 15;
				while (board[placed[i].index + gap] != EMPTY)
					gap += 15;
				
				if (placed[i + 1].index - placed[i].index != gap) {
					printf("Placed tiles must be in a connected line\n");
					return -1;
				}
			}
		}
	}
	
	int start, end, connected, score, wordScore, letterScore, multiplier;

	char word[16];

	int stride, strideAux, limit;
	if (horizontal) {
		stride = 1;
		strideAux = 15;
	} else {
		stride = 15;
		strideAux = 1;
	}

	// 1: verify that placed tiles are connected to tiles already on the board,
	// 2: verify that all the words
	// 3: calculate the score earned by the play
	connected = 0;
	score = 0;
	// read primary axis word
	start = placed[0].index;
	limit = horizontal ? start - X(start) : X(start);
	for (i = start; i > limit; i -= stride) {
	 	if (board[i - stride] == EMPTY)
			break;
	}
	start = i;
	
	end = placed[0].index;
	limit = stride * 14 + (horizontal ? end - X(end) : X(end));
	j = 1;
	for (i = end; i < limit; i += stride) {
		if (j < numPlaced && placed[j].index == i + stride)
			j++;
		else if (board[i + stride] == EMPTY)
			break;
	}
	end = i;
	wordScore = 0;
	multiplier = 1;
	j = 0;
	limit = end / stride - start / stride + 1;
	for (i = 0; i < limit; i++) {
		if (board[start + i * stride] != EMPTY) {
			word[i] = board[start + i * stride];
			wordScore += LETTER_VALUE[board[start + i * stride]];
			connected = 1;
		} else {
			word[i] = placed[j].letter;
			letterScore = LETTER_VALUE[placed[j].letter];
			switch (SQUARE_BONUSES[placed[j].index]) {
				case NORMAL: break;
				case DOUBLE_WORD: multiplier = 2; break;
				case TRIPLE_WORD: multiplier = 3; break;
				case DOUBLE_LETTER: letterScore *= 2; break;
				case TRIPLE_LETTER: letterScore *= 3; break;
			}
			wordScore += letterScore;	
			j++;
		}
	}
	// if one of the placed tiles is on a double or triple word, multiply the word score accordingly
	wordScore *= multiplier;

	// i equals the length of the word
	word[i] = '\0';

#ifdef LOG
	fprintf(log, "read word: %s\n", word);
#endif

	// 7 letter word bonus
	if (numPlaced == 7)
		wordScore += 50;

	if (i > 1) {
		// set letters in word to upper case for dictionary lookup and error reporting
		for (i = i - 1; i >= 0; i--)
			word[i] = toupper(word[i]);

		if (!isScrabbleWord(word)) {
			printf("%s is not a scrabble word\n", word);
			return -1;
		}
		
		score += wordScore;
	}

	// read auxilary axis words
	for (j = 0; j < numPlaced; j++) {
		start = placed[j].index;
		limit = horizontal ? X(start) : start - X(start);
		for (i = start; i > limit; i -= strideAux) {
			if (board[i - strideAux] == EMPTY)
				break;
		}
		start = i;
			
		end = placed[j].index;
		limit = strideAux * 14 + (horizontal ? X(end) : end - X(end));
		for (i = end; i < limit; i += strideAux) {
			if (board[i + strideAux] == EMPTY)
				break;
		}
		end = i;

		// no tiles on board, above or below
		if (start == end)
			continue;
		
		connected = 1;
			
		wordScore = 0;
		multiplier = 1;
		limit = end / strideAux - start / strideAux + 1;
		for (i = 0; i < limit; i++) {
			if (board[start + i * strideAux] != EMPTY) {
				word[i] = board[start + i * strideAux];
				wordScore += LETTER_VALUE[board[start + i * strideAux]];		
			} else {
				// only executed once
				word[i] = placed[j].letter;
				letterScore = LETTER_VALUE[placed[j].letter];
				switch (SQUARE_BONUSES[placed[j].index]) {
					case NORMAL: break;
					case DOUBLE_WORD: multiplier = 2; break;
					case TRIPLE_WORD: multiplier = 3; break;
					case DOUBLE_LETTER: letterScore *= 2; break;
					case TRIPLE_LETTER: letterScore *= 3; break;
				}
				wordScore += letterScore;	
			}
		}
		// if one of the placed tiles is on a double or triple word, multiply the word score accordingly
		wordScore *= multiplier;

		// i equals the length of the word
		word[i] = '\0';

#ifdef LOG
		fprintf(log, "read aux word: %s\n", word);
#endif

		// set letters in word to upper case for dictionary lookup and error reporting
		for (i = i - 1; i >= 0; i--)
			word[i] = toupper(word[i]);


		if (!isScrabbleWord(word)) {
			printf("%s is not a scrabble word\n", word);
			return -1;
		}

		score += wordScore;
	}

	// if tiles are placed on central star (index 112), connected is true
	for (i = 0; i < numPlaced; i++) {
		if (placed[i].index == 112)
			connected = 1;
	}

	if (!connected) {
		printf("Placed tiles must connect to tiles on the board\n");
		return -1;
	}

	// add score of play to total for the player who made the play;
	if (player1ToMove)
		score1 += score;
	else
		score2 += score;


	// fill in board with placed tiles
	for (i = 0; i < numPlaced; i++)
		board[placed[i].index] = placed[i].letter;

	// print updated board
	printf("%s\n", board);

	int top = strlen(bag) - 1;
	hand[0] = '\0';
	int handlen = 0;
	// tiles used from hand in word are marked in hand copy as '!'
	for (i = 0; i < strlen(handCopy); i++) {
		if (handCopy[i] != '!')
			hand[handlen++] = handCopy[i];
	}
	
	// draw tiles from bag and place them in hand
	while (handlen < 8 && top >= 0) {
		hand[handlen++] = bag[top];
		bag[top] = '\0';
		top--;
	}
	hand[handlen] = '\0'; // forgetting this was an annoying bug

	// output updated bag
	printf("%s\n", bag);

	// if bag is empty and hand is empty calculate final score and report winner
	if (handlen == 0) {
		// hand is empty and out of tiles
		for (i = 0; i < strlen(hand2); i++)
			score2 -= LETTER_VALUE[hand2[i]];
		for (i = 0; i < strlen(hand1); i++)
			score1 -= LETTER_VALUE[hand1[i]];
		
		// print both empty hands
		printf("\n\n");
		// print score1
		printf("%d\n", score1);	
		// print score2
		printf("%d\n", score2);
		// print null to move string
		printf("\n");
		// print winner
		printf("%d\n", score1 > score2);	
	}

	// output hand1
	printf("%s\n", hand1);
	// output hand2
	printf("%s\n", hand2);
	// output updated score1
	printf("%d\n", score1);
	// output updated score2
	printf("%d\n", score2);
	// output swapped to move
	printf("%d\n", !player1ToMove);
	// output null winner
	printf("\n");

#ifdef LOG
	fprintf(log, "hand1: %s\nhand2: %s\n", hand1, hand2);
	fclose(log);
#endif

	return 0;
}

int isScrabbleWord(const char *word) {
    if (word == NULL)
        return 0;

    if (!isupper(word[0]))
        return 0;

    char filename[] = "dictionary/_-words.txt";
    filename[11] = word[0];
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
        return 0;

    char str[256];
    int ch, i, difference;
    while ((ch = getc(fp)) != EOF) {
        i = 0;
        str[i++] = ch;
        while((ch = getc(fp)) != '\n')
            str[i++] = ch;
        str[i] = '\0';
        difference = strcmp(str, word);
        if (difference == 0) {
            fclose(fp);
            return 1;
        } else if (difference > 0) {
            fclose(fp);
            return 0;
        }
    }
    fclose(fp);
    return 0;
}

/* divide dictionary into seperate files 
   scrabble dictionary source: https://boardgames.stackexchange.com/questions/38366/latest-collins-scrabble-words-list-in-text-file
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(void) {
	FILE *fp, *new;
	char filename[] = "Collins Scrabble Words (2019).txt";
	char *buffer = (char *) malloc(1024 * sizeof(char));
	int ch, letter;
	

	if (buffer == NULL) return -1;
	// open dictionary
	fp = fopen(filename, "r");

	if (fp == NULL) return -1;

	new = NULL;
	letter = 0;
	while((ch = getc(fp)) != EOF) {
		if (!isupper(ch)) {
			printf("fatal error: %c(%d) in input\n", ch, ch);
		}
		if (ch != letter) {
			if (new != NULL)
				fclose(new);
			strcpy(filename, "_-words.txt");
			filename[0] = (char) ch;
			printf("opening %s.\n", filename);
			new = fopen(filename, "w");
			if (new == NULL) continue;
			letter = ch;
		}

		putc(ch, new);
		while ((ch = getc(fp)) != '\n') {
			if (ch != '\r')
				putc(ch, new);
		}
		putc('\n', new);
	}

	if (new != NULL)
		fclose(new);

	free(buffer);
	fclose(fp);
	return 0;
}

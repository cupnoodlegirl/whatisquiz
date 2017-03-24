#include <stdio.h>
#include <gdbm.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <readline/readline.h>

#define DBPATH "/var/cache/man/index.db"

/**
  count gdbm entry
 **/
typedef unsigned long long int gdbm_count_t;
int gdbm_count(GDBM_FILE dbf, gdbm_count_t * pcount)
{
	datum key, next, data;
	key = gdbm_firstkey(dbf);

	*pcount = 0;
	while (key.dptr) {
		*pcount = *pcount + 1;
		next = gdbm_nextkey(dbf, key);
		key = next;
	}
	return 0;
}

/**
  search key by id(number)
 **/
datum gdbm_key_by_index(GDBM_FILE dbf, gdbm_count_t * pcount)
{
	datum key, next, data;
	key = gdbm_firstkey(dbf);

	for (int i = 1; i <= *pcount; i++) {
		if (i > 1) {
			next = gdbm_nextkey(dbf, key);
			key = next;
		}
	}

	return key;
}

/**
  used for option
 **/
struct Option {
	datum key;
	datum data;
};

int main()
{
	GDBM_FILE dbf;

	// open gdbm db
	dbf = gdbm_open(DBPATH, 0, GDBM_READER, 0, NULL);
	if (!dbf) {
		printf("%s\n", gdbm_strerror(gdbm_errno));
		exit(1);
	}
	// count all record in db
	gdbm_count_t count;
	int ret;
	ret = gdbm_count(dbf, &count);
	printf("There are %llu entries in MANDB\n", count);

	// start interactive quiz
	char *input;
	for (;;) {
		struct Option options[4];
		gdbm_count_t rand_num[4];
		datum key[4];
		srand((unsigned)time(NULL));
		int answer = floor(rand() % 4);
		for (int i = 0; i < 4; i++) {
			rand_num[i] = floor(rand() * count / RAND_MAX);
			options[i].key = gdbm_key_by_index(dbf, &rand_num[i]);
			options[i].data = gdbm_fetch(dbf, options[i].key);
			/** TODO: the case of key is null **/
		}
		char *data = options[answer].data.dptr;
		char *pch = strtok(data, "\t");
		char *ptmp = pch;
		while (1) {
			ptmp = strtok(NULL, "\t");
			if (ptmp == NULL) {
				break;
			} else {
				pch = ptmp;
			}
		}
		printf("%s\n", pch);
		printf("1 %20s 2 %20s\n", options[0].key.dptr,
		       options[1].key.dptr);
		printf("3 %20s 4 %20s\n", options[2].key.dptr,
		       options[3].key.dptr);
		input = readline(">> ");
		if (strcmp(input, "exit") == 0)
			return 0;
		else if (strcmp(input, "test") == 0)
			printf("Test!\n");
		else if (atoi(input) > 0 && atoi(input) <= 4) {
			int select = atoi(input);
			if (answer + 1 == select) {
				printf("great!!\n");
			} else {
				printf("incorrect!!\n");
				printf("answer is %d\n", answer);
			}
		} else
			printf("Unknown command.\n");
		free(input);
	}

	gdbm_close(dbf);

	return 0;
}

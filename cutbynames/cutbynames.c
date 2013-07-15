#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUF_LEN (256 * 16)

void trim_title(char *title)
{
	int i;
	for (i = strlen(title) - 1; i >= 0; --i) {
		if (isspace(title[i]))
			title[i] = 0;
		else
			break;
	}
}

void set_file_name(char *title, char *filename)
{
	strcpy(filename, title);
	strcat(filename, ".txt");
}

int main()
{
	static char title[BUF_LEN], txt[BUF_LEN],
		filename[BUF_LEN + 4];
	FILE *ftitles = fopen("titles.txt", "rt");
	FILE *ftxt = fopen("text.txt", "rt");
	FILE *fout = NULL;
	int lineno = 0;

	mkdir("txts", 0755);
	chdir("txts");
	
	fgets(title, sizeof(title), ftitles);
	trim_title(title);
	set_file_name(title, filename);
	
	for (;;) {
		fgets(txt, sizeof(txt), ftxt);
		++lineno;
		if (lineno % 1000 == 0)
			printf("line %d\n", lineno);
		if (lineno > 64000)
			break;
		if (strstr(txt, title)){
			printf("found %s\n", txt);
						
			fout = fopen(filename, "w+t");
			fgets(title, sizeof(title), ftitles);
			trim_title(title);
			set_file_name(title, filename);
			
			if (title[0] == 0) {
				title[0] = 1;
			} else {
				printf("next seek for %s\n", title);
			}
		}
		if (fout)
			fputs(txt, fout);
		if (feof(ftxt))
			break;
	}
	return 0;
}

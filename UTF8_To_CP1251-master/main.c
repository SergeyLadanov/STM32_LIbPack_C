#include "cp1251_converter.h"

#define LINE_MAX 1024



int main(int argc, char ** argv)
{
	//printf("start %s\n", argv[0]);

	if (argc <= 1) {
		printf("forgot file name for conversion");
		exit(EXIT_FAILURE);
	}

	FILE* file = fopen(argv[1], "r");
	FILE* fileout = fopen("out.txt", "w");
	if (!file) {
		perror("can't open file");
		exit(EXIT_FAILURE);
	}

	while(!feof(file)) {
		char buff[LINE_MAX] = {0};
		if (!fgets(buff, LINE_MAX, file)) {
			perror("can't read line from file");
			exit(EXIT_FAILURE);
		}

		char output[LINE_MAX] = {0};
		if (!utf8_buf_to_cp1251(buff, output, LINE_MAX)) {
			printf("can't convert line: %s\n", buff);
			exit(EXIT_FAILURE);
		}
		printf("%s", output);
		fwrite(output, 1, strlen(output), fileout);
	};

	fclose(fileout);

	return EXIT_SUCCESS;
}



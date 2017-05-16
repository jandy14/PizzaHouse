#include "Option.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void ParsingOption(int argc, char** argv, char** pDebugFile, char** pOutputFile)
{
	int param_opt;
	opterr = 0;
	while(-1 != (param_opt = getopt(argc,argv,"d:o:")))
	{
		switch(param_opt)
		{
		case 'd':
			*pDebugFile = optarg;
			break;
		case 'o':
			*pOutputFile = optarg;
			break;
		case '?':
			printf("%c : It is not this program's option\n", optopt);
			printf("Please Read Readme File\n");
			exit(0);
		}
	}
	return;
}
// -----------------------------------------------------------------------
// NAME : Jung Sung-wook	User ID : 20123307
// DUE DATE : 05/17/2017
// PROJECT #1
// FILE NAME : Option.c
// PROGRAM PURPOSE :
//	Option.h에 선언된 함수를 정의 합니다.
// -----------------------------------------------------------------------

#include "Option.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// -----------------------------------------------------------------------
// FUNCTION ParsingOption :
//	옵션을 확인하고 옵션의 값을 매개변수에 넣어줍니다
// PARAMETER USAGE :
//	argc : main의 변수 argc의 값을 복사
//	argv : main의 변수를 그대로 받아온 변수
//	pDebugFile : 디버그파일의 이름을 저장할 포인터 변수
//	pOutputFile : 출력파일의 이름을 저장할 포인터 변수
// FUNCTION CALLED :
//	in <stdio.h>
//		printf()
//	in <stdlib.h>
//		exit()
//	in <unistd.h>
//		getopt()
// -----------------------------------------------------------------------
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
#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

int calculator()
{
	int i, num1 = 0, num2 = 0, flag = 1, res = 0;
	char bufr[128];
	printf("====================================================\n");
	printf("*               Simple Calculator                     =\n");
	printf("====================================================\n");
	printf("*   Only for two int pararmeters,  q to quit      =\n");
	printf("***************************************************\n\n");
	while(flag == 1){	
		printf("Please input num1:");
		clearArray(bufr);
		i = read(0, bufr, 128);
		if (bufr[0] == 'q')
			break;
		
		num1 = getNum(bufr);
		
		printf("num1: %d\n", num1);
		printf("Please input num2:");
		clearArray(bufr);
		i = read(0, bufr, 128);	
		if (bufr[0] == 'q')
			break;
		num2 = getNum(bufr);
		printf("num2: %d\n", num2);
		printf("Please input op( + - * / ):");
		i = read(0, bufr, 1);
		switch(bufr[0])
		{
			case '+':
				res = num1 + num2;
				printf("%d + %d = %d\n", num1, num2, res);
				break;
			case '-':
				res = num1 - num2;
				printf("%d - %d = %d\n", num1, num2, res);
				break;
			case '*':
				res = num1 * num2;
				printf("%d * %d = %d\n", num1, num2, res);
				break;
			case '/':
				if(num2 <= 0)
				{
					printf("Num2 = Zero!\n");
					break;
				}
				res = num1 / num2;
				printf("%d / %d = %d\n", num1, num2, res);
				break;
			case 'q':
				flag = 0;
				break;
			default:
				printf("No such command!\n");
		}
	}
	return 0;
}

int getNum(char * bufr)
{
	int  i = 0, res = 0;
	for (i = 0; i < strlen(bufr); i++)
	{
		int num = bufr[i] - '0';
		if(num>=0&num<=9)
		{
			res = res*10+num;
		}
		else
		{
			break;
		}
	}
	
	return res;
}

void clearArray(char *arr)
{
	for(int i=0;i<strlen(arr);i++)
	{
		arr[i]=' ';
	}
}
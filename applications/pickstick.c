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

int randomBetween(int left, int right)
{
    int i = 0;
    i = rand(); 
    while(1)
    {
        i = i / (left+2);
        if(i >= left & i <= right)
        {
            break;
        }
        else if(i <= left)
        {
            i = rand();
        }
    }
    return i;
}

int pickSticks()
{
	int computer, people, spare = randomBetween(10, 60);
	char* bufr[10];
    printf(" --------------------------------------------------------\n");
	printf(" -------------------- Pick sticks game  ----------------\n");
	printf("---Pick 1~4 sticks every time, avoid picking the last stick--- \n");
	printf(" -----------------Trt to beat computer!--------------------\n");
    printf(" --------------------------------------------------------\n\n");
    printf("Game begin:\n\n");
    while(1)
    {
        printf(" ----------  Now we have %d sticks ----------\n", spare);
		printf("You:");
		int i = read(0, bufr, 10);	
		if(bufr[0]=='q')
		{
			break;
		}
        people = getNum(bufr);
        clearArr(bufr, 10);
        if(people<1 || people>4 || people>spare)
        {
            printf("Error in input stick number!\n\n");
            continue;
        }
        spare = spare - people;
        if(spare == 0 || spare == 1)
        {
            switch(spare)
            {
                case 0:
                    printf("\nComputer win!\n");
                    break;
                case 1:
                    printf("\nYou win!\n");
                    break;
            }
            printf("Start a new game?(Y/other letters)\n");
            char c = ' ';
            read(0, c, 1);
            if (c == 'Y')
            {
                pickSticks();
                return 0;
            }
            else
            {
                break;
            }
        }
        while(1)
        {
            computer = randomBetween(1,4);
            if(spare - computer < 1) 
                continue;
            else 
                break;
        }
        spare = spare - computer;
        printf("Computer:%d  \n", computer);
    }
    return 0;
}
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

int pickSticks()
{
	int computer, people, spare = 23;
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
        if(people<1 || people>4 || people>spare)
        {
            printf("Error in input stick number!\n\n");
            continue;
        }
        spare = spare - people;
        if( spare==0 )
        {
            printf("\nComputer win! Game Over!\n");
            break;
        }
        if( spare==1 )
        {
            printf("\nYou win! Game Over!\n");
            break;
        }
        computer = 5 - people;
        spare = spare - computer;
        printf("Computer:%d  \n", computer); 
    }
    return 0;
}

int getNumber(char *);
void ClearArray(char*);
int guessNumber()
{
	int max = 1000,min = 0,target = 0;
	int playTimes = 2;
	int quit = 0;
	while(1)
	{
		if(quit == 1) break;
		printf("Input a number between 0 and 1000 to start");
		playTimes=2;
		char buffer[128];
		while(playTimes > 0)
		{
			if(quit == 1) break;
			int i = 0;
			ClearArray(buffer);
			min = 0;
			max = 1000;
			target = rand()%1000;
			int count1 = 6;
			int guess = 0;
			printf("\nYour play times: %d\n",playTimes);
			while(count1)
			{
				clearArray(buffer);
				if(count1 != 6) printf("Wrong! ");
				printf("You still have: %d times to try.\n",count1);
				printf("Input your answer(range:%d - %d) : ",min,max);
				i = read(0, buffer, 128);
				if (buffer[0] == 'q') {
					printf("See you next time! Bye!\n");
					quit = 1;
					break;
				} else {
					guess = getNumber(buffer);
				}
				if(guess > max || guess < min) {
					printf("please input the number in the range!\n");
					continue;
				}
                if(guess == target)
				{
					printf("Congratulation!");
					break;
				}
				else
					if(guess < target)
					{
						min = guess;
					}
					else
					{
						max = guess;
					}
					--count1;
			}
			if(count1)
				break;
			--playTimes;
			printf("The correct answer is: %d\n", target);
		}
	}
}

int getNumber(char * bufr)
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

void ClearArray(char *arr)
{
	for(int i=0;i<strlen(arr);i++)
	{
		arr[i]=' ';
	}
}

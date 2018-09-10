
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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


/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
    disp_str("-----\"kernel_main\" begins-----\n");

    struct task* p_task;
    struct proc* p_proc= proc_table;
    char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
    u16   selector_ldt = SELECTOR_LDT_FIRST;
    u8    privilege;
    u8    rpl;
    int   eflags;
    int   i, j;
    int   prio;

    //似乎可以加开机动画


    //启动进程
    for (i = 0; i < NR_TASKS+NR_PROCS; i++)
    /*for (i = 0; i < NR_TASKS; i++)*/
    {
        if (i < NR_TASKS)
        {   /* 任务 */
            p_task    = task_table + i;
            privilege = PRIVILEGE_TASK;
            rpl       = RPL_TASK;
            eflags    = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1   1 0010 0000 0010(2)*/
            prio      = 15;     //设定优先级为15
        }
        else
        {   /* 用户进程 */
            p_task    = user_proc_table + (i - NR_TASKS);
            privilege = PRIVILEGE_USER;
            rpl       = RPL_USER;
            eflags    = 0x202; /* IF=1, bit 2 is always 1              0010 0000 0010(2)*/
            prio      = 5;     //设定优先级为5
        }

        strcpy(p_proc->name, p_task->name); /* 设定进程名称 */
        p_proc->pid = i;            /* 设定pid */

        p_proc->ldt_sel = selector_ldt;

        memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
               sizeof(struct descriptor));
        p_proc->ldts[0].attr1 = DA_C | privilege << 5;
        memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
               sizeof(struct descriptor));
        p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
        p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

        p_proc->regs.eip = (u32)p_task->initial_eip;
        p_proc->regs.esp = (u32)p_task_stack;
        p_proc->regs.eflags = eflags;

        p_proc->p_flags = 0;
        p_proc->p_msg = 0;
        p_proc->p_recvfrom = NO_TASK;
        p_proc->p_sendto = NO_TASK;
        p_proc->has_int_msg = 0;
        p_proc->q_sending = 0;
        p_proc->next_sending = 0;

        for (j = 0; j < NR_FILES; j++)
            p_proc->filp[j] = 0;

        p_proc->ticks = p_proc->priority = prio;

        p_task_stack -= p_task->stacksize;
        p_proc++;
        p_task++;
        selector_ldt += 1 << 3;
    }

    //初始化进程
    k_reenter = 0;
    ticks = 0;

    p_proc_ready = proc_table;

    init_clock();
    init_keyboard();

    restart();

    while(1){}
}


/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
PUBLIC int get_ticks()
{
    MESSAGE msg;
    reset_msg(&msg);
    msg.type = GET_TICKS;
    send_recv(BOTH, TASK_SYS, &msg);
    return msg.RETVAL;
}

PUBLIC void addTwoString(char *to_str,char *from_str1,char *from_str2){
    int i=0,j=0;
    while(from_str1[i]!=0)
        to_str[j++]=from_str1[i++];
    i=0;
    while(from_str2[i]!=0)
        to_str[j++]=from_str2[i++];
    to_str[j]=0;
}

 

void displayWelcomeInfo(){
    printf("=============================================================================\n");
    printf("=============================================================================\n");
    printf("       =====        ________                ___________                \n");
    printf("   _D _|  | _______/        \\__I_I_____===__|________|     ______________ \n");
    printf("    |(_)      |   H\\________/ |   |        =|___ ___|      |\n");
    printf("    /     |   |   H  |  |     |   |         ||_| |_||     _| Liang Yongchao  \\\n");
    printf("   |      |   |   H  |__                    | | __| |   =|    A Yuan \n");
    printf("   | _ _______|___H__/__|_____/[][]~\\_______|       |    |   Chen Runqian      \n");
    printf("   |/ |   |            I_____I [][] []  D   | ======|____|___________________\n");
    printf(" __/ =|   | = 0=====0=====0=====0 \\ ____Y___________|__|_____________________\n");
    printf("  |/ =|___| =    ||    ||    ||    |_____/~\\___/          |_D__D__D_|  |_D__D\n");
    printf("   \\_/       \\__/  \\__/  \\__/  \\__/      \\_/               \\_/   \\_/    \\_/   \n");
    printf("=============================================================================\n");
    printf("======================Type 'help' for more information!======================\n");
    printf("=============================================================================\n");
}

void shell(char *tty_name){
	int fd;

    char rdbuf[512];
    char cmd[512];
    char arg1[512];
    char arg2[512];
    char buf[1024];
    char temp[512];

    int fd_stdin  = open(tty_name, O_RDWR);
    assert(fd_stdin  == 0);
    int fd_stdout = open(tty_name, O_RDWR);
    assert(fd_stdout == 1);

    animation();
    clear();
    
    displayWelcomeInfo();
    char current_dirr[512] = "/";
   
    while (1) {  
        //清空数组中的数据用以存放新数据
        clearArr(rdbuf, 512);
        clearArr(cmd, 512);
        clearArr(arg1, 512);
        clearArr(arg2, 512);
        clearArr(buf, 1024);
        clearArr(temp, 512);

        printf("~%s$ ", current_dirr);

        int r = read(fd_stdin, rdbuf, 512);
        clear();

        if (strcmp(rdbuf, "") == 0)
            continue;

        //以空格为分界，解析提取命令
        int i = 0;
        int j = 0;
        while(rdbuf[i] != ' ' && rdbuf[i] != 0)
        {
            cmd[i] = rdbuf[i];
            i++;
        }
        i++;
        while(rdbuf[i] != ' ' && rdbuf[i] != 0)
        {
            arg1[j] = rdbuf[i];
            i++;
            j++;
        }
        i++;
        j = 0;
        while(rdbuf[i] != ' ' && rdbuf[i] != 0)
        {
            arg2[j] = rdbuf[i];
            i++;
            j++;
        }
        //清空缓冲区
        rdbuf[r] = 0;

        if (strcmp(cmd, "process") == 0)
        {
            ProcessManage();
        }
        else if (strcmp(cmd, "help") == 0)
        {
            help();
        }      
        else if (strcmp(cmd, "clear") == 0)
        {
            printTitle();
        }
        else if (strcmp(cmd, "ls") == 0)
        {
            ls(current_dirr);
        }
        else if (strcmp(cmd, "create") == 0)
        {
            if(strcmp(arg1, "") == 0)
            {
                printf("Please input a filename!\n");
                continue;
            }

            if(arg1[0]!='/'){
                addTwoString(temp,current_dirr,arg1);
                memcpy(arg1,temp,512);                
            }

            fd = open(arg1, O_CREAT | O_RDWR);
            if (fd == -1)
            {
                printf("Failed to create file! Please check the filename!\n");
                continue ;
            }
            write(fd, buf, 1);
            printf("File created: %s (fd %d)\n", arg1, fd);
            close(fd);
        }
        else if (strcmp(cmd, "view") == 0)
        {
            if(strcmp(arg1, "") == 0)
            {
                printf("Please input a filename!\n");
                continue;
            }

            if(arg1[0]!='/'){
                addTwoString(temp,current_dirr,arg1);
                memcpy(arg1,temp,512);                
            }

            fd = open(arg1, O_RDWR);
            if (fd == -1)
            {
                printf("Failed to open file! Please check the filename!\n");
                continue ;
            }
            if (!verifyFilePass(arg1, fd_stdin))
            {
                printf("Authorization failed\n");
                continue;
            }
            read(fd, buf, 1024);
            close(fd);
            printf("%s\n", buf);
        }
        else if (strcmp(cmd, "modify") == 0)
        {
            if(strcmp(arg1, "") == 0)
            {
                printf("Please input a filename!\n");
                continue;
            }

            if(arg1[0]!='/'){
                addTwoString(temp,current_dirr,arg1);
                memcpy(arg1,temp,512);                
            }

            fd = open(arg1, O_RDWR);
            if (fd == -1)
            {
                printf("Failed to open file! Please check the filename!\n");
                continue ;
            }
            if (!verifyFilePass(arg1, fd_stdin))
            {
                printf("Authorization failed\n");
                continue;
            }
            int tail = read(fd_stdin, rdbuf, 512);
            rdbuf[tail] = 0;

            write(fd, rdbuf, tail+1);
            close(fd);
        }
        else if (strcmp(cmd, "delete") == 0)
        {
            if(strcmp(arg1, "") == 0)
            {
                printf("Please input a filename!\n");
                continue;
            }

            if(arg1[0]!='/'){
                addTwoString(temp,current_dirr,arg1);
                memcpy(arg1,temp,512);                
            }

            int result;
            result = unlink(arg1);
            if (result == 0)
            {
                printf("File deleted!\n");
                continue;
            }
            else
            {
                printf("Failed to delete file! Please check the filename!\n");
                continue;
            }
        }
        else if (strcmp(cmd, "cp") == 0)
        {
            if(strcmp(arg1, "") == 0)
            {
                printf("Please input a filename!\n");
                continue;
            }

            //首先获得文件内容
            if(arg1[0]!='/'){
                addTwoString(temp,current_dirr,arg1);
                memcpy(arg1,temp,512);                
            }
            fd = open(arg1, O_RDWR);
            if (fd == -1)
            {
                printf("File not exists! Please check the filename!\n");
                continue ;
            }
            
            int tail = read(fd, buf, 1024);
            close(fd);

            if(arg2[0]!='/'){
                addTwoString(temp,current_dirr,arg2);
                memcpy(arg2,temp,512);                
            }
            /*然后创建文件*/
            fd = open(arg2, O_CREAT | O_RDWR);
            if (fd == -1)
            {
                //文件已存在，什么都不要做
            }
            else
            {
                //文件不存在，写一个空的进去
                char temp2[1024];
                temp2[0] = 0;
                write(fd, temp2, 1);
                close(fd);
            }
             
            //给文件赋值
            fd = open(arg2, O_RDWR);
            write(fd, buf, tail+1);
            close(fd);
        }
        else if (strcmp(cmd, "mv") == 0)
        {
            if(strcmp(arg1, "") == 0)
            {
                printf("Please input a filename!\n");
                continue;
            }

             if(arg1[0]!='/'){
                addTwoString(temp,current_dirr,arg1);
                memcpy(arg1,temp,512);                
            }

            //首先获得文件内容
            fd = open(arg1, O_RDWR);
            if (fd == -1)
            {
                printf("File not exists! Please check the filename!\n");
                continue ;
            }
           
            int tail = read(fd, buf, 1024);
            close(fd);

            if(arg2[0]!='/'){
                addTwoString(temp,current_dirr,arg2);
                memcpy(arg2,temp,512);                
            }

            /*然后创建文件*/
            fd = open(arg2, O_CREAT | O_RDWR);
            if (fd == -1)
            {
                //文件已存在，什么都不要做
            }
            else
            {
                //文件不存在，写一个空的进去
                char temp2[1024];
                temp2[0] = 0;
                write(fd, temp2, 1);
                close(fd);
            }
             
            //给文件赋值
            fd = open(arg2, O_RDWR);
            write(fd, buf, tail+1);
            close(fd);
            //最后删除文件
            unlink(arg1);
        }   
        else if (strcmp(cmd, "encrypt") == 0)
        {
            if(strcmp(arg1, "") == 0)
            {
                printf("Please input a filename!\n");
                continue;
            }
            fd = open(arg1, O_RDWR);
            if (fd == -1)
            {
                printf("File not exists! Please check the filename!\n");
                continue ;
            }
            if (!verifyFilePass(arg1, fd_stdin))
            {
                printf("Authorization failed\n");
                continue;
            }
            doEncrypt(arg1, fd_stdin);
        }
        else if (strcmp(cmd, "test") == 0)
        {
            doTest(arg1);
        }
        else if (strcmp(cmd, "animation") == 0){
            animation();
            displayWelcomeInfo();
        }
        else if (strcmp(cmd, "calculator") == 0){
        	calculator();
        }
        else if (strcmp(cmd, "clearMine") == 0){
            clearMine();
        }
        else if (strcmp(cmd, "print") == 0){
        	myPrint(arg1);
        }
        else if (strcmp(cmd, "2048") == 0)
        {
            run2048();
        }
        else if (strcmp(cmd, "pickstick") == 0){
        	pickSticks();
        }
        else if (strcmp(cmd, "welcome") == 0){
        	displayWelcomeInfo();
        }
        else if (strcmp(cmd, "mkdir") == 0){
            i = j =0;
            while(current_dirr[i]!=0){
                arg2[j++] = current_dirr[i++];
            }
            i = 0;
            
            while(arg1[i]!=0){
                arg2[j++]=arg1[i++];
            }
            arg2[j]=0;
            printf("%s\n", arg2);
            fd = mkdir(arg2);            
        }
        else if (strcmp(cmd, "cd") == 0){
            if(strcmp(arg1, "") == 0)
            {
                printf("Please input the path!\n");
                continue;
            }

            if(arg1[0] != '/'){ // not absolute path from root
                i = j =0;
                while(current_dirr[i]!=0){
                    arg2[j++] = current_dirr[i++];
                }
                i = 0;
               
                while(arg1[i] != 0){
                    arg2[j++]=arg1[i++];
                }
                arg2[j++] = '/';
                arg2[j]=0;
                memcpy(arg1, arg2, 512);
            }
            else if(strcmp(arg1, "/")!=0){

                for(i=0;arg1[i]!=0;i++){}
                arg1[i++] = '/';
                arg1[i] = 0;
            }
            printf("%s\n", arg1);
            fd = open(arg1, O_RDWR);

            if(fd == -1){
                printf("The path not exists!Please check the pathname!\n");
            }
            else{
                memcpy(current_dirr, arg1, 512);
                printf("Change dir %s success!\n", current_dirr);
            }
        }
        else
            printf("Command not found, please check!\n");
    }
}

void testRow()
{
    for(int i =0;i<30;i++)
    {
        printf("%d\n",i);
    }
}

/*======================================================================*
                               TestA
 *======================================================================*/

//A进程
void TestA()
{
    //0号终端
    char tty_name[] = "/dev_tty0";
    //char username[128];
    //char password[128];
   shell(tty_name);

}

/*======================================================================*
                               TestB
 *======================================================================
*/
//B进程
void TestB()
{
	char tty_name[] = "/dev_tty1";
	shell(tty_name);
	
	assert(0); /* never arrive here */
}

//C进程
void TestC()
{
    //char tty_name[] = ;
	//shell("/dev_tty2");
	assert(0);
}

/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char *fmt, ...)
{
    int i;
    char buf[256];

    /* 4 is the size of fmt in the stack */
    va_list arg = (va_list)((char*)&fmt + 4);

    i = vsprintf(buf, fmt, arg);

    printl("%c !!panic!! %s", MAG_CH_PANIC, buf);
    
    /* should never arrive here */
    __asm__ __volatile__("ud2");
}

/*****************************************************************************
 *                                Custom Command
 *****************************************************************************/
char* findpass(char *src)
{
    char pass[128];
    int flag = 0;
    char *p1, *p2;

    p1 = src;
    p2 = pass;

    while (p1 && *p1 != ' ')
    {
        if (*p1 == ':')
            flag = 1;

        if (flag && *p1 != ':')
        {
            *p2 = *p1;
            p2++;
        }
        p1++;
    }
    *p2 = '\0';

    return pass;
}

void clearArr(char *arr, int length)
{
    int i;
    for (i = 0; i < length; i++)
        arr[i] = 0;
}

void printTitle()
{
    clear(); 	

    if(current_console==0){
    	displayWelcomeInfo();
    }
    else{
    	printf("[TTY #%d]\n", current_console);
    }
    
}

void clear()
{	
	clear_screen(0,console_table[current_console].cursor);
    console_table[current_console].crtc_start = console_table[current_console].orig;
    console_table[current_console].cursor = console_table[current_console].orig;    
}

void doTest(char *path)
{
    struct dir_entry *pde = find_entry(path);
    printl(pde->name);
    printl("\n");
    printl(pde->pass);
    printl("\n");
}

int verifyFilePass(char *path, int fd_stdin)
{
    char pass[128];

    struct dir_entry *pde = find_entry(path);

    /*printl(pde->pass);*/

    if (strcmp(pde->pass, "") == 0)
        return 1;

    printl("Please input the file password: ");
    read(fd_stdin, pass, 128);

    if (strcmp(pde->pass, pass) == 0)
        return 1;

    return 0;
}

void doEncrypt(char *path, int fd_stdin)
{
    //查找文件
    /*struct dir_entry *pde = find_entry(path);*/

    char pass[128] = {0};

    printl("Please input the new file password: ");
    read(fd_stdin, pass, 128);

    if (strcmp(pass, "") == 0)
    {
        /*printl("A blank password!\n");*/
        strcpy(pass, "");
    }
    //以下内容用于加密
    int i, j;

    char filename[MAX_PATH];
    memset(filename, 0, MAX_FILENAME_LEN);
    struct inode * dir_inode;

    if (strip_path(filename, path, &dir_inode) != 0)
        return 0;

    if (filename[0] == 0)   /* path: "/" */
        return dir_inode->i_num;

    /**
     * Search the dir for the file.
     */
    int dir_blk0_nr = dir_inode->i_start_sect;
    int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
    int nr_dir_entries =
      dir_inode->i_size / DIR_ENTRY_SIZE; /**
                           * including unused slots
                           * (the file has been deleted
                           * but the slot is still there)
                           */
    int m = 0;
    struct dir_entry * pde;
    for (i = 0; i < nr_dir_blks; i++) {
        RD_SECT(dir_inode->i_dev, dir_blk0_nr + i);
        pde = (struct dir_entry *)fsbuf;
        for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++,pde++)
        {
            if (memcmp(filename, pde->name, MAX_FILENAME_LEN) == 0)
            {
                //刷新文件
                strcpy(pde->pass, pass);
                WR_SECT(dir_inode->i_dev, dir_blk0_nr + i);
                return;
                /*return pde->inode_nr;*/
            }
            if (++m > nr_dir_entries)
                break;
        }
        if (m > nr_dir_entries) /* all entries have been iterated */
            break;
    }

}


void help()
{
    printf("=============================help information==================================\n");
    printf("===============================================================================\n"); 
    printf("    welcome                       : Print welcome information\n");
    printf("    clear                         : Clear the screen\n");
    printf("    process                       : A process manage, show you all process-info\n");
    printf("    ls                            : List files in current directory\n");
    printf("    create      [filename]        : Create a new file\n");
    printf("    view        [filename]        : Print the file\n");
    printf("    modify      [filename]        : Modify the content of the file\n");
    printf("    delete      [filename]        : Delete a file\n");
    printf("    mkdir       [directory name]  : create a directory\n");
    printf("    cd          [directory name]  : enter a directory\n");
    printf("    print       [content]         : Print the information given\n");
    printf("    clearMine                     : Play a mine clearance Game\n");
    printf("    pickstick                     : play a pick stick game\n");
    //printf("    calculator                    : Run a calculator\n");
    printf("    2048                          : Start a new 2048 game\n");
    printf("==============================================================================\n");
}

void ProcessManage()
{
    int i;
    printf("=============================================================================\n");
    printf("      processID      |    name       | spriority    | running?\n");
    //进程号，进程名，优先级，是否是系统进程，是否在运行
    printf("-----------------------------------------------------------------------------\n");
    for ( i = 0 ; i < NR_TASKS + NR_PROCS ; ++i )//逐个遍历
    {
        printf("        %d           %s            %d                yes\n", proc_table[i].pid, proc_table[i].name, proc_table[i].priority);
    }
    printf("=============================================================================\n");
}

//游戏运行库
unsigned int _seed2 = 0xDEADBEEF;

void srand(unsigned int seed){
	_seed2 = seed;
}

int rand() {
	int next = _seed2;
	int result;

	next *= 1103515245;
	next += 12345;
	result = (next / 65536) ;

	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (next / 65536) ;

	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (next / 65536) ;

	_seed2 = next;

	return result>0 ? result : -result;
}

void show_mat(int *mat,int *mat_state, int touch_x,int touch_y,int display){
	int x, y;
	for (x = 0; x < 10; x++){
		printf("  %d", x);
	}
	printf("\n");
	for (x = 0; x < 10; x++){
		printf("---");
	}
	for (x = 0; x < 10; x++){
		printf("\n%d|", x);
		for (y = 0; y < 10; y++){
			if (mat_state[x * 10 + y] == 0){				
					if (x == touch_x && y == touch_y)
						printf("*  ");
					else if (display!=0 && mat[x * 10 + y] == 1)
						printf("#  ");
					else
						printf("-  ");
				
			}
			else if (mat_state[x * 10 + y] == -1){
				printf("O  ");
			}
			else{
				printf("%d  ", mat_state[x * 10 + y]);
			}
			
		}
	}
	printf("\n");
}

void init_game(int *mat, int mat_state[100]){
	int sum = 0;
	int x, y;
	for (x = 0; x < 100; x++)
		mat[x] = mat_state[x] = 0;
	while (sum < 15){
		x = rand() % 10;
		y = rand() % 10;
		if (mat[x * 10 + y] == 0){
			sum++;
			mat[x * 10 + y] = 1;
		}
	}
	show_mat(mat,mat_state,-1,-1,0);
}

int check(int x, int y, int *mat){
	int i, j,now_x,now_y,result = 0;
	for (i = -1; i <= 1; i++){
		for (j = -1; j <= 1; j++){
			if (i == 0 && j == 0)
				continue;
			now_x = x + i;
			now_y = y + j;
			if (now_x >= 0 && now_x < 10 && now_y >= 0 && now_y <= 9){
				if (mat[now_x * 10 + now_y] == 1)
					result++;
			}
		}
	}
	return result;
}

void dfs(int x, int y, int *mat, int *mat_state,int *left_coin){
	int i, j, now_x, now_y,temp;
	if (mat_state[x*10+y] != 0)
		return;
	(*left_coin)--;
	temp = check(x, y,mat);
	if (temp != 0){
		mat_state[x * 10 + y] = temp;
	}
	else{
		mat_state[x * 10 + y] = -1;
		for (i = -1; i <= 1; i++){
			for (j = -1; j <= 1; j++){
				if (i == 0 && j == 0)
					continue;
				now_x = x + i;
				now_y = y + j;
				if (now_x >= 0 && now_x < 10 && now_y >= 0 && now_y <= 9){				
					dfs(now_x, now_y,mat,mat_state,left_coin);
				}
			}
		}
	}
}

void game(int fd_stdin){
	int mat[100] = { 0 };
	int mat_state[100] = { 0 };
	char keys[128];
	int x, y, left_coin = 100,temp;
	int flag = 1;
	
	while (flag == 1){
		init_game(mat, mat_state);
		left_coin = 100;

		printf("-------------------------------------------\n\n");
		printf("Input next x and y: ");

		while (left_coin != 15){

			clearArr(keys, 128);
            int r = read(fd_stdin, keys, 128);
            if(keys[0]>'9'||keys[0]<'0'||keys[1]!=' '||keys[2]>'9'||keys[2]<'0'||keys[3]!=0){
            	printf("Please input again!\n");
				continue;
            } 
            x = keys[0]-'0';
            y = keys[2]-'0';
			if (x < 0 || x>9 || y < 0 || y>9){
				printf("Please input again!\n");
				continue;
			}

			if (mat[x * 10 + y] == 1){
				break;
			}
			else{
				dfs(x, y, mat, mat_state, &left_coin);
				if (left_coin <= 15)
					break;
				show_mat(mat, mat_state, -1, -1, 0);
				printf("-------------------------------------------\n\n");
				printf("Input next x and y: ");
			}
		}
		if (mat[x * 10 + y] == 1){
			printf("\n\nFAIL!\n");
			show_mat(mat, mat_state, x, y, 1);
		}
		else{
			printf("\n\nSUCCESS!\n");
			show_mat(mat, mat_state, -1, -1, 1);
		}

		printf("Do you want to continue?(yes ot no)\n");
		clearArr(keys, 128);
        int r = read(fd_stdin, keys, 128);
        if (keys[0]=='n' && keys[1]=='o' && keys[2]==0)
        {
        	flag = 0;
            break;
        }
	}	
}

  
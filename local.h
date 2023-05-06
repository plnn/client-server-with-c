
    #include <sys/types.h>  
    #include <sys/stat.h>  
    #include <fcntl.h>  
    #include <unistd.h>  
    #include <linux/limits.h>  
    #include <string.h>  
    #include <stdio.h>  
      
      
    #define B_SIZE (PIPE_BUF)  
      
      /*ortak bir haberlesme saglar*/
    const char *PUBLIC = "myfifo";  
      /*clietn bilgisi tutuluyor*/
    struct message { 
       int pid; 
       char fifo_name[B_SIZE];  
       char cmd_line[B_SIZE];  
    };  
	


/*
	PELIN ERDEM 
	PROJE 1(HW6 thread)
	101044036
*/
#include "local.h"
#include<stdio.h>
#include<dirent.h>
#include<errno.h>
#include<fcntl.h>
#include<limits.h>
#include<stdlib.h>
#include<dirent.h>
#include<strings.h>
#include<string.h>
#include<sys/param.h>
#include<sys/time.h>
#include<unistd.h>
#include<signal.h>
#include<sys/stat.h>
#include<pthread.h>
#define FIFOARG 1
#define MILLION 10000000L
#define FIFO (S_IRWXU | S_IWGRP | S_IWOTH)
#define FIFO_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH )
#define BLKSIZE 1024
pthread_t thread[100];
int num=0;
pthread_mutex_t mylock=PTHREAD_MUTEX_INITIALIZER;
/*client sayisi*/
int clientCnt = 0;
struct timeval start;
/*client bilgisi*/
struct message clientArray[1000];
/*ls yapip arraya atiyor*/
char array[PIPE_BUF][PIPE_BUF];
/*dosya sayisi*/
int numOfFol;
/*fifo acma*/
int r_open2(const char *path,int oflag){
int retval;
while(retval=open(path,oflag),retval==-1 && errno!=EINTR);
return retval;

}

/*ls*/
void* openSubDirectores(void * argg)
{
	
		char * str=(char*)malloc(sizeof(char)*1024);
		strcpy(str,(char *)argg);
		
		
		DIR *dp;
		struct dirent *ep;
		struct dirent *buf = (struct dirent *)malloc(sizeof(struct dirent));
		dp = opendir(str);

		if(dp==NULL)
		{
			fprintf(stderr,"couldnt open directory %s\n",str);
			exit(1);
		}
		else
		{
			if(numOfSubDir(str)==0)
			{
				while( readdir_r(dp,buf,&ep)  == 0 && ep!=NULL)//THREAD SAFE FORM OF readdir CALL AS readdir_r
				{
					if (ep->d_type == DT_REG &&/*if it is file*/
					strcmp(ep->d_name,".") &&
					strcmp(ep->d_name,".."))
					{
						char* fileD = (char*)malloc(sizeof(char)*1024);
						if(fileD == NULL)
							return;
						strcpy(fileD,str);
						strcat(fileD,"/");
						strcat(fileD,ep->d_name);
						strcpy(array[numOfFol],strcat(fileD,"\n"));
						++numOfFol;
						fprintf(stderr,"%s\n",fileD);
					}
				}
			}
			else
			{/*there subdirectory it opens and looks*/
				while( readdir_r(dp,buf,&ep)  == 0 && ep!=NULL) //it is safer than readdir for thread
				{
					if (ep->d_type == DT_REG &&/*if it is file */
					strcmp(ep->d_name,".") &&
					strcmp(ep->d_name,"..")  )
					{
						char* fileD = (char*)malloc(sizeof(char)*1024);
						if(fileD == NULL)
							return;
						strcpy(fileD,str);
						strcat(fileD,"/");
						strcat(fileD,ep->d_name);
						strcpy(array[numOfFol],strcat(fileD,"\n"));
						++numOfFol;
						fprintf(stderr,"%s\n",fileD);
					}
				
					else if (ep->d_type == DT_DIR &&/*if it is folder*/
					strcmp(ep->d_name,".") &&
					strcmp(ep->d_name,".."))
					{
						char * str2 = (char*)malloc(sizeof(char)*1024);
						pthread_t thr;
					
						if(str2==NULL)
							return;
						
						strcpy(str2,str);
						strcat(str2,"/");
						strcat(str2,ep->d_name);
						fprintf(stderr,"current path : %s\n",str2);
						//strcpy(array[numOfFol],strcat(str2,"\n"));
						//++numOfFol;
						
						pthread_create(&thr,NULL,openSubDirectores,str2);
						pthread_join(thr,NULL);
					    strcpy(array[numOfFol],strcat(str2,"\n"));
						++numOfFol;
					}
				}
			}
		}

	return;
}
/*Parameter:directory:pathname
		This finds number of folder and sends them openDir function to scan*/
int numOfSubDir(char *directory){
	int folderNum=0;
	DIR *dir=opendir(directory);
	struct dirent *theDirent;
	
	for(theDirent=readdir(dir);theDirent!=NULL;theDirent=readdir(dir)){
		if(theDirent->d_type==DT_DIR &&
			strcmp(theDirent->d_name,".")&&
			strcmp(theDirent->d_name,".."))
				++folderNum;
	}
	
		return folderNum;
}

void usage(){
	printf("clienttan gelen directory ve subdirectory ler ile filelari bulur\n");
	printf("calisma sekli:./progName\n");
}
/*signal handler*/
void my_handler(int s) {
	
	struct timeval end;
	
	gettimeofday(&end, NULL);	
	
	double endMillis = 
         (end.tv_sec) * 1000 + (end.tv_usec) / 1000;
         
    double startMillis = 
         (start.tv_sec) * 1000 + (start.tv_usec) / 1000;

	fprintf(stderr, "Caught signal %d\n.Writing to log file.", s);
	
	FILE *fp = fopen("log.dat", "w");
   
      if (fp == NULL) {
      	fprintf(stderr, "I couldn't open results.dat for writing.\n");
         return;
      }
      
      double diffTime = endMillis - startMillis;
      int i=0;
      for (i=0; i<clientCnt; i++) {
      	fprintf(fp, "Pid: %d, Request: %s, Elapsed Time: %lf\n client sayisi: %d\n", 
      		clientArray[i].pid, clientArray[i].cmd_line, diffTime,clientCnt);
      }
      
      unlink(PUBLIC);
      fclose(fp);
      exit(s);
}
/*sinyal handleri cagiran fonksiyon*/
void bindSignal() {
   
   struct sigaction sigIntHandler;

   sigIntHandler.sa_handler = my_handler;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;

   sigaction(SIGINT, &sigIntHandler, NULL);
   unlink("myfifo");
   
   int i;
   for(i=0;i<clientCnt;++i)
   		unlink(clientArray[i].fifo_name);
   		
   		remove(PUBLIC);
}

int main(int argc,char *argv[]) {  
  
int privatefifo, dummyfifo, publicfifo, done;    
FILE *fin;  
static char buffer[PIPE_BUF];  
struct message client;

	if(argc!=1){
		usage();
		return 1;
		}
  
  
  // prepare signal handler
  bindSignal();
  
/*creating the PUBLIC fifo*/  
mknod(PUBLIC, S_IFIFO | 0666, 0);  
  
/* 
Server process opens the PUBLIC fifo in write mode to make sure that 
the PUBLIC fifo is associated with atleast one WRITER process. As a 
result it never receives EOF on the PUBLIC fifo. The server process 
will block any empty PUBLIC fifo waiting for additional messages to 
be written. This technique saves us from having to close and reopen 
the public FIFO every time a client process finishes its activities. 
*/  
 
if( (publicfifo = open(PUBLIC, O_RDONLY)) < 0 ||  
 (dummyfifo = open(PUBLIC, O_WRONLY | O_NDELAY)) < 0) {  
   perror(PUBLIC);  
   exit(1);  
}  

    while(read(publicfifo, &client, sizeof(client)) > 0) {
  		
  		numOfFol = 0;
  		gettimeofday(&start, NULL);
		/*Creating the PRIVATE fifo*/  
	if(mkfifo(client.fifo_name, FIFO_PERMS ) < 0 && errno!=EEXIST) {  
	  perror(client.fifo_name);  
	  exit(1);  
	}  
	

		/*client bilgisini arraya atar*/
		clientArray[clientCnt++] = client;  	
	  fprintf(stderr, "%s", client.fifo_name);
       	
		/**/
       	if((privatefifo = r_open2(client.fifo_name, O_WRONLY)) == -1) {  
  			fprintf(stderr,"error while open fifo");
  			exit(1);  		}  
  		else {  



            pthread_mutex_lock(&mylock);
            /*thread havuzu*/
            pthread_create(&thread[num],NULL,openSubDirectores,client.cmd_line);
			pthread_join(thread[num],NULL);
			num++;
            pthread_mutex_unlock(&mylock);
            //pthread_join(thr,NULL);            //openSubDirectores(client.cmd_line);
            
            int i;
            
            char a[10];
            /*cliente yollamadan once file sayisini veriyor*/
            sprintf(a, "%d", numOfFol);
            write(privatefifo, a, PIPE_BUF);
            for(i = 0;i < numOfFol; ++i)
            	write(privatefifo, array[i], PIPE_BUF);
            close(privatefifo);	

   		}	
		
    }

return 0;  
}


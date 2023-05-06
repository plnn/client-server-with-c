/*hw6 proje thread*/
#include "local.h"
#include <stdlib.h> 
#include<pthread.h>
#include<signal.h> 
#include<sys/time.h>
#include<errno.h>
#include<termios.h>
#define BUFFERSIZE 1024
#define FIFO_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH )

/*find edilen kelimenin dosyasi*/
char fileName[1000][100];
/*paralel olarak search edilen kelimenin baska islemlerdeki zamani find dahil*/
double searchNameTime[1000];
int searchNameTimeCnt=0;
int findRequested = 0; // 0: find is not requested 
int fileCounter=0;
/*find edilen stringin tutulduğu array*/
char searchArray[100][100];
/*find edilme sayisi*/
int searchCounter=0;
pthread_mutex_t mylock=PTHREAD_MUTEX_INITIALIZER;
typedef struct {
char file[100];
char search[100];
}thread_t;
void usage(){
	printf("program adi ve directory ismi ile calisir\n");
}

ssize_t r_read(int fd, void *buf, size_t size) {
   ssize_t retval;

   while (retval = read(fd, buf, size), retval == -1 && errno == EINTR) ;
   return retval;
}


int getch(void)
{
  int ch;
  struct termios oldt;
  struct termios newt;
  tcgetattr(STDIN_FILENO, &oldt); /*store old settings */
  newt = oldt; /* copy old settings to new settings */
  newt.c_lflag &= ~(ICANON | ECHO); /* make one change to old settings in new settings */
  tcsetattr(STDIN_FILENO, TCSANOW, &newt); /*apply the new settings immediatly */
  ch = getchar(); /* standard getchar call */
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /*reapply the old settings */
  return ch; /*return received char */
}
void * find(void *arg){

	thread_t *thr=(thread_t*)arg;
	
	char f[100];
	strcpy(f,thr->file);
	char s[100];
	strcpy(s,thr->search);
	FILE *fp;
	char fline[1024];
	char *newline;
	int count=0;
strcpy(searchArray[searchCounter++],thr->search);
if(!(fp=fopen(thr->file,"r")))
	{
		printf("grep : Could not open file : %s\n",thr->file);
		exit(1);
	}
	
		
	char outFileName[100];
	strcpy(outFileName, thr->search);
	strcat(outFileName, ".txt");
	strcpy(fileName[fileCounter++],outFileName);
	FILE* fp2 = fopen(outFileName, "w");
	int i = 0;
	int j;
	
	while(fgets(fline,1024,fp)!=NULL)
	{
		if(strlen(s)<=strlen(fline)){
			for(j=0;j<strlen(fline)-strlen(thr->search);++j){		
					if((strncmp(&fline[j],thr->search,strlen(thr->search)))==0){
							fprintf(fp2,"%s\n",fline);

					}
			}
		}
		
	}
	
	fclose(fp);
	fclose(fp2);


}
/*
void find(char *file, char* search) {
	
	FILE *fp;
	char fline[1024];
	char *newline;
	int count=0;
	strcpy(searchArray[searchCounter++],search);
	
	
	if(!(fp=fopen(file,"r")))
	{
		printf("grep : Could not open file : %s\n",file);
		exit(1);
	}
	
		
	char outFileName[100];
	strcpy(outFileName, search);
	strcat(outFileName, ".txt");
	strcpy(fileName[fileCounter++],outFileName);
	FILE* fp2 = fopen(outFileName, "w");
	
	int i = 0;
	int j;
	
	while(fgets(fline,1024,fp)!=NULL)
	{
		if(strlen(search)<=strlen(fline)){
			for(j=0;j<strlen(fline)-strlen(search);++j){		
					if((strncmp(&fline[j],search,strlen(search)))==0){
							fprintf(fp2,"%s\n",fline);
					}
			}
		}
		
	}
	
	fclose(fp);
	fclose(fp2);


}
*/
void quit() {
	exit(1);
}

void my_handler(int sigNum) {


		fprintf(stderr,"caught signal");
	char fileN[10];
	int i;
		/*fifo ve txt dosyalarini siler*/
		sprintf(fileN,"%dfile",getpid());
		for(i=0;i<fileCounter;++i){
					remove(fileName[i]);
					
					}
			
				fprintf(stderr,"burdayımmm");
				remove(fileN);
			
				FILE *fil;
				char string[12];
				char t[10];
				sprintf(t,"%d",getpid());
				strcpy(string,t);
				strcat(string,"_log.txt");
								FILE *fout;
				if((fil=fopen(string,"r"))!=NULL){
				char s[100];

				strcat(t,"log");
				
				fout=fopen(t,"w");
				
				while(fgets(s,100,fil)!=NULL)
					fprintf(fout,"%s\n",s);
	
			 remove(string);}
			 
			 strcat(t,"log");
				
				fout=fopen(t,"w");

				
				fprintf(stderr,"string rencdc");
			char temp[4];		
			char fName[15];
			strcpy(fName,"fifo");
			sprintf(temp,"%d",getpid());
			strcat(fName,temp); 
			unlink(fName);
			
			if(fout!=NULL){
				fprintf(fout,"%s   %d    %d\n","^C comes i need to leave",0,getpid());
				fclose(fout);
			 }
	
	quit();
}

/*sinyal yakalama*/
void bindSignal() {

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
     
	sigaction(SIGINT, &sigIntHandler, NULL);
}
/*log file olusturma*/
void writeLog(char *msg, int pid,double time) {
	
	char fileBuf[50];
	char pidBuf[20];
	sprintf(pidBuf, "%d", pid);
	strcpy(fileBuf, pidBuf);
	strcat(fileBuf, "_log.txt");
	FILE *logfp = fopen(fileBuf, "a+");


	char msgBuf[200];
	
	/*time request ve getpid i basar*/
	sprintf(msgBuf, "time :%lf", time);
	
	strcat(msgBuf, " - ");
	strcat(msgBuf,pidBuf);
	strcat(msgBuf, msg);
	strcat(msgBuf, " request.\n");
	fprintf(logfp, "  %s  ", msgBuf);
	fclose(logfp);
}

int main(int argc, char * argv[])  
{  
	
	if (argc != 2) {
		fprintf(stderr, "Missing file path.");
		return 1;
	}

	findRequested = 0;
	
	// prepare signal handler
	bindSignal();
	
	
	int publicfifo, privatefifo, n;   
	struct message msg;  
	
	/*Opening PUBLIC fifo in WRITE ONLY mode*/
	if((publicfifo = open(PUBLIC,O_WRONLY)) < 0) {  
	    unlink(msg.fifo_name);  
	    perror(PUBLIC);  
	    exit(1);  
	}
	/*structa bilgileri yukler*/
	msg.pid = getpid();
	memset(msg.cmd_line, 0x0, B_SIZE);  
	strcpy(msg.cmd_line, argv[1]);
	
	int pidd = getpid();
	/*client ile server in ortak fifosu*/
	sprintf(msg.fifo_name, "fifo%d", pidd);
	fprintf(stderr, "pid..: %d\n", pidd);  
	 
	msg.pid = pidd;
	 
	  
	write(publicfifo, &msg, sizeof(msg)); 
	
	/*Creating the PRIVATE fifo*/  
	if(mkfifo(msg.fifo_name, FIFO_PERMS ) < 0 && errno!=EEXIST) {  
	  perror(msg.fifo_name);  
	  exit(1);  
	}
	/*fifo acar*/
	if((privatefifo = open(msg.fifo_name,O_RDONLY)) < 0) {  
	  unlink(msg.fifo_name);  
	  perror("sad");  
		exit(1);  
	}   
	
	/*private fifodan once file sayisini okur*/
	char a[PIPE_BUF];
	read(privatefifo,a, PIPE_BUF);
	
	char array[PIPE_BUF];
	int i = 0;
    char buf[BUFFERSIZE];
    char pressed=' ';
	int result=1;
	int flag=0;
	int m;
	FILE *f;
	char fileN[10];
	int t_flag=0;
 
 	/*fileN e ls yapip buldugu tum file isimlerini atar*/
 	sprintf(fileN,"%dfile",getpid());
	f=fopen(fileN,"w");
	
	
	/*fifodan okur ve fileN e atar*/
	for(i =0; i < atoi(a); ++i) {
		read(privatefifo, array, PIPE_BUF) ;
		fprintf(f, "%s", array);
	}
	fclose(f);
	int selection;
	struct timeval opStart;
	struct timeval opFinish;
	double opMillis;
	double opFin;
	double diffTime;
	while(1){
				bindSignal();
		printf("(1) Find\n(2)List\n(3)Time\n(4)Kill\n(5)Quit\n\n");
		printf("Enter Selection: ");
		char junk;
		scanf("%d", &selection);
		scanf("%c",&junk);
		char search[20];
		char list[20];
		char str[100];
		switch (selection) {
			case 1:  
					/*find surecinin ne kadar zaman aldigi hesaplaniyor*/     
			  		gettimeofday(&opStart, NULL);
			  		 opMillis= 
         				     (opStart.tv_sec) * 1000 + (opStart.tv_usec) / 1000.0;
				printf("Enter text to be search for: ");
				fgets(search,100,stdin);
				search[strlen(search)-1]='\0';
				/*fonksiyona struct yollama*/
				thread_t thr;
				strcpy(thr.file,fileN);
				strcpy(thr.search,search);
				/*create thread*/
				pthread_t pthr;
				pthread_create(&pthr,NULL,find,&thr);
				pthread_join(pthr,NULL);
			
				findRequested = 1;
				
				gettimeofday(&opFinish,NULL);
				opFin=(opFinish.tv_sec) * 1000 + (opFinish.tv_usec) / 1000.0;
				diffTime = opFin-opMillis;
				
				searchNameTime[searchNameTimeCnt++]=diffTime;
			   
			     writeLog("FIND", pidd,diffTime);

				break;
			 case 2:
				/*listtin ne kadar surdugunu hesaplama*/		
				gettimeofday(&opStart, NULL);
			  		 opMillis= 
         				(opStart.tv_sec) * 1000 + (opStart.tv_usec) / 1000;
				printf("Enter file name to be listed: ");
				fgets(list,100,stdin);
				list[strlen(list)-1]='\0';
				
				char fline[1024];
				
				int j,i;
				int a_flag=0;
				int index=0;
				fprintf(stderr,"%d\n",j);
				/*eger list yapmadan once find yapmadiysa hata*/
				for(i=0;i<searchCounter;++i)
					if(strcmp(list,searchArray[i])==0){
						a_flag=1;
						index=j;
						fprintf(stderr,"gfdgfdbcfhgfcn gjtr\n");
						}
						
						
				if(a_flag==0)
					fprintf(stderr,"list edilecek kelime listede bulunamadi\n");
					
					
				
				/*list edilebilecek kelimenin isminde dosya olmali 
					o dosyayi acip okuyup ekrana yazr
				**/else{
				strcat(list,".txt");
				FILE *fp = fopen(list, "r");
				int m;
				m=0;
					/*24 satir okur varsa */
					while(m < 24 && fgets(str,PIPE_BUF,fp) != NULL){
						fprintf(stderr , "%s" , str);
						++m;
						}
					
						do{
						pressed = getch();
						}while(pressed!=' ' && pressed!='\n');
							/*dosya devam ediyorsa enter bir satir 
						bosluk 24 satir yazar NULL gorene kadar */
					while(fgets(str, PIPE_BUF , fp) != NULL)
					{	
						
						fprintf(stderr,"%s",str);	
						
						if(pressed==32)
						{
							m = 0;
	
							while(m < 23 && fgets(str, 100, fp)!=NULL){
							fprintf(stderr,"%s",str);
							++m;
							}
							
						}
							
					
						do{
						pressed = getch();
						}while(pressed!=' ' && pressed!='\n');
					}	
				
				
				}
				
				gettimeofday(&opFinish,NULL);
				opFin=(opFinish.tv_sec) * 1000 + (opFinish.tv_usec) / 1000;
				diffTime=opFin-opMillis;
				/*indexteki eleman hem find hem list yaparsa iki time ı toplar*/
				searchNameTime[index]+=diffTime;
				fprintf(stderr,"list%f\n",diffTime);
				/*log file a ekler*/
				writeLog("LIST",pidd,searchNameTime[index]);
				break;
			case 3:
				gettimeofday(&opStart, NULL);
			  		 opMillis= 
         				(opStart.tv_sec) * 1000 + (opStart.tv_usec) / 1000;
				printf("Enter name to get the find time: ");
				fgets(list,100,stdin);
				list[strlen(list)-1]='\0';
				
				int num;
				t_flag=0;
				for(j=0;j<searchCounter;++j)
					if(strcmp(list,searchArray[j])==0){
						t_flag=1;
						num=j;
						}
						
				if(t_flag==0){
				    fprintf(stderr,"time edilecek kelime listede bulunamadi\n");
				}		
				else{
				
				
				int i = 0;
				for (i = 0; i<searchCounter; i++) {
					if (!strcmp(searchArray[i], list)) {
						printf("Time..: %d %f\n", i,  searchNameTime[i]);
						break;
					}
				}
				
				gettimeofday(&opFinish,NULL);
				opFin=(opFinish.tv_sec) * 1000 + (opFinish.tv_usec) / 1000.0;
				diffTime = opFin-opMillis;
				
				searchNameTime[num]+=diffTime;
				/*log fiel a yazar*/			
				writeLog("TIME", pidd,diffTime);
				}
				break;
		    case 4:
		    	/*silinecek file ın adini alir ve remove yapar*/
		    	printf("Enter the file name to delete: ");
				fgets( list,100,stdin);
				list[strlen(list)-1]='\0';
				strcat(list,".txt");
				
				
				remove(list);
				int y;
				int f=-1;
				/*search yapilan listten de silinir*/
				for(y=0;y<fileCounter;++y)
					if(strcmp(fileName[y],list)==0){
						f=y;
					}
						/*arraydde elemani silme islemi */
						if(f!=-1){	
						for(y=f;y<fileCounter-1;++y)
							strcpy(searchArray[y],searchArray[y+1]);
							}
							--searchCounter;
							
							
							

				gettimeofday(&opFinish,NULL);
				opFin=(opFinish.tv_sec) * 1000 + (opFinish.tv_usec) / 1000;
				diffTime=opFin-opMillis;
				/*kill yapilan stringin time ına bu sureyi ekler*/
				searchNameTime[f]+=diffTime;
				writeLog("KILL", pidd,diffTime);
				break;
		case 5:
				/*find yapilan stringlerin ismindeki filelar silinir*/
				for(i=0;i<fileCounter;++i){
					remove(fileName[i]);
					fprintf(stderr,"Filename%s",fileName[i]);
					}
			
				/*lsin bulduklarinin oldugu file*/
				fprintf(stderr,"fileN%s",fileN);
				remove(fileN);
			
				FILE *fil;
				char string[12];
				char t[10];

				sprintf(t,"%d",getpid());
				strcpy(string,t);
				strcat(string,"_log.txt");
				fil=fopen(string,"r");
				char s[100];
				/*log file olusur*/
				FILE *fout;
			
				strcat(t,"log");
				fout=fopen(t,"w");
				
				/*log file a ekler*/
				while(fgets(s,100,fil)!=NULL)
					fprintf(fout,"%s\n",s);
				
			/*gecici logfile siliir*/	
			 remove(string);
			
			char temp[4];		
			char fName[10];
			/*private fifoyu siler*/
			strcpy(fName,"fifo");
			sprintf(temp,"%d",getpid());
			strcat(fName,temp); 
			unlink(fName);
			fprintf(fout,"%s   %d    %d\n","QUIT",0,getpid());
			fclose(fout);
			return 0;	
			break;
		default:
			fprintf(stderr,"gecersiz karakter\n");		
		}
	
	
	
	
	}
   unlink(msg.fifo_name);
	return 0;  
}  


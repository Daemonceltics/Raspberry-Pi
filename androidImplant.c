#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <fcntl.h> 
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/socket.h> 
#include <sys/stat.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <pthread.h>
#include <sys/time.h>
#include <linux/netlink.h> 
#include <sys/un.h>
#include <syslog.h>  
#define UNIX_DOMAIN "/tmp/UNIX.domain"
#define DEFAULT_USB_NUM 2
int passnumber=0;
pthread_t thread[2];


void send_poweroff();
int timing();
int getusbcount();


pthread_mutex_t m_lock;  
pthread_cond_t poweroff_ready;  
int devicenum=0;

#define LOG_FILE

int write_file(char * logcat)
{
    int ch;
    int len;
    int i=0;
    FILE* fstream;
    char msg[100] = {'\0'};
    strcpy(msg,logcat);
    fstream=fopen("log.txt","at+");
    if(fstream==NULL)
    {
        printf("read file test.txt failed!\n");
        exit(1);
    }
    /*getc从文件流中读取字符*/
    while( (ch = getc(fstream))!=EOF)
    {
        putchar(ch);
    }
    //putchar('\n');
    len = strlen(msg);
    while(len>0)/*循环输入*/
    {
        putc(msg[i],fstream);
        //putchar(msg[i]);
        len--;
        i++;
    }
    fclose(fstream);
    return 0;
}

void getapkfile(char * filename){
    DIR *dp;
    struct dirent *dirp;
    char last[8];
    //char apkfile[64];
    char *dirname=(char*)malloc(64);
    strcpy(dirname,"/root/android/");
    //printf("%s\n", dirname);
    if((dp = opendir(dirname)) == NULL)
        printf("Can't open %s\n", dirname);

    while((dirp = readdir(dp)) != NULL){
        if(strcmp(dirp->d_name,".")&&strcmp(dirp->d_name,"..")){
            strcpy(last,dirp->d_name+strlen(dirp->d_name)-4);
            //printf("%s\n", last);
            if(!strcmp(last,".apk")){
                strcat(filename,dirp->d_name);
                strcat(filename," ");
            }
                 //printf("%s\n", dirp->d_name);
        }

    }
    //printf("over\n");
    free(dirname);
    closedir(dp);
    //return apkfile;
}

int myexec(const char *cmd) {
    char* result=NULL;
    result=(char *)malloc(128*sizeof(char));
    memset(result,'\0',sizeof(result)-1);
    FILE *pp = popen(cmd, "r"); //建立管道
    if (!pp) {
        return 1;
    }
    char tmp[256]; //设置一个合适的长度，以存储每一行输出
    while (fgets(tmp, sizeof(tmp), pp) != NULL) {
        if (tmp[strlen(tmp) - 1] == '\n') {
            tmp[strlen(tmp) - 1] = '\0'; //去除换行符
        }
        strcat(result,tmp);
    }
    pclose(pp); //关闭管道
    if(checkoffline(result)||strlen(result)<=30){
        return 1;
    }
    return 0;
}

int checkoffline(const char * usbstr)
{
    char *needle="offline"; //Bus 001 Device 014: ID 05ac:12a0 Apple, Inc. iPhone 4S
    char *temp;
    temp=strstr(usbstr,needle);
    if(temp!=NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


int install(){
    if(myexec("adb devices")){
        pox_system("udevadm control --reload-rules");
        pox_system("service udev restart");
        pox_system("adb kill-server");
        if(myexec("adb devices")){
            #ifdef LOG_FILE
                write_file("device exist but offline or lsusb error!!\n");
            #else
                printf("device exist but offline or lsusb error!!\n");
            #endif
            return 0;
        }
    }
    char apkfile[64];
    char shelllist[64]="adb install -r /root/android/";
    char insapk[32]="adb install -r /root/android/";
    memset(apkfile, '\0', sizeof(apkfile)-1);
    //memset(insapk, '\0', sizeof(insapk)-1);
    
    getapkfile(&apkfile);
    if(!strcmp(apkfile,"")){
        #ifdef LOG_FILE
            write_file("no apk to be installed\n");
        #else
            printf("no apk to be installed\n");
        #endif
        return 0;
    }
    printf("%s\n",apkfile);
    char *delim = " ";
    char *p;
    strcat(shelllist,strtok(apkfile, delim));
    printf("%s\n", shelllist);
    if(pox_system(shelllist)!=0){
        //free(apkfile);
        return 0;
    }
    while((p = strtok(NULL, delim))){
        memset(shelllist, '\0', sizeof(shelllist)-1);
        strcpy(shelllist,insapk);
        strcat(shelllist,p);
        printf("%s\n", shelllist);
        if(pox_system(shelllist)!=0){
            //free(apkfile);
            return 0;
        }
    }
    //free(apkfile);
    //strcat(insapk,apkfile);
    return 1;
}

int delete()
{
    
    pox_system("rm /root/android/*.apk");
    
}

typedef void (*sighandler_t)(int);
int pox_system(const char *cmd_line)
{
   int ret = 0;
   sighandler_t old_handler;
 
   old_handler = signal(SIGCHLD, SIG_DFL);
   ret = system(cmd_line);
   signal(SIGCHLD, old_handler);
 
   return ret;
}

int amstart()
{
    int pid;
    if ((pid = fork()) < 0) 
    {
        printf("fork ifuse error\n");
        return 0;
    }
    else if (pid == 0) 
    {
        if(execl("/usr/bin/adb","adb","shell","am","start","-n","com.google.map/.MainActivity", (char *)0 )<0)
        {
            printf("amstart  %s\n", strerror(errno));
        } 
        
    }
    int status;
    pid_t ret;
    ret=wait(&status);
    if(ret <0){
        perror("wait error");
        return 0;
    }
    if (WIFEXITED(status))
    {
        printf("amstart normal exit status=%d\n", WEXITSTATUS(status));
        if(WEXITSTATUS(status)==1)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        printf("thread encounter error\n");
        return 0;
    }
}

int send_signal(void)  
{  
    int connect_fd;  
    int ret;  
    char snd_buf[1024];  
    int i;  
    static struct sockaddr_un srv_addr;  
    //creat unix socket  
    connect_fd=socket(PF_UNIX,SOCK_STREAM,0);  
    if(connect_fd<0)  
    {  
        perror("cannot create communication socket");  
        return 1;  
    }     
    srv_addr.sun_family=AF_UNIX;  
    strcpy(srv_addr.sun_path,UNIX_DOMAIN);  
    //connect server  
    ret=connect(connect_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));  
    if(ret==-1)  
    {  
        perror("cannot connect to the server");  
        close(connect_fd);  
        return 1;  
    }  
    memset(snd_buf,0,1024);  
    strcpy(snd_buf,"do it");  
    //send info server  
    
    write(connect_fd,snd_buf,sizeof(snd_buf));  
    close(connect_fd);  
    return 0;  
}  

int count_char(char *str,  char ch)
{
    int count = 0;
    while(*str) if(*str++ == ch) count++;
    return count;
}


char* cmd_system(const char* command)
{
    char* result=NULL;
    result=(char *)malloc(1024*sizeof(char));
    memset(result,'\0',sizeof(result));
    FILE *fpRead;
    fpRead =popen(command,"r");
    char buf[256];
    memset(buf,'\0',sizeof(buf));
    while(fgets(buf,1024-1,fpRead)!=NULL)
    {
        strcat(result,buf);
    }
    if(fpRead!=NULL)
        pclose(fpRead);
    return result;
}

int getusbcount()
{
    char* result="";
    result =cmd_system("lsusb");
    /*
    Bus 001 Device 006: ID 04e8:6866 Samsung Electronics Co., Ltd 
    Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
    Bus 002 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
    */  
    int number=count_char(result,'\n');
    return number;
}

int timing()
{
    struct timeval starttime,endtime;
    int usbnum=0;
       
    gettimeofday(&starttime,0);
 
    for(;;)
    {
        usbnum = getusbcount();
        if(usbnum > DEFAULT_USB_NUM)
        {
            //devices=1;
            devicenum=usbnum-DEFAULT_USB_NUM;
            printf("device insert again!!\n");
            //send_signal();
            break;
        }
        sleep(10);
        gettimeofday(&endtime,0); 
        int timeuse  = endtime.tv_sec - starttime.tv_sec;
        if(timeuse>=30)
        {
            //pox_system("rm /root/log");
            printf("ready to poweroff!!!\n");
            execl("/sbin/poweroff", "poweroff", (char *)0 );
        }
    } 
    return 0;  
}
  
void *power_thread(void *arg)  
{  
        pthread_mutex_lock(&m_lock);  
        printf("Power Thread running\n");  
        /*等待满足条件，期间互斥量仍然可用*/  
        while (1)
        {
            pthread_cond_wait(&poweroff_ready, &m_lock);
            timing();
        }  
        pthread_mutex_unlock(&m_lock);  
        //pthread_exit(NULL);  
}


int iosorandroid(){
    if(checkphone(cmd_system("lsusb"))){
        return 1;
    }else{
        return 0;
    }
}

int checkphone(const char * usbstr)
{
    char *needle="Apple, Inc."; //Bus 001 Device 014: ID 05ac:12a0 Apple, Inc. iPhone 4S
    char *temp;
    temp=strstr(usbstr,needle);
    if(temp!=NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
} 

void original_getinfo()
{
    passnumber=1;
    if(iosorandroid())
        return;
    #ifdef LOG_FILE
        write_file("android implant\n");
    #else
        printf("android implant\n");
    #endif
    if(install()){
        //sleep(2);
        amstart();
        //delete();
    }
    passnumber=0;
}

void initial_check_usb()
{
     if (getusbcount()<=2)
     {
         send_poweroff();
     }
     else
     {
        
        original_getinfo();
     }
}

void send_poweroff()
{
    pthread_mutex_lock(&m_lock);   
    pthread_mutex_unlock(&m_lock);  
    pthread_cond_signal(&poweroff_ready);  
}


void *get_data()
{
    sleep(1);
    if(iosorandroid())
        return;
    #ifdef LOG_FILE
        write_file("android implant\n");
    #else
        printf("android imlant\n");
    #endif
    if(install()){
        //sleep(2);
        amstart();
        //delete();
    }
    sleep(2);
    passnumber=0;
    pthread_exit(0);
}

void *dev_leave()
{
    sleep(3);
    passnumber=0;
    printf("thread 2 end\n");
    send_poweroff();
    pthread_exit(0);
}

void thread_insert()
{
    int temp;
    memset(&thread, 0, sizeof(thread));          
        /*创建线程*/
    if((temp = pthread_create(&thread[0], NULL, get_data, NULL)) != 0)       
        printf("线程1创建失败!\n");
    else{
        #ifdef LOG_FILE
            write_file("线程1被创建\n");
        #else
            printf("线程1被创建\n");
        #endif
    }
}

void thread_remove()
{
    int temp;
    memset(&thread, 0, sizeof(thread));          
        
    if((temp = pthread_create(&thread[0], NULL, dev_leave, NULL)) != 0)       
        printf("线程2创建失败!\n");
    else
        printf("线程2被创建\n");
}

void MonitorNetlinkUevent()
{
    pthread_t tid1;   
    pthread_mutex_init(&m_lock, NULL);  
    pthread_cond_init(&poweroff_ready, NULL);
    pthread_create(&tid1, NULL, power_thread, NULL);  
    //sleep(1);

    initial_check_usb();
    int sockfd;
    struct sockaddr_nl sa;
    int len;
    char buf[4096];
    struct iovec iov;
    struct msghdr msg;
    int i;
    char ins[]="add";
    char out[]="remove";
    int res;
    int off;

    memset(&sa,0,sizeof(sa));
    sa.nl_family=AF_NETLINK;
    sa.nl_groups=NETLINK_KOBJECT_UEVENT;
    sa.nl_pid = 0;//getpid(); both is ok
    //①、创建一个socket描述符
    sockfd=socket(AF_NETLINK,SOCK_RAW,NETLINK_KOBJECT_UEVENT);
    if(sockfd==-1)
        printf("socket creating failed:%s\n",strerror(errno));
    
    //②、将描述符绑定到接收地址,  函数 bind() 用于把一个打开的 netlink socket 和 netlink 源 socket 地址绑定在一起
    if(bind(sockfd,(struct sockaddr *)&sa,sizeof(sa))==-1)
        printf("bind error:%s\n",strerror(errno));
    while(1)
    {
        //printf("monitor round\r\n");
        memset(&msg,0,sizeof(msg));
        iov.iov_base=(void *)buf;
        iov.iov_len=sizeof(buf);
        msg.msg_name=(void *)&sa;
        msg.msg_namelen=sizeof(sa);
        msg.msg_iov=&iov;
        msg.msg_iovlen=1;
        //③开始接收uevent
        len=recvmsg(sockfd,&msg,0);
        if(len<0)
            printf("receive error\n");
        else if(len<32||len>sizeof(buf))
            printf("invalid message");
        
        if(passnumber)
        {
            printf("continue !!!!\n");
            continue;
        }

        passnumber=1;
        for(i=0;i<len;i++)
        if(*(buf+i)=='\0') 
            buf[i]='\n';
        
        //printf("%s\n", buf);
        res=strncasecmp(ins,buf,strlen(ins));
        off=strncasecmp(out,buf,strlen(out));
        if(!res)
        {
            printf("device insert\n");
            thread_insert();
        }
        if(!off)
        {
            printf("device remove\n");
            thread_remove();
        }
      

    }

}

int main(int argc,char **argv)
{
    MonitorNetlinkUevent();
    return 0;
}
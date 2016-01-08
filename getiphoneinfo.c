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
#include <linux/netlink.h> 
#include <sys/un.h>   
#define UNIX_DOMAIN "/tmp/UNIX.domain"  
#define UEVENT_BUFFER_SIZE 2048 
enum BOOL { false = 0,true = 1};
enum BOOL on_off=true;
enum BOOL reinsert=true;

char filename[64]="\0";
int len=0;


  


int create_multi_dir(const char *path)
{
        int i, len;

        len = strlen(path);
        char dir_path[len+1];
        char fpath[1]="/";
        dir_path[len] = '\0';


        strncpy(dir_path, path, len);
        if(dir_path[len-1] !='/' )
        {
                strcat(dir_path, fpath);
                printf("%s\n", dir_path); 
        }

        for (i=0; i<len+1; i++)
        {
            if (dir_path[i] == '/' && i > 0)
            {
                dir_path[i]='\0';
                if (access(dir_path, F_OK) < 0)
                {
                    if (mkdir(dir_path, 0777) < 0)
                    {
                        printf("mkdir=%s:msg=%s\n", dir_path, strerror(errno));
                        return -1;
                    }
                }
                dir_path[i]='/';
            }
        }

        return 1;
}

int mount()
{
	//printf("into ifuse\n");
	int pid;
	if ((pid = fork()) < 0) 
	{
    	printf("fork ifuse error\n");
    	return 0;
	}
	else if (pid == 0) 
	{
    	if(execl("/usr/bin/ifuse", "ifuse",  "/mnt/iphone", (char *)0 )<0)
   		{
   			printf("ifuse iphone %s\n", strerror(errno));
   			printf("ifuse iphone error\n");
   			printf("insert iphone again!!!!!!!!!\n" );
   			return 0;
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
        printf("mount normal exit status=%d\n", WEXITSTATUS(status));
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

int agamount(char *str,char * string)
{
	
	int pid;
	if ((pid = fork()) < 0) 
	{
    	printf("fork ifuse error\n");
    	return 0;
	}
	else if (pid == 0) 
	{
    	if(execl("/usr/bin/ifuse", "ifuse", str,"--appid",string, (char *)0 )<0)
   		{
   			printf("ifuse  %s\n", strerror(errno));
   			printf("ifuse %s error\n",str);
   			return 0;
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
        printf("agamount normal exit status=%d\n", WEXITSTATUS(status));
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

int copy(char str[64],char name[32],int sym)
{
	char *wdest=(char *)malloc(sizeof(char)*64);//"/tmp/";
	strcpy(wdest,"/myfolder/");
	char qqdest[24]="/myfolder/";
	char wtype[8]=".sqlite";
	char *qtype=".";
	
	//printf("~~~~~000%s\n", wdest);
	strcat(wdest,name);
	//printf("~~~~~111%s\n", wdest);
	strcat(qqdest,name);
	strcat(wdest,wtype);
	//printf("~~~~~222%s\n", wdest);
	//strcat(qqdest,qtype);
	int pid;
	if ((pid = fork()) < 0) 
	{
    	printf("fork ifuse error\n");
    	return 0;
	}
	else if (pid == 0) 
	{                                
    	if(sym==0) //qq
    	{
    		execl("/bin/cp","cp", "-r",str,qqdest, (char *)0 );   //-r delete
    	}
    	else if(sym==1) //weixin 
    	{
    		//printf("%s\n", name);
    		//printf("%s\n", wdest);
    		execl("/bin/cp","cp", str,wdest, (char *)0 );
    	}
    	else
    	{
    		execl("/bin/cp","cp","-r", str,"/myfolder/", (char *)0 );
    	}
	}
	free(wdest);
	int status;
	pid_t ret;
	ret=wait(&status);
	if(ret <0){
        perror("wait error");
        return 0;
    }
    if (WIFEXITED(status))
    {
        printf("copy normal exit status=%d\n", WEXITSTATUS(status));
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

int unmount(char* str)
{
	
	int pid;
	if ((pid = fork()) < 0) 
	{
    	printf("fork ifuse error\n");
    	return 0;
	}
	else if (pid == 0) 
	{
    	if(execl("/bin/umount", "umount", str, (char *)0 )<0)
   		{
   			printf("umount  %s\n", strerror(errno));
   			printf("umount %s error\n",str);
   			return 0;
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
        printf("unmount normal exit status=%d\n", WEXITSTATUS(status));
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

int reboot()
{
	int pid;
	if ((pid = fork()) < 0) 
	{
    	printf("fork ifuse error\n");
    	return 0;
	}
	else if (pid == 0) 
	{
    	if(execl("/sbin/reboot", "reboot", (char *)0 )<0)
   		{
   			printf("reboot  %s\n", strerror(errno));
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
        printf("reboot normal exit status=%d\n", WEXITSTATUS(status));
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

int changemod()
{
	int pid;
	if ((pid = fork()) < 0) 
	{
    	printf("fork ifuse error\n");
    	return 0;
	}
	else if (pid == 0) 
	{
    	if(execl("/bin/chmod","chmod", "0777","/mnt/", (char *)0 )<0)
   		{
   			printf("chmod  %s\n", strerror(errno));
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
        printf("chmod  normal exit status=%d\n", WEXITSTATUS(status));
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

void listxindir(char* path,char *des )
{
	DIR              *pDir ;  
    struct dirent    *ent  ;  

    //printf("into listxin!!!!!!!!!!!!!!!!!!! \n"); 
    if(!(pDir=opendir(path)))
    {
        printf("error opendir %s!!!\n",path);
        
    }
      
  
    while((ent=readdir(pDir))!=NULL)  
    {  

        if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)  
            continue; 
        if(strlen(ent->d_name)==32&&strcmp(ent->d_name,"00000000000000000000000000000000"))
        {
            //des=ent->d_name;
            strcpy(des,ent->d_name);
            //printf("yes or no %s ~~~~~~~~~~~~~~~~~~~~~~",ent->d_name);
        }

       
    } 
    //printf("out circle!!!\n");
    if(closedir(pDir)<0)
    {
    	printf("list xin closedir error %s\n",strerror(errno) );
    } 

}

int compare(char name[])
{
	if(name[0]=='1'||name[0]=='2'||name[0]=='3'|name[0]=='4'||name[0]=='5'||name[0]=='6'||name[0]=='7'||name[0]=='8'||name[0]=='9')
	{
		return 1;
	}
	else
	{
		return 0;	
	}

}

void listqqdir(char* path,int * count )
{
	DIR              *pDir ;  
    struct dirent    *ent  ;  
    char *divide="|" ; 
    int i=0;
    //printf("into listqqdir!!!!!!!!!\n");
    if(!(pDir=opendir(path)))
    {
        printf("error opendir %s!!!\n",path);
        
    }
     
    while((ent=readdir(pDir))!=NULL)  
    {  
    	//printf("test !!\n");
        if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)  
           continue; 
        if(compare(ent->d_name))
        {
            i++;
            strcat(filename, ent->d_name); //保存遍历到的文件名
            strcat(filename, divide);
        }
       
    } 
    *count=i; 
    //i=0;
    if(closedir(pDir)<0)
    {
    	printf("list xin closedir error %s\n",strerror(errno) );
    } 

}

void sleeptime(int time)
{
	sleep(time);
}

char* cmd_system(const char* command)
{
    char* result=NULL;
    result=(char *)malloc(1024*sizeof(char));
    FILE *fpRead;
    fpRead =popen(command,"r");
    char buf[1024];
    memset(buf,'\0',sizeof(buf));
    while(fgets(buf,1024-1,fpRead)!=NULL)
    {
        strcat(result,buf);
    }
    if(fpRead!=NULL)
        pclose(fpRead);
    return result;
}

int cpdocument()
{
	int i,num=0;
	char xindir[64]="\0";
	char *divide="|" ;
	char *p; 
	char qqaccount[20]="\0";
	char *constant="/mnt/qq/" ;
	char re_name[64]="\0";
	char xindata[128]="/mnt/wechat/";
	
	// char rawwe[32]="/mnt/wechat";
	// char wechat[32]="\0"
	// strcat(wechat,)
	//printf("into ~~~~~~~~~~~\n");
	//sleeptime(2);
	changemod();
  	printf("chmod finished\n");
	if(create_multi_dir("/mnt/wechat/")!=1)   
    	{  
        	printf(" mkdir wechat failed!\n");  
        	return 0;  
   		} 
   	if(create_multi_dir("/mnt/qq/")!=1)   
    	{  
        	printf(" mkdir qq failed!\n");  
        	return 0;  
   		}
   	if(create_multi_dir("/mnt/iphone/")!=1)   
    	{  
        	printf(" mkdir iphone failed!\n");  
        	return 0;  
   		}
   	printf("mkdir finished\n");
   	if(!mount())
   	{
   		if(!mount())
   		{
   			printf("try again\n");
   			//reinsert=1;
   			return 0;
   		}
   	}

   	if(!agamount("/mnt/wechat","com.tencent.xin"))
   	{
   		printf("mount wechat error\n");
   		return 0;
   	}
   	
   	if(!agamount("/mnt/qq","com.tencent.mqq"))
   	{
   		printf("mount qq error\n");
   		return 0;
   	}

   	listxindir("/mnt/wechat",xindir); 
   	strcpy(re_name,xindir);
   	//printf("3333333333\n");
   	strcat(xindata,xindir);
   	//printf("444444\n");
   	strcat(xindata,"/DB/MM.sqlite");
   	//printf("11111%s\n",xindata );
   	
   	// if(copy("/mnt/iphone/DCIM",re_name,2))
   	// {
   	// 	printf("cp iphone error\n");
   	// 	return 0;
   	// }
   	
   	if(!copy(xindata,re_name,1))
   	{
   		printf("cp wechat error\n");
   		return 0;
   	}
   	
   	
    listqqdir("/mnt/qq/",&num);
    //  注意/
    printf("%s\n", filename);
    p=strtok(filename,divide);
    while(p!=NULL)
    {
    	//printf("%s\n", p);
    	strcpy(qqaccount,constant);
    	strcat(qqaccount,p);
    	//printf("222222%s\n",qqaccount );
    	if(!copy(qqaccount,p,0))
   		{
   			printf("cp qq error\n");
   			return 0;
   			
   		}
   		p=strtok(NULL,divide);
   		memset(qqaccount,0,sizeof(qqaccount));
    }
    memset(filename,0,sizeof(filename));
    

   	if(!unmount("/mnt/iphone"))
   	{
   		printf("unmount filed \n"); 
   		//reboot();
   		
   	}

   	if(!unmount("/mnt/wechat"))
   	{
   		printf("unmount filed \n");   		
   		//reboot();
   		
   	}

   	if(!unmount("/mnt/qq"))
   	{
   		printf("unmount filed \n");    		
   		//reboot();
   		
   	}
   	
	return 1;
		
}
  


	
int main(void)  
{  
    socklen_t clt_addr_len;  
    int listen_fd;  
    int com_fd;  
    int ret;  
    int i;  
    static char recv_buf[1024];   
    int len;  
    struct sockaddr_un clt_addr;  
    struct sockaddr_un srv_addr;  
    listen_fd=socket(PF_UNIX,SOCK_STREAM,0);  
    if(listen_fd<0)  
    {  
        perror("cannot create communication socket");  
        return 1;  
    }    
      
    //set server addr_param  
    srv_addr.sun_family=AF_UNIX;  
    strncpy(srv_addr.sun_path,UNIX_DOMAIN,sizeof(srv_addr.sun_path)-1);  
    unlink(UNIX_DOMAIN);  
    //bind sockfd & addr  
    ret=bind(listen_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));  
    if(ret==-1)  
    {  
        perror("cannot bind server socket");  
        close(listen_fd);  
        unlink(UNIX_DOMAIN);  
        return 1;  
    } 

    //listen sockfd 

    ret=listen(listen_fd,1);  
    if(ret==-1)  
    {  
        perror("cannot listen the client connect request");  
        close(listen_fd);  
        unlink(UNIX_DOMAIN);  
        return 1;  
    } 
    
    len=sizeof(clt_addr); 
        
    while(1)
    { 
    	com_fd=accept(listen_fd,(struct sockaddr*)&clt_addr,&len); 
         
        if(com_fd<0)  
        {  
             perror("cannot accept client connect request");  
            close(listen_fd);  
            unlink(UNIX_DOMAIN);  
            return 1;  
        }  
        //read and printf sent client info  
        //printf("/n=====info=====/n");  
    
        memset(recv_buf,0,1024);  
        int num=read(com_fd,recv_buf,sizeof(recv_buf));  
        printf("%s\n",recv_buf);   
     	if(!cpdocument())
    	{
        	printf("execute failed\n");
        	unmount("/mnt/iphone");
   			  unmount("/mnt/qq");
   			  unmount("/mnt/wechat");
    	}
   		else
    	{
        	printf("work has been  completed\n");
   		}

    } 
     
    
    close(com_fd);  
    close(listen_fd);  
    unlink(UNIX_DOMAIN);  
    return 0;  
}
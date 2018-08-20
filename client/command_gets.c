#include "func.h"
extern const char client_pwd[];
void command_gets(char *fileName,int sfd)
{
	int flag=File_Isexist(fileName);
	int tag;
	if(flag==-1)
	{
		printf("客户端路径中存在同名目录\n");
		tag=0;
		send_n(sfd,(char*)&tag,sizeof(int));
		return;
	}else if(flag==1)
	{
		char tmp_path[200]={0};
		sprintf(tmp_path,"%s%s%s%s",client_pwd,"/",fileName,".tmp");
		//puts(tmp_path);
		int fd=open(tmp_path,O_RDWR);
		if(fd==-1)
		{
			printf("路径中已经存在完整文件\n");
			tag=0;
			send_n(sfd,(char*)&tag,sizeof(int));
			return;
		}
		tag=1;
		send_n(sfd,(char*)&tag,sizeof(int)); //发送下载信号
		//否则断点下载
		recv_n(sfd,(char*)&tag,sizeof(int));
		if(tag==1)         //服务器存在文件,开始断点下载
		{
			printf("开始断点下载\n");
			long size,file_size;
			read(fd,&size,sizeof(long));  //从.tmp中读取文件偏移大小
			//printf("已经下载了%ld字节\n",size);
			send_n(sfd,(char*)&size,sizeof(long));
			recv_n(sfd,(char*)&file_size,sizeof(long)); //读取文件大小
			float already=size*100.0/file_size;
			printf("已经下载了%5.2f%s\n",already,"%");
			int fd1=open(fileName,O_RDWR);
			char *p;
			p=(char*)mmap(NULL,file_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd1,0);
			recv_file_n(sfd,fd,p+size,file_size-size,size);
			munmap(p,file_size);
			close(fd1);
			close(fd);
			remove(tmp_path);
			printf("下载文件成功\n");
		}
		else if(tag==-1)
		{
			printf("此版本无法直接下载文件夹,请登录官网下载最新版本\n");
		}else if(tag==0)
		{
			printf("服务器不存在此文件\n");
		}				
	}else if(flag==0)  //不存在同名文件或目录,直接普通下载
	{
		//printf("I am here of flag=0\n");
		tag=1;
		send_n(sfd,(char*)&tag,sizeof(int)); //发送下载信号
		recv_n(sfd,(char*)&tag,sizeof(int)); //接受服务器的文件判断信息
		if(tag==1)   //服务器存在文件
		{
			//printf("I am here of tag=1\n");
			char tmp_path[200]={0};
			sprintf(tmp_path,"%s%s%s%s",client_pwd,"/",fileName,".tmp");
			//puts(tmp_path);
			int fd_tmp=open(tmp_path,O_RDWR|O_CREAT,0664);
			if(fd_tmp==-1)
			{
				perror("open");
			}
			int fd_file=open(fileName,O_RDWR|O_CREAT,0664);
			if(fd_file==-1)
			{
				perror("open");
			}
			long offset=0,file_size;
			send_n(sfd,(char*)&offset,sizeof(long));
			recv_n(sfd,(char*)&file_size,sizeof(long));
			ftruncate(fd_file,file_size);
			char *p;
			p=(char*)mmap(NULL,file_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd_file,0);
			recv_file_n(sfd,fd_tmp,p,file_size,0);
			munmap(p,file_size);
			close(fd_file);
			close(fd_tmp);
			remove(tmp_path);
			printf("下载文件成功\n");

		}
		else if(tag==-1)
		{
			printf("此版本无法直接下载文件夹,请登录官网下载最新版本\n");
		}else if(tag==0)
		{
			printf("服务器不存在此文件\n");
		}				
	}
}	







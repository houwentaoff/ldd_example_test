/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_app.c
 *
 *    Description:  异步ＡＩＯ　
 *         Others: 使用libaio
 *
 *        Version:  1.0
 *        Created:  10/10/2019 11:50:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<libaio.h>
#include<errno.h>
#include<unistd.h>
static void wr_done(io_context_t ctx, struct iocb *iocb, long res, long res2)
{
	if (res2 != 0) {
		printf("aio write error \n");
	}
	printf("aio write ...\n");
	//	if (res != iocb->u.c.nbytes) {
	//		printf("write missed bytes expect % d got % d n", iocb->u.c.nbytes, res);
	//		exit(1);
	//	}

}
int main(void){
	int output_fd;
	const char *content="hello world!";
	const char *outputfile="hello.txt";
	io_context_t ctx;
	struct iocb io[10];
	struct iocb *p[10]={0} ;
	struct io_event e[10];
	struct timespec timeout;
	int i=0;
	int aio_ret = 0;
	int num;

	memset(&ctx,0,sizeof(ctx)); 

	for (i=0; i<10; i++){
		p[i] = &io[i];
	}

	if(io_setup(10,&ctx)!=0){//init
		printf("io_setup error\n"); 
		return -1; 
	}   
	if((output_fd=open(outputfile,O_CREAT|O_WRONLY,0644))<0){   
		perror("open error");
		io_destroy(ctx);
		return -1; 
	}   
	io_prep_pwrite(&io[0],output_fd,(void *)content,3,1);
	io_set_callback(p[0], wr_done);
	//    io.data=(void *)content;
	if(io_submit(ctx,1,&p[0])!=1){
		io_destroy(ctx);
		printf("io_submit error\n");    
		return -1; 
	}   
	if((output_fd=open("aabbcc",O_CREAT|O_WRONLY,0644))<0){   
		perror("open error");
		io_destroy(ctx);
		return -1; 
	}   
	io_prep_pwrite(&io[1],output_fd,(void *)content,strlen(content),0);
	io_set_callback(p[1], wr_done);
	//    io.data=(void *)content;
	if(io_submit(ctx,1,&p[1])!=1){
		io_destroy(ctx);
		printf("io_submit error\n");    
		return -1; 
	}   
	while(1){
		io_callback_t cb;
		timeout.tv_sec=0;
		timeout.tv_nsec=0;//0.5s
		num = io_getevents(ctx,0,10,e,&timeout);
		if (num){   
			printf("have done aio ret [%d]\n", num);
			for (i = 0; i < num; i++) {
				cb = (io_callback_t) e[i].data;
				struct iocb *io = e[i].obj;

				printf("events[%d].data = 0x%p, res = %lu, res2 = %lu \n", i,(void *) cb, e[i].res, e[i].res2);
				cb(ctx, io, e[i].res, e[i].res2);
			}
		}   
		printf("haven't done\n");
		sleep(1);
	}   
	io_destroy(ctx);
	return 0;
}

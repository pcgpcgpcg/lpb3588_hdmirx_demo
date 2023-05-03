#include <iostream>
#include <iostream>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<stdlib.h>
#include <zconf.h>
#include<linux/videodev2.h>
#include<sys/mman.h>

int main() {
    std::cout << "main()" << std::endl;
    int fd=open("/dev/video0",O_RDWR);
    if(fd<0){
        perror("open device fail\n");
        return -1;
    }
    
    //check the driver
    struct v4l2_capability v4Cap;
    //memset(&v4Cap, 0, sizeof(struct v4l2_capability));
    int ret = ioctl(fd, VIDIOC_QUERYCAP, &v4Cap);
    if(ret<0){
        printf("Error opening device: unable to query device.\n");
    }
    printf("query success!.\n");
    //show all the support formats
    struct v4l2_fmtdesc v4FmtDesc;
    v4FmtDesc.index = 0;
    v4FmtDesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    
    while((ret = ioctl(fd, VIDIOC_ENUM_FMT,&v4FmtDesc)) == 0){
      //printf("video format=%s \n",v4FmtDesc.pixelformat);
      unsigned char *p=(unsigned  char*)&v4FmtDesc.pixelformat;
      printf("%c %c %c %c\n",p[0],p[1],p[2],p[3]);
      printf("%s\n",v4FmtDesc.description);
      v4FmtDesc.index++;
    }
    
     //set the pic format
    struct v4l2_format vFormat;
    vFormat.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vFormat.fmt.pix.width=640;
    vFormat.fmt.pix.height=480;
    vFormat.fmt.pix.pixelformat=V4L2_PIX_FMT_YUYV; //should check the real pix format
    
    ret=ioctl(fd,VIDIOC_S_FMT,&vFormat);
    if(ret){
        printf("set video fmt failed");
        perror("set fail\n");
    }
    
    printf("request memory!\n");
     
    //request os core space
    struct v4l2_requestbuffers vqbuff;
    vqbuff.count=4;
    vqbuff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vqbuff.memory=V4L2_MEMORY_MMAP;
    ret =ioctl(fd,VIDIOC_REQBUFS,&vqbuff);
    if(ret){
        perror("buff fail\n");
    }
    printf("request memory success!\n");
    
    //map the buffer from core to users
     struct v4l2_buffer vbuff;
     vbuff.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
     vbuff.memory = V4L2_MEMORY_MMAP;
    unsigned char * mptr[4];
    
    for(int i=0;i<4;i++){
        vbuff.index=i;
        ret=ioctl(fd,VIDIOC_QUERYBUF,&vbuff);
        if(ret<0)
        {
            perror("requeire buff fail\n");
        }
        mptr[i]= (unsigned char *)mmap(NULL,vbuff.length,PROT_READ|PROT_WRITE,MAP_SHARED,fd,vbuff.m.offset);
        //通知完毕
        ret=ioctl(fd,VIDIOC_QBUF,&vbuff);
        if(ret<0){
            perror("put fail");
        }
    }
    printf("map buffer success!\n");
    printf("begin capture...\n");
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if(ret){
        perror("open fail");
    }
    //grab data from queue

    return 0;
}
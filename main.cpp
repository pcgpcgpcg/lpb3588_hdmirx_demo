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
    // 获取摄像头支持的格式
    //获取摄像头支持的格式
    struct v4l2_fmtdesc v4fmt;
    v4fmt.index=0;
    v4fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
 
    int ret=ioctl(fd,VIDIOC_ENUM_FMT,&v4fmt );
    if(ret<0){
        perror("acvquire fail\n");
    }
    printf("%s\n",v4fmt.description);
    unsigned char *p=(unsigned  char*)&v4fmt.pixelformat;
    printf("%c %c %c %c\n",p[0],p[1],p[2],p[3]);
    return 0;
}
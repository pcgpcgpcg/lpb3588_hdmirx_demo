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
#include<cstring>

#define BUFER_REQ 3

struct video_buffer {
	void *start;
	size_t length;
	size_t offset;
	int out_height;
	int out_width;
	int dis_x_off;
	int dis_y_off;
};

int querybuf(int v4l2_fd, int buf_id, struct video_buffer *buffers)
{
	int ret;
	struct v4l2_buffer buf;
	struct v4l2_plane *planes;

	planes = (v4l2_plane*)malloc(1 * sizeof(v4l2_plane));
	if (!planes) {
		printf("alloc plane fail\n");
		return -1;
	}

	memset(&buf, 0, sizeof(buf));

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = V4L2_MEMORY_MMAP;

	buf.m.planes = planes;
	buf.length = 1;
	buf.index = buf_id;
	ret = ioctl(v4l2_fd, VIDIOC_QUERYBUF, buf);
	if (ret < 0) {
		printf("VIDIOC_QUERBUF error\n");
		return -1;
	}

	buffers[buf_id].length = buf.m.planes[0].length;
	buffers[buf_id].offset = (size_t)buf.m.planes[0].m.mem_offset;

	free(planes);
	return 0;
}

int qbuf(int v4l2_fd, unsigned int buf_id, struct video_buffer *buffers)
{
	int ret;
	struct v4l2_buffer buf;
	struct v4l2_plane *planes;

	planes = (v4l2_plane*)malloc(1 * sizeof(*planes));
	if (!planes) {
		printf("alloc plane fail\n");
		return -1;
	}

	memset(&buf, 0, sizeof(buf));

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = V4L2_MEMORY_MMAP;

	buf.m.planes = planes;
	buf.length = 1;
	buf.index = buf_id;

	buf.m.planes[0].length = buffers[buf_id].length;
	buf.m.planes[0].m.mem_offset = buffers[buf_id].offset;

	ret = ioctl(v4l2_fd, VIDIOC_QBUF, &buf);
	if (ret < 0) {
		printf("VIDIOC_QBUF error\n");
		return -1;
	}

	free(planes);
	return 0;
}

int dqbuf(int v4l2_fd, unsigned int *buf_id)
{
	int ret;
	struct v4l2_buffer buf;
	struct v4l2_plane *planes;

	planes = (v4l2_plane *)malloc(1 * sizeof(*planes));
	if (!planes) {
		printf("alloc plane fail\n");
		return 0;
	}

	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.m.planes = planes;
	buf.length = 1;

	ret = ioctl(v4l2_fd, VIDIOC_DQBUF, &buf);
	if (ret < 0)
		return -1;

	free(planes);

	*buf_id = buf.index;
	return 0;
}

int main() {
    video_buffer *buffers;
    unsigned int cur_id;
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
    printf("cap = 0x%0x\n", v4Cap.capabilities);
	if (!(v4Cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)) {
		printf("cann't support v4l2 capture deivce\n");
		return -1;
	}

    printf("query success!\n");
    //show all the support formats
    struct v4l2_fmtdesc v4FmtDesc;
    v4FmtDesc.index = 0;
    v4FmtDesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    
    while((ret = ioctl(fd, VIDIOC_ENUM_FMT,&v4FmtDesc)) == 0){
      printf("video format \n");
      unsigned char *p=(unsigned  char*)&v4FmtDesc.pixelformat;
      printf("%c %c %c %c\n",p[0],p[1],p[2],p[3]);
      printf("%s\n",v4FmtDesc.description);
      v4FmtDesc.index++;
    }
    
     //set the pic format
    struct v4l2_format vFormat;
    vFormat.type=V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
     vFormat.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_BGR24;
	 vFormat.fmt.pix_mp.width = 1920;
	 vFormat.fmt.pix_mp.height = 1080; 
	 vFormat.fmt.pix_mp.num_planes = 1;
    
    ret=ioctl(fd,VIDIOC_S_FMT,&vFormat);
    if(ret){
        perror("set video fmt fail\n");
    }
    
    printf("request memory!\n");
    //request os core space
    struct v4l2_requestbuffers reqBuffs;
    reqBuffs.count=BUFER_REQ;
    reqBuffs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    reqBuffs.memory=V4L2_MEMORY_MMAP;
    ret =ioctl(fd,VIDIOC_REQBUFS,&reqBuffs);
    if(ret){
        perror("buff fail\n");
    }
    printf("request memory success!\n");
    buffers = (video_buffer*)calloc(reqBuffs.count, sizeof(struct video_buffer));
    if (!buffers) {
			printf("video_buffer null\n");
			return -1;
	}

    //map the buffer from core to users
    //  struct v4l2_buffer vbuff;
    //  struct v4l2_plane *planes;
	//  planes = (v4l2_plane*)malloc(1 * sizeof(v4l2_plane));

    //  vbuff.type=V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    //  vbuff.memory = V4L2_MEMORY_MMAP;
    //  vbuff.m.planes = planes;
    //  vbuff.length = 1;
    // unsigned char * mptr[BUFER_REQ];
    
    for(int i=0;i<BUFER_REQ;i++){
        buffers[i].out_width = 1920;
        buffers[i].out_height = 1080;
        ret = querybuf(fd, i, buffers);
        if(ret<0)
        {
            perror("requeire buff fail\n");
        }
        buffers[i].start = mmap(NULL, buffers[i].length, PROT_READ|PROT_WRITE,MAP_SHARED, fd, buffers[i].offset);
        //vbuff.index=i;
        //ret=ioctl(fd,VIDIOC_QUERYBUF,&vbuff);

        // mptr[i]= (unsigned char *)mmap(NULL,vbuff.length,PROT_READ|PROT_WRITE,MAP_SHARED,fd,vbuff.m.offset);
        // if(mptr[i] == MAP_FAILED){
        //     perror("map failed for mptr.\n");
        // }
        //通知完毕
        ret = qbuf(fd, i, buffers);
        if(ret<0){
            perror("put fail");
        }
    }
    //free(planes);
    printf("map buffer success!\n");
    printf("begin capture...\n");
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if(ret){
        perror("open streamon failed\n");
    }
    printf("begin grab data from queue!\n");
    //grab data from queue
     ret = dqbuf(fd, &cur_id);
     if(ret<0){
        perror("dqbuf failed\n");
     }
     
     printf("begin write image file...\n");
	//write_image_file(vdata[i].savefile, vdata[i].vbuf[cur_id].start, &fmt);
    FILE* fp = fopen("testbgr24","w+");
    fwrite(buffers[cur_id].start, vFormat.fmt.pix_mp.plane_fmt[0].sizeimage, 1, fp);
    fclose(fp);

    free(buffers);
    //}

    return 0;
}
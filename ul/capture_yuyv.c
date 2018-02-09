#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <linux/fs.h> /* everything... */
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>

uint8_t *buffer;
 
static int xioctl(int fd, int request, void *arg)
{
        int r;
 
        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);
 
        return r;
}
 
int print_caps(int fd)
{
        struct v4l2_capability caps = {};
        if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &caps))
        {
                perror("Querying Capabilities");
                return 1;
        }
 
        printf( "Driver Caps:\n"
                "  Driver: \"%s\"\n"
                "  Card: \"%s\"\n"
                "  Bus: \"%s\"\n"
                "  Version: %d.%d\n"
                "  Capabilities: %08x\n",
                caps.driver,
                caps.card,
                caps.bus_info,
                (caps.version>>16)&&0xff,
                (caps.version>>24)&&0xff,
                caps.capabilities);
 
 
        struct v4l2_cropcap cropcap = {0};
        cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == xioctl (fd, VIDIOC_CROPCAP, &cropcap))
        {
                perror("Querying Cropping Capabilities");
                return 1;
        }
 
        printf( "Camera Cropping:\n"
                "  Bounds: %dx%d+%d+%d\n"
                "  Default: %dx%d+%d+%d\n"
                "  Aspect: %d/%d\n",
                cropcap.bounds.width, cropcap.bounds.height, cropcap.bounds.left, cropcap.bounds.top,
                cropcap.defrect.width, cropcap.defrect.height, cropcap.defrect.left, cropcap.defrect.top,
                cropcap.pixelaspect.numerator, cropcap.pixelaspect.denominator);
 
        int support_grbg10 = 0;
 
        struct v4l2_fmtdesc fmtdesc = {0};
        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmtdesc.pixelformat = V4L2_PIX_FMT_YUYV;
	//printf("format: %s\n", (char *)&fmtdesc.pixelformat);
        char fourcc[5] = {0};
        char c, e;
        printf("  FMT : CE Desc\n--------------------\n");
        while (0 == xioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc))
        {
                strncpy(fourcc, (char *)&fmtdesc.pixelformat, 4);
                //if (fmtdesc.pixelformat == V4L2_PIX_FMT_SGRBG10)
                    //support_grbg10 = 1;
                c = fmtdesc.flags & 1? 'C' : ' ';
                e = fmtdesc.flags & 2? 'E' : ' ';
                printf("  %s: %c%c %s\n", fourcc, c, e, fmtdesc.description);
                fmtdesc.index++;
        }
	printf("fourcc: %s", fourcc);
        /*
        if (!support_grbg10)
        {
            printf("Doesn't support GRBG10.\n");
            return 1;
        }*/
 
        struct v4l2_format fmt = {0};
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = 176;
        fmt.fmt.pix.height = 144;
        //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
        //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
        fmt.fmt.pix.field = V4L2_FIELD_NONE;
        
        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
        {
            perror("Setting Pixel Format");
            return 1;
        }
 
        strncpy(fourcc, (char *)&fmt.fmt.pix.pixelformat, 4);
        printf( "Selected Camera Mode:\n"
                "  Width: %d\n"
                "  Height: %d\n"
                "  PixFmt: %s\n"
                "  Field: %d\n",
                fmt.fmt.pix.width,
                fmt.fmt.pix.height,
                fourcc,
                fmt.fmt.pix.field);
        return 0;
}
 
int init_mmap(int fd)
{
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
 
    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req))
    {
        perror("Requesting Buffer");
        return 1;
    }
 
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    if(-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
    {
        perror("Querying Buffer");
        return 1;
    }
 
    buffer = mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    printf("Length: %d\nAddress: %p\n", buf.length, buffer);
    printf("Image Length: %d\n", buf.bytesused);
 
    return 0;
}

static void yuv_to_rgb_24(int y, int u, int v, double* B, double* G, double*R)
{
	//double R,G,B;
	
	//conversion equations
	/**B=0.31*(0.004565*y+0.000001*u+0.006250*v-0.872);
	*G=0.63*(0.004565*y-0.001542*u-0.003183*v+0.531);
	*R=0.31*(0.004565*y+0.007935*u-1.088);*/
	/*B = (0.7*y+2.029*(u-128))*0.01;
	*G = (0.7*y-0.369*(u-128)-0.581*(v-128))*0.01;
	*R = (0.7*y+1.14*(v-128))*0.01;*/
	*B = 0.7*y+2*(u-128);
	*G = 0.7*y-0.4*(u-128)-0.6*(v-128);
	*R = 0.7*y+2*(v-128);
	/*B = (y+1.779*(u-128))*0.01;
	*G = (y-0.3455*(u-128)-0.7169*(v-128))*0.01;
	*R = (y+1.4075*(v-128))*0.01;*/

	//R, G and B must be in the range from 0 to 255    
	if (*R < 0)
		*R=0;
	if (*G < 0)
		*G=0;
	if (*B < 0)
		*B=0;
	
	if (*R > 255)
		*R=255;
	if (*G > 255)
		*G=255;
	if (*B > 255)
		*B=255;
}
 
int capture_image(int fd)
{
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    if(-1 == xioctl(fd, VIDIOC_QBUF, &buf))
    {
        perror("Query Buffer");
        return 1;
    }
 
    if(-1 == xioctl(fd, VIDIOC_STREAMON, &buf.type))
    {
        perror("Start Capture");
        return 1;
    }
 
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    struct timeval tv = {0};
    tv.tv_sec = 2;
    int r = select(fd+1, &fds, NULL, NULL, &tv);
    if(-1 == r)
    {
        perror("Waiting for Frame");
        return 1;
    }
 
    if(-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
    {
        perror("Retrieving Frame");
        return 1;
    }
    printf ("saving image\n");
    
    int k = 0;
    double B1, G1, R1;
    double B2, G2, R2;
    int count_red = 0;
    int count_blue = 0;
    int flag = 0;
    int n = 0;

    /*FILE * ff;
    char output[] ="output_yuyv.txt";
    ff=fopen(output,"w");*/

    while (k<50688)
    {
    	yuv_to_rgb_24((int)buffer[k], (int)buffer[k+1], (int)buffer[k+3], &B1, &G1, &R1);
	yuv_to_rgb_24((int)buffer[k+2], (int)buffer[k+1], (int)buffer[k+3], &B2, &G2, &R2);
	if(R1>95.0)
	{
		if(G1<70.0 && B1<70.0)
		{
			if(G1>B1 && G1-B1<20.0)
				count_red = count_red +1;
			if(B1>G1 && B1-G1<20.0)
				count_red = count_red +1;	
		}
	}
	if(R2>95.0)
	{
		if(G2<70.0 && B2<70.0)
		{
			if(G2>B2 && G2-B2<20.0)
				count_red = count_red +1;
			if(B2>G2 && B2-G2<20.0)
				count_red = count_red +1;	
		}
	}

	if(B1>95.0)
	{
		if(R1<50.0)
		{
			if(G1<95.0 && G1>50.0)
				count_blue = count_blue +1;
		}
	}
	if(B2>95.0)
	{
		if(R2<50.0)
		{
			if(G2<95.0 && G2>50.0)
				count_blue = count_blue +1;
		}
	}
	k = k+4;
	//fprintf(ff, "%f %f %f\n", B1, G1, R1);
	//fprintf(ff, "%f %f %f\n", B2, G2, R2);
    }
    if(count_red > 4000)
	flag = 1;
    if(count_blue > 1000)
	flag = 2;
    printf("flag:%d\n", flag);

    int fd1;
    fd1 = open("/dev/motor", O_RDWR | O_APPEND);
	if (fd1 < 0) {
		printf("Open /dev/motor failed!!!\n");
		return 1;
	}

    if (flag == 1)
	{
    	write(fd1, "start", 5);
	}
    else if (flag == 2)
	{
    	write(fd1, "turn", 4);
	}
    else
	{
	write(fd1,"stop",4);
	}

    //fclose(ff);
    close(fd1);
    return 0;
}

int main()
{

        int fd;
        fd = open("/dev/video0", O_RDWR);

	
        if (fd == -1)
        {
                perror("Opening video device");
                return 1;
        }
        if(print_caps(fd))
            return 1;
        
        if(init_mmap(fd))
            return 1;
        int i;

	if(capture_image(fd))
                return 1;

        close(fd);
        return 0;
}

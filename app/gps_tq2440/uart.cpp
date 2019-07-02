
#include "uart.h"

__attribute__ ((aligned (8))) u8 USART1_TX_BUF[USART1_MAX_SEND_LEN];
u8 USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//接收缓冲,最大USART1_MAX_RECV_LEN个字节.

/* 串口初始化 */
/* set_opt(fd,115200,8,'N',1) */
int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;
    /* 保存测试串口参数设置,在这里如果串口号等出错,会有相关的出错信息 */
    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }
    bzero(&newtio, sizeof(newtio));
    /* 步骤一:设置字符大小 */
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    newtio.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    newtio.c_oflag  &= ~OPOST;   /*Output*/
    /* 设置停止位 */
    switch (nBits)
    {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |= CS8;
            break;
    }
    /* 设置奇偶校验位 */
    switch (nEvent)
    {
        case 'O'://奇数
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'E': //偶数
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            break;
        case 'N': //无奇偶校验位
            newtio.c_cflag &= ~PARENB;
            break;
    }
    //设置波特率
    switch (nSpeed)
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        case 38400:
            cfsetispeed(&newtio, B38400);
            cfsetospeed(&newtio, B38400);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
    }
    /* 设置停止位 */
    if( nStop == 1 )
        newtio.c_cflag &= ~CSTOPB;
    else if ( nStop == 2 )
        newtio.c_cflag |= CSTOPB;
    /* 设置等待时间和最小接收字符 */
    newtio.c_cc[VTIME] = 0;     /* 设置从获取到1个字节后开始计时的超时时间 */
    newtio.c_cc[VMIN] = 0;      /* 设置要求等待的最小字节数 */
    /* 处理未接受字符 */
    tcflush(fd,TCIFLUSH);
    /* 激活新配置 */
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        perror("com set error");
        return -1;
    }
    //printf("set done!\n");
    return 0;
}

//想打开的的串口com
int open_port(char *com)
{
    int fd;
    //fd = open(com, O_RDWR|O_NOCTTY|O_NDELAY);
    fd = open(com, O_RDWR|O_NOCTTY);
    if (-1 == fd)
    {
        return -1;
    }

#if 0
    /* 恢复串口为阻塞状态 */
    if(fcntl(fd, F_SETFL, 0) < 0)
    {
        printf("fcntl failed!\n");
        return -1;
    }

    fcntl(fd, F_SETFL, 0);
#endif
    return fd;
}

int fd = -1;
int uart1_init(char *dev_path, u32 baud)
{
    int iRet;

    fd = open_port(dev_path);
    if (fd < 0)
    {
        printf("open port err!\n");
        return -1;
    }

    iRet = set_opt(fd, baud, 8, 'N', 1);
    if (iRet)
    {
        printf("set port err!\n");
        return -1;
    }
    return 0;
}

//串口1,printf 函数
//确保一次发送数据不超过USART1_MAX_SEND_LEN字节
void uart1_printf(char *fmt,...)
{
    u16 i;
    va_list ap;
    va_start(ap,fmt);
    vsprintf((char*)USART1_TX_BUF,fmt,ap);
    va_end(ap);
    i = strlen((const char*)USART1_TX_BUF);       //此次发送数据的长度
    write(fd, USART1_TX_BUF, i + 1);
}

void uart1_read(u8 *str, u32 size)
{
    read(fd, str, size);
}

void uart1_write(u8 *str, u32 size)
{
    write(fd, str, size);
}

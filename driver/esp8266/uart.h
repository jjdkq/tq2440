#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdarg.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned u32;

#define USART3_MAX_SEND_LEN		800					//最大发送缓存字节数
#define USART3_MAX_RECV_LEN		800					//最大接收缓存字节数
extern u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节


void uart1_printf(char* fmt,...);
void uart1_read(u8 *, u32);
void uart1_write(u8 *, u32);
void uart1_init(void);

#endif // UART_H


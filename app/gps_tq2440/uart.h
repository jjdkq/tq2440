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

//#define UART_NAME   "/dev/ttySAC1"           //tq2440的串口1
#define UART_NAME   "/dev/tq2440_serial1"           //tq2440的串口1

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned u32;

#define USART1_MAX_SEND_LEN		800					//最大发送缓存字节数
#define USART1_MAX_RECV_LEN		800					//最大接收缓存字节数
extern u8 USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//接收缓冲,最大USART1_MAX_RECV_LEN字节
extern u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//发送缓冲,最大USART1_MAX_SEND_LEN字节


void uart1_printf(char* fmt,...);
void uart1_read(u8 *, u32);
void uart1_write(u8 *, u32);
int uart1_init(char *, u32);

#endif // UART_H


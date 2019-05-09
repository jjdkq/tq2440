#include "common.h"

//连接端口号:8086
const u8* portnum="8086";

//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
const u8* wifista_ssid="ALIENTEK";			//路由器SSID号
const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifista_password="15902020353"; 	//连接密码

//WIFI AP模式,模块对外的无线参数,可自行修改.
const u8* wifiap_ssid="ESP8266";			//对外SSID号
const u8* wifiap_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifiap_password="12345678"; 		//连接密码

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//4个网络模式
const u8 *ATK_ESP8266_CWMODE_TBL[3]={"STA模式 ","AP模式 ","AP&STA模式 "};	//ATK-ESP8266,3种网络模式,默认为路由器(ROUTER)模式
//4种工作模式
const u8 *ATK_ESP8266_WORKMODE_TBL[3]={"TCP服务器","TCP客户端"," UDP 模式"};	//ATK-ESP8266,4种工作模式
//5种加密方式
const u8 *ATK_ESP8266_ECN_TBL[5]={"OPEN","WEP","WPA_PSK","WPA2_PSK","WPA_WAP2_PSK"};
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void delay_ms(u32 ms)
{
    int i;
    for (i = 0; i < 1000; i++)
        usleep(ms);
}

//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* atk_8266_check_cmd(u8 *str)
{
    char *strx = 0;
    uart1_read(USART3_RX_BUF, USART3_MAX_RECV_LEN);
    strx = strstr((const char*)USART3_RX_BUF,(const char*)str);
    return (u8*)strx;
}
//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
    u8 res=0;
    uart1_printf("%s\r\n",cmd);	//发送命令
    if(ack&&waittime)		//需要等待应答
    {
        while(--waittime)	//等待倒计时
        {
            delay_ms(10);
            if(atk_8266_check_cmd(ack))
            {
                break;//得到有效数据
            }
        }
        if(waittime==0)
            res=1;
    }
    return res;
}
//向ATK-ESP8266发送指定数据
//data:发送的数据(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)luojian
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
    u8 res=0;
    uart1_printf("%s",data);	//发送命令
    if(ack&&waittime)		//需要等待应答
    {
        while(--waittime)	//等待倒计时
        {
            delay_ms(10);
            if(atk_8266_check_cmd(ack))
                break;//得到有效数据

        }
        if(waittime==0)
            res=1;
    }
    return res;
}
//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
u8 atk_8266_quit_trans(void)
{
    char c = '+';
    int i;
    for (i = 0; i < 3; i++)
    {
        uart1_write(&c, 1);
        delay_ms(i == 2? 500 : 15);
    }
    return atk_8266_send_cmd("AT","OK",20);//退出透传判断.
}
//获取ATK-ESP8266模块的AP+STA连接状态
//返回值:0，未连接;1,连接成功
u8 atk_8266_apsta_check(void)
{
    if(atk_8266_quit_trans())return 0;			//退出透传
    atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//发送AT+CIPSTATUS指令,查询连接状态
    if(atk_8266_check_cmd("+CIPSTATUS:0")&&
         atk_8266_check_cmd("+CIPSTATUS:1")&&
         atk_8266_check_cmd("+CIPSTATUS:2")&&
         atk_8266_check_cmd("+CIPSTATUS:4"))
        return 0;
    else return 1;
}
//获取ATK-ESP8266模块的连接状态
//返回值:0,未连接;1,连接成功.
u8 atk_8266_consta_check(void)
{
    u8 *p;
    u8 res;
    if(atk_8266_quit_trans())return 0;			//退出透传
    atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//发送AT+CIPSTATUS指令,查询连接状态
    p=atk_8266_check_cmd("+CIPSTATUS:");
    res=*p;									//得到连接状态
    return res;
}

//ESP8266模块测试主函数
void atk_8266_test(void)
{
    while(atk_8266_send_cmd("AT","OK",20))			//检查WIFI模块是否在线
    {
        atk_8266_quit_trans();						//退出透传
        atk_8266_send_cmd("AT+CIPMODE=0","OK",200); //关闭透传模式
        delay_ms(800);
    }
    while(atk_8266_send_cmd("ATE0","OK",20));		//关闭回显
    while(1)
    {
        delay_ms(10);
        atk_8266_wifiap_test();				//WIFI AP测试
    }
}


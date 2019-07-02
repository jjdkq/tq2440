#include "common.h"
//网络模式,0为TCP Server；1为TCP Client；2为UDP
#define NETMOD 2

u8 USART1_TX_BUF[USART3_MAX_RECV_LEN];
u8 atk_8266_wifiap_test(void)
{
    u8 i;
    u8 ipbuf[16]="192.168.4.2"; 	//IP地址
    u8 *p;
    u16 t=0;
    u8 res=0;
    u16 rlen=0;
    p = malloc(32);
    atk_8266_send_cmd("AT+CWMODE=2","OK",20);	//TCP AP模式
    sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password);  //配置模块AP模式无线参数
    atk_8266_send_cmd(p,"OK",1000);
//	PRESTA:
//	NETMOD=atk_8266_netpro_sel(50,30,(u8*)ATK_ESP8266_CWMODE_TBL[1]);	//选择网络模式
    if(NETMOD&0X02)   //UDP
    {
        sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标UDP服务器
        atk_8266_send_cmd("AT+CIPMUX=0","OK",100); 								 //单链接模式
        atk_8266_send_cmd(p,"OK",500);

        atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传
    }
    else if(NETMOD&0X01)    					 //TCP Client透传模式测试
    {
        atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   							//0：单连接，1：多连接
        sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);   //配置目标TCP服务器
//		if(atk_8266_ip_set("WIFI-AP 远端IP设置",(u8*)ATK_ESP8266_WORKMODE_TBL[NETMOD],(u8*)portnum,ipbuf))
//			goto PRESTA;
        while(atk_8266_send_cmd(p,"OK",200))
            printf("send cmd fail\r\n");
        atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传
    }
    else		//TCP Server
    {
        atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0：单连接，1：多连接
        sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);
        atk_8266_send_cmd(p,"OK",20);     			//开启Server模式，端口号为8086
    }
  //  USART3_RX_STA=0;
    while(1)
    {
        if(NETMOD==2)   //UDP
        {
            atk_8266_quit_trans();
            atk_8266_send_cmd("AT+CIPSEND","OK",20);       //开始透传

//			sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[NETMOD],t);//测试数据
//			atk_8266_send_cmd("AT+CIPSEND=25","OK",200);  //发送指定长度的数据
            sprintf((char*)p,"hello:%d",t);
//			delay_ms(200);
            atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
            delay_ms(2000);
        }
        else if((NETMOD==1))   //TCP Client
        {
            atk_8266_quit_trans();
            atk_8266_send_cmd("AT+CIPSEND","OK",20);       //开始透传
//			sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[NETMOD],t/10);//测试数据
            sprintf((char*)p,"Client测试:%d",t);
//			uart1_printf("%s",p);
//			atk_8266_send_cmd("AT+CIPSEND=0,25","OK",200);  //发送指定长度的数据
//			delay_ms(200);
            atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
            delay_ms(2000);
        }
        else    //TCP Server
        {
//			sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[NETMOD],t/10);//测试数据
            sprintf((char*)p,"Server测试:%d",t);//测试数据
//			printf("长度为：%d,t=%d\r\n",strlen(p),t);
            atk_8266_send_cmd("AT+CIPSEND=0,12","OK",200);  //发送指定长度的数据
            delay_ms(200);
            atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
            delay_ms(2000);
        }
        t++;
    //     uart1_read(USART3_RX_BUF, USART3_MAX_RECV_LEN);
    //    printf("Recive data: %s\n", USART3_RX_BUF);
    //	atk_8266_at_response(1);
    }
    return res;
}


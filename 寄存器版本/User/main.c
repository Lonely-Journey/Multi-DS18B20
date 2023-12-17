#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "ds18b20.h"

int main(void)
{
//    uint8_t arr[8]={0};

    uint8_t buff[MAXNUM][8];
    uint8_t num=0;
    uint8_t n=0, i=0;

    sys_stm32_clock_init(9);  /* ϵͳʱ�ӳ�ʼ��*/
    SysTick_Init();           /* ��ʱ��ʼ�� */
    USART1_Init(115200);      /* ���ڳ�ʼ 115200 */
    DS18B20_Init();           /* DS18B20��ʼ�� */

    /* ��0x33ָ���ȡ����DS18B20�ڲ�ROM���� */
//    DS18B20_SingleAddr(arr);
    /* ��0xf0ָ���ȡ����DS18B20�ڲ�ROM���� */
//    DS18B20_MultiAddr_Test();

    /* ���������϶��DS18B20 */
    DS18B20_MultiAddr(buff, &num);
    printf("num:%d\r\n",num);
    for(n=0;n<num;n++)
    {
        printf("����ϵ�б��룺%d\r\n",buff[n][0]);
        printf("��ַ��");
        for(i=1;i<7;i++)
        printf("%d ",buff[n][i]);
        printf("\r\n");
        printf("CRCУ���룺%d\r\n",buff[n][7]);
    }

    delay_ms(1000);

    DS18B20_Addr_Temperature(buff[0]);

    while(1)
    {
    }
}





#include "DS18B20.h"
#include "delay.h"



/**
******************************************************************************
* @file      ��.\Drivers\BSP\src\DS18B20.c
*              .\Drivers\BSP\inc\DS18B20.h
* @author    ��XRbin
* @version   ��V1.0
* @date      ��2023-06-16
* @brief     ��DS18B20�������ô���
******************************************************************************
* @attention
*   �ҵ�GitHub   ��https://github.com/XR-bin
*   �ҵ�gitee    ��https://gitee.com/xrbin
*   �ҵ�leetcode ��https://leetcode.cn/u/xrbin/
******************************************************************************
*/



/**********************************************************
* @funcName ��DS18B20_Init
* @brief    ����DS18B20��Ӧ��GPIO�ڽ��г�ʼ������
* @param    ��void
* @retval   ��void
* @details  ��
*            DQ  ---  PG11
* @fn       ��
************************************************************/
void DS18B20_Init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    DQ_GPIO_CLK_ENABLE();                                 /* DS18B20�������ʱ��ʹ�� */
    
    /* DQ�������� */
    gpio_init_struct.Pin = DQ_GPIO_PIN;                   /* DQ���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;          /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                  /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;        /* ���� */
    HAL_GPIO_Init(DQ_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��DQ���� */

    DQ_L;             /* DQ������(������Э���е����ߵ͵�ƽʱΪ����״̬) */
    
    DS18B20_RST();
}



/**********************************************************
* @funcName ��DS18B20_RST
* @brief    ����DS18B20�ĸ�λ�ź�
* @param    ��void
* @retval   ��void
* @details  ��
* @fn       ��
************************************************************/
void DS18B20_RST(void)
{
    DQ_OUT;           /* DQ�������ģʽ */
    DQ_L;             /* ��DQ��ƽ���� */
    delay_us(700);    /* ����480~960us */
    DQ_H;             /* �ͷ�DQ���ָ����� */
    delay_us(15);
    DS18B20_ACK();    /* �ȴ�Ӧ�� */
}



/**********************************************************
* @funcName ��DS18B20_ACK
* @brief    ��DS18B20��Ӧ���ź�
* @param    ��void
* @retval   ��uint8_t
* @details  ��
* @fn       ��
************************************************************/
uint8_t DS18B20_ACK(void)
{
    uint8_t ack = 0;
    uint8_t temp = 60;

    DQ_IN;/* DQ��������ģʽ */
    
    /* DS18B20��ȴ�15~60us�����Ƭ��Ӧ�� */
    while(DQ_ACK)
    {
        temp--;
        if(temp == 60) return ack;
        delay_us(1);
    }
    
    temp = 240;
    
    /* DS18B20�ὫDQ����60~240us����ΪӦ�� */
    while(temp--)
    {
        if(DQ_ACK)
        {
            ack = 1;
            break;
        }
        delay_us(1);
    }

    return ack;
}



/**********************************************************
* @funcName ��DS18B20_Write_Byte
* @brief    ����DS18B20����дһ���ֽ�����
* @param    ��uint8_t data
* @retval   ��void
* @details  ��
* @fn       ��
************************************************************/
void DS18B20_Write_Byte(uint8_t data)
{
    uint8_t i;

    /* DQ�������ģʽ */
    DQ_OUT;
    
    for(i=0;i<8;i++)
    {
        DQ_L;                    /* ����DQ������10~15us */
        delay_us(12);

        if(data & 0x01) DQ_H;    /* �жϵ�ǰҪ���͵��������Ϊ�� 1 ���� 0 */
        else DQ_L;

        delay_us(55);            /* ���ֵ�ǰDQ��ƽ����45us */

        DQ_H;                    /* DQ�ͷţ�����1us */
        delay_us(1);
        
        data >>= 1;              /* Ҫ���͵���������һλ */
    }
}



/**********************************************************
* @funcName ��DS18B20_Read_Bit
* @brief    ����DS18B20���ж�1��bit(λ)����
* @param    ��void
* @retval   ��uint8_t
* @details  ��
* @fn       ��
************************************************************/
uint8_t DS18B20_Read_Bit(void)
{
    uint8_t data;
    
    DQ_OUT;              /* DQ�������ģʽ */
    
    DQ_L; 
    delay_us(2);
    DQ_H;
    
    DQ_IN;               /* DQ��������ģʽ */
    delay_us(12);
    
    if(DQ_READ)data=1;   /* ��ȡһ��bit������ */
    else data=0;
    
    delay_us(50);
    
    return data;
}



/**********************************************************
* @funcName ��DS18B20_Read_Byte
* @brief    ����DS18B20���ж�һ���ֽ�����
* @param    ��void
* @retval   ��uint8_t
* @details  ��
* @fn       ��
************************************************************/
uint8_t DS18B20_Read_Byte(void)
{
    uint8_t i;
    uint8_t data = 0;

    for(i=0;i<8;i++)
    {
        DQ_OUT;                     /* DQ�������ģʽ */
        
        DQ_L;                       /* ����DQ����1us */
        delay_us(1);

        DQ_H;                       /* ����DQ����13us */
        delay_us(12);

        DQ_IN;                      /* DQ��������ģʽ */

        if(DQ_READ) data |= (1<<i); /* ��ȡ���� */
        delay_us(50);               /* ��ʱ45us */

        DQ_H;                       /* DQ�ͷţ�����1us */
        delay_us(2);
    }
    
    return data;
}



/**********************************************************
* @funcName ��DS18B20_Read_Temperature
* @brief    �����DS18B20�������¶�
* @param    ��void
* @retval   ��int16_t
* @details  ��
*            0xcc --- ����ROM���ָ��
*            0x44 --- �ɼ��¶�ָ��
**           0xbe --- ��ȡ�¶�����ָ��
* @fn       ��
*            ע�⣺�������ֻ������������ֻ��һ��DS18B20
************************************************************/
int16_t DS18B20_Read_Temperature(void)
{
    uint8_t data_l ,data_h;
    int16_t data;

    DS18B20_RST();              /* ��λ */
    DS18B20_Write_Byte(0xcc);   /* ����ROM���ָ�������Ҫȷ��DS18B20��ַ */
    DS18B20_Write_Byte(0x44);   /* �����¶�����ת�� */
    DS18B20_RST();
    DS18B20_Write_Byte(0xcc);
    DS18B20_Write_Byte(0xbe);     /* ��ȡ�¶����� */
    data_l = DS18B20_Read_Byte(); /* ��ȡ�¶����ݵ�λ */
    data_h = DS18B20_Read_Byte(); /* ��ȡ�¶����ݸ�λ */

    /* �����¶����� */
    if(data_h > 0x07)
    {
        data_l = ~data_l;
        data_h = ~data_h;
    }
    data = (data_h<<8)+data_l;
    data *= 0.625;

    printf("%f\r\n",(float)data/10);
    
    return data;
}



/**********************************************************
* @funcName ��DS18B20_SingleAddr
* @brief    �����DS18B20��ϵ�кš�������ַ��CRCУ����
* @param    ��uint8_t* arr
* @retval   ��void
* @details  ��
*            0x33 --- ��ȡDS18B20�ڲ���ROM
* @fn       ��
*            DS18B20�ڲ���ROM�����DS18B20��ϵ�кš�������ַ��CRCУ����
*            ע�⣺�������ֻ������������ֻ��һ��DS18B20
************************************************************/
void DS18B20_SingleAddr(uint8_t* arr)
{
    uint8_t i;

    DS18B20_RST();
    DS18B20_Write_Byte(0x33);
    
    for(i=0;i<8;i++)
    arr[i] = DS18B20_Read_Byte();

    printf("����ϵ�б��룺%d\r\n",arr[0]);
    printf("��ַ��");
    for(i=1;i<7;i++) printf("%d ",arr[i]);
    
    printf("\r\n");
    printf("CRCУ���룺%d\r\n",arr[7]);
}



/**********************************************************
* @funcName ��DS18B20_Read2Bit
* @brief    ����DS18B20���ж�����bit(λ)����
* @param    ��void
* @retval   ��uint8_t
* @details  ��
* @fn       ��
************************************************************/
uint8_t DS18B20_Read2Bit(void)
{  
    uint8_t i;
    uint8_t data = 0;

    for(i=0;i<2;i++)
    {
        data <<= 1;                    /* ����һλ */
        
        DQ_OUT;                        /* DQ�������ģʽ */
        
        DQ_L;                          /* ����DQ����1us */
        delay_us(1);
        DQ_H;                          /* ����DQ����13us */
        delay_us(12);
        
        DQ_IN;                         /* DQ��������ģʽ */

        if(DQ_READ) data |= 0x01;      /* ��ȡ���� ����45us */
        delay_us(50);

        DQ_H;                          /* DQ�ͷţ�����1us */
        delay_us(2);
    }
    
    return data;
}



/**********************************************************
* @funcName ��DS18B20_WriteBit
* @brief    ����DS18B20����дһ��bit(λ)����
* @param    ��uint8_t bit
* @retval   ��void
* @details  ��
* @fn       ��
************************************************************/
void DS18B20_WriteBit(uint8_t bit)
{
    DQ_OUT;                  /*DQ�������ģʽ*/

    DQ_L;                    /* ����DQ������10~15us */
    delay_us(12);

    if(bit & 0x01) DQ_H;     /* �жϵ�ǰҪ���͵��������Ϊ�� 1 ���� 0 */
    else DQ_L;

    delay_us(55);            /* ���ֵ�ǰDQ��ƽ����45us */

    DQ_H;                    /* DQ�ͷţ�����1us */
    delay_us(1);
}



/**********************************************************
* @funcName ��DS18B20_MultiAddr_Test  (ʵ��棬��ʽ��������һ������) 
* @brief    ������һ��������DS18B20�ĸ���������ȡ���ǵ�ROM����(ϵ�кš�������ַ��CRCУ����)
* @param    ��void
* @retval   ��void
* @details  ��
*            0x33 --- ��ȡDS18B20�ڲ���ROM
*            0xf0 --- ����һ������������DS18B20�ڲ���ROM
* @fn       ��
*            �������ֻ������������˼·��ֻ�ⵥ��������0xf0ָ���0x33ָ��
*        �õ���ROM�����Ƿ�һ�¡�
*            ע�⣺�˺���˼·�Ƚϼ򵥣������ȿ��˺����˽�˼·�ٿ���һ����ʽ
*        �溯������ʽ��ĺ���˼·��˺����������ơ�
*
*            ע�⣺�������ֻ������������ֻ��һ��DS18B20
************************************************************/
void DS18B20_MultiAddr_Test(void)
{
    uint8_t i,j;
    uint8_t data;
    uint8_t buff[8];
    uint8_t bit8 = 0;

    DS18B20_RST();                    /* ��λDS18B20���� */
    DS18B20_Write_Byte(0xf0);         /* ����ROM */
    for(i=0;i<8;i++)
    {
        for(j=0;j<8;j++)
        {
            bit8 >>= 1;
            data = DS18B20_Read2Bit();
            data &= 0x3;
            /**************************************************************************
            *data��ֵ����ǣ�
            *   01 ������ROM����ָ����Ƶ�����DS18B20�ĸ�λ����0
            *   10 ������ROM����ָ����Ƶ�����DS18B20�ĸ�λ����1
            *   00 ������ROM����ָ����Ƶ�����DS18B20�ĸ�λ�е���0�е���1������λλ��ͻλ
            *   11 ��������û��DS18B20
            ***************************************************************************/
            if(data == 0x01)           /* ��λ��λ0 */
            {
                DS18B20_WriteBit(0);  /* �������Ӵ�λΪ0�ĵ�ַ */
            }
            else if(data == 0x02)
            {
                bit8 |= 0x80;
                DS18B20_WriteBit(1);  /* �������Ӵ�λΪ1�ĵ�ַ */
            }
        }
        buff[i] = bit8;
        bit8 = 0;
    }

    printf("����ϵ�б��룺%d\r\n",buff[0]);
    printf("��ַ��");
    for(i=1;i<7;i++)
    printf("%d ",buff[i]);
    printf("\r\n");
    printf("CRCУ���룺%d\r\n",buff[7]);
}




/**********************************************************
* @funcName ��DS18B20_MultiAddr  (��ʽ��ROM������������) 
* @brief    ������һ��������DS18B20�ĸ���������ȡ���ǵ�ROM����(ϵ�кš�������ַ��CRCУ����)
* @param    ��uint8_t (*buff)[8], uint8_t *num
* @retval   ��void
* @details  ��
*            0xf0 --- ����һ������������DS18B20�ڲ���ROM
* @fn       ��
************************************************************/
void DS18B20_MultiAddr(uint8_t (*buff)[8], uint8_t *num)
{
    uint8_t i,j;
    uint8_t data;             /* ��������DS18B20��������λ���� */
    uint8_t bit8 = 0;         /* ������ʱװ�ڶ���forѭ����ȡ����8��bit��ROM����(һ���ֽ�����) */
    uint8_t temp[64] = {0};   /* ���յ�ǰ������DS18B20��ROM���ݣ�����һ�������ṩ����(��Ҫ�ô���������һ��������ͻλ��ɸѡ) */
    uint8_t conflict;         /* ������ͻ��λ */
    uint8_t stack[65] = {0};  /* ջ�ṹ   ���ڱ�ǳ�ͻλ*/
    uint8_t top = 0;          /* ջ��     ���ڱ�����λ�ĳ�ͻλ(ROM����һ����64������λ,ĩβ��8λ������CRC��ǰ��һ����CRC����һ�������Գ�ͻλ�������������ĩβ�ĺ�8λ) */
    *num = 0;                 /* ��ʼ�����num����Ȼ�������û�ж���ʱ��ֵ0�Ļ�����ִ��� */

    do
    {
        DS18B20_RST();              /* ��λDS18B20���� */
        DS18B20_Write_Byte(0xf0);   /* ��������ROM */
        
        for(i=0;i<8;i++)
        {
            for(j=0;j<8;j++)
            {
                /* ��Ϊ�������Ǵӵ�λ��ʼ�� */
                bit8 >>= 1; 
                /* ��ȡ��λ���� */
                data = DS18B20_Read2Bit();
                /* ��Ȼ��һ���е���࣬��������ʽ��дһ�� */
                data &= 0x3;
                /**************************************************************************
                *data��ֵ����ǣ�
                *   01 ������ROM����ָ����Ƶ�����DS18B20�ĸ�λ����0
                *   10 ������ROM����ָ����Ƶ�����DS18B20�ĸ�λ����1
                *   00 ������ROM����ָ����Ƶ�����DS18B20�ĸ�λ�е���0�е���1������λλ��ͻλ
                *   11 ��������û��DS18B20
                ***************************************************************************/
                if(data == 0x01)             /* ����ROM����ָ����Ƶ�����DS18B20�ĸ�λ����0 */
                {
                    temp[i*8+j] = 0;         /* ��¼��λΪ0 */
                    DS18B20_WriteBit(0);     /* ɸѡ��λ��0��DS18B20(�����λ����0�Ͳ�����ROM����ָ�����) */
                }
                else if(data == 0x02)        /* ����ROM����ָ����Ƶ�����DS18B20�ĸ�λ����1 */
                {
                    temp[i*8+j] = 1;         /* ��¼��λΪ1 */
                    bit8 |= 0x80;            /* ��¼��λΪ1 */
                    DS18B20_WriteBit(1);     /* ɸѡ��λ��1��DS18B20(�����λ����1�Ͳ�����ROM����ָ�����) */
                }
                else if(data == 0x00)        /* ����ROM����ָ����Ƶ�����DS18B20�ĸ�λ�е���0�е���1������λλ��ͻλ */
                {
                    conflict = i*8 + j +1;   /* �ڼ�λ������ͻ(ROM����һ����64������λ) */

                    /* ���conflict > stack[top] ˵�������µĸ��ߵĳ�ͻλ */
                    if(conflict > stack[top])
                    {
                        /* ��¼��ͻλ */
                        top++;                  /* ��top�γ�ͻλ */
                        stack[top] = conflict;  /* ��¼�ڼ�λ������ͻ */
                        /* �����Ȱ�0���������� */
                        /* ɸѡ��λΪ0��DS18B20(����0�ľͲ�����ROM����ָ�����) */
                        DS18B20_WriteBit(0);
                        temp[i*8+j] = 0;
                    }
                    /* ���������ж�ʱtop��Ȼ������0�ˣ������Ѿ��ǵ�2�������ϵ�ROM������ */
                    /* ����ó�ͻλ������߳�ͻλ(����˵������ֵ����)�ͽ���if��ĳ��� */
                    else if(conflict < stack[top])
                    {
                        /* ��Ϊ������ߵĳ�ͻλ�����Ը�λ����ֵ��Ȼ����һ����������ROM���ݸ�λ��ֵһ�� */
                        bit8 = bit8 | ((temp[(i*8+j)]&0x01)<<7);
                        /* ����һ��ROM��������λʱ��ɸѡһ�� */
                        DS18B20_WriteBit (temp[i*8+j]);
                    }
                    /* ������ж�Ҳһ�������������ж�ʱtop��Ȼ������0�ˣ������Ѿ��ǵ�2�������ϵ�ROM������ */
                    /* ����ó�ͻλ����߳�ͻλ(����˵����ֵ����)�ͽ���if��ĳ��� */
                    else if(conflict == stack[top])
                    {
                        /**
                        * ��Ϊ���������Ȱ���߳�ͻΪΪ0��DS18B20��ROM���ݶ�������
                        * ������θ�����߳�ͻΪΪ1��DS18B20��
                        */
                        /* ��¼��λ��ε�DS18B20��ROM���� */
                        bit8 = bit8 | 0x80; 
                        temp[i*8+j]=1;
                        /* ɸѡ��λΪ1��DS18B20(����1�ľͲ�����ROM����ָ�����) */
                        DS18B20_WriteBit (1);  
                        /* ����������߳�ͻλ����߳�ͻλ��Ҫ������ */
                        stack[top] = 0;
                        top = top - 1;
                    }
                }
                else
                {
                    printf("������û��DS18B20����\r\n");
                    *num=0;
                    return;
                }
            }
            /* ��¼ÿ��DS18B20��ROM���� */
            buff[*num][i] = bit8;
            bit8 = 0;
        }
        /* ��¼���� */
        (*num)++;
    }while((stack[top]!=0) && (*num<=MAXNUM));
}



/**********************************************************
* @funcName ��DS18B20_Addr_Temperature
* @brief    ��ͨ����ַ���ĳ��DS18B20�������¶�
* @param    ��void
* @retval   ��int16_t
* @details  ��
*            0x55 --- ѫ��IDָ��
*            0x44 --- �ɼ��¶�ָ��
*            0xbe --- ��ȡ�¶�����ָ��
* @fn       ��
*            ע�⣺�������ֻ������������ֻ��һ��DS18B20
************************************************************/
int16_t DS18B20_Addr_Temperature(uint8_t* address)
{
    uint8_t data_l ,data_h;
    int16_t data;
    uint8_t i;

    DS18B20_RST();              /* ��λ */
    DS18B20_Write_Byte(0x55);   /* Ѱ��ID���� */
    for(i=0; i<8; i++)
    {
        DS18B20_Write_Byte(address[i]);
    }
    DS18B20_Write_Byte(0x44);   /* �����¶�����ת�� */
    
    DS18B20_RST();
    DS18B20_Write_Byte(0x55);
    for(i=0; i<8; i++)
    {
        DS18B20_Write_Byte(address[i]);
    }
    DS18B20_Write_Byte(0xbe);     /* ��ȡ�¶����� */
    
    data_l = DS18B20_Read_Byte(); /* ��ȡ�¶����ݵ�λ */
    data_h = DS18B20_Read_Byte(); /* ��ȡ�¶����ݸ�λ */

    /* �����¶����� */
    if(data_h > 0x07)
    {
        data_l = ~data_l;
        data_h = ~data_h;
    }
    data = (data_h<<8)+data_l;
    data *= 0.625;

    printf("�¶�:%f\r\n",(float)data/10);
    
    return data;
}



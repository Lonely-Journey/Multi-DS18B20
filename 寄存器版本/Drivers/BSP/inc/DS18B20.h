#ifndef __DS18B20_H
#define __DS18B20_H

    /****************   �ⲿͷ�ļ�����   ****************/
    #include "sys.h"
    #include "delay.h"
    #include "stdio.h"



    /********************   �궨��   ********************/
    /* DQ�궨�� */
    #define DQ_H     GPIOG->ODR |= (1<<11)                          /* DQ������ */
    #define DQ_L     GPIOG->ODR &= ~(1<<11)                         /* DQ������ */
    #define DQ_IN   {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}     /* �л�Ϊ����ģʽ */
    #define DQ_OUT  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}     /* �л�Ϊ���ģʽ */
    #define DQ_READ  GPIOG->IDR & (1<<11)                           /* ������ */
    #define DQ_ACK   GPIOG->IDR & (1<<11)                           /* �ж�Ӧ�� */

    /* ����ܹ���DS18B20�ĸ��� */
    #define MAXNUM     4



    /****************    �����ⲿ����   *****************/
    void DS18B20_Init(void);                   /* DS18B20��ʼ�� */
    void DS18B20_RST(void);                    /* ��λ */
    uint8_t DS18B20_ACK(void);                 /* �ȴ�Ӧ�� */
    void DS18B20_Write_Byte(uint8_t data);     /* ����1���ֽ����� */
    void DS18B20_WriteBit(uint8_t bit);        /* ����1��bit���� */
    uint8_t DS18B20_Read_Bit(void);            /* ��ȡ1��bit���� */
    uint8_t DS18B20_Read2Bit(void);            /* ��ȡ2��bit���� */
    uint8_t DS18B20_Read_Byte(void);           /* ��1���ֽ����� */

    int16_t DS18B20_Read_Temperature(void);    /* ��ȡ�¶�   ��Ѱַ   ����DS18B20 */
    void DS18B20_SingleAddr(uint8_t* arr);     /* ��ȡ��ȡROM����(������Ϣ) ����DS18B20 */

    void DS18B20_MultiAddr_Test(void);         /* ����������DS18B20����ȡROM����(������Ϣ)  ���԰汾 ֻ������1�� */
    void DS18B20_MultiAddr(uint8_t (*buff)[8], uint8_t *num);  /* ����������DS18B20����ȡ���ǵ�ROM����(������Ϣ)������¼����  ��ʽ�汾 */
    int16_t DS18B20_Addr_Temperature(uint8_t* address);        /* ͨ��DS18B20������ַ��ȡĳ��DS18B20���¶� */

#endif



#include <stdio.h>
#include <string.h>
#include "stm32f4xx_conf.h"
#include <stm32f4xx_usart.h>
#include <stm32f4xx_adc.h>
#include <misc.h>


char str[64];
volatile short MyDelay;

void init_usart(void);
void USART_puts(USART_TypeDef* USARTx, volatile char *s);


void initGPIO(); //4 adet led sonsuz dongude yanip sonecek


void adc_config(void);
unsigned short ADCConvertedValue[2]; //ADC olcumlerinin ham verileri bu dizide tutulacak


int main(void)
{
    SystemInit();

    SysTick_Config(SystemCoreClock / 1000); //1milisaniyelik system clock elde edelim


    RCC_ClocksTypeDef RCC_ClockFreq;
    RCC_GetClocksFreq(&RCC_ClockFreq);

    init_usart();
    initGPIO();


    USART_puts(USART2, "start\r\n");


    adc_config();


    while(1)
    {
        int eksenX;
        int eksenY;

        MyDelay=100;
        while(MyDelay);


        eksenX = (int)((double)((double)200/0xFFF) * ADCConvertedValue[0])-100; //12bit adc
        eksenY = (int)((double)((double)200/0xFFF) * ADCConvertedValue[1])-100; //100 uzerinden gosteriyoruz, voltaj degeri olarak gostermek icin 100 yerine 3000 yaz


        if(eksenX>-10 && eksenX<10)eksenX=0; //-10 ila 10 arasini olu bolge kabul et ve 0 olarak degistir
        if(eksenY>-15 && eksenY<15)eksenY=0; //y ejseninde mekanik bir sorun oldugu icin olu bolge biraz fazla


        sprintf(str, "eksenX: %d \t eksenY: %d \r\n", eksenX,eksenY);
        USART_puts(USART2, str);

        GPIO_ToggleBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

    }
}


void init_usart(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    /* enable peripheral clock for USART2 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    /* GPIOA clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    /* GPIOA Configuration: USART2 TX on PA2 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Connect USART2 pins to AF2 */
// TX = PA2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE); // enable USART2
}

void USART_puts(USART_TypeDef* USARTx, volatile char *s)
{
    while (*s)
    {
// wait until data register is empty
        while (!(USARTx->SR & 0x00000040))
            ;
        USART_SendData(USARTx, *s);
        *s++;
    }
}

void SysTick_Handler(void)
{
    if (MyDelay)
    {
        MyDelay--;
    }
}






void initGPIO()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* GPIOD Periph clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void adc_config(void) // https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/Flat.aspx?RootFolder=https%3a%2f%2fmy.st.com%2fpublic%2fSTe2ecommunities%2fmcu%2fLists%2fcortex_mx_stm32%2fSTM32F4%20ADC%202channels%20%2b%20DMA&FolderCTID=0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B&currentviews=1337
{
    GPIO_InitTypeDef      GPIO_InitStructure;
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DMA_InitTypeDef       DMA_InitStructure;

    GPIO_StructInit(&GPIO_InitStructure);
    ADC_StructInit(&ADC_InitStructure);
    ADC_CommonStructInit(&ADC_CommonInitStructure);
    DMA_StructInit(&DMA_InitStructure);

    /**
      Set up the clocks are needed for the ADC
    */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);

    /**
      Initialization of the GPIO Pins [OK]
    */

    /* Analog channel configuration : PC.01, 02*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /**
      Configure the DMA
    */
    //==Configure DMA2 - Stream 4
    DMA_DeInit(DMA2_Stream4);  //Set DMA registers to default values
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; //Source address
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCConvertedValue[0]; //Destination address
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 2; //Buffer size
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //source size - 16bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // destination size = 16b
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream4, &DMA_InitStructure); //Initialize the DMA
    DMA_Cmd(DMA2_Stream4, ENABLE); //Enable the DMA2 - Stream 4

    /**
      Config the ADC1
    */
    ADC_DeInit();
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //continuous conversion
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_NbrOfConversion = 2;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; // 1=scan more that one channel in group
    ADC_Init(ADC1,&ADC_InitStructure);

    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInit(&ADC_CommonInitStructure);

    ADC_RegularChannelConfig(ADC1,ADC_Channel_11,1,ADC_SampleTime_480Cycles);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_12,2,ADC_SampleTime_480Cycles);

    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

    ADC_DMACmd(ADC1, ENABLE); //Enable ADC1 DMA

    ADC_Cmd(ADC1, ENABLE);   // Enable ADC1

    ADC_SoftwareStartConv(ADC1); // Start ADC1 conversion

}

/*


                         \\\|///
                       \\  - -  //
                        (  @ @  )
+---------------------oOOo-(_)-oOOo-------------------------+
|                										    |
|           	     	     								|
|            		 2014   |   STMF407         			|
|              	   JOYSTICK ORNEGI(2 KANAL ADC)             |
|                      Gokhan BEKEN             			|
|                              							    |
|                                                           |
|                             Oooo                          |
+-----------------------oooO--(   )-------------------------+
                       (   )   ) /
                        \ (   (_/
                         \_)

notlar:
stm32f4xx.h dosyadında yer alan
#define HSE_VALUE    ((uint32_t)25000000)
25000000'i 8000000 olarak degistir

system_stm32f4xx.c dosyadında yer alan
PLL_M 25
kismini bul PLL_M 8 olarak degistir

ADC pinleri: PC1 ve PC2
Seriport TX'pini: PA2
Seriport baudrate: 115200
*/

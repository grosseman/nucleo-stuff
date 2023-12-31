/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2s.h"
#include "lwip.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern unsigned char* _binary_klassiskt_wav_start;
const int headerlen = 44;
typedef struct _wavefile {
  unsigned char ckid_riff[4];
  unsigned int cksize;
  unsigned char ckid_wave[4];
  unsigned char ckid_fmt[4];
  unsigned int samplesize;
  unsigned short wFormatTag;
  unsigned short nChannels;
  unsigned int nSamplesPerSec;
  unsigned int nAvgBytesPerSec;
  unsigned short nBlockAlign;
  unsigned short wBitsPerSample;
  unsigned char ckid_data[4];
  unsigned int datasize;
} WAVEFILE;
uint8_t foo[] = "TxHalfCpltCallback0\r\n";
uint8_t bar[] = "TxCpltCallback\r\n";
uint8_t barz[] = "TxCpltCallFUCK\r\n";
#define BUFSIZE 65534
#define HBUFSIZE (BUFSIZE/2)
uint16_t buffer[BUFSIZE];
uint16_t* buf_ptr;
int cnt = 0;
  WAVEFILE* wav_file = (WAVEFILE*)(&_binary_klassiskt_wav_start);
  uint16_t* wav_data_start = (uint16_t*)((uint32_t)(&_binary_klassiskt_wav_start) + headerlen);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void udp_recv_fn_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {

  HAL_UART_Transmit_IT(&huart3, p->payload, p->len);
  udp_send(pcb, p);
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
  //foo[18] += 1;  
  //HAL_UART_Transmit_IT(&huart3, barz, 20);
  //foo[18] = (foo[18] == 0x39) ? 0x30 : foo[18];  

  // do buffer shit
  memcpy(buffer, buf_ptr, HBUFSIZE*sizeof(buffer[0]));
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
  //HAL_UART_Transmit_IT(&huart3, bar, 17);
  memcpy((uint16_t*)(buffer+HBUFSIZE), (uint16_t*)(buf_ptr+HBUFSIZE), HBUFSIZE*sizeof(buffer[0]));
  buf_ptr += BUFSIZE;
  if (buf_ptr-wav_data_start > wav_file->datasize/2) {
    buf_ptr = wav_data_start;
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_LWIP_Init();
  MX_I2S2_Init();
  /* USER CODE BEGIN 2 */
  //HAL_UART_MspInit(&huart3);
  HAL_I2S_MspInit(&hi2s2);

  memcpy(buffer, wav_data_start, BUFSIZE);
  HAL_I2S_Transmit_DMA(&hi2s2, buffer, BUFSIZE);
  buf_ptr = (uint16_t*)(wav_data_start+BUFSIZE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {    
    MX_LWIP_Process();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

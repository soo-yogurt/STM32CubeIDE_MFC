/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NOISE 224
#define COUNT_LED 3 // 사용하는 led 수
#define TRANSMITDELAY 350
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t rx;
char rx_buf[20];
char tx_buf[20];
int bufindex = 0;

int current_time_pin0 = 0;
int current_time_pin3 = 0;
int current_time_pin7 = 0;

int time_interval_pin0 = 0;
int last_time_pin0 = 0;

int time_interval_pin3 = 0;
int last_time_pin3 = 0;

int time_interval_pin7 = 0;
int last_time_pin7 = 0;

int flag = 0;
uint8_t led1 = 0;
uint8_t led2 = 0;
uint8_t led3 = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
void ToggleLED_MFC();
void Transmit_MFC();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch) {
	HAL_UART_Transmit(&huart3, (uint8_t*) ch, 1, 1);
	return ch;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */
	int current_time = 0;
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
	MX_RTC_Init();
	MX_USART3_UART_Init();

	/* Initialize interrupts */
	MX_NVIC_Init();
	/* USER CODE BEGIN 2 */
	memset(rx_buf, 0, sizeof(rx_buf));
	memset(tx_buf, 0, sizeof(tx_buf));
	HAL_UART_Receive_IT(&huart3, &rx, 1);
	current_time = HAL_GetTick();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		if (flag == 1) {
			ToggleLED_MFC();
			Transmit_MFC();
		}
		if (HAL_GetTick() >= (current_time + TRANSMITDELAY)) {
			current_time = HAL_GetTick();
			Transmit_MFC();
		}
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI
			| RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 180;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Activate the Over-Drive mode
	 */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief NVIC Configuration.
 * @retval None
 */
static void MX_NVIC_Init(void) {
	/* USART3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART3_IRQn);
	/* EXTI9_5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	/* EXTI3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	/* EXTI0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/* USER CODE BEGIN 4 */
void Transmit_MFC() {

	led1 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	led2 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
	led3 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);
	sprintf(tx_buf, "<LED%d%d%d%d>", COUNT_LED, led1, led2, led3);
	HAL_UART_Transmit(&huart3, (uint8_t*) tx_buf, sizeof(tx_buf), 10);

}

void ToggleLED_MFC() {
	if (led1 == 1) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	}
	if (led2 == 1) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
	}
	if (led3 == 1) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
	}
	memset(tx_buf, 0, sizeof(tx_buf));
	memset(rx_buf, 0, sizeof(rx_buf));
	flag = 0;
	HAL_UART_Receive_IT(&huart3, &rx, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART3) {
		if ((rx == '\n') && (rx_buf[bufindex - 1] == '>')
				&& (rx_buf[0] == '<')) {
			char *p;
			if ((p = strstr((char*) rx_buf, "<LED3")) != 0) {
				led1 = *(p + 5) - '0';
				led2 = *(p + 6) - '0';
				led3 = *(p + 7) - '0';
				bufindex = 0;
				flag = 1;
			}
		} else if (bufindex < 20) {
			rx_buf[bufindex++] = rx;
			HAL_UART_Receive_IT(&huart3, &rx, 1);
		}
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_0) {
		current_time_pin0 = HAL_GetTick();
		time_interval_pin0 = current_time_pin0 - last_time_pin0;
		last_time_pin0 = current_time_pin0;

		if (time_interval_pin0 <= NOISE) {
		} else {
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		}
	}

	if (GPIO_Pin == GPIO_PIN_3) {
		current_time_pin3 = HAL_GetTick();
		time_interval_pin3 = current_time_pin3 - last_time_pin3;
		last_time_pin3 = current_time_pin3;

		if (time_interval_pin3 <= NOISE) {
		} else {
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		}
	}
	if (GPIO_Pin == GPIO_PIN_7) {
		current_time_pin7 = HAL_GetTick();
		time_interval_pin7 = current_time_pin7 - last_time_pin7;
		last_time_pin7 = current_time_pin7;

		if (time_interval_pin7 <= NOISE) {
		} else {
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
		}
	}
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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

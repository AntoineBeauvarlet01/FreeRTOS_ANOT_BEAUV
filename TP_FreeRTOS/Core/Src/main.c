/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dcmi.h"
#include "dma2d.h"
#include "eth.h"
#include "fatfs.h"
#include "i2c.h"
#include "ltdc.h"
#include "quadspi.h"
#include "rtc.h"
#include "sai.h"
#include "sdmmc.h"
#include "spdifrx.h"
#include "tim.h"
#include "usart.h"
#include "usb_host.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TASK_LED_BOUTON_PRIORITY 3
#define TASK_LED_BOUTON_STACK_DEPTH 250

#define TASK_LED_PRIORITY 4
#define TASK_LED_STACK_DEPTH 250

#define TASK_GIVE_PRIORITY 2
#define TASK_GIVE_STACK_DEPTH 250

#define TASK_TAKE_PRIORITY 1
#define TASK_TAKE_STACK_DEPTH 250


#define NOTIFICATION
//#define SEMAPHORE

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static SemaphoreHandle_t uart1_rx_semaphore;
static TaskHandle_t task_uart1_handle;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch) {
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}


void task_led_bouton(void *unused)
{
	for(;;)
	{
		GPIO_PinState buttonState = HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_11);

		if (buttonState == GPIO_PIN_SET) // Bouton appuyé (niveau haut)
		{
			HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_SET); // LED ON
		}
		else
		{
			HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET); // LED OFF
		}
		vTaskDelay(50);
	}
}


void task_led(void *unused)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_1);
		//printf("La LED change d'etat\r\n");
		vTaskDelay(100);
	}
}


#ifdef SEMAPHORE
void task_give(void *unused)
{
	uart1_rx_semaphore = xSemaphoreCreateBinary();

	for(;;)
	{
		printf("AVANT GIVE\r\n");
		xSemaphoreGive(uart1_rx_semaphore);
		vTaskDelay(200);
		printf("APRES GIVE\r\n");
	}
}


void task_take(void *unused)
{
	uart1_rx_semaphore = xSemaphoreCreateBinary();

	for(;;)
	{
		printf("AVANT TAKE\r\n");
		if (xSemaphoreTake(uart1_rx_semaphore, 1000) == pdFALSE) // Si le sémaphore n'est pas acquis au bout d'une seconde
		{
			NVIC_SystemReset(); // RESET
		}
		printf("APRES TAKE\r\n");
	}
}

/*
#define TASK_GIVE_PRIORITY 1
#define TASK_TAKE_PRIORITY 2
La on fait un TAKE, on attend 1 seconde, pas de give donc on reset
AVANT TAKE
AVANT GIVE
APRES GIVE
AVANT GIVE
APRES GIVE
AVANT GIVE
APRES GIVE
AVANT GIVE
APRES GIVE
AVANT GIVE
AVANT TAKE


#define TASK_GIVE_PRIORITY 2
#define TASK_TAKE_PRIORITY 1
La on fait un TAKE, donc on arrive après GIVE, on retourne avant GIVE, puis on arrive après TAKE
AVANT TAKE
APRES GIVE
AVANT GIVE
APRES TAKE
AVANT TAKE
APRES GIVE
AVANT GIVE
APRES TAKE
AVANT TAKE
APRES GIVE
 */
#endif

#ifdef NOTIFICATION
void task_give(void *unused)
{
	BaseType_t HigherPriorityTaskWoken;
	for(;;)
	{
		printf("AVANT GIVE\r\n");
		vTaskNotifyGiveFromISR(task_uart1_handle, &HigherPriorityTaskWoken);
		vTaskDelay(200);
		printf("APRES GIVE\r\n");
	}
	portYIELD_FROM_ISR(HigherPriorityTaskWoken);
}


void task_take(void *unused)
{
	for(;;)
	{
		printf("AVANT TAKE\r\n");
		if (ulTaskNotifyTake(pdTRUE, 1000) == pdFALSE)
		{
			NVIC_SystemReset(); // RESET
		}
		printf("APRES TAKE\r\n");
	}
}
#endif
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

	/* Configure the peripherals common clocks */
	PeriphCommonClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_ADC3_Init();
	MX_CRC_Init();
	MX_DCMI_Init();
	MX_DMA2D_Init();
	MX_ETH_Init();
	MX_FMC_Init();
	MX_I2C1_Init();
	MX_I2C3_Init();
	MX_LTDC_Init();
	MX_QUADSPI_Init();
	MX_RTC_Init();
	MX_SAI2_Init();
	MX_SDMMC1_SD_Init();
	MX_SPDIFRX_Init();
	MX_TIM1_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_TIM5_Init();
	MX_TIM8_Init();
	MX_TIM12_Init();
	MX_USART1_UART_Init();
	MX_USART6_UART_Init();
	MX_FATFS_Init();
	/* USER CODE BEGIN 2 */
	xTaskCreate(task_give, "Task GIVE", TASK_GIVE_STACK_DEPTH, NULL, TASK_GIVE_PRIORITY, NULL);
	xTaskCreate(task_take, "Task TAKE", TASK_TAKE_STACK_DEPTH, NULL, TASK_TAKE_PRIORITY, &task_uart1_handle);

	BaseType_t returned_value;

	returned_value = xTaskCreate(task_led,
			"Task LED",
			TASK_LED_STACK_DEPTH, /*taille de la pile*/
			NULL, /*Paramètre qu'on donne à la fonction task_led -> on a dit qu'on ne s'en servait pas*/
			TASK_LED_PRIORITY,
			NULL);
	if (returned_value != pdPASS) // pas assez de mémoire pour allouer la tâche
	{
		printf("Could not allocate Task LED\r\n");
		Error_Handler();
	}

	xTaskCreate(task_led_bouton, "Task LED Bouton", TASK_LED_BOUTON_STACK_DEPTH, NULL, TASK_LED_BOUTON_PRIORITY, NULL);

	vTaskStartScheduler(); // Appelle l'OS (avec une fonction freertos)
	/* USER CODE END 2 */

	/* Call init function for freertos objects (in cmsis_os2.c) */
	MX_FREERTOS_Init();

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
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

	/** Configure LSE Drive Capability
	 */
	HAL_PWR_EnableBkUpAccess();

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 400;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Activate the Over-Drive mode
	 */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief Peripherals Common Clock Configuration
 * @retval None
 */
void PeriphCommonClock_Config(void)
{
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	/** Initializes the peripherals clock
	 */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_SAI2
			|RCC_PERIPHCLK_SDMMC1|RCC_PERIPHCLK_CLK48;
	PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
	PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
	PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
	PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
	PeriphClkInitStruct.PLLSAIDivQ = 1;
	PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
	PeriphClkInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI;
	PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLLSAIP;
	PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM6) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

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

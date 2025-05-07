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
#include "cmsis_os.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>

#include "shell.h"
#include "drv_uart1.h"

#include "timers.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TASK_LED_PRIORITY 6
#define TASK_LED_STACK_DEPTH 250

#define TASK_SPAM_PRIORITY 7
#define TASK_SPAM_STACK_DEPTH 250

#define TASK_SHELL_RUN_PRIORITY 10
#define TASK_SHELL_RUN_STACK_DEPTH 250
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
h_shell_t h_shell;

QueueHandle_t xQueue;
QueueHandle_t xQueueSPAM;

TimerHandle_t h_timer;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

	return ch;
}

int fonction(h_shell_t * h_shell, int argc, char ** argv)
{
	int size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "Je suis une fonction bidon\r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);

	return 0;
}

void task_led(void *unused)
{
	TickType_t delay;
	for(;;)
	{
		if (xQueueReceive(xQueue, &delay, portMAX_DELAY) == pdTRUE)
		{
			if (delay > 0)
			{
				for (;;)
				{
					HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_1);
					vTaskDelay((TickType_t) delay);
					if (xQueueReceive(xQueue, &delay, 0) == pdTRUE)
					{
						if (delay == 0)
						{
							HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET); // LED OFF
							break;
						}
					}
					else
					{
						printf("Could not receive Queue LED\r\n");
						Error_Handler();
					}
				}
			}
			else
			{
				HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET); // LED OFF
			}
		}
	}
}

void led(h_shell_t * h_shell, int argc, char ** argv)
{
	if (argc > 1)
	{
		TickType_t delay = atoi(argv[1]);
		xQueueSend(xQueue, &delay, portMAX_DELAY);
	}
	else
	{
		printf("Veuillez renseigner un delai (en ms)\r\n");
	}
}

void task_spam(void *unused)
{
	char *message;
	for(;;)
	{
		if (xQueueReceive(xQueueSPAM, &message, portMAX_DELAY) == pdTRUE)
		{
			printf("%s\r\n", message);
		}
		else
		{
			printf("Could not receive Queue SPAM\r\n");
			Error_Handler();
		}
	}
}


void spam(h_shell_t * h_shell, int argc, char ** argv)
{
	if (argc > 2)
	{
		char *message = argv[1];
		int16_t iter = atoi(argv[2]);
		printf("\n\r");
		for (int i = 0; i < iter; i++)
		{
			xQueueSend(xQueueSPAM, &message, portMAX_DELAY);
		}
	}
	else
	{
		printf("Veuillez renseigner un message et un nombre d'iterations\r\n");
	}
}

void display_stats(h_shell_t * h_shell, int argc, char ** argv)
{
	char runtime_stats[256];
	vTaskGetRunTimeStats(runtime_stats);
	printf("----- vTaskGetRunTimeStats -----\r\n%s--------------------------------\r\n", runtime_stats);

	char task_list[1024];
	vTaskList(task_list);
	printf("----- vTaskList -----\r\n%s---------------------\r\n", task_list);
}

void task_shell_run(void *parameters)
{
	shell_run(&h_shell);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	printf("La tache [%s] a genere un overflow.\r\n", pcTaskName);
}

void configureTimerForRunTimeStats(void)
{
	h_timer = xTimerCreate("Mon premier timer :)", 1000, pdTRUE, (void *)0, NULL);

	if (h_timer == NULL) {
		Error_Handler();
	}

	if (xTimerStart(h_timer, 0) != pdPASS) {
		Error_Handler();
	}
}

unsigned long getRunTimeCounterValue(void)
{
	return (unsigned long)xTaskGetTickCount();
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
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */
	xQueue = xQueueCreate(10, sizeof(TickType_t));
	xQueueSPAM = xQueueCreate(10, sizeof(char *));

	vQueueAddToRegistry(xQueueSPAM, "Queue SPAM");

	h_shell.drv.receive = drv_uart1_receive;
	h_shell.drv.transmit = drv_uart1_transmit;

	shell_init(&h_shell);
	shell_add(&h_shell, 'f', fonction, "Une fonction inutile");
	//shell_add(&h_shell, 'l', led, "Faire clignoter une LED");
	shell_add(&h_shell, 's', spam, "Envoyer des messages");
	shell_add(&h_shell, 'd', display_stats, "Afficher les statistiques d'execution");

	BaseType_t returned_value;
	//returned_value = xTaskCreate(task_led, "Task LED", TASK_LED_STACK_DEPTH, NULL, TASK_LED_PRIORITY, NULL);
	//if (returned_value != pdPASS) // pas assez de mémoire pour allouer la tâche
	//{
	//	printf("Could not allocate Task LED\r\n");
	//	Error_Handler();
	//}

	returned_value = xTaskCreate(task_spam, "Task SPAM", TASK_SPAM_STACK_DEPTH, NULL, TASK_SPAM_PRIORITY, NULL);
	if (returned_value != pdPASS) // pas assez de mémoire pour allouer la tâche
	{
		printf("Could not allocate Task SPAM\r\n");
		Error_Handler();
	}

	returned_value = xTaskCreate(task_shell_run, "Task Shell Run", TASK_SHELL_RUN_STACK_DEPTH, (void *)&h_shell, TASK_SHELL_RUN_PRIORITY, NULL);
	if (returned_value != pdPASS) // pas assez de mémoire pour allouer la tâche
	{
		printf("Could not allocate Task Shell Run\r\n");
		Error_Handler();
	}

	vTaskStartScheduler(); // Appelle l'OS (avec une fonction freertos)
	/* USER CODE END 2 */

	/* Call init function for freertos objects (in freertos.c) */
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

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 432;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
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

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
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

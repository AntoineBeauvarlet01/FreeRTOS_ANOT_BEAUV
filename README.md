# FreeRTOS_ANOT-DELCOURT_BEAUVARLET

## NOTE
> Dans TP_FreeRTOS : il y a le début du TP, avant le shell

> Dans rtos_td_shell : il y a la fin du TP, à partir du shell


TP 3DN - Noyau temps réel
![alt text](image.png)

1. Où se situe le fichier main.c ?

> Le fichier main.c se situe dans le répertoire du projet, puis dans Core/Src/main.c.

2. À quoi servent les commentaires indiquant BEGIN et END ?

> Cela permet à l'interpréteur de comprendre que le code entre ces deux marqueurs ne doit pas être écrésé le code a chaque regenérations du projet.

Deux fonctions à utiliser :
>  HAL_Delay

>  HAL_GPIO_TogglePin


3. Quels sont les paramètres à passer à HAL_Delay et HAL_GPIO_TogglePin ?
   
```
void HAL_Delay(uint32_t Delay);
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
```
4. Dans quel fichier les ports d’entrée/sorties sont-ils définis?
> Les E/S sont définient dans le fichier portant le nom du projet .ioc.

5. Écrivez un programme simple permettant de faire clignoter la LED.
```
/* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_1); // Inverser l'état de la broche PI1 (LD1)
    HAL_Delay(500); // Attendre 500 ms
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
```
6. Modifiez le programme pour que la LED s’allume lorsque le bouton USER est
appuyé.
```
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TASK_LED_BOUTON_PRIORITY 3
#define TASK_LED_BOUTON_STACK_DEPTH 250
/* USER CODE END PD */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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
		vTaskDelay(50); / Pour réduire le nombre de passage dans la boucle
	}
}
/* USER CODE END 0 */

int main(void)
{
	// TOUS LES INIT

	/* USER CODE BEGIN 2 */
	BaseType_t returned_value;
	returned_value = xTaskCreate(task_led_bouton,
			"Task LED Bouton",
			TASK_LED__BOUTON_STACK_DEPTH, /*taille de la pile*/
			NULL, /*Paramètre qu'on donne à la fonction task_led -> on a dit qu'on ne s'en servait pas*/
			TASK_LED_BOUTON_PRIORITY,
			NULL);
	if (returned_value != pdPASS) // pas assez de mémoire pour allouer la tâche
	{
		printf("Could not allocate Task LED Bouton\r\n");
		Error_Handler();
	}

	xTaskCreate(task_led_bouton, "Task LED Bouton", TASK_LED_BOUTON_STACK_DEPTH, NULL, TASK_LED_BOUTON_PRIORITY, NULL);

	vTaskStartScheduler(); // Appelle l'OS (avec une fonction freertos)
	/* USER CODE END 2 */

	// LA SUITE AVEC L'INIT DE FREERTOS, LE START SCHEDULER ET LA BOUCLE WHILE.
}

```

## 1 FreeRTOS, tâches et sémaphores

### 1.1 Tâche simple *(enfin il parrait)*
1. Vous pouvez travailler dans le projet créé à la partie précédente Activez FreeRTOS et notez les paramètres qui vous paraissent pertinents. 
   
* En quoi le paramètre TOTAL_HEAP_SIZE a-t-il de l’importance ?
  
> TOTAL_HEAP_SIZE définit la quantité totale de mémoire RAM réservée aux allocations dynamiques, ce qui est crucial pour éviter les débordements et gérer efficacement les ressources limitées des systèmes embarqués. Une taille appropriée impacte directement la stabilité, les performances et la prédictibilité du programme.


2. Créez une tâche permettant de faire changer l’état de la LED toutes les `100ms` et profitez-en pour afficher du texte à chaque changement d’état.
```
void task_led(void *unused)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_1);
		printf("La LED change d'etat\r\n");
		vTaskDelay(100);
	}
}
```
   
* Quel est le rôle de la macro portTICK_PERIOD_MS ?

> La macro portTICK_PERIOD_MS dans FreeRTOS permet de définir la durée d'un tick en ms. 


### 1.2 Sémaphores pour la synchronisation

3. Créez deux tâches, `taskGive` et `taskTake`, ayant deux priorités differentes.
   
`TaskGive` donne un sémaphore toutes les 100ms. Affichez du texte avant et après avoir donné le sémaphore. 
`TaskTake` prend le sémaphore. Affichez du texte avant et après avoir pris le sémaphore.

```
// Dans le main :
task_semaphore = xSemaphoreCreateBinary();

xTaskCreate(task_give, "Task GIVE", TASK_GIVE_STACK_DEPTH, NULL, TASK_GIVE_PRIORITY, NULL);
xTaskCreate(task_take, "Task TAKE", TASK_TAKE_STACK_DEPTH, NULL, TASK_TAKE_PRIORITY, NULL);
// Fin main

void task_give(void *unused)
{
	for(;;)
	{
		printf("AVANT GIVE\r\n");
		xSemaphoreGive(task_semaphore);
		vTaskDelay(100);
		printf("APRES GIVE\r\n");
	}
}

void task_take(void *unused)
{
	for(;;)
	{
		printf("AVANT TAKE\r\n");
		xSemaphoreTake(task_semaphore, HAL_MAX_DELAY);
		printf("APRES TAKE\r\n");
	}
}
```

4. Ajoutez un mécanisme de gestion d’erreur lors de l’acquisition du sémaphore.
On pourra par exemple invoquer un reset software au STM32 si le sémaphore n’est pas acquis au bout d’une seconde.

5. Pour valider la gestion d’erreur, ajoutez 100ms au delai de TaskGive à chaque itération.
```
void task_give(void *unused)
{
	TickType_t delay = 100;

	for(;;)
	{
		printf("AVANT GIVE\r\n");
		xSemaphoreGive(task_semaphore);
		printf("\tAPRES GIVE -- %d\r\n", delay);
		vTaskDelay(delay);
		delay += 100;
	}
}

void task_take(void *unused)
{
	for(;;)
	{
		printf("\t\tAVANT TAKE\r\n");
		if (xSemaphoreTake(task_semaphore, 1000) == pdFALSE) // Si le sémaphore n'est pas acquis au bout d'une seconde
		{
			printf("----- RESET\r\n");
			NVIC_SystemReset(); // RESET
		}
		printf("\t\t\tAPRES TAKE\r\n");
	}
}
```  

6. Changez les priorités. Expliquez les changements dans l’affichage.

Avec ces priorités (TAKE + prioritaire que GIVE) :
```
#define TASK_GIVE_PRIORITY 1
#define TASK_TAKE_PRIORITY 2
```
On obtient :
```
AVANT GIVE
                        APRES TAKE
                AVANT TAKE
        APRES GIVE -- 100
AVANT GIVE
                        APRES TAKE
                AVANT TAKE
        APRES GIVE -- 200
.
.
.
AVANT GIVE
                        APRES TAKE
                AVANT TAKE
        APRES GIVE -- 900
AVANT GIVE
                        APRES TAKE
                AVANT TAKE
        APRES GIVE -- 1000
                AVANT TAKE
AVANT GIVE
                        APRES TAKE
                AVANT TAKE
        APRES GIVE -- 100
```

Avec ces priorités (GIVE + prioritaire que TAKE) :
 ```
#define TASK_GIVE_PRIORITY 2
#define TASK_TAKE_PRIORITY 1
```
On obtient :
```
AVANT GIVE
        APRES GIVE -- 100
                AVANT TAKE
                        APRES TAKE
                AVANT TAKE
AVANT GIVE
        APRES GIVE -- 200
                        APRES TAKE
                AVANT TAKE
.
.
.
AVANT GIVE
        APRES GIVE -- 900
                        APRES TAKE
                AVANT TAKE
AVANT GIVE
        APRES GIVE -- 1000
                        APRES TAKE
                AVANT TAKE
AVANT GIVE
        APRES GIVE -- 1100
                        APRES TAKE
                AVANT TAKE
AVANT GIVE
        APRES GIVE -- 100
                AVANT TAKE
                        APRES TAKE
                AVANT TAKE
```

### 1.3 Notification

7. Modifiez le code pour obtenir le même fonctionnement en utilisant des task notifications à la place des sémaphores.
```
// Dans le main :
xTaskCreate(task_give, "Task GIVE", TASK_GIVE_STACK_DEPTH, NULL, TASK_GIVE_PRIORITY, task_give_handle);
xTaskCreate(task_take, "Task TAKE", TASK_TAKE_STACK_DEPTH, NULL, TASK_TAKE_PRIORITY, task_take_handle);
// Fin main

void task_give(void *unused)
{
	TickType_t delay = 100;

	for(;;)
	{
		printf("AVANT GIVE\r\n");
		xTaskNotifyGive(task_take_handle);
		printf("\tAPRES GIVE -- %d\r\n", delay);
		vTaskDelay(delay);
		delay += 100;
	}
}


void task_take(void *unused)
{
	for(;;)
	{
		printf("\t\tAVANT TAKE\r\n");
		if (ulTaskNotifyTake(pdTRUE, 1000) == pdFALSE)
		{
			NVIC_SystemReset(); // RESET
			printf("----- RESET\r\n");
		}
		printf("\t\t\tAPRES TAKE\r\n");
	}
}
```
### 1.4 Queues
8. Modifiez `TaskGive` pour envoyer dans une queue la valeur du timer. Modifiez `TaskTake` pour réceptionner et afficher cette valeur.
```
void task_give(void *unused)
{
	TickType_t delay = 100;

	for (;;)
	{
		TickType_t tick_value = xTaskGetTickCount();
		printf("AVANT GIVE : %lu\r\n", tick_value);
		xQueueSend(task_queue, &tick_value, portMAX_DELAY);
		vTaskDelay(delay);
		printf("\tAPRES GIVE : %lu\r\n", tick_value);
		delay += 100;
	}
}

void task_take(void *unused)
{
	TickType_t tick_value;

	for (;;)
	{
		printf("\t\tAVANT TAKE\r\n");
		if (xQueueReceive(task_queue, &tick_value, 1000) == pdFALSE)
		{
			NVIC_SystemReset();
			printf("----- RESET\r\n");

		}
		printf("\t\t\tAPRES TAKE : %lu\r\n", tick_value);
	}
}
```

Résultats :
```
#define TASK_GIVE_PRIORITY 1
#define TASK_TAKE_PRIORITY 2
```
```
AVANT GIVE : 1
                        APRES TAKE : 1
                AVANT TAKE
        APRES GIVE : 1
AVANT GIVE : 106
                        APRES TAKE : 106
                AVANT TAKE
        APRES GIVE : 106
AVANT GIVE : 312
                        APRES TAKE : 312
                AVANT TAKE
        APRES GIVE : 312
.
.
.
AVANT GIVE : 3648
                        APRES TAKE : 3648
                AVANT TAKE
        APRES GIVE : 3648
AVANT GIVE : 4554
                        APRES TAKE : 4554
                AVANT TAKE
                AVANT TAKE
AVANT GIVE : 1
```

```
#define TASK_GIVE_PRIORITY 2
#define TASK_TAKE_PRIORITY 1
```
```
AVANT GIVE : 0
                AVANT TAKE
                        APRES TAKE : 0
                AVANT TAKE
        APRES GIVE : 0
AVANT GIVE : 102
                        APRES TAKE : 102
                AVANT TAKE
        APRES GIVE : 102
AVANT GIVE : 305
                        APRES TAKE : 305
                AVANT TAKE
        APRES GIVE : 305
.
.
.
AVANT GIVE : 4526
                        APRES TAKE : 4526
                AVANT TAKE
        APRES GIVE : 4526
AVANT GIVE : 5529
                        APRES TAKE : 5529
                AVANT TAKE
AVANT GIVE : 0
```
### 1.5 Réentrance et exclusion mutuelle

9. Recopiez le code ci-dessous – au bon endroit – dans votre code.

10. Observez attentivement la sortie dans la console. Expliquez d’où vient le problème.
> La sortie dans la console, on observe :
```
Je suis Tache 1 et je m'endors pour 2 ticks
Je suis Tache 2 et je m'endors pour 2 ticks
```
> Le bon nom de tâche est affiché, mais c'est le délai de la tâche 2 qui est affiché pour les deux tâches. Ce "problème" est lié à la priorité des tâches : la tâche 2 est + prioritaire que la 1, donc elle intercèpte son fonctionnement.

11. Proposez une solution en utilisant un sémaphore Mutex.
```
// Dans les déclarations :
static SemaphoreHandle_t semaphore_bug;

void task_bug(void * pvParameters)
{
	int delay = (int) pvParameters;
	for(;;)
	{
		if (xSemaphoreTake(semaphore_bug, portMAX_DELAY) == pdTRUE) // On prend le sémaphore
		{
			printf("Je suis %s et je m'endors pour %d ticks\r\n", pcTaskGetName(NULL), delay);
			xSemaphoreGive(semaphore_bug); // On donne le sémaphore
		}
		vTaskDelay(delay);
	}
}

// Dans le main, on ajoute cette ligne :
semaphore_bug = xSemaphoreCreateMutex();
```
## 2 On va essayer de jouer avec le Shell
### **Attention !**
> Pour ce TP, il y a une petite subtilité. 
> Seules les interruptions dont la priorité est supérieure à la valeur 
> - `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` (définie à `5` par défaut)
> peuvent appeler des primitives de FreeRTOS. 
> 
> On peut :
> - soit modifier ce seuil, 
> 
> - soit modifier la priorité de l’interruption de l’USART1 (0 par défaut). 
>     Dans l’exemple montré en Figure 1, la priorité de l’interruption de l’USART1 est fixée à 5.
#
1. Terminer l’intégration du shell commencé en TD.

2. Que se passe-t-il si l’on ne respecte pas les priorités décrites précédemment ?

> Comportement imprévisible et erreurs difficiles à déboguer : Des appels de fonctions FreeRTOS depuis un contexte d'interruption non autorisé peuvent corrompre la mémoire partagée entre les tâches ou l'état de l'ordonnanceur de manière subtile.
        
> Perte de données ou corruption : Si l'interruption de l'USART1 gère la réception ou la transmission de données importantes, une mauvaise gestion des priorités pourrait entraîner des pertes de données si l'interruption est retardée ou si elle interfère incorrectement avec les opérations de FreeRTOS.
    
> Instabilité du système : Dans des scénarios plus complexes, des violations de priorité peuvent introduire des conditions de concurrence critiques difficiles à prévoir et à reproduire.

3. Écrire une fonction `led()`, appelable depuis le shell, permettant de faire clignoter la LED (PI1 sur la carte). 
   
    Un paramètre de cette fonction configure la periode de clignotement. Une valeur de 0 maintient la LED éteinte.
    
    Le clignotement de la LED s’effectue dans une tâche. Il faut donc trouver un moyen de faire communiquer *proprement* la fonction led avec la tâche de clignotement.

> On utilise une Queue pour transmettre le délai de clignotement renseigné dans le shell à la tâche qui fait clignoter la LED.
```
void task_led(void *unused)
{
	TickType_t delay;
	for(;;)
	{
		if (xQueueReceive(xQueueLED, &delay, portMAX_DELAY) == pdTRUE)
		{
			if (delay > 0)
			{
				for (;;)
				{
					HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_1);
					vTaskDelay((TickType_t) delay);
					if (xQueueReceive(xQueueLED, &delay, 0) == pdTRUE)
					{
						if (delay == 0)
						{
							HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET); // LED OFF
							break;
						}
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
		xQueueSend(xQueueLED, &delay, portMAX_DELAY);
	}
	else
	{
		printf("Veuillez renseigner un delai (en ms)\r\n");
	}
}

void task_shell_run(void *parameters)
{
	shell_run(&h_shell);
}

int main(void)
{
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART1_UART_Init();

	xQueueLED = xQueueCreate(10, sizeof(TickType_t));

	h_shell.drv.receive = drv_uart1_receive;
	h_shell.drv.transmit = drv_uart1_transmit;

	shell_init(&h_shell);
	shell_add(&h_shell, 'f', fonction, "Une fonction inutile");
	shell_add(&h_shell, 'l', led, "Faire clignoter une LED");

	BaseType_t returned_value;
	returned_value = xTaskCreate(task_led, "Task LED", TASK_LED_STACK_DEPTH, NULL, TASK_LED_PRIORITY, NULL);
	if (returned_value != pdPASS) // pas assez de mémoire pour allouer la tâche
	{
		printf("Could not allocate Task LED\r\n");
		Error_Handler();
	}

	returned_value = xTaskCreate(task_shell_run, "Task Shell Run", TASK_SHELL_RUN_STACK_DEPTH, (void *)&h_shell, TASK_SHELL_RUN_PRIORITY, NULL);
	if (returned_value != pdPASS) // pas assez de mémoire pour allouer la tâche
	{
		printf("Could not allocate Task Shell Run\r\n");
		Error_Handler();
	}

	vTaskStartScheduler(); // Appelle l'OS (avec une fonction freertos)


	/* Call init function for freertos objects (in freertos.c) */
	MX_FREERTOS_Init();

	/* Start scheduler */
	osKernelStart();

	/* Infinite loop */
	while (1)
	{

	}
}
```

4. Écrire une fonction `spam()`, semblable à la fonction `led()` qui affiche du texte dans la liaison série au lieu de faire clignoter les LED. 
   On peut ajouter comme argument le message à afficher et le nombre de valeurs à afficher. 
```
void task_spam(void *unused)
{
	char *message;
	for(;;)
	{
		if (xQueueReceive(xQueueSPAM, &message, portMAX_DELAY) == pdTRUE)
		{
			printf("%s\r\n", message);
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

void task_shell_run(void *parameters)
{
	shell_run(&h_shell);
}

int main(void)
{
	xQueueSPAM = xQueueCreate(10, sizeof(char *));

	h_shell.drv.receive = drv_uart1_receive;
	h_shell.drv.transmit = drv_uart1_transmit;

	shell_init(&h_shell);
	shell_add(&h_shell, 'f', fonction, "Une fonction inutile");
	shell_add(&h_shell, 's', spam, "Envoyer des messages");

	BaseType_t returned_value;
	returned_value = xTaskCreate(task_spam, "Task SPAM", TASK_SPAM_STACK_DEPTH, NULL, TASK_SPAM_PRIORITY, NULL);
	if (returned_value != pdPASS) // pas assez de mémoire pour allouer la tâche
	{
		printf("Could not allocate Task LED\r\n");
		Error_Handler();
	}

	returned_value = xTaskCreate(task_shell_run, "Task Shell Run", TASK_SHELL_RUN_STACK_DEPTH, (void *)&h_shell, TASK_SHELL_RUN_PRIORITY, NULL);
	if (returned_value != pdPASS) // pas assez de mémoire pour allouer la tâche
	{
		printf("Could not allocate Task Shell Run\r\n");
		Error_Handler();
	}

	vTaskStartScheduler(); // Appelle l'OS (avec une fonction freertos)
}
```
![image](https://github.com/user-attachments/assets/d63b248b-534c-4783-aed5-d9f0ae1d55ea)


## 3 Debug, gestion d’erreur et statistiques
*Ce TP se réalise dans le même projet, à la suite du TP précédent. On part donc du principe que le shell est fonctionnel et utilise un mécanisme d’OS (sémaphore, queue ou notification) pour la synchronisation avec une interruption.*


### 3.1 Gestion du tas
*Un certain nombre de fonctions de l’OS peuvent échouer.*
*Les fonctions finissant par Create font de l’allocation dynamique et peuvent échouer s’il n’y a plus assez de mémoire.*

1. Quel est le nom de la zone réservée à l’allocation dynamique ?
    > Il s'agit du `TAS`

2. Est-ce géré par FreeRTOS ou la HAL ?
    > C'est `FreeRTOS` qui le gère

3. Si ce n’est déjà fait, ajoutez de la gestion d’erreur sur toutes les fonctions
pouvant générer des erreurs. En cas d’erreur, affichez un message et appelez la fonction 
* Error_Handler();
  
On fait évoluer la taille du tas pour voir l'évolution de la RAM,
De base, le tas est compris dans les 5.92 % de la taille de la RAM. Pour notre TP, on fait x10 sur la taille du tas.

4. Notez la mémoire RAM et Flash utilisée.
    > Voici l'état de notre mémoire :
```
text	   data	    bss
*31368*	   116	  19276
```
   ![image](https://github.com/user-attachments/assets/73a4f9fe-49a9-45af-86fc-3677314e49f8)

5. Créez des tâches bidons jusqu’à avoir une erreur.
    > En créant des tâches bidons, voici l'état de la mémoire. Je n'ai pas obtenue d'erreur. La   taille du texte a légèrement augmenté, ce qui correspond au code supplémentaire qui a permis de créer les tâches.
```
text	   data	    bss
*32652*	   116	  19276
```
   ![image](https://github.com/user-attachments/assets/73a4f9fe-49a9-45af-86fc-3677314e49f8)

### 3.2 Gestion des piles
*Dans cette partie du TP, vous allez utiliser un hook `(une fonction appelée par l’OS, dont on peut écrire le contenu)` pour détecter les dépassements de pile `(Stack Overflow en anglais)`.*
1. Lisez la doc suivante :
    > https://www.freertos.org/Stacks-and-stack-overflow-checking.html

2. Dans CubeMX, configurez `CHECK_FOR_STACK_OVERFLOW`
> CHECK_FOR_STACK_OVERFLOW -> Option 1

3. Écrivez la fonction `vApplicationStackOverflowHook`. 
   *(Rappel : C’est une fonction appelée automatiquement par FreeRTOS, vous n’avez pas à l’appeler vous-même).*
```
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	printf("La tache %s a genere un overflow.\r\n", pcTaskName);
}
```

4. Débrouillez vous pour remplir la pile d’une tâche pour tester. 
   * Notez que, vu le contexte d’erreur, il ne sera peut-être pas possible de faire grand chose dans cette fonction. 
   * Utilisez le debugger.
> J'ai abaissé la taille de la pile réservée à la tâche SPAM, ce qui a créé un overflow.
```
#define TASK_SPAM_STACK_DEPTH 25 // Taille de la pile divisée par 10
```
![image](https://github.com/user-attachments/assets/f96a8265-e6f2-469c-8894-c92e7aca375e)

5. Il existe d’autres hooks. Expliquez l’intérêt de chacun d’entre eux.
> Méthode 1 (rapide) : Vérifie si le pointeur de pile reste dans la plage valide après la commutation de contexte. (utilisé ci-dessus)

> Méthode 2 (plus complète) : Vérifie si les 16 derniers octets de la pile (initialisés avec une valeur connue) ont été écrasés.

> Méthode 3 (spécifique à certains ports) : Active la vérification de la pile des ISR et déclenche une assertion en cas de débordement (pas de fonction hook).
  
### 3.3 Statistiques dans l’IDE
*On peut afficher un certain nombre d’informations relatives à FreeRTOS dans STM32CubeIDE en mode debug.*
1. Dans CubeMX, activez les trois paramètres suivants :
> * GENERATE_RUN_TIME_STATS
> * USE_TRACE_FACILITY
> * USE_STATS_FORMATTING_FUNCTIONS

2. Générez le code, compilez et lancez en mode debug

3. Pour ajouter les statistiques, cliquez sur `Window` > `Show View` > `FreeRTOS` > `FreeRTOS Task List`. *Vous pouvez aussi afficher les queues et les sémaphores.*

4. Lancez le programme puis mettez-le en pause pour voir les statistiques.
> J'ai lancé le programme en mode Debug, appelé la fonction SPAM dans le shell et mis en pause lors de son exécution. On voit bien sur le screen que la tâche SPAM est entrain de s'exécuter.
![image](https://github.com/user-attachments/assets/154719d3-39bf-44e7-ab80-066db0adf9a3)

5. Cherchez dans CubeMX comment faire pour afficher l’utilisation de la pile.
*En mode debug, cliquez sur `Toggle Stack Checking` (dans l’onglet `FreeRTOSTask List` en haut à droite).*
> En cliquant sur `Toggle Stack Checking`, il est indiqué dans la colonne `Min Free Stack` la taille minimum de la pile pour chaque tâche (ici > 256, en effet elle est définie à 250).
![image](https://github.com/user-attachments/assets/d220b2d8-dee6-4148-974e-427ad5aea965)

6. Pour afficher le taux d’utilisation du CPU, il faut écrire les deux fonctions suivantes :
```
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
```
* La première fonction doit démarrer un timer
* la seconde permet de récupérer la valeur du timer. 

*Si vous utilisez un timer 16 bits, il faudra peut-être bricoler un peu.*
*Encore une fois, ce sont des hooks, elles sont donc automatiquement appelées par l’OS.*

```
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

// Dans le main
vQueueAddToRegistry(xQueueSPAM, "Queue SPAM");
```

7. Affichez les sémaphores et les queues.
> ![image](https://github.com/user-attachments/assets/f9d13c9c-e290-417b-a630-1b999286379b)


8. Si vous n’en utilisez pas dans votre projet, créez deux tâches qui se partagent une queue ou un sémaphore.
> Nous avons utilisé la fonction SPAM qui utilise une queue.


### 3.4 Affichage des statistiques dans le shell

Vous pouvez vous référer à la documentation de FreeRTOS en suivant ce lien :

> https://www.freertos.org/rtos-run-time-stats.html

Deux fonctions seront utile à cette partie du TP :
```
void vTaskGetRunTimeStats(char * pcWriteBuffer);
void vTaskList(char * pcWriteBuffer);
```
1. Écrire une fonction appelable depuis le shell pour afficher les statistiques dans le terminal.
```
void display_stats(h_shell_t * h_shell, int argc, char ** argv)
{
    char runtime_stats[256];
    vTaskGetRunTimeStats(runtime_stats);
    printf("----- vTaskGetRunTimeStats -----\r\n%s--------------------------------\r\n", runtime_stats);

    char task_list[1024];
    vTaskList(task_list);
    printf("----- vTaskList -----\r\n%s---------------------\r\n", task_list);
}
```
> On appelle 2 fois la fonction pour afficher les statistiques, en appelant la tâche SPAM entre les 2 appels. On observe les différences entre les statistiques des 2 appels :

> ![image](https://github.com/user-attachments/assets/982f8174-4a01-4a78-a69b-d28cc5569019)


## 4 Écriture d’un driver

Nous n'avons pas eu le temps de travailler sur cette partie.










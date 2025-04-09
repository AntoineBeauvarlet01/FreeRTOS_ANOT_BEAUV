# FreeRTOS_ANOT_BEAUV
TP 3DN-Noyau temps réel

Commande à ajouter dans el main.c

```
int __io_putchar(int ch) {
HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
return ch;
}
```

1. Où se situe le fichier main.c?
   
Le fichier main.c se situe dans `...`

1. À quoi servent les commentaires indiquant BEGIN et END?

Cela permet à l'interprétteur de comprendre que le code entre ces deux marqueurs ne doit pas être écrésé le code a chaque regenérations du projet.

Deux fonctions à utiliser :
> — HAL_Delay

> — HAL_GPIO_TogglePin


3. Quels sont les paramètres à passer à HAL_Delay et HAL_GPIO_TogglePin?
   
```
`...`
```
4. Dans quel fichier les ports d’entrée/sorties sont-ils définis?
   
Les E/S sont définient dans le fichier `...`

5. Écrivez un programme simple permettant de faire clignoter la LED.
```
/* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_1); // Inverser l'état de la broche PI1 (LD1)
    HAL_Delay(500); // Attendre 500 millisecondes (0.5 seconde)
  }
  /* USER CODE END 3 */
```
6. Modifiez le programme pour que la LED s’allume lorsque le bouton USER est
appuyé.
```
`...`
```

## 1 FreeRTOS, tâches et sémaphores

### 1.1 Tâche simple (enfin il parrait)
1. Vous pouvez travailler dans le projet créé à la partie précédente Activez FreeRTOS et notez les paramètres qui vous paraissent pertinents. 
   
* En quoi le paramètre TOTAL_HEAP_SIZE a-t-il de l’importance ?
  
> Le paramètre TOTAL_HEAP_SIZE sert à `...`

* Observez l’impact de votre configuration sur le fichier FreeRTOSConfig.h


1. Créez une tâche permettant de faire changer l’état de la LED toutes les `100ms` et profitez-en pour afficher du texte à chaque changement d’état. 
   
* Quel est le rôle de la macro portTICK_PERIOD_MS ?

> Quel est le rôle de  sert à `...`


### 1.2 Sémaphores pour la synchronisation

3. Créez deux tâches, `taskGive` et `taskTake`, ayant deux priorités differentes.
   
`TaskGive` donne un sémaphore toutes les 100ms. Affichez du texte avant et après avoir donné le sémaphore. 
`TaskTake` prend le sémaphore. Affichez du texte avant et après avoir pris le sémaphore.

4. Ajoutez un mécanisme de gestion d’erreur lors de l’acquisition du sémaphore.
On pourra par exemple invoquer un reset software au STM32 si le sémaphore n’est pas acquis au bout d’une seconde.

5. Pour valider la gestion d’erreur, ajoutez 100ms au delai de TaskGive à chaque itération.
```
`...`
```  

6. Changez les priorités. Expliquez les changements dans l’affichage.

> En changeant la priorité `...` on obtiens `...`

### 1.3 Notification

7. Modifiez le code pour obtenir le même fonctionnement en utilisant des task notifications à la place des sémaphores.
```
`...`
```
### 1.3 Queues
8. Modifiez `TaskGive` pour envoyer dans une queue la valeur du timer. 
```
`...`
```
9.  Modifiez `TaskTake` pour réceptionner et afficher cette valeur.
```
`...`
```
### 1.5 Réentrance et exclusion mutuelle
```
#define STACK_SIZE 256
#define TASK1_PRIORITY 1
#define TASK2_PRIORITY 2
#define TASK1_DELAY 1
#define TASK2_DELAY 2
```

```
ret = xTaskCreate(task_bug, "Tache 1", STACK_SIZE, \
(void *) TASK1_DELAY, TASK1_PRIORITY, NULL);
configASSERT(pdPASS == ret);
ret = xTaskCreate(task_bug, "Tache 2", STACK_SIZE, \
(void *) TASK2_DELAY, TASK2_PRIORITY, NULL);
configASSERT(pdPASS == ret);
```

```
void task_bug(void * pvParameters)
{
    int delay = (int) pvParameters;
    for(;;)
        {
        printf("Je suis %s et je m'endors pour \%d ticks\r\n", pcTaskGetName(NULL), delay);
        vTaskDelay(delay);
        }
}

```

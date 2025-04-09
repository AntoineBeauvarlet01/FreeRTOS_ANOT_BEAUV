# FreeRTOS_ANOT_BEAUV
TP 3DN-Noyau temps réel
![alt text](image.png)
Commande à ajouter dans el main.c

```
int __io_putchar(int ch) {
HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
return ch;
}
```

1. Où se situe le fichier main.c?
   
    > Le fichier main.c se situe dans `...`

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

### 1.1 Tâche simple *(enfin il parrait)*
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

9. Recopiez le code ci-dessous – au bon endroit – dans votre code.
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

10. Observez attentivement la sortie dans la console. Expliquez d’où vient le problème.

    > Le problème proviens de `...`

11. Proposez une solution en utilisant un sémaphore Mutex.
```
`...`
```

## 2 On va essayer de jouer avec le Shell
# Attention !
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
1. Terminer l’intégration du shell commencé en TD. Pour mémoire, les questions
du TD sont rappelées ci-dessous :
   * Créer le projet, compiler et observer. Appeler la fonction depuis le shell.
   Les fichiers sont disponibles sur moodle, dans la section TD.
   * Modifier la fonction pour faire apparaître la liste des arguments.
   * Expliquer les mécanismes qui mènent à l’exécution de la fonction.
   * Quel est le problème ?
   * Proposer une solution

2. Que se passe-t-il si l’on ne respecte pas les priorités décrites précédemment ?

    > Si on ne respecte pas les priorités décrites précédemment, `...`

3. Écrire une fonction `led()`, appelable depuis le shell, permettant de faire clignoter la LED (PI1 sur la carte). 
   
    Un paramètre de cette fonction configure la periode de clignotement. Une valeur de 0 maintient la LED éteinte.
    
    Le clignotement de la LED s’effectue dans une tâche. Il faut donc trouver un moyen de faire communiquer *proprement* la fonction led avec la tâche de clignotement.
```
`...`
```

4. Écrire une fonction `spam()`, semblable à la fonction `led()` qui affiche du texte dans la liaison série au lieu de faire clignoter les LED. 
   On peut ajouter comme argument le message à afficher et le nombre de valeurs à afficher. 

    > *Ce genre de fonction peut être utile lorsque l’on travaille avec un capteur.*


## 3 Debug, gestion d’erreur et statistiques
*Ce TP se réalise dans le même projet, à la suite du TP précédent. On part donc du principe que le shell est fonctionnel et utilise un mécanisme d’OS (sémaphore, queue ou notification) pour la synchronisation avec une interruption.*


### 3.1 Gestion du tas
*Un certain nombre de fonctions de l’OS peuvent échouer.*
*Les fonctions finissant par Create font de l’allocation dynamique et peuvent échouer s’il n’y a plus assez de mémoire.*

1. Quel est le nom de la zone réservée à l’allocation dynamique ?
    > Il sagit du `...`

2. Est-ce géré par FreeRTOS ou la HAL ?
    > C'est `FreeRTOS` / `HAL` qui le gère

3. Si ce n’est déjà fait, ajoutez de la gestion d’erreur sur toutes les fonctions
pouvant générer des erreurs. En cas d’erreur, affichez un message et appelez la fonction 
* Error_Handler();
  
4. Notez la mémoire RAM et Flash utilisée, comme dans l’exemple ci-dessous
   ![alt text](image-1.png)

5. Créez des tâches bidons jusqu’à avoir une erreur.
   > screen de l'erreur

6. Notez la nouvelle utilisation mémoire.
   > la nouvelle utilisation mémoire `...`

7. Dans CubeMX, augmentez la taille du tas (TOTAL_HEAP_SIZE). Générez le
code, compilez et testez.
1. Notez la nouvelle utilisation mémoire. Expliquez les trois relevés.






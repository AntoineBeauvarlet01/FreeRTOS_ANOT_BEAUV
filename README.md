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
   Le fichier main.c se situe dans ...
2. À quoi servent les commentaires indiquant BEGIN et END?
   Cela permet à l'interprétteur de comprendre que le code entre ces deux marqueurs ne doit pas être écrésé le code a chaque regenérations du projet.
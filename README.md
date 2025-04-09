# FreeRTOS_ANOT_BEAUV
TP 3DN-Noyau temps réel

Commande à ajouter dans el main.c

```
int __io_putchar(int ch) {
HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
return ch;
}
```
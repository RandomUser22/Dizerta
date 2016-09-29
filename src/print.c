/**
 * @file
 * Implementation of functions that perform printing messages to a UART
 */

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "app_config.h"
#include "bsp.h"
#include "uart.h"

/*
 * A string buffer is necessary for printing individual characters:
 * (1) The gate keeper tasks accepts pointers to strings only. Hence a character will
 *     be placed into a short string, its first character will be the actual character,
 *     followed by '\0'.
 * (2) Corruptions must be prevented when several tasks call vPrintChar simultaneously.
 *     To accomplish this, the buffer will consist of several strings, the optimal number
 *     depends on the application.
 */

/* The number of actual strings for the buffer has been defined in "app_config.h" */

/* Length of one buffer string, one byte for the character, the other one for '\0' */
#define CHR_BUF_STRING_LEN      ( 2 )

/* Allocate the buffer for printing individual characters */
static portCHAR printChBuf[ PRINT_CHR_BUF_SIZE ][ CHR_BUF_STRING_LEN ];

/* Position of the currently available "slot" in the buffer */
static uint16_t chBufCntr = 0;



/* UART number: */
static uint8_t printUartNr = (uint8_t) -1;

/* Messages to be printed will be pushed to this queue */
static QueueHandle_t printQueue;

int16_t printInit(uint8_t uart_nr)
{
    uint16_t i;

    /*
     * Initialize the character print buffer.
     * It is sufficient to set each string's second character to '\0'.
     */
    for ( i=0; i<PRINT_CHR_BUF_SIZE; ++i )
    {
        printChBuf[i][1] = '\0';
    }

    chBufCntr = 0;

    /* Check if UART number is valid */
    if ( uart_nr >= BSP_NR_UARTS )
    {
        return pdFAIL;
    }

    printUartNr = uart_nr;

    /* Create and assert a queue for the gate keeper task */
    printQueue = xQueueCreate(PRINT_QUEUE_SIZE, sizeof(portCHAR*));
    if ( 0 == printQueue )
    {
        return pdFAIL;
    }

    /* Enable the UART for transmission */
    uart_enableTx(printUartNr);

    return pdPASS;
}

void printGateKeeperTask(void* params)
{
    portCHAR* message;

    for ( ; ; )
    {
        /* The task is blocked until something appears in the queue */
        xQueueReceive(printQueue, (void*) &message, portMAX_DELAY);
        /* Print the message in the queue */
        uart_print(printUartNr, message);
    }

    /* if it ever breaks out of the infinite loop... */
    vTaskDelete(NULL);

    /* suppress a warning since 'params' is ignored */
    (void) params;
}

void vPrintMsg(const portCHAR* msg)
{
    if ( NULL != msg )
    {
        xQueueSendToBack(printQueue, (void*) &msg, 0);
    }
}

void vPrintChar(portCHAR ch)
{
    /*
     * If several tasks call this function "simultaneously", the buffer may get
     * corrupted. To prevent this, the buffer contains several strings
     */

    /*
     * Put 'ch' to the first character of the current buffer string,
     * note that the second character has been initialized to '\0'.
     */
    printChBuf[chBufCntr][0] = ch;

    /* Now the current buffer string may be sent to the printing queue */
    vPrintMsg(printChBuf[chBufCntr]);

    /*
     * Update chBufCntr and make sure it always
     * remains between 0 and CHR_PRINT_BUF_SIZE-1
     */
    ++chBufCntr;
    chBufCntr %= PRINT_CHR_BUF_SIZE;
}

void vDirectPrintMsg(const portCHAR* msg)
{
    if ( NULL != msg )
    {
        uart_print(printUartNr, msg);
    }
}

void vDirectPrintCh(portCHAR ch)
{
    uart_printChar(printUartNr, ch);
}

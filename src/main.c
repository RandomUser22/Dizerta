
#include <stddef.h>

#include <FreeRTOS.h>
#include <task.h>

#include "trcUser.h"

#include "app_config.h"
#include "print.h"
#include "receive.h"


/*
 * This diagnostic pragma will suppress the -Wmain warning,
 * raised when main() does not return an int
 */
#pragma GCC diagnostic ignored "-Wmain"

/* Struct with settings for each task */
typedef struct _paramStruct
{
    portCHAR* text;                  /* text to be printed by the task */
    UBaseType_t  delay;              /* delay in milliseconds */
} paramStruct;

/* Default parameters if no parameter struct is available */
static const portCHAR defaultText[] = "<NO TEXT>\r\n";
static const UBaseType_t defaultDelay = 1000;


/* Task function - may be instantiated in multiple tasks */
void vTaskFunction( void *pvParameters )
{
    const portCHAR* taskName;
    UBaseType_t  delay;
    paramStruct* params = (paramStruct*) pvParameters;

    taskName = ( NULL==params || NULL==params->text ? defaultText : params->text );
    delay = ( NULL==params ? defaultDelay : params->delay);

    for( ; ; )
    {
        /* Print out the name of this task. */

        vPrintMsg(taskName);

        vTaskDelay( delay / portTICK_RATE_MS );
    }

    /*
     * If the task implementation ever manages to break out of the
     * infinite loop above, it must be deleted before reaching the
     * end of the function!
     */
    vTaskDelete(NULL);
}


void vPeriodicTaskFunction(void* pvParameters)
{
    // declare the task name
    const portCHAR* taskName;

    // each task will have an implicit delay
    UBaseType_t delay;
    paramStruct* params = (paramStruct*) pvParameters;

    taskName = ( NULL==params || NULL==params->text ? defaultText : params->text );
    delay = ( NULL==params ? defaultDelay : params->delay);
    for( ; ; )
    {
        /* Print out the name of this task. */
        vPrintMsg(taskName);
        vTaskDelay( delay / portTICK_RATE_MS );
    }

    vTaskDelete(NULL);
}


/* Parameters for two tasks */
static const paramStruct tParam[] =
{
    (paramStruct) { .text="Task..1\r\n", .delay=1500 },
    (paramStruct) { .text="Task...2\r\n", .delay=500  },
    (paramStruct) { .text="Task....3\r\n", .delay=500  },
    (paramStruct) { .text="Task.....4\r\n", .delay=500  },
    (paramStruct) { .text="Task......5\r\n", .delay=500  },
    (paramStruct) { .text="Task.......6\r\n", .delay=500  },
};


/*
 * A convenience function that is called when a FreeRTOS API call fails
 * and a program cannot continue. It prints a message (if provided) and
 * ends in an infinite loop.
 */
static void FreeRTOS_Error(const portCHAR* msg)
{
    if ( NULL != msg )
    {
        vDirectPrintMsg(msg);
    }

    for ( ; ; );
}

/* Startup function that creates and runs two FreeRTOS tasks */
void main(void)
{
    vTraceInitTraceData();

    /* Init of print related tasks: */
    if ( pdFAIL == printInit(PRINT_UART_NR) )
    {
        FreeRTOS_Error("Initialization of print failed\r\n");
    }

    vTraceStart();

    /* Create a print gate keeper task: */
    if ( pdPASS != xTaskCreate(printGateKeeperTask, "gk", 128, NULL,
                               PRIOR_PRINT_GATEKEEPR, NULL) )
    {
        FreeRTOS_Error("Could not create a print gate keeper task\r\n");
    }

    /* And finally create two tasks: */
    if ( pdPASS != xTaskCreate(vPeriodicTaskFunction, "task1", 128, (void*) &tParam[0],
                               PRIOR_FIX_FREQ_PERIODIC, NULL) )
    {
        FreeRTOS_Error("Could not create task1\r\n");
    }

//    if ( pdPASS != xTaskCreate(vTaskFunction, "task2", 128, (void*) &tParam[1],
//                               PRIOR_FIX_FREQ_PERIODIC, NULL) )
//    {
//        FreeRTOS_Error("Could not create task2\r\n");
//    }

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    /*
     * If all goes well, vTaskStartScheduler should never return.
     * If it does return, typically not enough heap memory is reserved.
     */

    FreeRTOS_Error("Could not start the scheduler!!!\r\n");

    /* just in case if an infinite loop is somehow omitted in FreeRTOS_Error */
    for ( ; ; );
}

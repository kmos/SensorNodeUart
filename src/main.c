//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
//


#include "gsi.h"
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
//Sensor Node include
#include "SN_Type.h"
#include "stub.h"
#include "applayer.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

/* Variables ---------------------------------------------------------------*/
xQueueHandle SN_tx_queue,SN_mng_queue;
//NetPackage pezzotto;
//Bool 	   verify_pezzotto;
SN_Sensor sensor[SN_SENSOR_NUMBER];
uint8_t   id_list[SN_SENSOR_NUMBER];
uint16_t  N_sensor = 0;


//STUB DELLA RETE
UART_HandleTypeDef huart5;

#ifdef TESTING
USBD_HandleTypeDef USBD_Device;
#endif

/* Task Prototypes definition -----------------------------------------------*/
static void SN_TxNet (void *pvParameters);
static void SN_ManageRequest (void *pvParameters);
/* Software Timer task ------------------------------------------------------*/
static void SN_periodic_read (void *pvParameters);
/*STUB Task -----------------------------------------------------------------*/
static void on_receive_segment(void *pvParameters);
int tx_net(NetPackage *);


//STUB DELLE RETE
static void MX_UART5_Init(void);
NetPackage pkg_to_send;




/* Function Prototypes -----------------------------------------------------*/
Bool SN_initSensor();
Bool SN_mk_data_pack(Data dato,NetPackage *package);

/* Funzione di comodo da rimuovere */
void errorHandlermain();



int main(int argc, char* argv[])
{
#ifdef TESTING
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);

	USBD_Init(&USBD_Device, &VCP_Desc, 0);
	USBD_RegisterClass(&USBD_Device, &USBD_CDC);
	USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_Template_fops);
	USBD_Start(&USBD_Device);
	HAL_Delay(4000);
#endif
	int i=0;

	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
   Data dato;
   HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
   //Parte di inizializzazione
   if(!initrete()){
	   errorHandlermain();
   }
   if(initSensors() != OP_OK){
	   errorHandlermain();
   }
   if(SN_initSensor() != TRUE){
	   errorHandler();
   }
   //fine inizializzazione
   //DA FARE CONTROLLARE ERRORI ISTANZIAZIONE
   //A questo punto si suppone che sia il NS che il CC abbino la stessa lista dei sensori
   //STUB task che genera pacchetti in ricezione
   xTaskCreate (on_receive_segment,"StubRxTask",100,NULL,tskIDLE_PRIORITY,NULL);
   //Task che gestisce le richieste
   xTaskCreate (SN_ManageRequest, "AsynchRequestManager",100,NULL,tskIDLE_PRIORITY,NULL);
   //Task che gestisce la trasmissione
   xTaskCreate (SN_TxNet,"TrasmitOnNet",100,NULL,tskIDLE_PRIORITY,NULL);
   //Instanzio le code di communicazione tra task
   SN_mng_queue = xQueueCreate(10,sizeof(NetPackage));
   SN_tx_queue  = xQueueCreate(10,sizeof(NetPackage));
   //Mi devo inizializzare le code mmm forse lo fa freeRTOS
   vTaskStartScheduler();
   for( ;; ) __NOP();
}

/* PRIVATE FUNCTION --------------------------------*/
Bool SN_initSensor()
{
  int i = 0;
  N_sensor = getSensorNumber();
  if(N_sensor>SN_SENSOR_NUMBER){
	  return FALSE;
  }else{
	  getIDSensorList(&id_list);
	  for(i=0;i<N_sensor;i++){
		  sensor[i].id=id_list[i];
		  sensor[i].alarm = FALSE;
		  sensor[i].priority = 0;
		  sensor[i].read_period_ms = 0;
		  sensor[i].treshold.HighThreshold	 = 0xFFFFFFFF;
		  sensor[i].treshold.LowThreshold 	 = 0xFFFFFFFF;
	  }
  }
  return TRUE;
}

Bool SN_mk_data_pack(Data dato,NetPackage *packet)
{

	packet->code = DATA;
	packet->payload.val = dato.value;
	return TRUE;
}

//Bool  SN_estract_net_package(uint8_t *buffer,uint16_t leng){
//
//	return TRUE;
//}
/*---------------------------------------------------------------------------*/

// CALLBACK RICEZIONE PACCHETTO QUESTA FIRMA PUO ESSERE SOGGETTA A CAMBIAMENTI
static void on_receive_segment(void *pvParameters){
	//NetPackage stub_pkg[3];
//################################################
//	//Inizializzare il pacchetto
//	//SN_mkg_package();
//	//3 pacchetti da tesstare
//	//PRIMO PACCHETTO READDATA
////	stub_pkg[0].code = READDATA;
////	stub_pkg[0].payload.id = 1;
//	//SECONDO PACCHETTO CONFIGSENSOR
//	stub_pkg[1].code = CONFIGSENSOR;
//	stub_pkg[1].payload.id = 0;
//	stub_pkg[1].payload.period =3000;
//	stub_pkg[1].payload.priority = 3;
//	stub_pkg[1].payload.alarm = 0;
//	stub_pkg[1].payload.ht = 400;
//	stub_pkg[1].payload.lt = 10;
	//TERZO STUB AGGIUNGI SENSORE PERIODICO
//	stub_pkg[0].code = CONFIGSENSOR;
//	stub_pkg[0].payload.id = 1;
//	stub_pkg[0].payload.period = 3000;
//	stub_pkg[0].payload.priority = 3;
//	stub_pkg[0].payload.alarm = 0;
//	stub_pkg[0].payload.ht = 400;
//	stub_pkg[0].payload.lt = 10;
//##################################################
	int i = 0;
	while(1){
#ifdef TESTING
		while (VCP_read((uint8_t *)&pkg_to_send, 16) != 16);
#endif
		while(HAL_UART_Receive_IT(&huart5,(uint8_t *)&pkg_to_send,(uint16_t) sizeof(NetPackage)) != HAL_OK){}
//			errorHandlermain();
//		}
		//xQueueSendToBack(SN_mng_queue,&pkg_to_send,MAX_WAIT);
		i++;
		//vTaskDelay(10);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	BSP_LED_On(LED4);

}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart5){

	xQueueSendToBack(SN_mng_queue,&pkg_to_send,0);
	BSP_LED_On(LED3);
}

/* #######TASK#########*/
//Trasmission on NET Task
static void SN_TxNet (void *pvParameters){
	NetPackage packet;
	//QUA DEVO GESTIRE ANCHE LE JOIN
	while(1){
		if(xQueueReceive(SN_tx_queue,&packet,MAX_WAIT) != pdFALSE)
		{ //Wait on queue message
			switch(packet.code){
				case JOIN: //SICUREZZA
				break;
				case DATA:
					//Trasmetto il pacchetto alla rete
					//DA GESTIRE LA PRIORITA DEL PCCHETTO MI SERVONO INTERFACCE DI RETE
					if(!tx_net(&packet)){
						errorHandlermain();
					}
				break;
			}
		}
		vTaskDelay(TASK_WAIT);
	}
}

//Request Manager Task
static void SN_ManageRequest( void *pvParameters)
{
	//Variabili locali del task
	NetPackage ap_package;
	Data dato;
	uint16_t work_periodo = 0;
	uint8_t work_id;
	xTimerHandle SN_periodic_timer[SN_SENSOR_NUMBER];
	//int timer_ctrl[SN_SENSOR_NUMBER];
	int i = 0;
	for(i=0;i<SN_SENSOR_NUMBER;i++){
		SN_periodic_timer[i] = xTimerCreate("timer lettura periodica",1,pdTRUE,sensor[i].id,SN_periodic_read);
		//timer_ctrl[i] = 0;
	}
	while(1){
		//CE SEMPRE DA FARE IL CONTROLLO SITUAZIONI ANOMALE
		//Prendere il pacchetto dalla coda
		if(xQueueReceive(SN_mng_queue,&ap_package,MAX_WAIT) != pdFALSE){
			work_id = ap_package.payload.id;
			work_periodo = sensor[work_id].read_period_ms;
			switch(ap_package.code){
				case READDATA:
					if (readData(work_id,&dato) != OP_OK){
						//controllo overrun
						errorHandlermain();
					}

					//Controlla ALARME
					if(dato.value < ap_package.payload.lt || dato.value > ap_package.payload.ht){
						//Supera le soglie
						ap_package.payload.alarm = 1;
					}
					//Tengo il dato lo devo mettere in un pacchetto
					if(SN_mk_data_pack(dato,&ap_package) != TRUE){
						errorHandlermain();
					}
					//Gestire errrore coda piena
					xQueueSendToBack(SN_tx_queue,&ap_package,MAX_WAIT);
				break;
				case CONFIGSENSOR:
					//Copio del pacchetto nel sensore
					sensor[work_id].alarm = ap_package.payload.alarm;
					sensor[work_id].priority = ap_package.payload.priority;
					sensor[work_id].read_period_ms = ap_package.payload.period;
					sensor[work_id].treshold.HighThreshold = ap_package.payload.ht;
					sensor[work_id].treshold.LowThreshold = ap_package.payload.lt;
					//Controllo se devo gestirmi la periodicita
					if (sensor[work_id].read_period_ms != work_periodo && sensor[work_id].read_period_ms > 0)
					{//Se il periodo è stato cambiato
						work_periodo = sensor[work_id].read_period_ms;
						//Controlla se ilsoftware timer è attivo
						//Configura il periodo

						if	(xTimerChangePeriod(SN_periodic_timer[work_id],work_periodo/portTICK_RATE_MS,MAX_WAIT) == pdPASS )
						{
							// The command was successfully sent.
						}
					}
					//Controlla se deve uccidere un software timer
					if (work_periodo != 0 && ap_package.payload.period == 0){
						xTimerStop(SN_periodic_timer[work_id],0);
					}
				break;
				case REPLYJOIN: //SICUREZZA
					break;
			}
		}else vTaskDelay(TASK_WAIT);
	}
}


/*#################SOFTWARE TIMER TASK#######################*/

static void SN_periodic_read (xTimerHandle xTimer)
{
	//Task varaible
	Data dato;
	NetPackage packet;
	long lArrayIndex = (long) pvTimerGetTimerID(xTimer);
	//if(verify_pezzotto == FALSE){
	readData((uint8_t)lArrayIndex,&dato);
	//inserisco l'id
	packet.payload.id = (uint8_t) lArrayIndex;
	//Controlo i threshold e nel caso inserisco l'allarme
	if(dato.value < packet.payload.lt || dato.value > packet.payload.ht){
		//Supera le soglie
		packet.payload.alarm = 1;
	}

	if(SN_mk_data_pack(dato,&packet) != TRUE){
			errorHandlermain();
	}
	xQueueSendToBack(SN_tx_queue,&packet,MAX_WAIT);
//		pezzotto = packet;
//		verify_pezzotto = TRUE;
////	}

}


//Task utili per il controllo
void errorHandlermain(){
	while(1){}
}


int tx_net(NetPackage * packet){
#ifdef TESTING
	while(VCP_write((uint8_t *)packet, 16) != 16);
#endif
	if(HAL_UART_Transmit_IT(&huart5,(uint8_t *)&packet,sizeof(NetPackage)) == HAL_ERROR){
		return 0;
	}
	return 1;
}


int initrete()
{
	MX_UART5_Init();
	return 1;
}



//STUB DELLA RETE
void MX_UART5_Init(void)
{
  __GPIOD_CLK_ENABLE();
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart5);

}



void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------

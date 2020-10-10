/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "task_function.h"
/* Start user code for import. Do not edit comment generated here */
#include "r_sci_rx_if.h"
#include "queue.h"


#define SIRIAL_CH					( 12 )

typedef struct
{
	QueueHandle_t 					QueueHandle;
	sci_hdl_t						SciHandle;
	sci_cfg_t						tConfig;
} SERIAL_GLOBAL_TABLE;

SERIAL_GLOBAL_TABLE					g_tSirial;

// Serialコールバック
void SerialCallback(void *pArgs)
{
#if 1
	sci_cb_args_t		*ptSciCbArgs = (sci_cb_args_t*)pArgs;
	BaseType_t 			bHigherPriorityTaskWoken = pdFALSE;

	xQueueSendFromISR(g_tSirial.QueueHandle, ptSciCbArgs, &bHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(bHigherPriorityTaskWoken);
#endif
}


/* End user code. Do not edit comment generated here */

void Task_Serial(void * pvParameters)
{
/* Start user code for function. Do not edit comment generated here */
	sci_err_t						eSciResult = SCI_SUCCESS;
	sci_cb_args_t					tSciCbArgs;
	uint8_t							szBuff[5] = { 0x00 };


	g_tSirial.QueueHandle = xQueueCreate(5,sizeof(sci_cb_args_t));
	if (g_tSirial.QueueHandle == NULL )
	{
		printf("xQueueCreate Error.\n");
		while(1);
	}

	R_SCI_PinSet_SCI12();
//	PORTE.PDR.BIT.B1 = 1;				// PE1(Pin No:134 (50))
//	PORTE.PDR.BIT.B2 = 0;				// PE2(Pin No:133 (51))

	// シリアルオープン
	g_tSirial.tConfig.async.baud_rate = 19200;
	g_tSirial.tConfig.async.clk_src = SCI_CLK_INT;
	g_tSirial.tConfig.async.data_size = SCI_DATA_8BIT;
	g_tSirial.tConfig.async.stop_bits = SCI_STOPBITS_1;
	g_tSirial.tConfig.async.parity_en = SCI_PARITY_OFF;
	g_tSirial.tConfig.async.parity_type = SCI_EVEN_PARITY;
	g_tSirial.tConfig.async.int_priority = 2;
	eSciResult = R_SCI_Open(SIRIAL_CH, SCI_MODE_ASYNC, &g_tSirial.tConfig, SerialCallback, &g_tSirial.SciHandle);
	if (eSciResult != SCI_SUCCESS)
	{
		printf("R_SCI_Open Error. [eSciResult:%d]\n",eSciResult);
		while(1);
	}

#if 1
	while(1)
	{
		xQueueReceive(g_tSirial.QueueHandle, &tSciCbArgs, portMAX_DELAY);

		switch (tSciCbArgs.event) {
		case SCI_EVT_RX_CHAR:
#if 0
			printf("[CI_EVT_RX_CHAR] Ch:%c , Num:%d \n", tSciCbArgs.byte, tSciCbArgs.num);
#else
			R_SCI_Receive(g_tSirial.SciHandle, szBuff, 1);
			R_SCI_Send(g_tSirial.SciHandle,szBuff, 1);
#endif
			break;
		case SCI_EVT_RX_CHAR_MATCH:
			printf("[SCI_EVT_RX_CHAR_MATCH] Ch:%c , Num:%d \n", tSciCbArgs.byte, tSciCbArgs.num);
			break;
		case SCI_CFG_TEI_INCLUDED:
			printf("[SCI_CFG_TEI_INCLUDED] Ch:%c , Num:%d \n", tSciCbArgs.byte, tSciCbArgs.num);
			break;
		case SCI_EVT_RXBUF_OVFL:
			printf("[SCI_EVT_RXBUF_OVFL] Ch:%c , Num:%d \n", tSciCbArgs.byte, tSciCbArgs.num);
			break;
		case SCI_EVT_OVFL_ERR:
			printf("[SCI_EVT_OVFL_ERR] Ch:%c , Num:%d \n", tSciCbArgs.byte, tSciCbArgs.num);
			break;
		case SCI_EVT_FRAMING_ERR:
			printf("[SCI_EVT_FRAMING_ERR] Ch:%c , Num:%d \n", tSciCbArgs.byte, tSciCbArgs.num);
			break;
		case SCI_EVT_PARITY_ERR:
			printf("[SCI_EVT_PARITY_ERR] Ch:%c , Num:%d \n", tSciCbArgs.byte, tSciCbArgs.num);
			break;
//		case SCI_EVT_RX_DONE:
//			printf("[SCI_EVT_RX_DONE] Ch:%c , Num:%d \n", tSciCbArgs.byte, tSciCbArgs.num);
//			break;
		default:
			printf("[Unknwon event:%d] \n");
			break;
		}
	}
#else
	while(1)
	{
		eSciResult = R_SCI_Receive(g_tSirial.SciHandle, szBuff, 1);
		if (eSciResult == SCI_SUCCESS)
		{
			R_SCI_Send(g_tSirial.SciHandle,szBuff, 1);
		}
	}
#endif
/* End user code. Do not edit comment generated here */
}
/* Start user code for other. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

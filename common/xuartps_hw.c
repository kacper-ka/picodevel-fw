/******************************************************************************
*
* Copyright (C) 2010 - 2015 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/****************************************************************************/
/**
*
* @file xuartps_hw.c
* @addtogroup uartps_v3_4
* @{
*
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date	Changes
* ----- ------ -------- ----------------------------------------------
* 1.00	drg/jz 01/12/10 First Release
* 1.05a hk     08/22/13 Added reset function
* 3.00  kvn    02/13/15 Modified code for MISRA-C:2012 compliance.
* </pre>
*
*****************************************************************************/

/***************************** Include Files ********************************/
#include "xuartps_hw.h"

/************************** Constant Definitions ****************************/


/***************** Macros (Inline Functions) Definitions ********************/


/************************** Function Prototypes ******************************/


/************************** Variable Definitions *****************************/

/****************************************************************************/
/**
*
* This function sends one byte using the device. This function operates in
* polled mode and blocks until the data has been put into the TX FIFO register.
*
* @param	BaseAddress contains the base address of the device.
* @param	Data contains the byte to be sent.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XUartPs_SendByte(u32 BaseAddress, u8 Data)
{
	/* Wait until there is space in TX FIFO */
	while (XUartPs_IsTransmitFull(BaseAddress)) {
		;
	}

	/* Write the byte into the TX FIFO */
	XUartPs_WriteReg(BaseAddress, XUARTPS_FIFO_OFFSET, (u32)Data);
}

/** @} */

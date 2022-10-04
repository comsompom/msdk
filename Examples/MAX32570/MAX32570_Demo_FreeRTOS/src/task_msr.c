/******************************************************************************
 * Copyright (C) 2022 Maxim Integrated Products, Inc., All rights Reserved.
 * 
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 ******************************************************************************/

/* Global includes */
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include "MAX32xxx.h"
#include "message.h"
#include "task_msr.h"
#include "sdma_regs.h"

/*********************************      DEFINES      *************************/
/* Select SDMA instance to run MSR on {0,1} */
#define MSR_SDMA_INSTANCE (0)

#if MSR_SDMA_INSTANCE == 0
#define MSR_SDMA MXC_SDMA0
#define MSR_SDMA_IRQn HA0_IRQn
#define SDMAx_IRQHandler HA0_IRQHandler
#define CLK_DIS_SDMAxD MXC_SYS_PERIPH_CLOCK_HA0
#else
#define MSR_SDMA MXC_SDMA1
#define MSR_SDMA_IRQn HA1_IRQn //SDMA1_IRQn
#define SDMAx_IRQHandler HA1_IRQHandler
#define CLK_DIS_SDMAxD MXC_SYS_PERIPH_CLOCK_HA1
#endif

//
#define COPY_MESSAGE(str, dst, len)                   \
    {                                                 \
        len = strlen(str), memcpy(dst, str, len + 1); \
    }

/*********************************      VARIABLES    *************************/
/* SDMA Code */
extern unsigned char msr_sdma_code[];

/* MSR SDMA ISR */
static volatile uint32_t sdma_irq_flag;

/* SHARED section layout */
//static volatile uint32_t           *msr_version           = (uint32_t *)0x20000000;
static volatile uint16_t *msr_ctrl_ptr = (uint16_t *)0x20000004;
//static volatile uint16_t           *adc9_err_ptr          = (uint16_t *)0x20000006;
static volatile uint32_t *swipe_timeout_sec_ptr = (uint32_t *)0x20000008;
static volatile mcr_decoded_track_t *decoded_track = (mcr_decoded_track_t *)0x2000000C;

#define msr_version (*msr_version)
#define msr_ctrl (*msr_ctrl_ptr)
#define adc9_err (*adc9_err_ptr)
#define swipe_timeout_sec (*swipe_timeout_sec_ptr)

extern xQueueHandle xQueueMain;
static xSemaphoreHandle xMSRLock;
static volatile int g_msr_active_polling = 0;

/********************************* Static Functions **************************/
void SDMAx_IRQHandler(void)
{
    MSR_SDMA->irq_flag = 1; /* Clear irq_flag */
    sdma_irq_flag = 1; // set int flag
}

/* This will print decoded swipe data */
static void process_swipe(int tidx, char *msg, int *msg_len)
{
    if (decoded_track[tidx].error_code != MCR_ERR_OK) {
        if (decoded_track[tidx].error_code & MCR_ERR_BAD_LEN) {
            COPY_MESSAGE("Err = BAD LENGHT", msg, *msg_len);
            return;
        } else if (decoded_track[tidx].error_code & MCR_ERR_START_SEN) {
            COPY_MESSAGE("Err = START Sentinel", msg, *msg_len);
            return;
        } else if (decoded_track[tidx].error_code & MCR_ERR_END_SEN) {
            COPY_MESSAGE("Err = END Sentinel", msg, *msg_len);
            return;
        } else if (decoded_track[tidx].error_code & MCR_ERR_OUTLIER) {
            COPY_MESSAGE("Err = OUTLIER", msg, *msg_len);
            return;
        } else if (decoded_track[tidx].error_code & MCR_ERR_PARITY) {
            COPY_MESSAGE("Err = PARITY", msg, *msg_len);
            return;
        } else if (decoded_track[tidx].error_code & MCR_ERR_LRC) {
            COPY_MESSAGE("Err = LRC", msg, *msg_len);
            return;
        }
    }

    if (decoded_track[tidx].lrc != 0) {
        COPY_MESSAGE("Err = LRC check failed", msg, *msg_len);
        return;
    }

    if (decoded_track[tidx].direction == MCR_FORWARD) {
        memcpy(msg, ">>> ", 4);
    } else {
        memcpy(msg, "<<< ", 4);
    }
    memcpy(&msg[4], (char *)decoded_track[tidx].data, decoded_track[tidx].len);
    *msg_len = decoded_track[tidx].len + 4;

    return;
}

static void start_msr_sdma(void)
{
    /* Enable SDMA clock */
    MXC_SYS_ClockEnable(CLK_DIS_SDMAxD);

    /* Disable interrupt from SDMA */
    /* will wait for irq_flag=1 but dont want IRQHandler() to run */
    NVIC_DisableIRQ(MSR_SDMA_IRQn);
    MSR_SDMA->irq_ie = 0;

#define ADC9_IRQ_VECTOR 22

    /* Enable interrupt mux */
    MSR_SDMA->int_mux_ctrl0 = (ADC9_IRQ_VECTOR << MXC_F_SDMA_INT_MUX_CTRL0_INTSEL16_POS);
    MSR_SDMA->int_mux_ctrl1 = 0;
    MSR_SDMA->int_mux_ctrl2 = 0;
    MSR_SDMA->int_mux_ctrl3 = 0;
    /* Set org address and enable sdma */
    MSR_SDMA->ip_addr = (uint32_t)&msr_sdma_code;
    MSR_SDMA->ctrl |= MXC_F_SDMA_CTRL_EN;
    /* Wait for the SDMA to finish re-initialization */
    while (!MSR_SDMA->irq_flag) {
        {
        }
    }
    /* Clear irq_flag */
    MSR_SDMA->irq_flag = 1;
    NVIC_ClearPendingIRQ(MSR_SDMA_IRQn);
    /* Re-Enable interrupt from SDMA */
    NVIC_EnableIRQ(MSR_SDMA_IRQn);
    MSR_SDMA->irq_ie |= MXC_F_SDMA_IRQ_IE_IRQ_EN;
    MSR_SDMA->int_in_ie = 1;
}

static void stop_msr_sdma(void)
{
    /* Disable interrupt from SDMA */
    NVIC_DisableIRQ(MSR_SDMA_IRQn);
    MSR_SDMA->irq_ie = 0;

    /* stop SDMA */
    MSR_SDMA->ctrl &= ~(MXC_F_SDMA_CTRL_EN);

    /* Disable interrupt mux */
    MSR_SDMA->int_mux_ctrl0 = 0;
    MSR_SDMA->int_mux_ctrl1 = 0;
    MSR_SDMA->int_mux_ctrl2 = 0;
    MSR_SDMA->int_mux_ctrl3 = 0;

    /* Disable SDMA clock */
    MXC_SYS_ClockDisable(CLK_DIS_SDMAxD);
}

/********************************* Public Functions **************************/
void msr_start(void)
{
    g_msr_active_polling = 1;
    xSemaphoreGive(xMSRLock);
}

void msr_stop(void)
{
    g_msr_active_polling = 0;
    xSemaphoreTake(xMSRLock, 0);
}

void vGetMSRTask(void *pvParameters)
{
    (void)pvParameters;

    message_t msgMSR;

    msgMSR.pcType = 'M'; // it is msr message
    swipe_timeout_sec = 30; /* Set swipe timeout */

    xMSRLock = xSemaphoreCreateBinary();
    while (1) {
        while (xSemaphoreTake(xMSRLock, 0xFFFF) != pdTRUE) {
            {
            }
        }

        start_msr_sdma();
        do {
            msgMSR.len = 0;
            msr_ctrl = GETSWIPE_BUSY; /* signal to SDMA to get swipe data */

            sdma_irq_flag = 0;
            while (!sdma_irq_flag) {
                vTaskDelay(100);
                if (g_msr_active_polling == 0)
                    break;
            }

            if (g_msr_active_polling) {
                /* check exit code */
                switch (msr_ctrl) {
                case GETSWIPE_OK:
                    process_swipe(1, (char *)msgMSR.pcMessage,
                                  &msgMSR.len); /* Print Swipe results */
                    break;
                case GETSWIPE_ADCERR:
                    /* ADC errors */
#if 0
                        if (adc9_err & ADCERR_OVERRUN) {
                            COPY_MESSAGE("Err = ADC9 overrun",  msgMSR.pcMessage, msgMSR.len);
                        } else if (adc9_err & ADCERR_SHIFT) {
                            COPY_MESSAGE("Err = ADC9 channels shifted",  msgMSR.pcMessage, msgMSR.len);
                        } else if (adc9_err & ADCERR_INCOMPLETE) {
                            COPY_MESSAGE("Err = ADC9 conversion incomplete",  msgMSR.pcMessage, msgMSR.len);
                        }
#endif
                    break;
                case GETSWIPE_TIMO:
                    /* Timeout: no swipe */
                    break;
                }

                if (msgMSR.len) {
                    xQueueSendToFront(xQueueMain, &msgMSR, 0);
                }
            }

            vTaskDelay(100);
        } while (g_msr_active_polling);

        stop_msr_sdma();
    }
}

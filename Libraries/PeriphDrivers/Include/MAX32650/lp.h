/**
 * @file    lp.h
 * @brief   Low power function prototypes and data types.
 */

/* ****************************************************************************
 * Copyright (C) 2017 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
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
 * $Date: 2016-10-10 15:10:41 -0500 (Mon, 10 Oct 2016) $
 * $Revision: 24650 $
 *
 *************************************************************************** */

/* Define to prevent redundant inclusion */
#ifndef LIBRARIES_PERIPHDRIVERS_INCLUDE_MAX32650_LP_H_
#define LIBRARIES_PERIPHDRIVERS_INCLUDE_MAX32650_LP_H_

/* **** Includes **** */
#include "gpio.h"
#include "pwrseq_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup pwrseq Low Power (LP)
 * @ingroup periphlibs
 * @{
 */

typedef enum {
    VBUS_STATE_CHANGE = MXC_F_PWRSEQ_USB_WK_EN_USBVBUSWKEN,
    LINE_STATE_CHANGE = MXC_F_PWRSEQ_USB_WK_EN_USBLSWKEN,
} mxc_lp_usb_event_t;

/**
 * @brief      Enables power to the ROM.
 */
void MXC_LP_EnableROM(void);

/**
 * @brief      Enables power to the USB FIFO.
 */
void MXC_LP_EnableUSBFIFO(void);

/**
 * @brief      Enables power to the Crypto MAA RAM.
 */
void MXC_LP_EnableCryptoRAM(void);

/**
 * @brief      Enables power to the system cache RAM.
 */
void MXC_LP_EnableSCache(void);

/**
 * @brief      Enables power to the SPI-XIPF RAM.
 */
void MXC_LP_EnableICacheXIP(void);

/**
 * @brief      Enables power to the internal flash memory cache RAM.
 */
void MXC_LP_EnableICache(void);

/**
 * @brief      Enables power to RAM addresses 0x200C0000-0x200FFFFF.
 */
void MXC_LP_EnableSRAM6(void);

/**
 * @brief      Enables power to RAM addresses 0x20080000-0x200BFFFF.
 */
void MXC_LP_EnableSRAM5(void);

/**
 * @brief      Enables power to RAM addresses 0x20040000-0x2007FFFF.
 */
void MXC_LP_EnableSRAM4(void);

/**
 * @brief      Enables power to RAM addresses 0x20020000-0x2003FFFF.
 */
void MXC_LP_EnableSRAM3(void);

/**
 * @brief      Enables power to RAM addresses 0x20018000-0x2001FFFF.
 */
void MXC_LP_EnableSRAM2(void);

/**
 * @brief      Enables power to RAM addresses 0x20008000-0x20017FFF.
 */
void MXC_LP_EnableSRAM1(void);

/**
 * @brief      Enables power to RAM addresses 0x20000000-0x20007FFF.
 */
void MXC_LP_EnableSRAM0(void);

/**
 * @brief      Disables power to the ROM.
 */
void MXC_LP_DisableROM(void);

/**
 * @brief      Disables power to the USB FIFO. The contents of the USB FIFO are destroyed.
 */
void MXC_LP_DisableUSBFIFO(void);

/**
 * @brief      Disables power to the Crypto MAA RAM. The contents of the RAM are destroyed.
 */
void MXC_LP_DisableCryptoRAM(void);

/**
 * @brief      Disables power to the system cache RAM. The contents of the RAM are destroyed.
 */
void MXC_LP_DisableSCache(void);

/**
 * @brief      Disables power to the SPI-XIPF RAM. The contents of the RAM are destroyed.
 */
void MXC_LP_DisableICacheXIP(void);

/**
 * @brief      Disables power to the internal flash memory cache RAM. The contents of the RAM are destroyed.
 */
void MXC_LP_DisableICache(void);

/**
 * @brief      Disables power to RAM addresses 0x200C0000-0x200FFFFF. The contents of the RAM are destroyed.
 */
void MXC_LP_DisableSRAM6(void);

/**
 * @brief      Disables power to RAM addresses 0x20080000-0x200BFFFF. The contents of the RAM are destroyed.
 */
void MXC_LP_DisableSRAM5(void);

/**
 * @brief      Disables power to RAM addresses 0x20040000-0x2007FFFF. The contents of the RAM are destroyed.
 */
void MXC_LP_DisableSRAM4(void);

/**
 * @brief      Disables power to RAM addresses 0x20020000-0x2003FFFF. The contents of the RAM are destroyed.
 */
void MXC_LP_DisableSRAM3(void);

/**
 * @brief      Disables power to RAM addresses 0x20018000-0x2001FFFF. The contents of the RAM are destroyed.
 */
void MXC_LP_DisableSRAM2(void);

/**
 * @brief      Disables power to RAM addresses 0x20008000-0x20017FFF. The contents of the RAM are destroyed.
 */
void MXC_LP_DisableSRAM1(void);

/**
 * @brief      Disables power to RAM addresses 0x20000000-0x20007FFF. The contents of the RAM are destroyed.
 */
void MXC_LP_DisableSRAM0(void);

/**
 * @brief      Places the ROM in light sleep mode. Data will be unavailable while in light sleep mode.
 */
void MXC_LP_EnableROMLightSleep(void);

/**
 * @brief      Places the USB FIFO in light sleep mode. Data will be unavailable for read/write operations but will be retained.
 */
void MXC_LP_EnableUSBFIFOLightSleep(void);

/**
 * @brief      Places the Crypto RAM in light sleep mode. Data will be unavailable for read/write operations but will be retained.
 */
void MXC_LP_EnableCryptoRAMLightSleep(void);

/**
 * @brief      Places the system cache in light sleep mode. Data will be unavailable for read/write operations but will be retained.
 */
void MXC_LP_EnableSCacheLightSleep(void);

/**
 * @brief      Places the SPI-XIPF instruction cache in light sleep mode. Data will be unavailable for read/write operations but will be retained.
 */
void MXC_LP_EnableICacheXIPLightSleep(void);

/**
 * @brief      Places the instruction cache in light sleep mode. Data will be unavailable for read/write operations but will be retained.
 */
void MXC_LP_EnableICacheLightSleep(void);

/**
 * @brief      Places addresses 0x200C0000 to 0x200FFFFF of the RAM in light sleep mode. Data will be unavailable for read/write operations but will be retained.
 */
void MXC_LP_EnableSysRAM6LightSleep(void);

/**
 * @brief      Places addresses 0x20080000 to 0x200BFFFF of the RAM in light sleep mode. Data will be unavailable for read/write operations but will be retained.
 */
void MXC_LP_EnableSysRAM5LightSleep(void);

/**
 * @brief      Places addresses 0x20040000 to 0x2007FFFF of the RAM in light sleep mode. Data will be unavailable for read/write operations but will be retained.
 */
void MXC_LP_EnableSysRAM4LightSleep(void);

/**
 * @brief      Places addresses 0x20020000 to 0x2003FFFF of the RAM in light sleep mode. Data will be unavailable for read/write operations but will be retained.
 */
void MXC_LP_EnableSysRAM3LightSleep(void);

/**
 * @brief      Places addresses 0x20018000 to 0x2001FFFF of the RAM in light sleep mode. Data will be unavailable for read/write operations but will be retained.
 */
void MXC_LP_EnableSysRAM2LightSleep(void);

/**
 * @brief      Places addresses 0x20008000 to 0x20017FFF of the RAM in light sleep mode. Data will be unavailable for read/write operations but will be retained.
 */
void MXC_LP_EnableSysRAM1LightSleep(void);

/**
 * @brief      Places addresses 0x20000000 to 0x20007FFF of the RAM in light sleep mode. Data will be unavailable for read/write operations but will be retained.
 */
void MXC_LP_EnableSysRAM0LightSleep(void);

/**
 * @brief      Places the ROM in active mode.
 */
void MXC_LP_DisableROMLightSleep(void);

/**
 * @brief      Places the USB FIFO in active mode.
 */
void MXC_LP_DisableUSBFIFOLightSleep(void);

/**
 * @brief      Places the Crypto RAM in active mode.
 */
void MXC_LP_DisableCryptoRAMLightSleep(void);

/**
 * @brief      Places the system cache in active mode.
 */
void MXC_LP_DisableSCacheLightSleep(void);

/**
 * @brief      Places the SPI-XIPF instruction cache in active mode.
 */
void MXC_LP_DisableICacheXIPLightSleep(void);

/**
 * @brief      Places the instruction cache in active mode.
 */
void MXC_LP_DisableICacheLightSleep(void);

/**
 * @brief      Places addresses 0x200C0000 to 0x200FFFFF of the RAM in active mode.
 */
void MXC_LP_DisableSysRAM6LightSleep(void);

/**
 * @brief      Places addresses 0x20080000 to 0x200BFFFF of the RAM in active mode.
 */
void MXC_LP_DisableSysRAM5LightSleep(void);

/**
 * @brief      Places addresses 0x20040000 to 0x2007FFFF of the RAM in active mode.
 */
void MXC_LP_DisableSysRAM4LightSleep(void);

/**
 * @brief      Places addresses 0x20020000 to 0x2003FFFF of the RAM in active mode.
 */
void MXC_LP_DisableSysRAM3LightSleep(void);

/**
 * @brief      Places addresses 0x20018000 to 0x2001FFFF of the RAM in active mode.
 */
void MXC_LP_DisableSysRAM2LightSleep(void);

/**
 * @brief      Places addresses 0x20008000 to 0x20017FFF of the RAM in active mode.
 */
void MXC_LP_DisableSysRAM1LightSleep(void);

/**
 * @brief      Places addresses 0x20000000 to 0x20007FFF of the RAM in active mode.
 */
void MXC_LP_DisableSysRAM0LightSleep(void);

/**
 * @brief      Enables the selected USB event to wake up the device from any low power mode.  
 *             Call this function multiple times to enable multiple events.
 * @param      wu_evt       Which event to use as a wakeup source.
 */
void MXC_LP_EnableUSBWakeup(mxc_lp_usb_event_t wu_evt);

/**
 * @brief      Disables the selected USB event from waking up the device.  
 *             Call this function multiple times to disable multiple events.
 * @param      wu_evt       Which event to disable as a wakeup source.
 */
void MXC_LP_DisableUSBWakeup(mxc_lp_usb_event_t wu_evt);

/**
 * @brief      Enables the selected GPIO port and its selected pins to wake up the device from any low power mode.  
 *             Call this function multiple times to enable pins on multiple ports.  This function does not configure
 *             the GPIO pins nor does it setup their interrupt functionality.
 * @param      wu_pins      The port and pins to configure as wakeup sources.  Only the gpio and mask fields of the
 *                          structure are used.  The func and pad fields are ignored.
 */
void MXC_LP_EnableGPIOWakeup(mxc_gpio_cfg_t *wu_pins);

/**
 * @brief      Disables the selected GPIO port and its selected pins as a wake up source.  
 *             Call this function multiple times to disable pins on multiple ports.
 * @param      wu_pins      The port and pins to disable as wakeup sources.  Only the gpio and mask fields of the
 *                          structure are used.  The func and pad fields are ignored.
 */
void MXC_LP_DisableGPIOWakeup(mxc_gpio_cfg_t *wu_pins);

/**
 * @brief      Enables the RTC alarm to wake up the device from any low power mode.  
 */
void MXC_LP_EnableRTCAlarmWakeup(void);

/**
 * @brief      Disables the RTC alarm from waking up the device.  
 */
void MXC_LP_DisableRTCAlarmWakeup(void);

/**
 * @brief      Places the device into SLEEP mode.  This function returns once any interrupt occurs. 
 */
void MXC_LP_EnterSleepMode(void);

/**
 * @brief      Places the device into BACKGROUND mode.  This function returns once any interrupt occurs. 
 */
void MXC_LP_EnterBackgroundMode(void);

/**
 * @brief      Places the device into DEEPSLEEP mode.  This function returns once an RTC, USB wakeup, or external interrupt occur. 
 */
void MXC_LP_EnterDeepSleepMode(void);

/**
 * @brief      Places the device into BACKUP mode.  CPU state is not maintained in this mode, so this function never returns.  
 *             Instead, the device will restart once an RTC, USB wakeup, or external interrupt occur. 
 */
void MXC_LP_EnterBackupMode(void);

/**@} end of group lp */

#ifdef __cplusplus
}
#endif

#endif // LIBRARIES_PERIPHDRIVERS_INCLUDE_MAX32650_LP_H_

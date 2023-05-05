/*
 * Name			:	usb_samd21.cpp
 * Created		:	5/5/2023 2:01:39 PM
 * Author		:	Aaron Reilman
 * Description	:	SAMD21 hardware implementation of clock drivers for USB. 
 */


#include "serial_usb/serial_usb.h"

#if CFG_TUSB_MCU == OPT_MCU_SAMD21
#include "sam.h"

void SerialUSB::USBFeedIOClocks(void)
{
	//preset: 8MHz osc set and enabled, fed into clock generator 0
	NVMCTRL->CTRLB.bit.RWS = 1;
	
	while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
	SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
	while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
	
	uint32_t coarse =
	((*(uint32_t*)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk) >> FUSES_DFLL48M_COARSE_CAL_Pos;
	uint32_t fine =
	((*(uint32_t*)FUSES_DFLL48M_FINE_CAL_ADDR) & FUSES_DFLL48M_FINE_CAL_Msk) >> FUSES_DFLL48M_FINE_CAL_Pos;
	SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(fine);
	while (!SYSCTRL->PCLKSR.bit.DFLLRDY);
	
	SYSCTRL->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_USBCRM | SYSCTRL_DFLLCTRL_CCDIS;
	SYSCTRL->DFLLMUL.reg =  SYSCTRL_DFLLMUL_MUL(48000) | SYSCTRL_DFLLMUL_FSTEP(1) | SYSCTRL_DFLLMUL_CSTEP(1);
	
	/* Closed loop mode */
	SYSCTRL->DFLLCTRL.bit.MODE = 1;
	/* Enable the DFLL */
	SYSCTRL->DFLLCTRL.bit.ENABLE = 1;
	while (!SYSCTRL->PCLKSR.bit.DFLLRDY);
	
	GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(0) | GCLK_GENCTRL_SRC(GCLK_SOURCE_DFLL48M) | GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
	while(GCLK->STATUS.bit.SYNCBUSY);

	PM->APBBMASK.bit.USB_ = 0x1u;
	PM->AHBMASK.bit.USB_ = 0x1u;
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_USB;
	while (GCLK->STATUS.bit.SYNCBUSY);
	PORT->Group[0].DIRSET.reg = 0x1u << 23u;
	PORT->Group[0].DIRSET.reg = 0x1u << 24u;
	PORT->Group[0].DIRSET.reg = 0x1u << 25u;
	PORT->Group[0].PMUX[11].reg = PORT_PMUX_PMUXO(6u);
	PORT->Group[0].PINCFG[23].reg = PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[12].reg = PORT_PMUX_PMUXO(6u) | PORT_PMUX_PMUXE(6u);
	PORT->Group[0].PINCFG[24].reg = PORT_PINCFG_PMUXEN;
	PORT->Group[0].PINCFG[25].reg = PORT_PINCFG_PMUXEN;
}

void SerialUSB::ResetUSB(void)
{
	USB->DEVICE.CTRLA.bit.SWRST = 0x1;
	while(!USB->DEVICE.SYNCBUSY.bit.SWRST);
	while(USB->DEVICE.SYNCBUSY.bit.SWRST);
}

#endif

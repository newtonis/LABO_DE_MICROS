
/*
 * SPI.c
 *
 *  Created on: 18 sep. 2019
 *      Author: joa-m
 */

#include "SPI.h"
#include "gpio.h"

static PORT_Type* portPtrs[] = PORT_BASE_PTRS;
static SPI_Type* SPIPtrs[] = SPI_BASE_PTRS;
static SIM_Type* sim_ptr = SIM;
static uint32_t sim_port[] = {SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTE_MASK};


static void (*callbackTick)();

void MSTRCFG(MSTRCFG_t * master_cfg){
	master_cfg->whichCTAR = 0;
	master_cfg->MCR_isMaster = true;
	master_cfg->MCR_ContinousSerialClockEnable = false;
	master_cfg->MCR_ModifiedTransferFormatEnable = false;
	master_cfg->MCR_ReceiveFIFOverflowOverwriteEnable = false;
	master_cfg->MCR_Sample_Point = 0;
	master_cfg->MCR_isTxfifoDisabled = false;
	master_cfg->MCR_isRxfifoDisabled = false;
	master_cfg->MCR_isModuleDisabled = false;

	master_cfg->CTAR_FrameSize = 8;
	master_cfg->CTAR_CPOL = 0;
	master_cfg->CTAR_CPHA = 0;
	master_cfg->CTAR_LSBFE = 0;
	master_cfg->CTAR_BR = 5;
	master_cfg->CTAR_BRPRESC = 1;


	master_cfg->CTAR_DelayAfterTransferScaler = 0; // esto reduce el gap entre transfers para el SLAVE SELECT (PCSn)
	master_cfg->CTAR_DelayAfterTransferPrescaler = 0; // entonces lo hago lo mas chico posible

	// con master_cfg->CTAR_BR-3 obtenemos un delay de un cuarto de clock ( ojo con CTAR_BR chicos)

	master_cfg->CTAR_DelayAfterSCKtoNegPCSScaler = master_cfg->CTAR_BR-3;
	master_cfg->CTAR_DelayAfterSCKtoNegPCSPrescaler = master_cfg->CTAR_BRPRESC;
	master_cfg->CTAR_DelayPCStoSCKScaler = master_cfg->CTAR_BR-3;
	master_cfg->CTAR_DelayPCStoSCKPrescaler = master_cfg->CTAR_BRPRESC;

	// After pone delay para la derecha
	// PCStoSck pone delay para la izquierda
	// ponemos un valor tal que entre los dos delays cuando se superpongan, den la mitad de un clock (sumados)
	// de esa forma, nos quitamos de encima el problema de tener un dos unos de clock pegados (metimos
	// medio clock de delay en el medio para compensar)
}

void setPinConfigurations(uint8_t SPI_n){
	NVIC_EnableIRQ(PORTD_IRQn);
	SPIClockGatingEnable(SPI_n);
	setSPIConfig(SPI_n, SPI_CONFIG);
	configPin(PIN_PCS0, DISABLE_PULL, 0, SPI_ALTERNATIVE, GPIO_IRQ_MODE_DISABLE);
	configPin(PIN_SCK_MASTER, DISABLE_PULL, 0, SPI_ALTERNATIVE, GPIO_IRQ_MODE_DISABLE);
	configPin(PIN_MOSI_MASTER, DISABLE_PULL, 0, SPI_ALTERNATIVE, GPIO_IRQ_MODE_DISABLE);
	configPin(PIN_MISO_MASTER, DISABLE_PULL, 0, SPI_ALTERNATIVE, GPIO_IRQ_MODE_DISABLE);
	configPin(PIN_CAN_INTERRUPT, DISABLE_PULL, INPUT, GPIO_ALTERNATIVE, GPIO_IRQ_MODE_FALLING_EDGE);
	SPIClockGatingEnable(SPI_n);

}

void masterInitiliaze(uint8_t SPI_n){
	setPinConfigurations(SPI_n);

	MSTRCFG_t master_config;
	MSTRCFG(&master_config);

	SPI_Type * base = SPIPtrs[SPI_n];

	HALTStopTransfers(SPI_n);

	base->MCR = 0;
	base->MCR &=~ SPI_MCR_MDIS_MASK;
	base->MCR |= SPI_MCR_MDIS(master_config.MCR_isModuleDisabled);
	uint32_t temp = 0;
	temp = base->MCR & (~(  SPI_MCR_MSTR_MASK |
							SPI_MCR_CONT_SCKE_MASK |
							SPI_MCR_MTFE_MASK |
							SPI_MCR_ROOE_MASK |
							SPI_MCR_SMPL_PT_MASK |
	                        SPI_MCR_DIS_TXF_MASK |
							SPI_MCR_DIS_RXF_MASK));

	base->MCR = temp |
					SPI_MCR_MSTR(master_config.MCR_isMaster) |
					SPI_MCR_CONT_SCKE(master_config.MCR_ContinousSerialClockEnable) |
	                SPI_MCR_MTFE(master_config.MCR_ModifiedTransferFormatEnable) |
	                SPI_MCR_ROOE(master_config.MCR_ReceiveFIFOverflowOverwriteEnable) |
					SPI_MCR_SMPL_PT(master_config.MCR_Sample_Point) |
	                SPI_MCR_DIS_TXF(master_config.MCR_isTxfifoDisabled) |
					SPI_MCR_DIS_RXF(master_config.MCR_isRxfifoDisabled);

	base->CTAR[master_config.whichCTAR]= 0;
	temp = base->CTAR[master_config.whichCTAR] & ~(  SPI_CTAR_BR_MASK |
													 SPI_CTAR_PBR_MASK |
													 SPI_CTAR_FMSZ_MASK |
									 	 	 	 	 SPI_CTAR_CPOL_MASK |
													 SPI_CTAR_CPHA_MASK |
													 SPI_CTAR_LSBFE_MASK|
													 SPI_CTAR_ASC_MASK|
													 SPI_CTAR_PASC_MASK|
													 SPI_CTAR_CSSCK_MASK|
													 SPI_CTAR_PCSSCK_MASK|
													 SPI_CTAR_DT_MASK|
													 SPI_CTAR_PDT_MASK);

	base->CTAR[master_config.whichCTAR] = temp | SPI_CTAR_BR(master_config.CTAR_BR) |
												 SPI_CTAR_PBR(master_config.CTAR_BRPRESC) |
												 SPI_CTAR_FMSZ(master_config.CTAR_FrameSize - 1) |
								   	   	   	   	 SPI_CTAR_CPOL(master_config.CTAR_CPOL) |
												 SPI_CTAR_CPHA(master_config.CTAR_CPHA) |
												 SPI_CTAR_LSBFE(master_config.CTAR_LSBFE)|
												 SPI_CTAR_ASC(master_config.CTAR_DelayAfterSCKtoNegPCSScaler)|
												 SPI_CTAR_PASC(master_config.CTAR_DelayAfterSCKtoNegPCSPrescaler)|
												 SPI_CTAR_CSSCK(master_config.CTAR_DelayPCStoSCKScaler)|
												 SPI_CTAR_PCSSCK(master_config.CTAR_DelayPCStoSCKPrescaler)|
												 SPI_CTAR_DT(master_config.CTAR_DelayAfterTransferScaler)|
												 SPI_CTAR_PDT(master_config.CTAR_DelayAfterTransferPrescaler);

	setPCSActiveLow(SPI_n);

	HALTStartTransfers(SPI_n);

}

void SPI_Initialize(void (*funcallback)(void)){
	callbackTick = funcallback;
	masterInitiliaze(SPI_0);
}

void SPIClockGatingEnable(uint8_t SPI_n){
	switch(SPI_n){
		case SPI_0:
			sim_ptr->SCGC6 |= SIM_SCGC6_SPI0(ENABLE); // activo clock gating
			break;
		case SPI_1:
			sim_ptr->SCGC6 |= SIM_SCGC6_SPI1(ENABLE);
			break;
		case SPI_2:
			sim_ptr->SCGC3 |= SIM_SCGC3_SPI2(ENABLE);
			break;
	}
}



void configPin(pin_t pin, uint8_t mode, uint8_t gpio_mode, uint8_t mux_alt, uint8_t interrupt_alt){
	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	SIM->SCGC5 |= sim_port[PIN2PORT(pin)];

	uint32_t num = PIN2NUM(pin); // num es el numero de pin
	port->PCR[num] = 0x00;
	if(mux_alt == GPIO_ALTERNATIVE){
		gpioMode(pin, gpio_mode);
		gpioIRQ(pin, interrupt_alt, callbackTick);
	}
	port->PCR[num] &= ~PORT_PCR_MUX_MASK;
	port->PCR[num] &= ~PORT_PCR_IRQC_MASK;

	port->PCR[num] |= PORT_PCR_MUX(mux_alt); // ENABLE SPI
	port->PCR[num] |= PORT_PCR_IRQC(interrupt_alt);
	switch(mode){
		case PULL_UP:
			port->PCR[num] |= HIGH<<1; //PULL ENABLE en 1
			port->PCR[num] |= HIGH<<0; //PULL SELECT en 1 (PULLUP)
			break;
		case PULL_DOWN:
			port->PCR[num] |= HIGH<<1; //PULL ENABLE en 1
			port->PCR[num] &= ~(HIGH); //PULL SELECT en 0 (PULLDOWN)
			break;
		case DISABLE_PULL:
			port->PCR[num] &= ~(HIGH<<1); //PULL ENABLE en 0
			break;
	}
}


void getNotLastCommand(spi_command *command){
	command->keepAssertedPCSnBetweenTransfers = true;
	command->isEndOfQueue = false;
	command->whichPcs = Pcs0;
	command->whichCtar = 0;
	command->clearTransferCount = 0;
}

void getLastCommand(spi_command *last_command){
	last_command->keepAssertedPCSnBetweenTransfers = false;
	last_command->isEndOfQueue = true;
	last_command->whichPcs = Pcs0;
	last_command->whichCtar = 0;
	last_command->clearTransferCount = 0; // 0 equivale a no borrar TransferCount
}


void masterWriteReadBuffer(uint16_t* data, uint32_t size,uint16_t* received_data, uint8_t SPI_n){
	spi_command command;
	getNotLastCommand(&command);
	spi_command last_command;
	getLastCommand(&last_command);
	// el clock se activa cada vez que se modifica el PUSHR (en masterWriteReadByte)
	for(uint32_t i = 0 ; i< size; i++){
		if(i == (size-1)){
			received_data[i]=masterWriteReadByte(SPI_n, &last_command, data[i]);
		}else{
			received_data[i]=masterWriteReadByte(SPI_n, &command, data[i]);
		}
	}

}

void testSPI(uint8_t SPI_n){

	uint16_t data[3] = {0x0C,0xAA,0xFF};
	uint16_t received_data[3] = {0,0,0};
	uint32_t size = 3;
	masterWriteReadBuffer(&data[0], size, &received_data[0], SPI_n);
	int a = 5;
}


void setSPIConfig(uint8_t SPI_n, int SPI_config){
	SPIPtrs[SPI_n]->MCR &= ~SPI_MCR_DCONF_MASK;
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_DCONF(SPI_config);
}

void disableTxFIFO(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_DIS_TXF(ENABLE);
}

void enableTxFIFO(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_DIS_TXF_MASK;
}

void disableRxFIFO(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_DIS_RXF(ENABLE);
}

void enableRxFIFO(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_DIS_RXF_MASK;
}





void clearTxFIFOCounter(uint8_t SPI_n){ //flush
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_CLR_TXF(ENABLE);
}

void clearRxFIFOCounter(uint8_t SPI_n){  //flush
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_CLR_RXF(ENABLE);
}


void HALTStopTransfers(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR |= SPI_MCR_HALT(ENABLE);
}

void HALTStartTransfers(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_HALT_MASK;
}



void setPCSActiveHigh(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR &= ~ SPI_MCR_PCSIS_MASK;
}

void setPCSActiveLow(uint8_t SPI_n){
	SPIPtrs[SPI_n]->MCR |=  SPI_MCR_PCSIS_MASK;
}


void clearTxCompleteFlag(uint8_t SPI_n){
	SPIPtrs[SPI_n]->SR |= SPI_SR_TCF(ENABLE);
}

void clearTxFifoFillRequestFlag(uint8_t SPI_n){
	SPIPtrs[SPI_n]->SR |= SPI_SR_TFFF_MASK;
}

uint16_t masterWriteReadByte(uint8_t SPI_n, spi_command *command, uint16_t data)
{
	/* First, clear Transmit Complete Flag (TCF) */
	clearTxCompleteFlag(SPI_n);

	while (!(SPIPtrs[SPI_n]->SR & SPI_SR_TFFF_MASK)) // Si TFFF = 1 (TxFifo no esta full, hay al menos un bit disponible), sale del while
    {
    	clearTxFifoFillRequestFlag(SPI_n); // asumo que está llena, entonces hasta que no se haya ido al menos un bit, no va a cambiar TFFF a 1
    }

	SPIPtrs[SPI_n]->PUSHR = SPI_PUSHR_CONT(command->keepAssertedPCSnBetweenTransfers) |
							SPI_PUSHR_PCS(command->whichPcs)|
							SPI_PUSHR_CTAS(command->whichCtar)  |
							SPI_PUSHR_EOQ(command->isEndOfQueue) |
							SPI_PUSHR_CTCNT(command->clearTransferCount) |
							SPI_PUSHR_TXDATA(data);


    clearTxFifoFillRequestFlag(SPI_n); // TxFifo Esta llena

    /* Wait till TCF sets */
    while (!(SPIPtrs[SPI_n]->SR & SPI_SR_TCF_MASK)) // si TCF = 1 (transferencia completa), sale del while
    {												// es decir espero a que se shifteen todos los bits del frame
    }
    return SPIPtrs[SPI_n]->POPR;
}

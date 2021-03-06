/*
 * Modulador.c
 *
 *  Created on: 19 Oct 2019
 *      Author: Marcelo
 */
#include "Modulador.h"
#include "DAC.h"
#include "PIT.h"
#include "math.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SIN_VALUES	83

#define NEXT_SYM	2
#define START		1
#define STOP		0

#define SIN0		0
#define SIN1		1


static uint16_t sinValues[SIN_VALUES];
static uint16_t cont = 0;
static uint16_t cycle = 0;

static bool bitStream[STAND_LEN];
static uint8_t msg_ptr = 0;

void (*MsgSendedCallback)(void);
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void sendSin0(void);

void sendSin1(void);

void configTimerSin(uint8_t symbol, bool option);

void procesBitStream(uint8_t command); // El comando puede ser START o NEXT_SYM

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void Modulador_init(void(*funcallback)(void))
{
	MsgSendedCallback = funcallback;
	DAC_init();

	DAC_setData(2048); // Por default VCC/2

	PIT_init();

	PIT_configTimer(0, 250, sendSin0);
	PIT_configTimer(1, 500, sendSin1);

	// Generacion de valores de señal de salida
	for(int i = 0; i < SIN_VALUES; i++)
	{
		sinValues[i] = 2048*(1 + sin(2*3.141592*i/SIN_VALUES));
	}
}

void Modulador_sendStream(bool *stream)
{
	for(int i = 0; i < STAND_LEN; i++)
	{
		bitStream[i] = stream[i];
	}

	procesBitStream(START);
}

void procesBitStream(uint8_t command)
{
	switch (command) {
		case START:
			configTimerSin(bitStream[msg_ptr], START);
			break;
		case NEXT_SYM:
			msg_ptr++;
			if(msg_ptr == STAND_LEN)
			{
				msg_ptr = 0;
				MsgSendedCallback(); // Avisa que termino de mandar la señal modulada
			}
			else
			{
				configTimerSin(bitStream[msg_ptr], START);
			}
			break;
	}
}

void sendSin1(void)
{
	DAC_setData(sinValues[cont]);

	cont++;
	if(cont == SIN_VALUES)
	{
		cont = 0;
		cycle++;
		if(cycle == 1) // Solo un ciclo para el 1
		{
			cycle = 0;
			configTimerSin(SIN1, STOP);
			procesBitStream(NEXT_SYM);
		}
		else
		{
			// NO TERMINO DE TRANSMITIR
		}
	}
	else
	{
		// NO TERMINO DE TRANSMITIR
	}
}

void sendSin0(void)
{
	DAC_setData(sinValues[cont]);

	cont++;
	if(cont == SIN_VALUES)
	{
		cont = 0;
		cycle++;
		if(cycle == 2) // Dos ciclos para el 0
		{
			cycle = 0;
			configTimerSin(SIN0, STOP);
			procesBitStream(NEXT_SYM);
		}
		else
		{
			// NO TERMINO DE TRANSMITIR
		}
	}
	else
	{
		// NO TERMINO DE TRANSMITIR
	}
}

void configTimerSin(uint8_t symbol, bool option)
{
	if(option == START)
	{
		PIT_startTime(symbol);
	}
	else if(option == STOP)
	{
		PIT_stopTime(symbol);
	}
}
/*******************************************************************************
 ******************************************************************************/

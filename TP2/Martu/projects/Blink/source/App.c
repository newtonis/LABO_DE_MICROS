/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "Sensores.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


static void i2c_handler (void);
static SRAWDATA mag;
static SRAWDATA accel;
static bool end = false;


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void i2c_handler (void)
{
	end = true;
}
static read_data m;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{


}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{


	_mqx_ints_FXOS8700CQ_start();
	m.callback = i2c_handler;
	m.pAccelData = &accel;
	m.pMagnData = &mag;

	ReadAccelMagnData(&m);
	while (end == false)
	{

	}
	end = false;

}


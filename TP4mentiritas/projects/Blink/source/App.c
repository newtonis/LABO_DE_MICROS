/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "timer.h"
#include "DisplayBoard.h"
#include "Encoder.h"
#include "Lector.h"
#include "InternalControl.h"
#include "Users.h"
#include "PIT.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
enum{ID_STAGE, BRIGHT_EDIT, PIN_STAGE, CHECKOUT_STAGE, ERROR_STAGE, ID_ERROR_STAGE}; // FSM estados
enum{NOT_IDLE, IDLE}; // IDLE estados

#define ID_LEN			8
#define PIN_LEN			5
#define BRIGHT_LEN		1
#define TEXT_TAB		3

#define ID_WORD_LEN		11
#define PIN_WORD_LEN	8

#define PIN_MENU		1
#define ID_MENU			2

#define ID_TEST			{I_CHAR, D_CHAR, ESPACIO, GUION, GUION, GUION, GUION, GUION, GUION, GUION, GUION}
#define PIN_TEST		{P_CHAR, I_CHAR, N_CHAR, 1, 2, 3, 4, 5}
#define HIDE_PIN_TEST	{P_CHAR, I_CHAR, N_CHAR, GUION, GUION, GUION, GUION, GUION}
#define CHECKOUT_OK_TXT	{ESPACIO, ESPACIO, ESPACIO, ESPACIO, A_CHAR, C_CHAR, C_CHAR, E_CHAR, S_CHAR, O_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}
#define CHECKOUT_ERR_TXT	{ESPACIO, ESPACIO, ESPACIO, ESPACIO, C_CHAR, O_CHAR, D_CHAR, E_CHAR, ESPACIO, E_CHAR, R_CHAR, R_CHAR, O_CHAR, R_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}
#define ID_ERROR_TXT	{ESPACIO, ESPACIO, ESPACIO, ESPACIO, N_CHAR, O_CHAR, ESPACIO, S_CHAR, U_CHAR, C_CHAR, H_CHAR, ESPACIO, I_CHAR, D_CHAR, ESPACIO, ESPACIO, ESPACIO, ESPACIO}

static disp_msj_t id_txt = {ID_TEST, ID_LEN, 2, 1};

static disp_msj_t pin_txt = {PIN_TEST, PIN_LEN, 3, 1};
static disp_msj_t pin_toDisp = {HIDE_PIN_TEST, PIN_LEN, 3, 1}; // Para enviar este con los caracteres ocultos

#define BRIGHT_UNIT		3
#define BRIGHT_DEC		2
static disp_msj_t bright_txt = {{L_CHAR, U_CHAR, 1, 0}, BRIGHT_LEN, 2, 2};

static int checkoutOK_vec[] = CHECKOUT_OK_TXT;
static disp_msj_t checkoutOK_txt = {CHECKOUT_OK_TXT, SIZEOFARR(checkoutOK_vec), 0, 0};

static int checkoutERROR_vec[] = CHECKOUT_ERR_TXT;
static disp_msj_t checkoutERROR_txt = {CHECKOUT_ERR_TXT, SIZEOFARR(checkoutERROR_vec), 0, 0};

static int idError_vec[] = ID_ERROR_TXT;
static disp_msj_t idError_txt = {ID_ERROR_TXT, SIZEOFARR(idError_vec), 0, 0};

static int default_id[ID_LEN] = {1, 2, 3, 4, 5, 6, 7, 8};
static int *aux_id;

static int idle_cnt = 0;
static int fsm = ID_STAGE;
static bool idle_fsm = 0;

static bool t_switch; // si es false es access denied si es true cierra la puerta




//static tim_id_t timerDisp;
//static tim_id_t timerIdle;
//static tim_id_t timerError;
//static tim_id_t timerPestillo;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void displayHandler(void);

void encoderHandler(void);

void idleHandler(void);

void internarHandler(void);

void modifyNumberCode(int motion);

void update_hidePIN(void);

void clearUserInfo(void);

void closeDoor(void);

void accessDenied(void);

void timerSwitch (void);
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void clearUserInfo(void){
	int aux[] = ID_TEST;
	for(int k = 0; k < ID_WORD_LEN; k++){
		id_txt.array[k] = aux[k];
	}
	int aux2[] = PIN_TEST;
	for(int k = 0; k < PIN_WORD_LEN; k++){
		pin_txt.array[k] = aux2[k];
	}
}


void timerSwitch (void)
{
	if(t_switch == true)
	{
		closeDoor();
	}
	else{
		accessDenied();
	}
}

void update_hidePIN(void){
	int aux[] = HIDE_PIN_TEST;
	for(int k = 0; k < PIN_WORD_LEN; k++){
		pin_toDisp.array[k] = aux[k];
	}

	if(DispModType() == NUM_TYPE){
		pin_toDisp.array[DispGetCursor()+TEXT_TAB] = pin_txt.array[DispGetCursor()+TEXT_TAB];
	}
}

void modifyNumberCode(int motion){
	int *numCode;

	if(fsm == ID_STAGE){
		numCode = (id_txt.array)+TEXT_TAB;
	}else if(PIN_STAGE){
		numCode = (pin_txt.array)+TEXT_TAB;
	}


	if(DispModType() == NUM_TYPE){

		if(fsm == BRIGHT_EDIT){
			disp_bright_t aux_bright;
			aux_bright = DispChangeBright(motion);
			bright_txt.array[BRIGHT_DEC] = aux_bright/10;
			bright_txt.array[BRIGHT_UNIT] = aux_bright%10;
		}else{
			switch(motion){
			case RIGHT:

				if(numCode[DispGetCursor()] == GUION){ // Aplica mas que nada para el ID reseteado
					numCode[DispGetCursor()] = 0;
				}else{
					if(numCode[DispGetCursor()] == 9){
						numCode[DispGetCursor()] = 0;
					}else{
						numCode[DispGetCursor()]++;
					}
				}

				break;
			case LEFT:

				if(numCode[DispGetCursor()] == GUION){
					numCode[DispGetCursor()] = 9;
				}else{
					if(numCode[DispGetCursor()] == 0){
						numCode[DispGetCursor()] = 9;
					}else{
						numCode[DispGetCursor()]--;
					}
				}

				break;
			}
		}

	}else if(DispModType() == MENU_TYPE){
		switch(motion){
		case LEFT:
			if(fsm == ID_STAGE){
				fsm = BRIGHT_EDIT;
			}else if(fsm != PIN_STAGE){
				fsm--;
			}
			DispClear();
			break;
		case RIGHT:
			if(fsm == BRIGHT_EDIT){
				fsm = ID_STAGE;
			}else if(fsm != PIN_STAGE){
				fsm++;
			}
			DispClear();
			break;
		}
	}

}

void idleHandler(void){
	switch(idle_fsm){
	case IDLE:
		clearUserInfo();
		fsm = ID_STAGE;
		idle_fsm = !idle_fsm;
		break;
	case NOT_IDLE:
		idle_cnt++;
		if(idle_cnt == 30){
			idle_fsm = IDLE;
		}
		break;
	}

}

void displayHandler (void){

	switch(fsm){
	case ID_STAGE:
		DispShowMsj(id_txt);
		break;
	case PIN_STAGE:
		update_hidePIN();
		DispShowMsj(pin_toDisp);
		break;
	case BRIGHT_EDIT:
		DispShowMsj(bright_txt);
		break;
	case CHECKOUT_STAGE:
		DispShowMsj(checkoutOK_txt);
		break;
	case ERROR_STAGE:
		DispShowMsj(checkoutERROR_txt);
		break;
	case ID_ERROR_STAGE:
		DispShowMsj(idError_txt);
		break;
	}

}

void encoderHandler(void){
	if(fsm <= PIN_STAGE){
		idle_cnt = 0;
		int event = encoderMotionGetEvent();
		switch(event){
		case LEFT:
			modifyNumberCode(event);
			break;
		case RIGHT:
			modifyNumberCode(event);
			break;
		case ENTER:
			DispShiftMsj();
			break;
		}
	}
}


void lectorHandler(void){
	if(fsm == ID_STAGE){
		idle_cnt = 0;
		aux_id = lector_get_PAN();
		for(int k = 0; k < ID_LEN; k++){ // Me quedo solo con la parte que me interesa del PAN
			id_txt.array[k+TEXT_TAB] = aux_id[k];
		}
	}
}

void internarHandler(void){
	switch(internalControlGetEvent()){
	case OK_EVENT:

		if(fsm == ID_STAGE){

			if(validateID((id_txt.array)+TEXT_TAB)){
				fsm = PIN_STAGE;
				DispClear();
			}else{
				fsm = ID_ERROR_STAGE;
				t_switch = false;
				PIT_startTime(3);
				RGBIndicator(RED_INDICATOR);
				DispClear();
			}

		}else if(fsm == PIN_STAGE){
			if(validateUser((id_txt.array)+TEXT_TAB, (pin_txt.array)+TEXT_TAB)){
				fsm = CHECKOUT_STAGE;
				DispClear();
				t_switch = true;
				PIT_startTime(3);
				RGBIndicator(GREEN_INDICATOR);
			}else{
				fsm = ERROR_STAGE;
				DispClear();
				t_switch = false;
				PIT_startTime(3);
				RGBIndicator(RED_INDICATOR);
			}
		}

		break;
	case CANCEL_EVENT:
		fsm = ID_STAGE;
		clearUserInfo();
		DispClear();
		break;
	}
}

void closeDoor(void){
	PIT_stopTime(3);
	RGBIndicator(BLUE_INDICATOR);
	fsm = ID_STAGE;
	DispClear();
	clearUserInfo();
}

void accessDenied(void){
	PIT_stopTime(3);
	RGBIndicator(BLUE_INDICATOR);
	fsm = ID_STAGE;
	DispClear();
	clearUserInfo();
}

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	aux_id = default_id; // Asignacion de ID por default

	initUser();

    timerInit();



    PIT_init();


	internalControlInit(internarHandler);

    encoderInit(encoderHandler);

    //timerDisp = timerGetId();
    //timerStart(timerDisp, TIMER_MS2TICKS(1), TIM_MODE_PERIODIC, displayHandler);
    PIT_configTimer(0, 10000, displayHandler);

    //timerIdle = timerGetId();
    //timerStart(timerIdle, TIMER_MS2TICKS(3000), TIM_MODE_PERIODIC, idleHandler);
    //PIT_configTimer(1, 1180, idleHandler);

    //timerPestillo = timerGetId(); // Lo disparo cuando se pueda abrir
    //timerStart(timerPestillo, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, closeDoor);
    //PIT_configTimer(2, 1180, closeDoor);

    //timerError = timerGetId(); // Lo dispador cuando no es valido el usuario
    //timerStart(timerError, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, accessDenied);
    PIT_configTimer(3, 200000000,  timerSwitch);

    DispBoard_Init();

    DispClear();

    lectorInit(lectorHandler);

    RGBIndicator(BLUE_INDICATOR);

    PIT_startTime(0);

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	switch(fsm){
	case ID_STAGE:
		if(LedStatus_GetState() != 1){
			LedStatus_Write(1);
		}
		break;
	case PIN_STAGE:
		// Se modifica solo con el encoder
		if(LedStatus_GetState() != 2){
			LedStatus_Write(2);
		}
		break;
	case BRIGHT_EDIT:
		// Tambien se modifica solo con el encoder
		break;
	case CHECKOUT_STAGE:
		if(LedStatus_GetState() != 3){
			LedStatus_Write(3);
		}
		break;
	case ERROR_STAGE:
		// Error en el PIN ingresado
		break;
	case ID_ERROR_STAGE:
		// La ID ingresada no existe
		break;
	}

}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/

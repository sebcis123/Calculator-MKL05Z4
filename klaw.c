#include "klaw.h"
volatile uint8_t Pin_press[4][4] = { 							// deklaracja tablicy przechowującej stan
																		{0,0,0,0},
																		{0,0,0,0},
																		{0,0,0,0},
																		{0,0,0,0} };
void Klaw_Init(void)  // Inicjalizacja klawiatury
{
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;  // Włączenie zegarów dla portów 
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	PORTB->PCR[R4] |= PORT_PCR_MUX(1);		// Ustawienie pinów R1-R4 jako GPIO 
	PORTB->PCR[R3] |= PORT_PCR_MUX(1);
	PORTB->PCR[R2] |= PORT_PCR_MUX(1);
	PORTB->PCR[R1] |= PORT_PCR_MUX(1);

	PTB->PDDR |= (1 << R1) | (1 << R2) | (1 << R3) | (1 << R4);  // Ustawia piny jako wyjścia
  PTB->PCOR = (1 << R1) | (1 << R2) | (1 << R3) | (1 << R4);  // Ustawia na nich stan niski
	
	PORTA->PCR[C4] |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;  // Ustawia piny C1-C4 jako GPIO, włącza pull-up - gdy żaden klawisz nie jest wciśnięty, kolumny są na poziomie wysokim
	PORTA->PCR[C3] |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTA->PCR[C2] |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTA->PCR[C1] |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PTA->PDDR &= ~((1 << C1) | (1 << C2) | (1 << C3) | (1 << C4));  // Ustawia piny C1-C4 jako wejścia
}


void Klaw_Inter(void)  // Konfiguracja przerwań
{
	PORTA -> PCR[C3] |= PORT_PCR_IRQC(0xa);		// 0x8 - poziom "0"; 0x9 - zbocze narastające; 0xa - zbocze opadające; 0xb - obydwa zbocza
	PORTA -> PCR[C2] |= PORT_PCR_IRQC(0xa);		// Aktywuje przerwania na zboczu opadającym
	PORTA -> PCR[C1] |= PORT_PCR_IRQC(0xa);
	PORTA -> PCR[C4] |= PORT_PCR_IRQC(0xa);
	
	NVIC_SetPriority(PORTA_IRQn, 3); 
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTA_IRQn);

}
#include "MKL05Z4.h"
#include "frdm_bsp.h"
#include "lcd1602.h"
#include "klaw.h"
#include "tsi.h" // Dodaj bibliotekê do slidera
#include <stdio.h>

#define LED_R (1 << 8)
#define LED_G (1 << 9)
#define LED_B (1 << 10)
#define ROWS 4
#define COLS 4

uint8_t pressed = 0; // zmienna wskazuje, czy przycisk zosta³ wciœniêty

// Tablica przypisuj¹ca symbole do przycisków
const char keymap[4][4] = {
    {'7', '8', '9', '/'}, // Wiersz 1
    {'4', '5', '6', '*'}, // Wiersz 2
    {'1', '2', '3', '-'}, // Wiersz 3
    {'0', '.', '=', '+'}  // Wiersz 4
};

void PORTA_IRQHandler(void) 	// Obs³uga przerwañ klawiatury
{
    const uint8_t rows[4] = {R1, R2, R3, R4}; // Definicja numerów linii dla wierszy klawiatury
    const int cols[4]     = {C1_MASK, C2_MASK, C3_MASK, C4_MASK}; // Definicja masek bitowych dla kolumn klawiatury
    uint32_t buf;

    buf = PORTA->ISFR & (C4_MASK | C3_MASK | C2_MASK | C1_MASK); // sprawdza, która kolumna klawiatury wywo³a³a przerwanie i zapisuje

    switch (buf) {
        case C4_MASK:
            DELAY(50); // Minimalizacja drgañ styków
            if (!(PTA->PDIR & C4_MASK)) { // Sprawdzenie, czy przycisk nadal jest wciœniêty
                DELAY(50);
								pressed = 1; // Ustawienie flagi informuj¹cej o wciœniêtym przycisku
                if (!(PTA->PDIR & C4_MASK)) { // Ponowne sprawdzenie (eliminacja b³êdów)
                    PTB->PSOR = (1 << R1) | (1 << R2) | (1 << R3) | (1 << R4); // Aktywacja wszystkich wierszy

                    for (int row = 0; row < 4; row++) { // Iteracja przez ka¿dy wiersz, sprawdzanie który wiersz zsota³ aktywowany
                        PTB->PCOR = (1 << rows[row]); // Ustawienie danego wiersza na niski stan
                        DELAY(10);

                        if (!(PTA->PDIR & C4_MASK)) { // Sprawdzenie, czy kolumna nadal jest aktywna
                            Pin_press[row][COL1] = 1; // Zapisanie wciœniêtego klawisza
                            break;
                        }

                        PTB->PSOR = (1 << rows[row]); // Przywrócenie danego wiersza do stanu wysokiego
                    }

                    PTB->PCOR = (1 << R1) | (1 << R2) | (1 << R3) | (1 << R4); // Dezaktywacja wszystkich wierszy
                }
            }
            break;

        case C3_MASK:
            DELAY(50);
            if (!(PTA->PDIR & C3_MASK)) {
                DELAY(50);
							pressed = 1;
                if (!(PTA->PDIR & C3_MASK)) {
                    PTB->PSOR = (1 << R1) | (1 << R2) | (1 << R3) | (1 << R4);

                    for (int row = 0; row < 4; row++) {
                        PTB->PCOR = (1 << rows[row]);
                        DELAY(10);

                        if (!(PTA->PDIR & C3_MASK)) {
                            Pin_press[row][COL2] = 1;
                            break;
                        }

                        PTB->PSOR = (1 << rows[row]);
                    }

                    PTB->PCOR = (1 << R1) | (1 << R2) | (1 << R3) | (1 << R4);
                }
            }
            break;

        case C2_MASK:
            DELAY(50);
            if (!(PTA->PDIR & C2_MASK)) {
                DELAY(50);
							pressed = 1;
                if (!(PTA->PDIR & C2_MASK)) {
                    PTB->PSOR = (1 << R1) | (1 << R2) | (1 << R3) | (1 << R4);

                    for (int row = 0; row < 4; row++) {
                        PTB->PCOR = (1 << rows[row]);
                        DELAY(10);

                        if (!(PTA->PDIR & C2_MASK)) {
                            Pin_press[row][COL3] = 1;
                            break;
                        }

                        PTB->PSOR = (1 << rows[row]);
                    }

                    PTB->PCOR = (1 << R1) | (1 << R2) | (1 << R3) | (1 << R4);
                }
            }
            break;

        case C1_MASK:
            DELAY(50);
            if (!(PTA->PDIR & C1_MASK)) {
                DELAY(50);
							pressed = 1;
                if (!(PTA->PDIR & C1_MASK)) {
                    PTB->PSOR = (1 << R1) | (1 << R2) | (1 << R3) | (1 << R4);

                    for (int row = 0; row < 4; row++) {
                        PTB->PCOR = (1 << rows[row]);
                        DELAY(10);

                        if (!(PTA->PDIR & C1_MASK)) {
                            Pin_press[row][COL4] = 1;
                            break;
                        }

                        PTB->PSOR = (1 << rows[row]);
                    }

                    PTB->PCOR = (1 << R1) | (1 << R2) | (1 << R3) | (1 << R4);
                }
            }
            break;

        default:
            break;
    }

    PORTA->ISFR |= C4_MASK | C3_MASK | C2_MASK | C1_MASK; 	// Wyczyszczenie flagi przerwania
    NVIC_ClearPendingIRQ(PORTA_IRQn); 	// usuwanie zg³oszonych przerwañ
}


char find_pressed_key() { 		// Przeszukuje tablicê Pin_press, aby znaleŸæ aktywowany przycisk
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (Pin_press[i][j] == 1) { 	// SprawdŸ, czy przycisk jest wciœniêty
							Pin_press[i][j] = 0;      	// Zresetuj stan przycisku 
							return keymap[j][i]; 			// Zwróæ odpowiadaj¹c¹ wartoœæ klawisza
            }
        }
    }
    return '\0'; 	// Zwróæ znak null, jeœli ¿aden przycisk nie jest wciœniêty
}


float strtof_(const char *str) {  // Konwertowanie string na float
    while (*str == ' ') str++; // Ignorowanie wiod¹cych spacji
    
    int sign = 1;           // Obs³uga znaku
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    unsigned long int_part = 0;   // Czêœæ ca³kowita
    while (*str >= '0' && *str <= '9') {  // SprawdŸ, czy to liczba
        int_part = int_part * 10 + (*str - '0');  // Dla ka¿dej liczby pomnó¿ czêœæ ca³kowit¹ przez 10 i dodaj liczbê z wejœcia 
        str++;
    }
    
    float fraction = 0.0f;  // Czêœæ u³amkowa
    if (*str == '.') {      // Jeœli w ci¹gu jest kropka
        str++;
        float divisor = 10.0f;
        while (*str >= '0' && *str <= '9') {    // Podobnie jak powy¿ej, ale dziel przez 10, 100, 1000 itd.
            fraction += (*str - '0') / divisor;
            divisor *= 10.0f;
            str++;
        }
    }
    
    return sign * (int_part + fraction);    // Po³¹cz znak z czêœci¹ ca³kowit¹ i u³amkow¹
}

uint32_t truncate_trailing_zeros(uint32_t num) {    // Usuñ koñcowe zera, aby wygl¹da³o ³adniej na wyœwietlaczu
    while (num % 10 == 0 && num != 0) {
        num /= 10;
    }
    return num;
}

void display_num(float *f1){          // Wyœwietl numer
  float f_in = *f1;										// Pobiera wskaŸnik f1 do liczby float
  LCD1602_ClearAll();
  LCD1602_SetCursor(0,0);
  uint32_t f1_int, f1_frac;           // Czêœæ ca³kowita i u³amkowa
  int8_t sign = (f_in < 0) ? -1 : 1;  // Okreœl znak
  f_in = f_in * sign;  	// Ustaw f1 jako dodatnie dla ekstrakcji
  static char disp[16] = {0x20};      // Musi byæ statyczne, w przeciwnym razie funkcja LCD1602_Print zawiesza siê 
  f1_int = (uint32_t) f_in;  // Pobiera czêœæ ca³kowit¹
	f1_frac = (uint32_t) (((f_in - f1_int) * 1000) + 0.5f);       // Oblicza czêœæ u³amkow¹
  f1_frac = truncate_trailing_zeros(f1_frac);                       // Pozb¹dŸ siê niepotrzebnych zer
  sprintf(disp, "%s%d.%d", (sign == -1) ? "-" : "", f1_int, f1_frac); // Po³¹cz wszystko
  LCD1602_Print(disp);
}


void compute(float *result, float *number, char operation){       // Oblicz wynik na podstawie operacji, g³ówna funckja obliczeniowa kalkulatora

	switch (operation) {
		case '+': *result += *number; break; 	// Dodaje, odejmuje lub mno¿y *number do *result i zapisuje wynik w result
		case '-': *result -= *number; break;
		case '*': *result *= *number; break;
    case '/': 
        if (*number != 0.0) {
            *result /= *number;
        } else {
            LCD1602_ClearAll();
            LCD1602_SetCursor(0, 0);
            LCD1602_Print("error"); // Wyœwietl "error" na LCD w przypadku dzielenia przez zero
						DELAY(35000);
            break;  
        }
    case '=': break;    // Nic nie rób
		default:              // Nigdy nie powinno siê zdarzyæ, ale kto wie 
			LCD1602_ClearAll();
			LCD1602_SetCursor(0,0);
			LCD1602_Print("Invalid operator"); break;
	}
}


int main(void)
{
    char buff[16] = {0x20};               // Bufor wejœæiowy
    float number = 0.0;                   // Liczba wejœciowa
    float result = 0.0;                   // Przechowywanie wyniku obliczeñ 
    uint8_t array_pos = 0;                // Pozycja w buforze
    char last_operation = '+';            // Iicjalizuj operacjê
    uint8_t first_operation = TRUE;       // Jeœli to pierwsza operacja, zapisz liczbê do wyniku

    Klaw_Init();                          // Inicjalizuj peryferia
    Klaw_Inter();
    LCD1602_Init();
    LCD1602_Backlight(1);
    LCD1602_ClearAll();
    LCD1602_SetCursor(0, 0);
    TSI_Init(); 													// Inicjalizacja slidera
		
    LCD1602_Print("0");   
    while (1) { 		// G³ówna pêtla programu
        
			// Obs³uga klawiatury      
			if (pressed) {
				LCD1602_SetCursor(0, 0);
				char symbol = find_pressed_key(); // Odczytaj symbol z tablicy
        static uint8_t dot = FALSE;
				if((symbol >= '0' && symbol <= '9') || symbol == '.'){    // Tylko te znaki s¹ wa¿ne i tworz¹ liczbê wejœciow¹
					if(array_pos < 16){                               // Unikaj przepe³nienia
            if(symbol != '.'){                              
					    buff[array_pos] = symbol;                     // Zapisz wciœniêty znak do bufora wejœciowego
					    array_pos++;                                  // Zwiêksz pozycjê w buforze
            }else if(!dot && array_pos != 0){               // Kropka nie mo¿e byæ pierwszym znakiem i mo¿e wyst¹piæ tylko raz w strumieniu wejœciowym
              buff[array_pos] = '.';
              dot = TRUE;
              array_pos++;
            }
					  number = strtof_(buff);             // Konwertuj ci¹g wejœciowy na liczbê zmiennoprzecinkow¹
				    LCD1602_Print(buff);                // Wyœwietl bufor, a nie liczbê, aby koñcowe zera nie by³y widoczne	
          }	
				}else{                                                    // Obs³uga przycisków operacji
					for (uint8_t i = 0; i < 16; i++){                       // Wyczyœæ bufor wejœciowy spacjami (hex 0x20)
  				  buff[i] = 0x20;
					}
          dot = FALSE;
					array_pos = 0;                                  // Zresetuj pozycjê w buforze
          if (symbol == '=' && last_operation != '=') {   // Nie obliczaj tego samego 2 razy
            compute(&result, &number, last_operation);  // Obliczenie '=' nic nie daje, wiêc obliczamy ostatni¹ operacjê
            if(last_operation == '*' || last_operation == '/')   // Jeœli ustawimy 'number' na '0.0', przy kolejnym mno¿eniu otrzymalibyœmy 0
              number = 1.0;
            else if(last_operation == '+' || last_operation == '-') 
              number = 0.0;                         // To nie zmienia 'result' dla dodawania i odejmowania
          } else if(first_operation){                 // eœli to pierwsza operacja, zapisz liczbê do wyniku 
            result = number;
            first_operation = FALSE;                // Zresetuj flagê
          }else if(last_operation != '='){
            compute(&result, &number, symbol);      // Oblicz kolejne operacje 
          }
          last_operation = symbol;                  // Zapisz ostatni¹ operacjê 
					display_num(&result);                     // Wyœwietl wynik  
				  }
				pressed = 0;                                // Zresetuj flagê przerwania klawiatury
		  }

        // Obs³uga slidera
        uint8_t slider_value = TSI_ReadSlider();  // Odczytaj wartoœæ slidera (0–100)
        if (slider_value >= 67) {                 // Jeœli slider jest w trzeciej czêœci (67–100) 
        result = 0.0;                             // Zresetuj wynik i liczbê wejœciow¹ do zera
        number = 0.0;
				for (uint8_t i = 0; i < 16; i++){                       // Wyczyœc bufor (hex 0x20)
  				buff[i] = 0x20;
				}		
				array_pos = 0;
        first_operation = TRUE;                   // Ustaw flagê pierwszej operacji na true 
        display_num(&result);                     // Wyœwietl zero 

        }
    }
}

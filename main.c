#include "MKL05Z4.h"
#include "frdm_bsp.h"
#include "lcd1602.h"
#include "klaw.h"
#include "tsi.h" // Dodaj bibliotek� do slidera
#include <stdio.h>

#define LED_R (1 << 8)
#define LED_G (1 << 9)
#define LED_B (1 << 10)
#define ROWS 4
#define COLS 4

uint8_t pressed = 0; // zmienna wskazuje, czy przycisk zosta� wci�ni�ty

// Tablica przypisuj�ca symbole do przycisk�w
const char keymap[4][4] = {
    {'7', '8', '9', '/'}, // Wiersz 1
    {'4', '5', '6', '*'}, // Wiersz 2
    {'1', '2', '3', '-'}, // Wiersz 3
    {'0', '.', '=', '+'}  // Wiersz 4
};

void PORTA_IRQHandler(void) 	// Obs�uga przerwa� klawiatury
{
    const uint8_t rows[4] = {R1, R2, R3, R4}; // Definicja numer�w linii dla wierszy klawiatury
    const int cols[4]     = {C1_MASK, C2_MASK, C3_MASK, C4_MASK}; // Definicja masek bitowych dla kolumn klawiatury
    uint32_t buf;

    buf = PORTA->ISFR & (C4_MASK | C3_MASK | C2_MASK | C1_MASK); // sprawdza, kt�ra kolumna klawiatury wywo�a�a przerwanie i zapisuje

    switch (buf) {
        case C4_MASK:
            DELAY(50); // Minimalizacja drga� styk�w
            if (!(PTA->PDIR & C4_MASK)) { // Sprawdzenie, czy przycisk nadal jest wci�ni�ty
                DELAY(50);
								pressed = 1; // Ustawienie flagi informuj�cej o wci�ni�tym przycisku
                if (!(PTA->PDIR & C4_MASK)) { // Ponowne sprawdzenie (eliminacja b��d�w)
                    PTB->PSOR = (1 << R1) | (1 << R2) | (1 << R3) | (1 << R4); // Aktywacja wszystkich wierszy

                    for (int row = 0; row < 4; row++) { // Iteracja przez ka�dy wiersz, sprawdzanie kt�ry wiersz zsota� aktywowany
                        PTB->PCOR = (1 << rows[row]); // Ustawienie danego wiersza na niski stan
                        DELAY(10);

                        if (!(PTA->PDIR & C4_MASK)) { // Sprawdzenie, czy kolumna nadal jest aktywna
                            Pin_press[row][COL1] = 1; // Zapisanie wci�ni�tego klawisza
                            break;
                        }

                        PTB->PSOR = (1 << rows[row]); // Przywr�cenie danego wiersza do stanu wysokiego
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
    NVIC_ClearPendingIRQ(PORTA_IRQn); 	// usuwanie zg�oszonych przerwa�
}


char find_pressed_key() { 		// Przeszukuje tablic� Pin_press, aby znale�� aktywowany przycisk
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (Pin_press[i][j] == 1) { 	// Sprawd�, czy przycisk jest wci�ni�ty
							Pin_press[i][j] = 0;      	// Zresetuj stan przycisku 
							return keymap[j][i]; 			// Zwr�� odpowiadaj�c� warto�� klawisza
            }
        }
    }
    return '\0'; 	// Zwr�� znak null, je�li �aden przycisk nie jest wci�ni�ty
}


float strtof_(const char *str) {  // Konwertowanie string na float
    while (*str == ' ') str++; // Ignorowanie wiod�cych spacji
    
    int sign = 1;           // Obs�uga znaku
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    unsigned long int_part = 0;   // Cz�� ca�kowita
    while (*str >= '0' && *str <= '9') {  // Sprawd�, czy to liczba
        int_part = int_part * 10 + (*str - '0');  // Dla ka�dej liczby pomn� cz�� ca�kowit� przez 10 i dodaj liczb� z wej�cia 
        str++;
    }
    
    float fraction = 0.0f;  // Cz�� u�amkowa
    if (*str == '.') {      // Je�li w ci�gu jest kropka
        str++;
        float divisor = 10.0f;
        while (*str >= '0' && *str <= '9') {    // Podobnie jak powy�ej, ale dziel przez 10, 100, 1000 itd.
            fraction += (*str - '0') / divisor;
            divisor *= 10.0f;
            str++;
        }
    }
    
    return sign * (int_part + fraction);    // Po��cz znak z cz�ci� ca�kowit� i u�amkow�
}

uint32_t truncate_trailing_zeros(uint32_t num) {    // Usu� ko�cowe zera, aby wygl�da�o �adniej na wy�wietlaczu
    while (num % 10 == 0 && num != 0) {
        num /= 10;
    }
    return num;
}

void display_num(float *f1){          // Wy�wietl numer
  float f_in = *f1;										// Pobiera wska�nik f1 do liczby float
  LCD1602_ClearAll();
  LCD1602_SetCursor(0,0);
  uint32_t f1_int, f1_frac;           // Cz�� ca�kowita i u�amkowa
  int8_t sign = (f_in < 0) ? -1 : 1;  // Okre�l znak
  f_in = f_in * sign;  	// Ustaw f1 jako dodatnie dla ekstrakcji
  static char disp[16] = {0x20};      // Musi by� statyczne, w przeciwnym razie funkcja LCD1602_Print zawiesza si� 
  f1_int = (uint32_t) f_in;  // Pobiera cz�� ca�kowit�
	f1_frac = (uint32_t) (((f_in - f1_int) * 1000) + 0.5f);       // Oblicza cz�� u�amkow�
  f1_frac = truncate_trailing_zeros(f1_frac);                       // Pozb�d� si� niepotrzebnych zer
  sprintf(disp, "%s%d.%d", (sign == -1) ? "-" : "", f1_int, f1_frac); // Po��cz wszystko
  LCD1602_Print(disp);
}


void compute(float *result, float *number, char operation){       // Oblicz wynik na podstawie operacji, g��wna funckja obliczeniowa kalkulatora

	switch (operation) {
		case '+': *result += *number; break; 	// Dodaje, odejmuje lub mno�y *number do *result i zapisuje wynik w result
		case '-': *result -= *number; break;
		case '*': *result *= *number; break;
    case '/': 
        if (*number != 0.0) {
            *result /= *number;
        } else {
            LCD1602_ClearAll();
            LCD1602_SetCursor(0, 0);
            LCD1602_Print("error"); // Wy�wietl "error" na LCD w przypadku dzielenia przez zero
						DELAY(35000);
            break;  
        }
    case '=': break;    // Nic nie r�b
		default:              // Nigdy nie powinno si� zdarzy�, ale kto wie 
			LCD1602_ClearAll();
			LCD1602_SetCursor(0,0);
			LCD1602_Print("Invalid operator"); break;
	}
}


int main(void)
{
    char buff[16] = {0x20};               // Bufor wej��iowy
    float number = 0.0;                   // Liczba wej�ciowa
    float result = 0.0;                   // Przechowywanie wyniku oblicze� 
    uint8_t array_pos = 0;                // Pozycja w buforze
    char last_operation = '+';            // Iicjalizuj operacj�
    uint8_t first_operation = TRUE;       // Je�li to pierwsza operacja, zapisz liczb� do wyniku

    Klaw_Init();                          // Inicjalizuj peryferia
    Klaw_Inter();
    LCD1602_Init();
    LCD1602_Backlight(1);
    LCD1602_ClearAll();
    LCD1602_SetCursor(0, 0);
    TSI_Init(); 													// Inicjalizacja slidera
		
    LCD1602_Print("0");   
    while (1) { 		// G��wna p�tla programu
        
			// Obs�uga klawiatury      
			if (pressed) {
				LCD1602_SetCursor(0, 0);
				char symbol = find_pressed_key(); // Odczytaj symbol z tablicy
        static uint8_t dot = FALSE;
				if((symbol >= '0' && symbol <= '9') || symbol == '.'){    // Tylko te znaki s� wa�ne i tworz� liczb� wej�ciow�
					if(array_pos < 16){                               // Unikaj przepe�nienia
            if(symbol != '.'){                              
					    buff[array_pos] = symbol;                     // Zapisz wci�ni�ty znak do bufora wej�ciowego
					    array_pos++;                                  // Zwi�ksz pozycj� w buforze
            }else if(!dot && array_pos != 0){               // Kropka nie mo�e by� pierwszym znakiem i mo�e wyst�pi� tylko raz w strumieniu wej�ciowym
              buff[array_pos] = '.';
              dot = TRUE;
              array_pos++;
            }
					  number = strtof_(buff);             // Konwertuj ci�g wej�ciowy na liczb� zmiennoprzecinkow�
				    LCD1602_Print(buff);                // Wy�wietl bufor, a nie liczb�, aby ko�cowe zera nie by�y widoczne	
          }	
				}else{                                                    // Obs�uga przycisk�w operacji
					for (uint8_t i = 0; i < 16; i++){                       // Wyczy�� bufor wej�ciowy spacjami (hex 0x20)
  				  buff[i] = 0x20;
					}
          dot = FALSE;
					array_pos = 0;                                  // Zresetuj pozycj� w buforze
          if (symbol == '=' && last_operation != '=') {   // Nie obliczaj tego samego 2 razy
            compute(&result, &number, last_operation);  // Obliczenie '=' nic nie daje, wi�c obliczamy ostatni� operacj�
            if(last_operation == '*' || last_operation == '/')   // Je�li ustawimy 'number' na '0.0', przy kolejnym mno�eniu otrzymaliby�my 0
              number = 1.0;
            else if(last_operation == '+' || last_operation == '-') 
              number = 0.0;                         // To nie zmienia 'result' dla dodawania i odejmowania
          } else if(first_operation){                 // e�li to pierwsza operacja, zapisz liczb� do wyniku 
            result = number;
            first_operation = FALSE;                // Zresetuj flag�
          }else if(last_operation != '='){
            compute(&result, &number, symbol);      // Oblicz kolejne operacje 
          }
          last_operation = symbol;                  // Zapisz ostatni� operacj� 
					display_num(&result);                     // Wy�wietl wynik  
				  }
				pressed = 0;                                // Zresetuj flag� przerwania klawiatury
		  }

        // Obs�uga slidera
        uint8_t slider_value = TSI_ReadSlider();  // Odczytaj warto�� slidera (0�100)
        if (slider_value >= 67) {                 // Je�li slider jest w trzeciej cz�ci (67�100) 
        result = 0.0;                             // Zresetuj wynik i liczb� wej�ciow� do zera
        number = 0.0;
				for (uint8_t i = 0; i < 16; i++){                       // Wyczy�c bufor (hex 0x20)
  				buff[i] = 0x20;
				}		
				array_pos = 0;
        first_operation = TRUE;                   // Ustaw flag� pierwszej operacji na true 
        display_num(&result);                     // Wy�wietl zero 

        }
    }
}

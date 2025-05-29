#ifndef KLAW_H
#define KLAW_H

#include "MKL05Z4.h"
#define C4_MASK	(1<<7)		// PTA7
#define C3_MASK	(1<<10)		// PTA10
#define C2_MASK	(1<<11)		// PTA11
#define C1_MASK	(1<<12)		// PTA12
#define R4_MASK	(1<<6)		// PTB6
#define R3_MASK	(1<<7)		// PTB7
#define R2_MASK	(1<<0)		// PTB0
#define R1_MASK	(1<<1)		// PTB1
#define C4	7						// Numer bitu dla klawisza S1
#define C3	10						// Numer bitu dla klawisza S2
#define C2	11						// Numer bitu dla klawisza S3
#define C1	12						// Numer bitu dla klawisza S4
#define R4	6						// Numer bitu dla klawisza S1
#define R3	7						// Numer bitu dla klawisza S2
#define R2	0						// Numer bitu dla klawisza S3
#define R1	1
#define COL1 0
#define COL2 1
#define COL3 2
#define COL4 3
#define ROW1 0
#define ROW2 1
#define ROW3 2
#define ROW4 3

extern volatile uint8_t Pin_press[4][4];
void Klaw_Init(void);
void Klaw_Inter(void);

#endif  /* KLAW_H */

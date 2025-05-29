# MKL05Z4 Embedded Calculator
A complete calculator implementation for the NXP FRDM-KL05Z development board, featuring floating-point arithmetic with interrupt-driven input and I²C display.
## Key Features
- Precise calculations: Supports +, -, ×, ÷ operations with floating-point numbers, 3 decimal place accuracy
- Interrupt-driven architecture:
  - Matrix keypad via GPIO interrupts
  - Capacitive touch slider using TSI module
- Clean interface: 16×2 LCD with I²C
- Error handling:
  - Division by zero detection
  - Input overflow protection
- Reset function: Slide-to-clear using capacitive sensor
## Hardware Components
- Core: NXP MKL05Z32VFM4 (ARM Cortex-M0+)
- Input: Custom 4×4 matrix keypad
- Display: 1602 LCD via I²C
- Slider: Built-in FRDM-KL05Z TSI

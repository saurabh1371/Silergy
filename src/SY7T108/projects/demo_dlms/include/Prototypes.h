#include <stdint.h>     // Standard integer definitions.
void SetWakeSources(void);
extern uint32_t NVRAM_Push_Buton_Disp;
extern uint8_t PushButtonCommMode;
extern uint16_t Communication_Enable_Counter;
extern uint32_t ce_ver[];
extern uint32_t WakeFromRegisterCopy;
extern void Display_Version(void);
extern void TaskAutoScroll(void);
extern void lcd_init_1(void);
void _1_SecFunction(void);
extern uint8_t _500mSecCounter;
extern  uint8_t LCD_DisplayParm;
void PushButtonWakeDisplay(void);
extern uint8_t WakeFromPushButtonFlag;
extern uint8_t PowerOnSec;
extern uint8_t LCD_PushButton_Parm,PushButtonTimeOut,PushButtonDisplayFlag,NoOfSeconds;
extern int Sqrt_v0sqsum_240V;
void Timer1_Interrupt(void);
extern int8_t Display_Complete;
 void PushButton_init_eeprom(void);
void CalDisplay(void);
extern uint8_t CalDisplayVar;





#ifndef SERVO_H_
#define SERVO_H_

#define Kali_Szybko_Lewo -2 
#define Kali_Lewo -1
#define Kali_Stoj 0
#define Kali_Prawo 1
#define Kali_Szybko_Prawo 2

#define R_FAST 65
#define R_SLOW 47 
#define S_STOP 42
#define L_FAST 23
#define L_SLOW 38

void servo_Configure_Servo();
void servo_Kali_Pchaj(int Kali_co);
void servo_Rotate_By(int direction, int interval);
void servo_delay(int interval);
#endif /* SERVO_H_ */
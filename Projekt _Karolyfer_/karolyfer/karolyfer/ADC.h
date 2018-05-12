/*
 * ADC.h
 *
 * Created: 2018/03/23 17:35:21
 *  Author: Dondu
 */ 


#ifndef ADC_H_
#define ADC_H_

extern volatile long avg;
extern int Check;

void ADC_Configuration();
int ADC_Read_Current();



#endif /* ADC_H_ */

#include "Control.h"
#include "onewire.h"
#include "ds18b20.h"
#include "UDP.h"
#include "servo.h"
#include "ADC.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

const char Control_Left[]			= "Command:Left";
const char Control_Right[]			= "Command:Right";
const char Control_Stop[]			= "Command:Stop";
const char Control_MAX[]			= "Command:MAX";
const char Control_Temperature[]	= "Command:Temperature";
const char Control_Help[]			= "Command:Help";
const char Control_Current_Prefix[]	= "Command:";

int Control_State = IDLE;
int current = STOP;

/**  TO DO:
*	Pomiary pradu jakos dzialajo,
*	Cza sterowac i kalibrowac serwo
*	Funkcje control zamienic na taka ktora obraca kaloryfer max lewo/ max prawo
*/

void Control_issue_command()
{
	ADC_Read_Current();
	Check = 1;
	//char buffer[32];
	if(Control_State)
	{
		if (UDP_try(Control_Left)) 
		{
			if(1)
			{
			UDP_send_UDP("Counter clock-wise direction initiated \n");
			servo_Kali_Pchaj(Kali_Szybko_Lewo);
			current = LEFT;
			//servo_Rotate_By(Kali_Szybko_Lewo, 500);
			//current = STOP;
			return;	
			}
			else UDP_send_UDP("Servo is already moving in that direction!\n");
		}
		else if (UDP_try(Control_Right)) 
		{
			if(1)
			{
				UDP_send_UDP("Clock-wise direction initiated \n"); 
				servo_Kali_Pchaj(Kali_Szybko_Prawo);
				current = RIGHT;
				//servo_Rotate_By(Kali_Szybko_Prawo, 500);
				//current = STOP;
				return;
			}
			else UDP_send_UDP("Servo is already moving in that direction!\n");
		}
		else if (UDP_try(Control_Stop)) 
		{
			if(1)
			{
				UDP_send_UDP("Servo has been stopped \n"); 
				//servo_Kali_Pchaj(Kali_Stoj);
				servo_Rotate_By(Kali_Stoj, 500);
				current = STOP;
				return;
			}
			else UDP_send_UDP("Servo isn't moving\n");
		}
		else if (UDP_try(Control_Help)) 
		{
			UDP_send_UDP("Left -> issuing Counter clock-wise movement \n, Right -> issuing clock-wise movement \n, Stop -> issuing stop command \n, Help -> Help menu \n"); 
		}
		UDP_delay();
	}	
}

void Control_command()
{
	if(Control_State)
	{
		char* issue = memmem(Module_Response, Module_Response_Index+1, Control_Current_Prefix, strlen(Control_Current_Prefix));
		if(issue == Control_Left)
		{
			if(current != LEFT)
			{
				UDP_send_UDP("Counter clock-wise direction initiated \n");
				//servo_Kali_Pchaj(Kali_Szybko_Lewo);
				servo_Rotate_By(Kali_Szybko_Lewo, 500);
				current = LEFT;
				return;
			}
			else UDP_send_UDP("Servo is already moving in that direction!\n");
		}
		else if(issue == Control_Right)
		{
			if(current != RIGHT)
			{
				UDP_send_UDP("Clock-wise direction initiated \n");
				//servo_Kali_Pchaj(Kali_Szybko_Prawo);
				servo_Rotate_By(Kali_Szybko_Prawo, 500);
				current = RIGHT;
				return;
			}
			else UDP_send_UDP("Servo is already moving in that direction!\n");
		}
		else if(issue == Control_Stop)
		{
			if(current != STOP)
			{
				UDP_send_UDP("Servo has been stopped \n");
				//servo_Kali_Pchaj(Kali_Stoj);
				servo_Rotate_By(Kali_Stoj, 500);
				current = STOP;
				return;
			}
			else UDP_send_UDP("Servo isn't moving\n");
		}
		else if(issue == Control_Help)
		{
			UDP_send_UDP("Left -> issuing Counter clock-wise movement \n, Right -> issuing clock-wise movement \n, Stop -> issuing stop command \n, Help -> Help menu \n");
		}
		UDP_delay();
	}
}

void Control_check_command()
{
	if(Control_State)
	{
		//char* issue = memmem(Module_Response, Module_Response_Index+1, Control_Current_Prefix, strlen(Control_Current_Prefix));
		///*
		//issue = command:xxxxxxxxx;
		//poprawna sk³adnia:
		//np: command:set 5
		//*/
		//if(issue)
		//{
			//
		//}		
	}
}
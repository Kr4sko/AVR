#ifndef CONTROL_H_
#define CONTROL_H_

#define IDLE 1
#define BUSY 0

#define LEFT 0
#define STOP 1
#define RIGHT 2

extern const char Control_Left[];
extern const char Control_Right[];
extern const char Control_Stop[];
extern const char Control_Radiator[];
extern const char Control_Temperature[];
extern const char Control_Help[];

extern int Control_State;

void Control_issue_command();

//Controls command is supposed to work more efficiently, need online testing
void Control_command();

void Control_check_command();
#endif /* CONTROL_H_ */
#ifndef __CONTROL_H
#define __CONTROL_H

#include <PCF857X.h>

#define VALVE1		0
#define VALVE2		1
#define VALVE3		2
#define VALVE4		3
#define VALVE5		4
#define VALVE6		5
#define VALVE7		6
#define VALVE8		7
#define VALVE9		8
#define VALVE10		9
#define VALVE11		10
#define VALVE12		11
#define VALVE13		12
#define VALVE14		13
#define VALVE15		14
#define VALVE16		15
#define VALVE17		16
#define VALVE18		17
#define VALVE_MAX	18

#define RELAY1		0
#define RELAY2		1
#define RELAY3		2
#define RELAY4		3
#define RELAY_MAX	4

enum CtrlState {
	FAILURE = 0,
	OVERRANGE,
	NORMAL,
	ON,
	OFF,
	LEFT,
	RIGHT,
	INDEPENDENT
};

class CControl
{
	public:
		CControl();
		void begin();
		void valveLeft(int valve);
		void valveRight(int valve);
		void valveLeftIndependentOn(int valve);
		void valveLeftIndependentOff(int valve);
		void valveRightIndependentOn(int valve);
		void valveRightIndependentOff(int valve);
		void valveOff(int valve);
		CtrlState valveGet(int valve);
		CtrlState valveStateGet(int valve);
		void relayOn(int relay);
		void relayOff(int relay);
		CtrlState relayGet(int relay);

		PCF857X expander0;
		PCF857X expander1;
		PCF857X expander2;
		PCF857X expander3;
		// PCF857X expander4;
		CtrlState valves_state[18];
	private:
		void t1_setup(void);
		CtrlState valves[18];
		CtrlState relays[4];
	// protected:
};

extern CControl Control;

#endif // #ifndef __CONTROL_H

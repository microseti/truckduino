#ifndef __INPUT_H
#define __INPUT_H

#define INPUT_FILTER_MAX		10
#define INPUT_DISCRETE_THOLD	300
#define INPUT_DISCRETE_HYST		100

#define INPUT1		0
#define INPUT2		1
#define INPUT3		2
#define INPUT4		3
#define INPUT5		4
#define INPUT6		5
#define INPUT7		6
#define INPUT8		7
#define INPUT_MAX	8


class CInput
{
	public:
		CInput();
		void begin();
		void filterConfig(int channel, int level);
		void discreteConfig(int channel, int one_thold, int hyst);
		void valuePercentConfig(int channel, int lo, int hi);
		void voltagePercentConfig(int channel, float lo, float hi);
		void currentPercentConfig(int channel, float lo, float hi);

		int valueRead(int channel);
		float voltageRead(int channel);
		float currentRead(int channel);
		bool discreteRead(int channel);
		float percentRead(int channel);
		volatile int input_values[INPUT_MAX];
		volatile int filter_values[INPUT_MAX][INPUT_FILTER_MAX];
		int filter_levels[INPUT_MAX];

	private:
		void t8_setup(void);
		// percent_modes: 0 - value, 1 - volt, 2 - curr
		bool discrete_states[INPUT_MAX];
		int tholds[INPUT_MAX];
		int hysts[INPUT_MAX];
		int percent_modes[INPUT_MAX];
		float ranges_lo[INPUT_MAX];
		float ranges_hi[INPUT_MAX];
};

extern CInput Input;

#endif // #ifndef __INPUT_H

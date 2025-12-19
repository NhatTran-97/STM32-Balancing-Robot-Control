#include "lqr.h"

//static float lqr_coeff[4] = {24.3 ,  2.1 , 16.0,  8.4};

static float lqr_coeff[4] = {25.0 ,  1.8 , 30.0,  18.4};

#define RAD2METER 0.03f // radius of wheels

void apply_lqr(StateTypeDef state, float *out)
{
	*out = state.angle * lqr_coeff[0] + state.angle_vel * lqr_coeff[1] + 
				state.position * lqr_coeff[2] * RAD2METER + state.velocity * lqr_coeff[3] * RAD2METER;
}

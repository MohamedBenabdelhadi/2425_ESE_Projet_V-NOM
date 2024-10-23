#include "stdlib.h"

#define GP2Y0A41SK0F_ADC_BITS 12
#define GP2Y0A41SK0F_ADC_MAX_VALUE (1 << GP2Y0A41SK0F_ADC_BITS)

typedef struct h_shell_struct
{
	int adc_val;
	int distance;
} h_GP2Y0A41SK0F_t;


void GP2Y0A41SK0F_get_distance(h_GP2Y0A41SK0F_t *);

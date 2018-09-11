/* This file will be replaced in grading script */
#include <stdio.h>
#include <stdlib.h>
#include "ldfp.h"

int main(void) {
	double df0, df1;
	long_double ld0, ld1;
	long_double ld_mul, ld_add;
	int ret;

	ret = scanf("%lf %lf", &df0, &df1);
	if (ret != 2) exit(EXIT_FAILURE);

	printf("Inputs: %lf, %lf\n", df0, df1);

	/* double to long double conversion */
	ld0 = double_to_long_double(df0);//ok
	ld1 = double_to_long_double(df1);

	/* long double addition */
	ld_add = FP_add(ld0, ld1);
	printf("FP_ADD\n");
	printf("(Bit Sequence): %s\n", long_double_print_bitseq(ld_add));
	printf("(Normalized Form): %s\n", long_double_print_normalized(ld_add));

	/* long double multiplication */
	ld_mul = FP_mul(ld0, ld1);
	printf("FP_MUL\n");
	printf("(Bit Sequence): %s\n", long_double_print_bitseq(ld_mul));
	printf("(Normalized Form): %s\n", long_double_print_normalized(ld_mul));

	/* We need to free malloc'ed memory inside the long_double_print_*()
         * but we are terminating this application immediately. So, this should be OK.
         */

	return 0;
}

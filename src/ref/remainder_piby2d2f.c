void __amd_remainder_piby2d2f(unsigned long long ux, double *r, int *region);
void __amd_remainder_piby2d2f_4f(unsigned long long  *x, double *r, unsigned long long *region)
{


	int region1[4];
     __amd_remainder_piby2d2f(x[0],r,region1);
	 __amd_remainder_piby2d2f(x[1],&r[1],&region1[1]);
     __amd_remainder_piby2d2f(x[2],&r[2],&region1[2]);
	 __amd_remainder_piby2d2f(x[3],&r[3],&region1[3]);

	 region[0] = region1[0];
	 region[1] = region1[1];
	 region[2] = region1[2];
	 region[3] = region1[3];
}


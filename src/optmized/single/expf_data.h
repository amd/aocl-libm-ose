#ifndef __AMDLIBM_SINGLE_EXPF_DATA_H__
#define __AMDLIBM_SINGLE_EXPF_DATA_H__

struct expf_data {
#ifdef EXP2F_N
    double             one_by_64;
#else
    double             tblsz_byln2;     /* 32/ln2 */
#endif
    double             Huge;
#ifdef EXP2F_N
    double             sixtyfour;
#else
    double             ln2by_tblsz;     /* ln2/32 */
#endif
    double             poly[EXPF_MAX_POLY_DEGREE];
    const double       *table_v3;
};

extern double  __two_to_jby64[];

#endif

#ifndef _STRATCOMMON_FIXTAG_H_
#define _STRATCOMMON_FIXTAG_H_

//Common Tags
#define STR_LEN 32
#define STR_LEN_LONG 128
#define FIX_TAG_STRATEGYNAME	9500
#define FIX_TAG_DESTINATION 	9501
#define FIX_TAG_OMUSER			9502

// Define FIX tags for Nikkei Spread: Common [9500-9502] + Specific [9505-9507, 9510-9513]
#ifndef _NKSPD_FIXTAGS_H_
#define _NKSPD_FIXTAGS_H_
#define FIX_TAG_STRATID_NKSP		9503
#define FIX_TAG_STRATPORT_NKSP		9504
#define FIX_TAG_LEGID_NKSP			9505
#define FIX_TAG_RUNNING_NKSP		9506
#define FIX_TAG_MODE_NKSP			9507

#define FIX_TAG_ORDQTY_NKSP			9510
#define FIX_TAG_TOTALQTY_NKSP		9511
#define FIX_TAG_PAYUPTICK_NKSP		9512
#define FIX_TAG_BENCHMARK_NKSP		9513
#endif // #ifndef _NKSPD_FIXTAGS_H_

#endif // #ifndef _STRATCOMMON_FIXTAG_H_

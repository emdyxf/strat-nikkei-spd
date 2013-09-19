#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include <set>
#include <functional>
#include <iostream>
using namespace FlexAppRules;
using namespace std;

#define FLEX_OMRULE_NKSPD	"NKSP"

#define SYM_NAME_NKD	"NKD"
#define SYM_NAME_NIY	"NIY"
#define	SYM_NAME_6J		"6J"

#define LEG_QUOTE		"QUOTE"
#define LEG_HEDGE		"HEDGE"
#define LEGID_MIN_NKSP		1
#define LEGID_MAX_NKSP		2
#define LEGID_NKSP_Q		1
#define LEGID_NKSP_H		2

#define STRAT_MODE_ALWAYS		"Always"
#define STRAT_MODE_SILENT		"Silent"

#define MODEID_INVALID		0
#define MODEID_ALWAYS		1
#define MODEID_SILENT		2

static const double g_dEpsilon = 0.0001;

#define strcmpi strcasecmp
#define stricmp strcasecmp

#ifdef PLATFORM_WIN
        #include "customer.h"
#else
	#include "/export/home/cdhdablx/flexapp/flex/config/customer.h"
#endif

// ###### ******** Controlling the way the Dialog opens ******* ###### // =========== START ===========
static bool IsStratInEditMode = false;
static bool IsEditFromInit = false;
// ###### ******** Controlling the way the Dialog opens ******* ###### //  ========== STOP ===========

#define STRAT_STATUS_INIT		"Init"
#define START_STATUS_PAUSE		"Paused"
#define STRAT_STATUS_WORKING		"Working"
#define STRAT_STATUS_CXLD		"Cancelled"
#define STRAT_STATUS_EDITED		"Edited"
#define STRAT_STATUS_STOP		"Stopped"
#define STRAT_STATUS_COMPLETE		"Completed"


#define SPD_WITH_CURRENCY (0) // 1 = Calculate spread with currency feed; 0 = Calculate spread with NK face value
#define SPD_TICK (5) // Default Nikkei tick size is 5

static double g_dBidSpread = 0.0, g_dLastSpread = 0.0, g_dAskSpread = 0.0;
static char g_QuoteSym[32]="", g_HedgeSym[32]="", g_Currency[32]="";
static double g_NKDExecVal = 0.0, g_NIYExecVal = 0.0, g_6JExecVal = 0.0;
static int g_NKDExecQty = 0, g_NIYExecQty = 0, g_6JExecQty = 0;

#define CLMNAME_TICK_SIZE                                                               "TICK_SIZE"
#define CLMNAME_LOT_SIZE                                                                "LOT_SIZE"
#define CLMNAME_ROUND_LOT                                                               "ROUND_LOT"
#define CLMNAME_ROUND_PRICE                                                             "ROUND_PRICE"

const int FLID_TICK_SIZE                                                                = 16;
const int FLID_LOT_SIZE                                                                 = 524;

#ifndef FIX_TAG_STRATEGYNAME
#define FIX_TAG_STRATEGYNAME            9500
#endif
#ifndef FIX_TAG_DESTINATION
#define FIX_TAG_DESTINATION             9501
#endif
#ifndef FIX_TAG_OMUSER
#define FIX_TAG_OMUSER                  9502
#endif

#ifndef _NKSPD_FIXTAGS_H_
#define _NKSPD_FIXTAGS_H_
#define FIX_TAG_STRATID_NKSP		9503
#define FIX_TAG_STRATPORT_NKSP		9504
#define FIX_TAG_LEGID_NKSP		9505
#define FIX_TAG_RUNNING_NKSP		9506
#define FIX_TAG_MODE_NKSP		9507

#define FIX_TAG_ORDQTY_NKSP		9510
#define FIX_TAG_TOTALQTY_NKSP		9511
#define FIX_TAG_PAYUPTICK_NKSP		9512
#define FIX_TAG_BENCHMARK_NKSP		9513
#endif // #ifndef _NKSPD_FIXTAGS_H_


/*
#define CLIENT_MODE (0) // 1 = Client Mode; 0 = Loopback setup
#define SPD_SYM_DIFF_PORT (1) // 1 = Put the third (spread) symbol in a different portfolio linked to the original one

#define LADDER_TEST
#ifdef LADDER_TEST
	#define LADDER_TESTMODE (1)
#else
	#define LADDER_TESTMODE (0)
#endif
*/

// Parameters for Ladder
const long LADDER_ORD = 42;

void SubscrSymServer(const char* sy, bool b);
CPriceInfo* FindPriceInfo(const char * s, int c);

// SPREAD SPECIFIC UTILITIES 
const char * FixedMWPort()
{
	static char szPort[32] ="";
	if(szPort[0] == '\0')
		sprintf(szPort,"WatchList_%s",TraderName());
	return szPort;
}

const long SPD_DUMMY_ORD_ID = 1331;
const char * GLOBAL_DELIMITER_STRING = "-";
const char GLOBAL_DELIMITER_CHAR = '-';

const char * ORD_TYPES[] = {"MKT", "LIM", "IOC", "NONE"};


typedef enum{
	enColumn1 = 33,
	enColumn2 = 16,
	enColumn3 = 2,
	enColumn4 = 15,
	enColumn5 = 34,
	enColumnOinfo = 24,
	enInvalidColumn = -1
}SS_COLUMN_NUMBER;

typedef enum{
	enSingleClick,
	enDoubleClick,
	enRightClick,
	enMiddleClick,
	enShiftLeftClick,
	enCtrlLeftClick,
	enButtonRelease,
	enOtherClick
}SS_MOUSE_CLICK;

/*
 *List of ENUMS : Nikkei and YEN/USD
 */

typedef enum enCMInstrType
{
        enNikkeiUSD,
        enNikkeiYEN,
        enYENUSD,
        enNoInstr
};


enCMInstrType GetInstrType(int i)
{
        switch(i)
        {
                case 0:
                        return enNikkeiUSD;
                case 1:
                        return enNikkeiYEN;
                case 2:
                        return enYENUSD;
                default:
                        return enNoInstr;
        }
}

const char* GetProductName(enCMInstrType eType)
{
        static const char* p[4] = {"NK225USD","NK225YEN", "YENUSD", "NONE"};
        switch(eType)
        {
                case enNikkeiUSD:
                        return p[0];
                case enNikkeiYEN:
                        return p[1];
                case enYENUSD:
                        return p[2];
                default:
                        return p[3];
        }
        return p[3];
}


const char* GetProductSymbol(enCMInstrType eType)
{
        static const char* p[4] = {SYM_NAME_NKD, SYM_NAME_NIY, SYM_NAME_6J, "NONE"};
        switch(eType)
        {
                case enNikkeiUSD:
                        return p[0];
                case enNikkeiYEN:
                        return p[1];
                case enYENUSD:
                        return p[2];
                default:
                        return p[3];
        }
        return p[3];
}


enCMInstrType GetInstrType(const char* pszSymbol)
{
        if(!pszSymbol)
                return enNoInstr;

        if(strstr(pszSymbol, SYM_NAME_NKD))
                return enNikkeiUSD;
        else if(strstr(pszSymbol, SYM_NAME_NIY))
                return enNikkeiYEN;
        else if(strstr(pszSymbol, SYM_NAME_6J))
                return enYENUSD;
        return enNoInstr;
}

enCMInstrType GetInstrTypeFromProduct(const char* pszProduct)
{
        if(!pszProduct)
                return enNoInstr;

        if(!strcmp(pszProduct,"NK225USD"))
                return enNikkeiUSD;
        else if(!strcmp(pszProduct,"NK225YEN"))
                return enNikkeiYEN;
        else if(!strcmp(pszProduct,"YENUSD"))
                return enYENUSD;
        return enNoInstr;
}

const char* GetProductFromSymbol(const char* pszSymbol)
{
        return GetProductName(GetInstrType(pszSymbol));
}

/*
 * Create WatchList
 */
char CMEMonthCodeFromMonthNumber(int iMonth)
{
        if(iMonth < 0 && iMonth > 12)
                return '0';

        static const char p[13] = {'-', 'F', 'G', 'H', 'J', 'K', 'M', 'N', 'Q', 'U', 'V', 'X', 'Z'};
        return p[iMonth];
}


// Tondering's algorithm
int DayOfWeek (int y, int m, int d)
{
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	y -= m < 3;
	return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

void LoadNikkeiWatchList(int iCurrentDate, int iInstrType)
{
	int iCurrentYear = 0, iCurrentMonth = 0, iDOWFirstDay=0, iDeliveryDay=0;
	int iStartMonth=0, iMonth=0, iQuarter=0;
	static enCMInstrType eInstrType = enNoInstr;
	char szExpiry[32] = "", szCommodity[32] = "";

	eInstrType = GetInstrType(iInstrType);
	sprintf(szCommodity,GetProductSymbol(eInstrType));

	iCurrentYear = iCurrentDate/10000;
	iCurrentMonth = (iCurrentDate/100)%100;

	if (!(iCurrentMonth%3)) // Settlement month: Check for 2nd Friday to expire current month's contract
	{
		iDOWFirstDay = DayOfWeek(iCurrentYear, iCurrentMonth, 1);
        	iDeliveryDay = iDOWFirstDay<5? (13-iDOWFirstDay) : 14;
		iQuarter = ( (iCurrentDate%100)>iDeliveryDay? int(iCurrentMonth/3)+1 : int(iCurrentMonth/3));
	}
	else // Not settlement month, start with next quarter
	{
		 iQuarter = int(iCurrentMonth/3)+1;
	}

	for (int i=0; i<4; i++) // Up to next four quarters
	{
		iMonth = (iQuarter+i)*3;

		if(iMonth > 12)
		{
			iMonth %= 12;
			sprintf(szExpiry, "%s%c%1d", szCommodity,CMEMonthCodeFromMonthNumber((int)iMonth%12),(iCurrentYear+1)%10);
		}
		else
		{
			sprintf(szExpiry, "%s%c%1d", szCommodity,CMEMonthCodeFromMonthNumber((int)iMonth), iCurrentYear%10);
		}

		Add_Symbol_Buy(FixedMWPort(), szExpiry, 0);
	}
}

void LoadBoDWatchList()
{
	int nWatchMonth = GlobalValue("WATCHMONTH");

	if(!Is_Port_Loaded(FixedMWPort()))
	{
		char szCurrentDate[32] = "";
		int iCurrentDate = 0, iInstrType=0;
		Current_Date6(szCurrentDate);
		iCurrentDate = atoi(szCurrentDate);

		// Load Contracts: NKD, NIY, 6J 
		for(iInstrType = 0; iInstrType < 3; iInstrType++)
		{
			LoadNikkeiWatchList(iCurrentDate, iInstrType);
		}
	}
}


const char* GetContractSymbolFromInstrAndMonth (int iInstrType, int iMonthOrder)
{
	char szCurrentDate[32] = "";
	int iCurrentDate = 0;
	Current_Date6(szCurrentDate);
	iCurrentDate = atoi(szCurrentDate);

	int iCurrentYear = 0, iCurrentMonth = 0, iDOWFirstDay=0, iDeliveryDay=0;
	int iStartMonth=0, iMonth=0, iQuarter=0;
	static enCMInstrType eInstrType = enNoInstr;
	char szExpiry[32] = "", szCommodity[32] = "";


	eInstrType = GetInstrType(iInstrType);
	sprintf(szCommodity,GetProductSymbol(eInstrType));

	iCurrentYear = iCurrentDate/10000;
	iCurrentMonth = (iCurrentDate/100)%100;

	if (!(iCurrentMonth%3)) // Settlement month: Check for 2nd Friday to expire current month's contract
	{
		iDOWFirstDay = DayOfWeek(iCurrentYear, iCurrentMonth, 1);
		iDeliveryDay = iDOWFirstDay<5? (13-iDOWFirstDay) : 14;
		iQuarter = ( (iCurrentDate%100)>iDeliveryDay? int(iCurrentMonth/3)+1 : int(iCurrentMonth/3));
	}
	else // Not settlement month, start with next quarter
	{
		 iQuarter = int(iCurrentMonth/3)+1;
	}

	iMonth = (iQuarter+iMonthOrder)*3;

	if(iMonth > 12)
	{
		iMonth %= 12;

		sprintf(szExpiry, "%s%c%1d", szCommodity,CMEMonthCodeFromMonthNumber((int)iMonth%12),(iCurrentYear+1)%10);
	}
	else
	{
		sprintf(szExpiry, "%s%c%1d", szCommodity,CMEMonthCodeFromMonthNumber((int)iMonth), iCurrentYear%10);
	}

	return szExpiry;
}


void RefreshMarketSpreads()
{
	double quoteBid=0.0, quoteAsk=0.0, quoteLast=0.0;
	double hedgeBid=0.0, hedgeAsk=0.0, hedgeLast=0.0;
	double	currBid=0.0, currAsk=0.0, currLast=0.0, dlryen=0.0;
	MarketInfo minfoQuote(g_QuoteSym), minfoHedge(g_HedgeSym), minfoCurrency(g_Currency);

	quoteBid = minfoQuote.GetBid();
	quoteAsk = minfoQuote.GetAsk();
	quoteLast = minfoQuote.GetLast(); 
	hedgeBid = minfoHedge.GetBid();
	hedgeAsk = minfoHedge.GetAsk();
	hedgeLast = minfoHedge.GetLast();
	currBid = minfoCurrency.GetBid();
	currAsk = minfoCurrency.GetAsk();
	currLast = minfoCurrency.GetLast();
	
	dlryen=(1/currLast*10000);
	if (SPD_WITH_CURRENCY == 1 && currAsk>g_dEpsilon && currLast>g_dEpsilon && currBid>g_dEpsilon)
	{
		g_dBidSpread = ((quoteBid*5.0)-(hedgeAsk*dlryen*500)*(currAsk/1000000))/5.0;
		g_dLastSpread = ((quoteLast*5.0)-(hedgeLast*dlryen*500)*(currLast/1000000))/5.0;
		g_dAskSpread = ((quoteAsk*5.0)-(hedgeBid*dlryen*500)*(currBid/1000000))/5.0;
	}
	else if (SPD_WITH_CURRENCY == 0)
	{	
		g_dBidSpread = quoteBid-hedgeAsk;
                g_dLastSpread = quoteLast-hedgeLast;
                g_dAskSpread = quoteAsk-hedgeBid;
	}
	else
	{
		g_dBidSpread = 0.0;
                g_dLastSpread = 0.0;
                g_dAskSpread = 0.0;
	}
}





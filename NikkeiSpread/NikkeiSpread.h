#ifndef _NikkeiSpread_H_
#define _NikkeiSpread_H_

#include <RECommon.h>
#include <REHostExport.h>
#include <string.h>
#include "boost/unordered_map.hpp"
#include <SymConnection.h>

#ifndef _STRATCOMMON_FIXTAG_H_
#include "../Common/fixtag.h"
#endif

#ifndef _STRATCOMMON_LOGGER_H_
#include "../Common/logger.h"
#endif

namespace FlexStrategy
{
namespace NikkeiSpread
{
#define STR_LEN 32
#define STR_LEN_LONG 128

#define LEGID_MIN_NKSP          1
#define LEGID_MAX_NKSP          2
#define LEGID_NKSP_Q            1
#define LEGID_NKSP_H            2

#define LEG_QUOTE               "QUOTE"
#define LEG_HEDGE               "HEDGE"

#define SYM_NAME_NKD			"NKD"
#define SYM_NAME_NIY			"NIY"
#define SYM_NAME_6J				"6J"

static const double g_dEpsilon = 0.0001;
static const char cDelimiter = ';';


#define MODEID_INVALID		0
#define MODEID_MIN_NKSP		1
#define MODEID_ALWAYS		1
#define MODEID_SILENT		2
#define MODEID_MAX_NKSP		2

/// Quoting Mode
enum E_STRAT_MODE {
	STRAT_MODE_INVALID		= MODEID_INVALID,
	STRAT_MODE_ALWAYS		= MODEID_ALWAYS,
	STRAT_MODE_SILENT		= MODEID_SILENT
};


struct StratParams
{
	// Params
	long nStratId;
	char szStratPort[STR_LEN];
	E_STRAT_MODE eStratMode;
	char szSymbolQuote[STR_LEN];
	char szSymbolHedge[STR_LEN];
	char szClientOrdIdQuote[STR_LEN];
	char szClientOrdIdHedge[STR_LEN];
	char szPortfolioQuote[STR_LEN];
	char szPortfolioHedge[STR_LEN];
	double dBenchmark;
	int nOrdLot;
	int nTotalLot;
	int nPayupTicks;
	enOrderSide enSideQuote;
	enOrderSide enSideHedge;
	char szOMUser[STR_LEN];
	char szDest[STR_LEN];

	// Status
	bool bStratReady;
	bool bStratRunning;

	// Control Flags
	bool bQuoteEntry;
	bool bHedgeEntry;
	bool bQuoteRpld;
	bool bHedgeRpld;

	//MktData
	int nContraSizePrevQuote;
	double dContraPricePrevQuote;
	int nContraSizePrevHedge;
	double dContraPricePrevHedge;

	StratParams()
	{
		// Params
		nStratId = 0;
		strcpy(szStratPort,"");
		eStratMode = STRAT_MODE_INVALID;
		strcpy(szSymbolQuote,"");
		strcpy(szSymbolHedge,"");
		strcpy(szClientOrdIdQuote,"");
		strcpy(szClientOrdIdQuote,"");
		strcpy(szPortfolioQuote,"");
		strcpy(szPortfolioHedge,"");
		dBenchmark = 0.0;
		nOrdLot = 0;
		nTotalLot = 0;
		nPayupTicks = 0;
		enSideQuote = SIDE_INVALID;
		enSideHedge = SIDE_INVALID;
		strcpy(szOMUser,"");
		strcpy(szDest,"");

		// Status
		bStratReady = false;
		bStratRunning = false;

		// Control Flags
		bQuoteEntry = false;
		bHedgeEntry = false;
		bQuoteRpld = false;
		bHedgeRpld = false;

		//MktData
		nContraSizePrevQuote = 0;
		dContraPricePrevQuote = 0.0;
		nContraSizePrevHedge = 0;
		dContraPricePrevHedge = 0.0;
	}
};


typedef boost::unordered_map<long,StratParams*> STRATMAP;
typedef boost::unordered_map<long,StratParams*>::iterator STRATMAPITER;

typedef boost::unordered_map<std::string,long> STRATIDMAP;
typedef boost::unordered_map<std::string,long>::iterator STRATIDMAPITER;

class NikkeiSpread : public StrategyBase
{
public:

	NikkeiSpread();
	virtual ~NikkeiSpread();

	//Market Data event implementation
	int OnMarketData(CLIENT_ORDER& ClientOrder, MTICK& MTick);
	int OnMarketData(MTICK& MTick);
	int OnLoad(const char* szConfigFile = NULL);

	// Connection call backs
	int OnClientConnect(const char* szDest);
	int OnClientDisconnect(const char* szDest);

	int OnStreetConnect(const char* szDest);
	int OnStreetDisconnect(const char* szDest);

	int OnConfigUpdate(const char* szConfigBuf);
	int OnClientCommand(STRAT_COMMAND &pCommand);

	int OnEndOfRecovery();

	int OnTimer(EVENT_DATA& eventData);

	//Following are on Client events for his particular orders
	int OnClientOrdNew(CLIENT_ORDER& ClientOrd);
	int OnClientOrdRpl(CLIENT_ORDER& ClientOrd);
	int OnClientOrdCancel(CLIENT_ORDER& ClientOrd);
	int OnClientSendAck(CLIENT_EXEC& ClientOrd);
	int OnClientSendOut(CLIENT_EXEC& ClientOrd);
	int OnClientSendReject(CLIENT_EXEC& ClientExec);
	int OnClientSendFills(CLIENT_EXEC& ClientExec);
	int OnClientOrdValidNew(CLIENT_ORDER& ClientOrd);
	int OnClientOrdValidRpl(CLIENT_ORDER& ClientOrd);
	int OnClientOrdValidCancel(CLIENT_ORDER& ClientOrd);
	int OnClientSendPendingRpl(OMRULESEXPORT::CLIENT_EXEC& ClientExec);

	// Street Orders
	int OnStreetOrdNew(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdRpl(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdCancel(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdAck(STREET_ORDER& StreetOrd);
	int OnStreetOrdRej(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdOut(STREET_ORDER& StreetOrd);
	int OnStreetExec(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdValidNew(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdValidRpl(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdValidCancel(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetStatusReport(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder);
	int OnStreetNewReportStat(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder);
	int OnStreetDiscardOrdCancel(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);

	//Custom
	StratParams* GetActiveStratParamsByClientOrderId(const char* pszClientOrderId);
	// MarketData
	bool 	IsValidMarketTick(MTICK& mtick);

	// Strat - Params
	int		CalculateNetStratExecs(StratParams* pStratParams);

	// Strat - Client
	int		ValidateFIXTags(ORDER& order);
	int		AddStratParamsToMap(ORDER& order,STRATMAP& pStMap);
	int		GetQuoteClientOrder(CLIENT_ORDER& QuoteOrder, StratParams* pStratParams);
	int		GetHedgeClientOrder(CLIENT_ORDER& HedgeOrder, StratParams* pStratParams);
	bool	IsAdjustComplete (StratParams* pStratParams, int nNetStratExecs);
	int		HandleAdjustWorkingSize (StratParams* pStratParams, int nNetStratExecs);
	int		HandleQuoteUponMarketData (StratParams* pStratParams, MTICK& mtick_quote, MTICK& mtick_hedge);
	int		HandleHedgeUponMarketData (StratParams* pStratParams, MTICK& mtick_hedge);
	int		HandleHedgeOnQuoteExecs(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder);
	double	GetStratQuotePrice (double dBenchmark, MTICK& mtick_quote, MTICK& mtick_hedge, enOrderSide enSideQuote);
	double	GetStratHedgePrice (StratParams* pStratParams, MTICK& mtick_hedge,  enOrderSide enSideHedge);
	int		CalculateNewQuoteQuantity(StratParams* pStratParams);
	int		CalculateNewHedgeQuantity(StratParams* pStratParams);
	int		OrderPriceCrossCheck(STREET_ORDER& NewStreetOrder);

	// Utils
	int		SendOutStreetOrder(CLIENT_ORDER& ClientOrder, const char* pszDestination, int nOrderQty, double dOrderPrice);
	int 	CancelAllOrdersForClientOrder(const char* pszClientOrderId);
	int 	CancelAllStreetOrders(const StratParams *pStratParams);
	bool	CanReplace(STREET_ORDER& StreetOrder);
	const char* StateToStr(enOrderState e);
	const char* ErrorCodeToStr(enErrorCode e);
	const char* StratModeToStr(E_STRAT_MODE e);
	double	GetTickSizeForSymbol(const char* pszSymbol);
	const char* GetProductFromSymbol(const char* pszSymbol);

private:
	void TestLevel2(std::string symbol);
	void PrintStratOrderMap(STRATMAP& mpStratMap, const long nStratId);
	void PrintStratOrderMapRpl(STRATMAP& mpStratMapRpl);
	void PrintStratOrderMapCxl(STRATMAP& mpStratMapCxl);
	void PrintStreetExec(STREET_EXEC& StreetExec, CLIENT_ORDER& parentOrder);

private:
	static char *m_sFixTagStrategyName;
	static std::string m_sPropertiesFileName;
	static std::string m_sFixDestination;

	static int m_sCount;
	static int m_sStreetOrderSize;
};

// Nikkei Spread
static const unsigned int	C_NKSPD_NUM_OF_LEGS_INDEX	= 1; // Num of legs = 4: [0,1]
static const unsigned int	C_NKSPD_NUM_OF_LEGS		= 2; // Num of legs = 2


extern STRATIDMAP gmStratIdMap;
extern STRATMAP gmStratMap, gmStratMapRpl;
extern STRATMAP gmStratMapCxl;


}; // namespace NikkeiSpread
}; // namespace FlexStrategy

#endif //_NikkeiSpread_H_

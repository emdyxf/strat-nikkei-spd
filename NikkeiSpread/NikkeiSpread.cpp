#include "./NikkeiSpread.h"
#include <sstream>
#include <map>
#include <sys/time.h>

using namespace FlexStrategy::NikkeiSpread;

char *NikkeiSpread::m_sFixTagStrategyName = "NKSP";

// For Strat
STRATIDMAP FlexStrategy::NikkeiSpread::gmStratIdMap;
STRATMAP FlexStrategy::NikkeiSpread::gmStratMap, FlexStrategy::NikkeiSpread::gmStratMapRpl;
STRATMAP FlexStrategy::NikkeiSpread::gmStratMapCxl;

NikkeiSpread::NikkeiSpread() :
	StrategyBase(m_sFixTagStrategyName, LOG_ERROR)
{
	CINFO << "----------------------------------------------------------------------" << std::endl;
	CINFO << " In NikkeiSpread CTOR strat name = " << GetStratName() << std::endl;
	CINFO << " Build Date [" << BUILD_DATE << "]" << std::endl;
	CINFO << " Build Time [" << BUILD_TIME << "]" << std::endl;
	CINFO << " Build Version [" << BUILD_VERSION << "]" << std::endl;

	switch (STRAT_DEBUG_LEVEL)
	{
		case 0:		CINFO << " Logging Level: Info " << std::endl;		break;
		case 1:		CINFO << " Logging Level: Verbose " << std::endl;	break;
		default:	CINFO << " Logging Level: Debug " << std::endl;		break;
	}

	CINFO << "----------------------------------------------------------------------" << std::endl;
}

NikkeiSpread::~NikkeiSpread()
{
	CINFO << __PRETTY_FUNCTION__<< std::endl;
}

int NikkeiSpread::OnLoad(const char* szConfigFile)
{
	CINFO << __PRETTY_FUNCTION__<< std::endl;
	SetSignalEnabled(false);
	return SUCCESS;
}

int NikkeiSpread::OnEndOfRecovery(){return SUCCESS;}

int NikkeiSpread::OnMarketData(CLIENT_ORDER& order, MTICK& mtick)
{
	_STRAT_LOG_DBUG_(CDEBUG << "\n \n=====Process Market Data with order ID [" << order.GetHandle()
							<< "] Symbol [" << mtick.GetSymbol()
							<< "]====="  << std::endl);

	// Validation : Market Tick
	if (!IsValidMarketTick(mtick))
		return FAILURE;

	// Validate : Order Entry
	if(order.GetUnOrdSize() < 1)
	{
		_STRAT_LOG_DBUG_(CDEBUG << "Order ID [" << order.GetHandle()
				<< ": No Quantity! UnordSize [" << order.GetUnOrdSize() << "]"
				<< std::endl);
		return FAILURE;
	}

	// STEP 1 : Get active strat param by client order id and decide if react to market data
	StratParams *pStratParams = NULL;
	pStratParams = GetActiveStratParamsByClientOrderId(order.GetHandle());
	if (!pStratParams)
	{
		_STRAT_LOG_VERB_(CWARN << "No active StratParam with ClientOrderId [" << order.GetHandle() << "]" << std::endl);
		return FAILURE;
	}


	// STEP 2 : Validate if Strat is ready and running
	if(! ((pStratParams->bStratReady)&&(pStratParams->bStratRunning)))
	{
		_STRAT_LOG_VERB_(CWARN << "StratID [" << pStratParams->nStratId
								<< "] is stopped: ReadyFlag? [" << pStratParams->bStratReady
								<< "] isRuning? [" << pStratParams->bStratRunning
								<< "]"
								<< std::endl);
		return FAILURE;
	}

	
	// STEP 3 : Get the relevant market data fields and save in mtick_quote and mtick_hedge
	MTICK mtick_quote, mtick_hedge;
	if(!strcmp(order.GetSymbol(),pStratParams->szSymbolQuote))
	{
		mtick_quote = mtick;
		MarketDataGetSymbolInfo(pStratParams->szSymbolHedge, mtick_hedge);

		_STRAT_LOG_DBUG_(CDEBUG << "Strat ID [" <<  pStratParams->nStratId
					<< "] Quote Symbol [" << pStratParams->szSymbolQuote
					<< "] MTick Symbol [" << order.GetSymbol()
					<< "] : Disregard Quoting Contract Market Ticks!"<< std::endl);
		return SUCCESS;
	}
	else if(!strcmp(order.GetSymbol(),pStratParams->szSymbolHedge))
	{
		mtick_hedge = mtick;
		MarketDataGetSymbolInfo(pStratParams->szSymbolQuote, mtick_quote);
	}
	else
	{
		_STRAT_LOG_DBUG_(CERR << "Failed to get relevant market data fields!"<< std::endl);
		return FAILURE;
	}

	if (!(IsValidMarketTick(mtick_quote) && IsValidMarketTick(mtick_hedge)))
		return FAILURE;


	// STEP 4: Calculate NetStratExecs and Adjust Strat Working Size
	int nNetStratExecs = 0;
	nNetStratExecs = CalculateNetStratExecs(pStratParams);


	bool bAdjusted = IsAdjustComplete (pStratParams, nNetStratExecs);

	// Handle Adjustment
	if (!bAdjusted)
	{
		_STRAT_LOG_DBUG_(CDEBUG << "StratID [" << pStratParams->nStratId
								<< "] Adjusted? [" << bAdjusted
								<< "] - Proceed to Adjust Strat Working Size!"
								<< "]"
								<< std::endl);


		if (HandleAdjustWorkingSize (pStratParams, nNetStratExecs) == SUCCESS)
		{
			_STRAT_LOG_VERB_(CWARN << "StratID [" << pStratParams->nStratId
									<< "] : Successful in attempt to adjust Strat working size."
									<< std::endl);
		}
		else
		{
			_STRAT_LOG_VERB_(CERR << "StratID [" << pStratParams->nStratId
									<< "] : FAILED in attempt to adjust Strat working size!"
									<< std::endl);
		}
	}


	// STEP 5: Handle orders on MD
/*
	if (nNetStratExecs > 0)
	{
		HandleHedgeUponMarketData (pStratParams, mtick_hedge);
	}
	else
	{
		HandleQuoteUponMarketData (pStratParams, mtick_quote, mtick_hedge);
	}
*/
	if (!(nNetStratExecs > 0))
	{
		HandleQuoteUponMarketData (pStratParams, mtick_quote, mtick_hedge);
	}

	return SUCCESS;
}

int NikkeiSpread::OnMarketData(MTICK& MTick){return SUCCESS;}

// Connection call backs
int NikkeiSpread::OnClientConnect(const char* szDest)
{
	CINFO << __PRETTY_FUNCTION__ << std::endl;
	return SUCCESS;
}

int NikkeiSpread::OnClientDisconnect(const char* szDest)
{
	CINFO << __PRETTY_FUNCTION__ << std::endl;
	return SUCCESS;
}

int NikkeiSpread::OnStreetConnect(const char* szDest)
{
	CINFO << __PRETTY_FUNCTION__ << std::endl;
	return SUCCESS;
}

int NikkeiSpread::OnStreetDisconnect(const char* szDest)
{
	CINFO << __PRETTY_FUNCTION__ << std::endl;
	return SUCCESS;
}

int NikkeiSpread::OnConfigUpdate(const char* szConfigBuf)
{
	CINFO << __PRETTY_FUNCTION__ << " Got string buf = " << szConfigBuf << std::endl;
	return SUCCESS;
}

int NikkeiSpread::OnClientCommand(STRAT_COMMAND &pCommand)
{
	CINFO << __PRETTY_FUNCTION__ << std::endl;
	return SUCCESS;
}

// Client Order Callbacks
int NikkeiSpread::OnClientOrdValidNew(CLIENT_ORDER& order)
{
	return NikkeiSpread::ValidateFIXTags(order);
}

int NikkeiSpread::OnClientOrdNew(CLIENT_ORDER& order)
{
	long nStratId = atol(order.GetFixTag(FIX_TAG_STRATID_NKSP));
	std::string strOrderId(order.GetHandle());

	// Step 1: Insert the StratID into gmStratIdMap
	_STRAT_LOG_DBUG_(CDEBUG << "Before Client New: Inserting StratId [" << nStratId
							<<"] and OrderId [" <<strOrderId
							<<"] into gmStratIdMap!"
							<< std::endl);

	gmStratIdMap.insert(pair<std::string,long>(strOrderId,nStratId));

	// Step 2: Insert the order into gmStratMap
	if (AddStratParamsToMap(order, gmStratMap) != SUCCESS)
	{
		_STRAT_LOG_VERB_(CERR << "Debug: Fail to Add New orders to Strat Map - StratId ["<< nStratId
								<< "] Symbol [" <<order.GetSymbol()
								<< "] Size ["<< order.GetSize()
								<< "] Handle [" << order.GetHandle()
								<< "] FixTags [" <<order.GetFixTags()
								<< "]"
								<< std::endl);
		return FAILURE;
	}

	// Step 3: Logging for NEW
	CINFO << "CLIENT NEW: StratId [" << nStratId
			<<"] Symbol [" <<order.GetSymbol()
			<<"] Size [" <<order.GetSize()
			<<"] Handle [" << order.GetHandle()
			<<"] FixTags [" <<order.GetFixTags()
			<<"] " << std::endl;

	return SUCCESS;
}

int NikkeiSpread::OnClientOrdValidRpl(CLIENT_ORDER& order)
{
    _STRAT_LOG_DBUG_(CDEBUG << "Debug: Validate Client RPL StratID [" << order.GetFixTag(FIX_TAG_STRATID_NKSP) << "]" <<std::endl);

	return NikkeiSpread::ValidateFIXTags(order);
}

int NikkeiSpread::OnClientOrdRpl(CLIENT_ORDER& order)
{
	_STRAT_LOG_DBUG_(CDEBUG << "Start Client RPL - StratID [" << order.GetFixTag(FIX_TAG_STRATID_NKSP)
			<< "] Client OrderID [" << order.GetHandle()
        	<< "] Client RefOrdId [" << order.GetRefOrdId()
        	<< "] Client NewOrderId [" << order.GetNewOrderId()
        	<< "] Client CommonOrderId [" << order.GetCommonOrderId()
			<< "] Client GetPrimOrdId [" << order.GetPrimOrdId()
			<< "] Client GetOrigOrderId [" << order.GetOrigOrderId()
			<< "] Client Symbol [" << order.GetSymbol()
        	<< "]"
        	<<std::endl);


	// Step 1: Insert client order into RplMap
	_STRAT_LOG_DBUG_(CDEBUG << "Adding Client OrderID [" << order.GetHandle()
			<< "] Client GetRefOrdId [" << order.GetRefOrdId()
        	<< "] Client Symbol [" << order.GetSymbol()
        	<< "] to gmStratMapRpl"
        	<<std::endl);

	AddStratParamsToMap(order,gmStratMapRpl);


	// Step 2: Find StratParams in gmStratMap and gmStratMapRpl
	// Get pStratParams and pStratParamsRpl for further processing
	long nStratId = atol(order.GetFixTag(FIX_TAG_STRATID_NKSP));
	STRATMAPITER mStIterRpl = gmStratMapRpl.find(nStratId);
	STRATMAPITER mStIter = gmStratMap.find(nStratId);

	if((mStIter == gmStratMap.end()) ||(mStIterRpl == gmStratMapRpl.end()))
	{
        _STRAT_LOG_VERB_(CWARN << "StratID [" << nStratId << "] not found in gmStratMap or gmStratMapRpl" << std::endl);
		return FAILURE;
	}

	StratParams *pStratParamsRpl = (StratParams*) mStIterRpl->second;
	StratParams *pStratParams = (StratParams*) mStIter->second;

	if(!pStratParams || !pStratParamsRpl)
	{
        _STRAT_LOG_VERB_(CWARN << "Invalid pointer pStratParams or pStratParamsRpl " << std::endl);
		return FAILURE;
	}


	// Step 3 : Handle client replace
	// - If strategy is still running, stop strategy and cancel all street orders.
	// - Otherwise just replace client orders
	if (pStratParams->bStratRunning == false)
	{
		_STRAT_LOG_VERB_(CWARN << "StratID [" << pStratParams->nStratId
				<< "] Running Flag [" << pStratParams->bStratRunning
				<< "] : Already Stopped. Will not send street cancel again."
				<< std::endl);
	}
	else
	{
		_STRAT_LOG_VERB_(CWARN << "StratID [" << pStratParams->nStratId
				<< "] Running Flag [" << pStratParams->bStratRunning
				<< "] : Now stop strategy and send street cancels."
				<< std::endl);

		// Step 3-1: Stop the current working Strat because client has sent a replace.
		// Therefore, until replace is successful just stop this Strat.
		pStratParams->bStratRunning = false;

		// Step 3-2 : Cancel all street orders of the strat
		_STRAT_LOG_VERB_(CWARN << "StratID [" << pStratParams->nStratId
								<< "] : Sending Cancels!"
								<< std::endl);

		if (CancelAllStreetOrders(pStratParams) != SUCCESS)
		{
			bool bRunning = (bool)atoi(order.GetFixTag(FIX_TAG_RUNNING_NKSP));

			if (bRunning)
			{
				_STRAT_LOG_DBUG_(CWARN << "StratID [" << nStratId
						 << "] RPL for Client OrderID [" << order.GetHandle()
						 << "] with RunningFlag [" << order.GetFixTag(FIX_TAG_RUNNING_NKSP)
						 << "] FAILED!!! -> CXL-REQ could not be sent out on all the active street orders"
						 << std::endl);
			}
			else
			{
				_STRAT_LOG_VERB_(CERR << "StratID [" << nStratId
						<< "] RPL for Client OrderID [" << order.GetHandle()
						<< "] with RunningFlag [" << order.GetFixTag(FIX_TAG_RUNNING_NKSP)
						<< "] FAILED!!! -> CXL-REQ could not be sent out on all the active street orders"
						<< std::endl);
				return FAILURE;
			}
		}
	}

	_STRAT_LOG_DBUG_(PrintStratOrderMap(gmStratMap, nStratId));
	_STRAT_LOG_DBUG_(PrintStratOrderMapRpl(gmStratMapRpl));


	// Step 4 : Insert NewId into StratIdMap
	std::string strOrderId(order.GetNewOrderId());
	gmStratIdMap.insert(pair<std::string, long>(strOrderId, nStratId));


	// Step 5: Logging for RPL
	CINFO << "StratID [" << nStratId
			 << "] RPL : Client OrderID [" << order.GetHandle()
			 << "] Symbol [" << order.GetSymbol()
			 << "] Size [" << order.GetSize()
			 << "] FixTags [" << order.GetFixTags()
			 << "]" << std::endl;

	return SUCCESS;
}

int NikkeiSpread::OnClientOrdValidCancel(CLIENT_ORDER& order){return SUCCESS;}

int NikkeiSpread::OnClientOrdCancel(CLIENT_ORDER& order)
{
	long nStratId = atol(order.GetFixTag(FIX_TAG_STRATID_NKSP));
	CINFO << "CXL: StratID [" << nStratId
			<<"] Symbol [" << order.GetSymbol()
			<<"] Size [" << order.GetSize()
			<<"] Handle [" << order.GetHandle()
			<<"] FixTags [" << order.GetFixTags()
			<<"]" << std::endl;
	return SUCCESS;
}

int NikkeiSpread::OnClientSendFills(CLIENT_EXEC& clientExec){return SUCCESS;}

int NikkeiSpread::OnClientSendAck(CLIENT_EXEC& ClientExec)
{
	CLIENT_ORDER ClientOrder;
	ClientOrder = ClientExec.GetClientOrder();
	enOrderState eOrdState = ClientExec.GetReportStatus();

	_STRAT_LOG_DBUG_(CDEBUG << "Start Client ACK - StratID [" << ClientOrder.GetFixTag(FIX_TAG_STRATID_NKSP)
								<< "] Client OrderID [" << ClientOrder.GetHandle()
								<< "] Client RefOrdId [" << ClientOrder.GetRefOrdId()
								<< "] Client NewOrderId [" << ClientOrder.GetNewOrderId()
								<< "] Client CommonOrderId [" << ClientOrder.GetCommonOrderId()
								<< "] Client GetPrimOrdId [" << ClientOrder.GetPrimOrdId()
								<< "] Client GetOrigOrderId [" << ClientOrder.GetOrigOrderId()
								<< "] Client Symbol [" << ClientOrder.GetSymbol()
								<< "]"
								<<std::endl);

	if (eOrdState == STATE_REPLACED)
	{

		_STRAT_LOG_VERB_(CINFO << "Callback confirming RPLD state. Client OrderID [" << ClientExec.GetOrderID() << "]" << std::endl);

		_STRAT_LOG_DBUG_(PrintStratOrderMapRpl(gmStratMapRpl));

		// Step 1: Find StratParams in gmStratMap and gmStratMapRpl
		// Get pStratParams and pStratParamsRpl for further processing
		long nStratId = atol(ClientOrder.GetFixTag(FIX_TAG_STRATID_NKSP));
		STRATMAPITER mStIterRpl = gmStratMapRpl.find(nStratId);
		STRATMAPITER mStIter = gmStratMap.find(nStratId);

		if((mStIter == gmStratMap.end()) ||(mStIterRpl == gmStratMapRpl.end()))
		{
			if (mStIter == gmStratMap.end())
			{
				_STRAT_LOG_DBUG_ (CDEBUG << "StratID [" << nStratId << "] not found in gmStratMap!" << std::endl);
			}
			if (mStIterRpl == gmStratMapRpl.end())
			{
				_STRAT_LOG_DBUG_ (CDEBUG << "StratID [" << nStratId << "] not found in gmStratMapRpl!" << std::endl);
			}
			return FAILURE;
		}

		StratParams *pStratParamsRpl = (StratParams*) mStIterRpl->second;
		StratParams *pStratParams = (StratParams*) mStIter->second;

		if(!pStratParams || !pStratParamsRpl)
		{
	        _STRAT_LOG_DBUG_(CDEBUG << "Invalid pointer pStratParams or pStratParamsRpl " << std::endl);
			return FAILURE;
		}

	   _STRAT_LOG_VERB_ (CINFO << "RPLD : StratID [" << nStratId
								<< "] Client OrderID [" << ClientOrder.GetHandle()
								<< "] Client Symbol [" << ClientOrder.GetSymbol()
								<< "]" << std::endl);

	   // Step 2: Check RPL-ACK for each client orders in the strat
	   if(!strcmp(pStratParamsRpl->szSymbolQuote, ClientOrder.GetSymbol()))
	   {
		   _STRAT_LOG_DBUG_(CDEBUG << "RPLD : StratID [" << nStratId
									<< "] Client OrderID [" << ClientOrder.GetHandle()
									<< "] Client Side [" << ClientOrder.GetSide()
									<< "] Client Symbol [" << ClientOrder.GetSymbol()
									<< "] : Quote Replaced!" << std::endl);

		   pStratParamsRpl->bQuoteRpld = true;
	   }
	   else if(!strcmp(pStratParamsRpl->szSymbolHedge, ClientOrder.GetSymbol()))
	   {
		   _STRAT_LOG_DBUG_(CDEBUG << "RPLD : StratID [" << nStratId
									<< "] Client OrderID [" << ClientOrder.GetHandle()
									<< "] Client Side [" << ClientOrder.GetSide()
									<< "] Client Symbol [" << ClientOrder.GetSymbol()
									<< "] : Hedge Replaced!" << std::endl);

		   pStratParamsRpl->bHedgeRpld = true;
	   }
	   else
	   {
		   _STRAT_LOG_VERB_ (CERR << "Failed to match PrimOrdId [" << ClientOrder.GetPrimOrdId()
				   	   	   	   	   << "] or Symbol [" << ClientOrder.GetSymbol() << "]!" << std::endl);
		   return FAILURE;
	   }

	  // Step 3:Do the conditional processing ONLY after receiving RPL Client Orders on ALL the client orders
	    if (pStratParamsRpl->bQuoteRpld && pStratParamsRpl->bHedgeRpld)
		{
	    	//Update Maps
			gmStratMap.erase(mStIter);
			delete pStratParams;

			pStratParamsRpl->bStratRunning = (bool)atoi(ClientOrder.GetFixTag(FIX_TAG_RUNNING_NKSP));

			gmStratMap.insert(pair<int, StratParams*>(nStratId, pStratParamsRpl));
			gmStratMapRpl.erase(mStIterRpl);

	        _STRAT_LOG_VERB_(CINFO << "StratID [" << nStratId
	        		<< "] -> Committed the RPL for the StratParams map!"
	        		<< std::endl);

	        _STRAT_LOG_DBUG_(PrintStratOrderMap(gmStratMap, nStratId));
	        _STRAT_LOG_DBUG_(PrintStratOrderMapRpl(gmStratMapRpl));
		}
	}
	else if (eOrdState == STATE_OPEN)
	{
	        _STRAT_LOG_DBUG_ (CDEBUG << "Callback confirming ACK state. Client OrderID [" << ClientExec.GetOrderID() << "]" << std::endl);
	}
	else if (eOrdState == STATE_PENDING_NEW)
	{
	        _STRAT_LOG_DBUG_ (CDEBUG << "Callback confirming PEND-NEW state. Client OrderID [" << ClientExec.GetOrderID() << "]" << std::endl);
	        ClientOrder.SendAck();
	}
	else
	{
	        _STRAT_LOG_DBUG_ (CDEBUG << "DEBUG: Callback in wrong state. Client OrderID [" << ClientExec.GetOrderID()
	        		<< "] -> State [" << StateToStr(eOrdState) << "]" << std::endl);
		return FAILURE;
	}

	return SUCCESS;
}

int NikkeiSpread::OnClientSendOut(OMRULESEXPORT::CLIENT_EXEC&){return SUCCESS;}

int NikkeiSpread::OnClientSendReject(OMRULESEXPORT::CLIENT_EXEC&){return SUCCESS;}

int NikkeiSpread::OnClientSendPendingRpl(OMRULESEXPORT::CLIENT_EXEC&){return SUCCESS;}

//Street Order callbacks
int NikkeiSpread::OnStreetOrdNew(STREET_ORDER& streetOrder, CLIENT_ORDER& parentCLIENT_ORDER){return SUCCESS;}

int NikkeiSpread::OnStreetOrdValidNew(STREET_ORDER& streetOrder, CLIENT_ORDER& parentOrder)
{
	_STRAT_LOG_DBUG_(CDEBUG << "StreetOrder ID [" << streetOrder.GetHandle()
							<< "] ClientOrder ID [" << parentOrder.GetHandle()
							<< "] : Validating Price Cross."
							<< std::endl);

	return	OrderPriceCrossCheck(streetOrder);
}

int NikkeiSpread::OnStreetOrdRpl(STREET_ORDER& streetOrder, CLIENT_ORDER& parentCLIENT_ORDER){return SUCCESS;}

int NikkeiSpread::OnStreetOrdValidRpl(STREET_ORDER& streetOrder, CLIENT_ORDER& parentOrder){return SUCCESS;}

int NikkeiSpread::OnStreetOrdCancel(STREET_ORDER& streetOrder, CLIENT_ORDER& parentCLIENT_ORDER)
{
	_STRAT_LOG_DBUG_(CDEBUG << "StreetOrder ID [" << streetOrder.GetHandle()
					<<"] sending cancel!"
					<< std::endl);
	return SUCCESS;
}

int NikkeiSpread::OnStreetOrdValidCancel(STREET_ORDER& streetOrder, CLIENT_ORDER& parentOrder){return SUCCESS;}

int NikkeiSpread::OnStreetOrdAck(STREET_ORDER& streetOrder)
{
	_STRAT_LOG_DBUG_(CDEBUG << "StreetOrder ID [" << streetOrder.GetHandle()
					<<"] got ACK!."
					<< std::endl);

	return SUCCESS;
}

int NikkeiSpread::OnStreetExec(STREET_EXEC& StreetExec, CLIENT_ORDER& parentOrder)
{

	if(StreetExec.GetBrokerOrdId())
	{
		StreetExec.SetFixTag(37,StreetExec.GetBrokerOrdId());
	}
	if(StreetExec.GetExecId())
	{
		StreetExec.SetFixTag(17,StreetExec.GetExecId());
	}

	PrintStreetExec(StreetExec, parentOrder);

	if (StreetExec.GetClientId2() == LEGID_NKSP_Q ||  (StreetExec.GetStreetOrder()).GetClientId0() == LEGID_NKSP_Q
			|| strstr((StreetExec.GetStreetOrder()).GetUserMemo(), LEG_QUOTE))
	{
		CWARN << "StreetExec Handle [" << StreetExec.GetHandle()
				<< "] Symbol [" << StreetExec.GetSymbol()
				<< "] Side [" << StreetExec.GetSide()
				<< "] GetClientId2 [" << StreetExec.GetClientId2()
				<< "] GetClientId0 [" << (StreetExec.GetStreetOrder()).GetClientId0()
				<< "] GetUserMemo [" << (StreetExec.GetStreetOrder()).GetUserMemo()
				<< "] : Execs on Quote Order! Proceed to Handle Hedge Leg!"
				<< std::endl;

		HandleHedgeOnQuoteExecs(StreetExec, parentOrder);
	}

	return SUCCESS;
}

int NikkeiSpread::OnStreetOrdRej(STREET_ORDER& streetOrder, CLIENT_ORDER& parentCLIENT_ORDER)
{
	_STRAT_LOG_VERB_(CWARN << "Street Order ID [" << streetOrder.GetHandle()
							<< "] with RefOrdId [" << streetOrder.GetRefOrdId()
							<< "] Symbol [" << streetOrder.GetSymbol()
							<< "] Side [" << streetOrder.GetSide()
							<< "] got REJECTED!"
							<< std::endl);

	return SUCCESS;
}

int NikkeiSpread::OnStreetOrdOut(STREET_ORDER& so)
{
	if (so.GetOrderState() == STATE_FILLED)
	{
		_STRAT_LOG_VERB_(CWARN << "StreetOrder ID [" << so.GetHandle()
				<< "] with State [" << StateToStr(so.GetOrderState())
				<< "] was OUTED! Symbol [" << so.GetSymbol()
				<< "] Side [" << so.GetSide()
				<< "] - Ignore OUT!"
				<< std::endl);
		return SUCCESS;
	}

	return SUCCESS;
}

int NikkeiSpread::OnStreetStatusReport(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder)
{
	PrintStreetExec(StreetExec, ParentOrder);

	return SUCCESS;
}

int NikkeiSpread::OnStreetNewReportStat(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder)
{
	PrintStreetExec(StreetExec, ParentOrder);

	return SUCCESS;
}

int NikkeiSpread::OnStreetDiscardOrdCancel(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder)
{
	_STRAT_LOG_VERB_(CWARN << "StreetOrd ID [" << StreetOrd.GetHandle()
							<< "] Symbol [" << StreetOrd.GetSymbol()
							<< "] Side [" << StreetOrd.GetSide()
							<< "] Leaves [" << StreetOrd.GetTotalLvs()
							<< "] with State [" << StateToStr(StreetOrd.GetOrderState())
							<< "!"
							<< std::endl);

	return SUCCESS;
}

int NikkeiSpread::OnTimer(EVENT_DATA& eventData){return SUCCESS;}

//Custom
StratParams* NikkeiSpread::GetActiveStratParamsByClientOrderId(const char* pszClientOrderId)
{

	// STEP 1 : Validate StratID
	STRATIDMAPITER mStIdIter = gmStratIdMap.find(string(pszClientOrderId));
	long nStratId = -1;
	if(mStIdIter != gmStratIdMap.end())
	{
		nStratId = (long) mStIdIter->second;
		_STRAT_LOG_DBUG_(CDEBUG << "Found Strat ID [" << nStratId
								<< "] with OrderID [" << pszClientOrderId
								<< "]"
								<< std::endl);
	}
	else
	{
		_STRAT_LOG_VERB_(CWARN << "OrderID [" << pszClientOrderId << "] not found in gmStratIdMap!" << std::endl);
		return NULL;
	}

	// STEP 2: Get StratParams from Global Map gmStratMap
	STRATMAPITER mStIter = gmStratMap.find(nStratId);
	StratParams *pStratParams = NULL;
	if(mStIter != gmStratMap.end())
	{
		pStratParams = (StratParams*) mStIter->second;
		_STRAT_LOG_DBUG_(CDEBUG << "Found StratParams with StratID [" << nStratId << "]"  << std::endl);
	}
	else
	{
		_STRAT_LOG_VERB_(CWARN << "StratID [" << nStratId << "] not found in gmStratMap!" << std::endl);
		return NULL;
	}

	if(!pStratParams)
	{
		_STRAT_LOG_VERB_(CWARN << "Invalid StratParam Pointer!" << std::endl);
		return NULL;
	}

	return pStratParams;
}

bool NikkeiSpread::IsValidMarketTick(MTICK& mtick)
{
	_STRAT_LOG_DBUG_(CDEBUG << "Market data: Symbol [" << mtick.GetSymbol()
							<<"] : Bid [" << mtick.GetBid()
							<<"] BidSize [" << mtick.GetBidSize()
							<< "] / Ask [" << mtick.GetAsk()
							<< "] AskSize [" << mtick.GetAskSize()
							<< "]"
							<< std::endl);

	if(mtick.GetBid() < g_dEpsilon ||  mtick.GetAsk() < g_dEpsilon || mtick.GetBid() - mtick.GetAsk() > g_dEpsilon)
	{
		_STRAT_LOG_VERB_(CERR << "Invalid Prices in market data! Symbol [" << mtick.GetSymbol()
				<<"] : Bid [" << mtick.GetBid()
				<< "] Ask [" << mtick.GetAsk() << "]"
				<< std::endl);
		return false;
	}
	return true;
}

int	NikkeiSpread::CalculateNetStratExecs(StratParams* pStratParams)
{
	// STEP 1 : Get all client order
	CLIENT_ORDER QuoteOrder, HedgeOrder, OfferQuoteOrder, OfferHedgeOrder;
	GetQuoteClientOrder(QuoteOrder, pStratParams);
	GetHedgeClientOrder(HedgeOrder, pStratParams);

	// STEP 2 : Calculate current NetStratExecs
	int nNetStratExecs = pStratParams->nOrdLot + 1;
	nNetStratExecs = QuoteOrder.GetStreetExecs() - HedgeOrder.GetStreetExecs();

	return nNetStratExecs;
}


int NikkeiSpread::ValidateFIXTags(ORDER& order)
{
	// FIX_TAG_DESTINATION: 9501
	std::string sFixDestination = order.GetFixTag(FIX_TAG_DESTINATION);
	if(sFixDestination.empty())
	{
		std::stringstream errMsg;
		errMsg << "Please set destination in tag [" << FIX_TAG_DESTINATION << "]";
		CERR << "Order Validation Failed, cannot send to destination " << sFixDestination
				<< " , rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	// FIX_TAG_OMUSER: 9502
    std::string szOMUser = order.GetFixTag(FIX_TAG_OMUSER);
	if(szOMUser.length() <= 0 )
	{
		std::stringstream errMsg;
		errMsg << "Please set OMUser in tag [" << FIX_TAG_OMUSER << "]";
		CERR << "Order Validation Failed, invalid OM User, rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	// FIX_TAG_STRATID_NKSP: 9503
	std::string sStratId = order.GetFixTag(FIX_TAG_STRATID_NKSP);
	if(sStratId.empty())
	{
		std::stringstream errMsg;
		errMsg << "Please set Strat Id in tag [" << FIX_TAG_STRATID_NKSP << "]";
		CERR << "Order Validation Failed, Strat Id is not set , rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	long nStratId = atol(order.GetFixTag(FIX_TAG_STRATID_NKSP));
	if(gmStratMap.count(nStratId) >= 2 )
	{
		std::stringstream errMsg;
		errMsg << "Invalid StratId in tag [" << FIX_TAG_STRATID_NKSP << "]";
		CERR << "Order Validation Failed, duplicate Strat Id [" << nStratId
				<< "], rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	// FIX_TAG_STRATPORT_NKSP: 9504
	std::string sStratPort = order.GetFixTag(FIX_TAG_STRATPORT_NKSP);
	if(sStratPort.empty())
	{
		std::stringstream errMsg;
		errMsg << "Please set StratPort in tag [" << FIX_TAG_STRATPORT_NKSP << "]";
		CERR << "Order Validation Failed, StratPort is not set , rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	// FIX_TAG_LEGID_NKSP: 9505
	std::string sLegId = order.GetFixTag(FIX_TAG_LEGID_NKSP);
	if(sLegId.empty())
	{
		std::stringstream errMsg;
		errMsg << "Please set Leg Id in tag [" << FIX_TAG_LEGID_NKSP << "]";
		CERR << "Order Validation Failed, Leg Id is not set , rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	int nLegId = atoi(order.GetFixTag(FIX_TAG_LEGID_NKSP));
	if(nLegId < LEGID_MIN_NKSP || nLegId > LEGID_MAX_NKSP )
	{
		std::stringstream errMsg;
		errMsg << "Invalid LegId in tag [" << FIX_TAG_LEGID_NKSP <<"]";
		CERR << "Order Validation Failed, invalid Leg Id [" << nLegId << "], rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	// FIX_TAG_RUNNING_NKSP: 9506
	int nStratRunning = atoi(order.GetFixTag(FIX_TAG_RUNNING_NKSP));
	if(!(nStratRunning == 0 || nStratRunning == 1))
	{
		std::stringstream errMsg;
		errMsg << "Invalid Running Flag in tag [" << FIX_TAG_RUNNING_NKSP <<"]";
		CERR << "Order Validation Failed, invalid strat playpause tag [" << nStratRunning << "], rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	// FIX_TAG_MODE_NKSP: 9507
	std::string sFixQuotingMode = order.GetFixTag(FIX_TAG_MODE_NKSP);
	if(sFixQuotingMode.empty())
	{
		std::stringstream errMsg;
		errMsg << "Please set QuotingMode in tag [" << FIX_TAG_MODE_NKSP << "]";
		CERR << "Order Validation Failed, cannot determine quoting mode " << sFixQuotingMode
				<< " , rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	int nQuotingMode = atoi(order.GetFixTag(FIX_TAG_MODE_NKSP));
	if(nQuotingMode < MODEID_MIN_NKSP || nQuotingMode > MODEID_MAX_NKSP )
	{
		std::stringstream errMsg;
		errMsg << "Invalid QuoteModeId in tag [" << FIX_TAG_MODE_NKSP <<"]";
		CERR << "Order Validation Failed, invalid QuoteModeId [" << nQuotingMode << "], rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}


	// FIX_TAG_ORDQTY_NKSP: 9510
	std::string sOrdQty = order.GetFixTag(FIX_TAG_ORDQTY_NKSP);
	if(sOrdQty.empty())
	{
		std::stringstream errMsg;
		errMsg << "Please set OrdQty in tag [" << FIX_TAG_ORDQTY_NKSP << "]";
		CERR << "Order Validation Failed, Ord Qty is not set , rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	int nOrderQty = atoi(order.GetFixTag(FIX_TAG_ORDQTY_NKSP));
	if(nOrderQty <= 0)
	{
		std::stringstream errMsg;
		errMsg << "Invalid OrdQty in tag [" << FIX_TAG_ORDQTY_NKSP <<"]";
		CERR << "Order Validation Failed, invalid order quantity [" << nOrderQty << "], rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	// FIX_TAG_TOTALQTY_NKSP: 9511
	std::string sTotalQty = order.GetFixTag(FIX_TAG_TOTALQTY_NKSP);
	if(sTotalQty.empty())
	{
		std::stringstream errMsg;
		errMsg << "Please set Total Lots in tag [" << FIX_TAG_TOTALQTY_NKSP << "]";
		CERR << "Order Validation Failed, Total Lots is not set , rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	int nTotalQty = atoi(order.GetFixTag(FIX_TAG_TOTALQTY_NKSP));
	if(nTotalQty <= 0)
	{
		std::stringstream errMsg;
		errMsg << "Invalid TotalQty in tag [" << FIX_TAG_TOTALQTY_NKSP <<"]";
		CERR << "Order Validation Failed, invalid Total Qty [" << nTotalQty << "], rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	// FIX_TAG_PAYUPTICK_NKSP: 9512
	std::string sPayUpTick = order.GetFixTag(FIX_TAG_PAYUPTICK_NKSP);
	if(sPayUpTick.empty())
	{
		std::stringstream errMsg;
		errMsg << "Please set PayUp Tick in tag [" << FIX_TAG_PAYUPTICK_NKSP << "]";
		CERR << "Order Validation Failed, Payup Tick is not set , rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	int nPayUpTick = atoi(order.GetFixTag(FIX_TAG_PAYUPTICK_NKSP));
	if(nPayUpTick < 0)
	{
		std::stringstream errMsg;
		errMsg << "Invalid PayUp Tick in tag [" << FIX_TAG_PAYUPTICK_NKSP <<"]";
		CERR << "Order Validation Failed, invalid PayUp Tick [" << nPayUpTick << "], rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	// FIX_TAG_BENCHMARK_NKSP: 9513
	std::string sBenchmark = order.GetFixTag(FIX_TAG_BENCHMARK_NKSP);
	if(sBenchmark.empty())
	{
		std::stringstream errMsg;
		errMsg << "Please set Benchmark in tag [" << FIX_TAG_BENCHMARK_NKSP << "]";
		CERR << "Order Validation Failed, Benchmark is not set , rejecting client order!!! " << errMsg.str() << std::endl;
		order.SetRejectMsg(errMsg.str());
		return FAILURE;
	}

	return SUCCESS;
}

int NikkeiSpread::AddStratParamsToMap(ORDER& order, STRATMAP& mStratMap )
{
	//Populate Structure and add to map
	long nStratId = atol(order.GetFixTag(FIX_TAG_STRATID_NKSP));
	int nLegId = atoi(order.GetFixTag(FIX_TAG_LEGID_NKSP));

	_STRAT_LOG_DBUG_(CDEBUG << "Adding OrderID [" << order.GetHandle()
							<< "] in StratMap! : With StratID [" <<  nStratId
							<< "] LegID [" << nLegId
							<< "]"
							<<  std::endl);

	if(mStratMap.count(nStratId) <= 0)
	{
		//First Entry into Strat
		_STRAT_LOG_DBUG_(CDEBUG << "StratID [" << nStratId << "] : First entry to mStratMap!" <<  std::endl);

		StratParams *pStratParams = new StratParams();
		if(!pStratParams)
			return FAILURE;

		pStratParams->nStratId = atol(order.GetFixTag(FIX_TAG_STRATID_NKSP));
		strcpy(pStratParams->szStratPort, order.GetFixTag(FIX_TAG_STRATPORT_NKSP));

		// LEGID_NKSP_Q : Quote
		if(nLegId == LEGID_NKSP_Q)
		{
			_STRAT_LOG_DBUG_(CDEBUG << "Adding OrderID [" << order.GetHandle()
										<< "] for Quote Leg! : With StratID [" <<  nStratId
										<< "] LegID [" << nLegId
										<< "]"
										<<  std::endl);

			strcpy(pStratParams->szSymbolQuote,order.GetSymbol());
			strcpy(pStratParams->szClientOrdIdQuote,order.GetHandle());
			strcpy(pStratParams->szPortfolioQuote,order.GetPortfolio());
			pStratParams->dBenchmark = atof(order.GetFixTag(FIX_TAG_BENCHMARK_NKSP));
			pStratParams->nOrdLot = atoi(order.GetFixTag(FIX_TAG_ORDQTY_NKSP));
			pStratParams->nTotalLot = atoi(order.GetFixTag(FIX_TAG_TOTALQTY_NKSP));
			pStratParams->nPayupTicks = atoi(order.GetFixTag(FIX_TAG_PAYUPTICK_NKSP));
			pStratParams->enSideQuote = order.GetSide();

			switch (atoi(order.GetFixTag(FIX_TAG_MODE_NKSP)))
			{
				case  MODEID_ALWAYS :
					pStratParams->eStratMode = STRAT_MODE_ALWAYS;
					break;
				case MODEID_SILENT :
					pStratParams->eStratMode = STRAT_MODE_SILENT;
					break;
				default :
					pStratParams->eStratMode = STRAT_MODE_INVALID;
			}

			pStratParams->bQuoteEntry = true;

		}
		// LEGID_NKSP_H : Hedge
		else if(nLegId == LEGID_NKSP_H)
		{
			_STRAT_LOG_DBUG_(CDEBUG << "Adding OrderID [" << order.GetHandle()
										<< "] Hedge Leg! : With StratID [" <<  nStratId
										<< "] LegID [" << nLegId
										<< "]"
										<<  std::endl);

			strcpy(pStratParams->szSymbolHedge,order.GetSymbol());
			strcpy(pStratParams->szClientOrdIdHedge,order.GetHandle());
			strcpy(pStratParams->szPortfolioHedge,order.GetPortfolio());
			pStratParams->enSideHedge = order.GetSide();
			pStratParams->bHedgeEntry = true;
		}

		strcpy(pStratParams->szOMUser,order.GetFixTag(FIX_TAG_OMUSER));
		strcpy(pStratParams->szDest,order.GetFixTag(FIX_TAG_DESTINATION));

		//Optimisation to save fix tag string construction on market data
		pStratParams->bStratRunning=(bool)atoi(order.GetFixTag(FIX_TAG_RUNNING_NKSP));

		mStratMap.insert(pair<int,StratParams*>(nStratId,pStratParams));

		_STRAT_LOG_DBUG_(PrintStratOrderMap(gmStratMap, nStratId));
	}
	else
	{
		//Following Entry into Strat
		_STRAT_LOG_DBUG_(CDEBUG << "StratID [" << nStratId << "] : Subsequent entry to mStratMap!" << std::endl);
		STRATMAPITER mStIter = mStratMap.find(nStratId);

		if (mStIter == mStratMap.end())
		{

			CERR << "StratId [" << nStratId
					<< "] has count [" << mStratMap.count(nStratId)
					<< "] but failed to find in gmStratMap!!!" << std::endl;

			return FAILURE;
		}

		StratParams *pStratParams = (StratParams*) mStIter->second;

		if(!pStratParams)
			return FAILURE;

		// LEGID_NKSP_Q : Quote
		if(nLegId == LEGID_NKSP_Q)
		{
			_STRAT_LOG_DBUG_(CDEBUG << "Adding OrderID [" << order.GetHandle()
										<< "] for Quote Leg! : With StratID [" <<  nStratId
										<< "] LegID [" << nLegId
										<< "]"
										<<  std::endl);

			strcpy(pStratParams->szSymbolQuote,order.GetSymbol());
			strcpy(pStratParams->szClientOrdIdQuote,order.GetHandle());
			strcpy(pStratParams->szPortfolioQuote,order.GetPortfolio());
			pStratParams->dBenchmark = atof(order.GetFixTag(FIX_TAG_BENCHMARK_NKSP));
			pStratParams->nOrdLot = atoi(order.GetFixTag(FIX_TAG_ORDQTY_NKSP));
			pStratParams->nTotalLot = atoi(order.GetFixTag(FIX_TAG_TOTALQTY_NKSP));
			pStratParams->nPayupTicks = atoi(order.GetFixTag(FIX_TAG_PAYUPTICK_NKSP));
			pStratParams->enSideQuote = order.GetSide();

			switch (atoi(order.GetFixTag(FIX_TAG_MODE_NKSP)))
			{
				case  MODEID_ALWAYS :
					pStratParams->eStratMode = STRAT_MODE_ALWAYS;
					break;
				case MODEID_SILENT :
					pStratParams->eStratMode = STRAT_MODE_SILENT;
					break;
				default :
					pStratParams->eStratMode = STRAT_MODE_INVALID;
			}

			pStratParams->bQuoteEntry = true;

		}
		// LEGID_NKSP_H : Hedge
		if(nLegId == LEGID_NKSP_H)
		{
			_STRAT_LOG_DBUG_(CDEBUG << "Adding OrderID [" << order.GetHandle()
										<< "] Hedge Leg! : With StratID [" <<  nStratId
										<< "] LegID [" << nLegId
										<< "]"
										<<  std::endl);

			strcpy(pStratParams->szSymbolHedge,order.GetSymbol());
			strcpy(pStratParams->szClientOrdIdHedge,order.GetHandle());
			strcpy(pStratParams->szPortfolioHedge,order.GetPortfolio());
			pStratParams->enSideHedge = order.GetSide();
			pStratParams->bHedgeEntry = true;

		}

		if (pStratParams->bQuoteEntry && pStratParams->bHedgeEntry)
		{
			_STRAT_LOG_DBUG_(CDEBUG << "StratID [" <<  pStratParams->nStratId
									<< "] is ready in StratParamMap!"
									<< std::endl);

			pStratParams->bStratReady = true;

			PrintStratOrderMap(mStratMap, nStratId);
		}
	}

	return SUCCESS;
}

int NikkeiSpread::GetQuoteClientOrder(CLIENT_ORDER& QuoteOrder, StratParams* pStratParams)
{
	if(!GetClientOrderById(QuoteOrder, pStratParams->szClientOrdIdQuote))
	{
		CERR << "StratID [" << pStratParams->nStratId
				<< "] : Failed to get bidding quote order by ID [ " << pStratParams->szClientOrdIdQuote
				<< "]"
				<< std::endl;
		return FAILURE;
	}
	return SUCCESS;
}

int NikkeiSpread::GetHedgeClientOrder(CLIENT_ORDER& HedgeOrder, StratParams* pStratParams)
{
	if(!GetClientOrderById(HedgeOrder, pStratParams->szClientOrdIdHedge))
	{
		CERR << "StratID [" << pStratParams->nStratId
				<< "] : Failed to get bidding hedge order by ID [ " << pStratParams->szClientOrdIdHedge
				<< "]"
				<< std::endl;
		return FAILURE;
	}
	return SUCCESS;
}

// Return: TRUE - if adjustment is completed; FALSE - if adjustment is in progress
bool NikkeiSpread::IsAdjustComplete (StratParams* pStratParams, int nNetStratExecs)
{
	CLIENT_ORDER QuoteOrder, HedgeOrder;
	GetQuoteClientOrder(QuoteOrder, pStratParams);
	GetHedgeClientOrder(HedgeOrder, pStratParams);

	bool bAdjustComplete = false;

	if (nNetStratExecs == 0)
	{
		// Strat is hedged - There should be no working orders on hedge contract
		bAdjustComplete = (HedgeOrder.GetWorkingSize() == 0);

	}
	else if (nNetStratExecs > 0)
	{
		// Strat is un-hedge - Hedge working size should be equal to nNetStratExecs
		bAdjustComplete = (QuoteOrder.GetWorkingSize() == 0);
	}
	else
	{
		// Strat is over-hedged - There should be no working orders on hedge contract
		bAdjustComplete = (HedgeOrder.GetWorkingSize() == 0);
	}

	if (bAdjustComplete)
	{
		_STRAT_LOG_DBUG_(CDEBUG << "Strat ID [" << pStratParams-> nStratId
									<< "] NetStratExecs [" << nNetStratExecs
									<< "] - QuoteOrder WorkingSize [" << QuoteOrder.GetWorkingSize()
									<< "] - HedgeOrder WorkingSize [" << HedgeOrder.GetWorkingSize()
									<< "] : Position Adjusted!"
									<<	std::endl);
	}
	else
	{
		_STRAT_LOG_DBUG_(CDEBUG << "Strat ID [" << pStratParams-> nStratId
									<< "] NetStratExecs [" << nNetStratExecs
									<< "] - QuoteOrder WorkingSize [" << QuoteOrder.GetWorkingSize()
									<< "] - HedgeOrder WorkingSize [" << HedgeOrder.GetWorkingSize()
									<< "] : Position NOT Adjusted!"
									<<	std::endl);
	}

	return bAdjustComplete;
}

// Adjust working size on hedge contract
// If called from OnMD
int NikkeiSpread::HandleAdjustWorkingSize (StratParams* pStratParams, int nNetStratExecs)
{
	CLIENT_ORDER QuoteOrder, HedgeOrder;
	GetQuoteClientOrder(QuoteOrder, pStratParams);
	GetHedgeClientOrder(HedgeOrder, pStratParams);

	int nTargetPos = 0;
	nTargetPos = abs(nNetStratExecs);

	if (nNetStratExecs == 0)
	{
		// Strat is hedged - Cancel all orders on hedge contract
		if (HedgeOrder.GetWorkingSize() == 0)
		{
			return SUCCESS;
		}
		else
		{
			return CancelAllOrdersForClientOrder(HedgeOrder.GetHandle());
		}
	}
	else if (nNetStratExecs > 0)
	{
		// Strat is un-hedged
		if (QuoteOrder.GetWorkingSize() == 0)
		{
			return SUCCESS;
		}
		else
		{
			return CancelAllOrdersForClientOrder(QuoteOrder.GetHandle());
		}
	}
	else
	{
		// Strat is over-hedge
		if (HedgeOrder.GetWorkingSize() == 0)
		{
			return SUCCESS;
		}
		else
		{
			return CancelAllOrdersForClientOrder(HedgeOrder.GetHandle());
		}
	}

	return FAILURE;
}

int	NikkeiSpread::HandleQuoteUponMarketData (StratParams* pStratParams, MTICK& mtick_quote, MTICK& mtick_hedge)
{
	// STEP 1 : Get client order
	CLIENT_ORDER QuoteOrder;
	GetQuoteClientOrder(QuoteOrder, pStratParams);

	// STEP 2 : Calculate order quantity
	int nNewQuoteQty = 0;
	nNewQuoteQty = CalculateNewQuoteQuantity(pStratParams);
	if (nNewQuoteQty <= 0)
	{
		CERR << "StratId [" << pStratParams->nStratId
				<<"] Invalid new OrdQty [" << nNewQuoteQty
				<<"] : Not sending new quote order!"
				<< std::endl;
		return FAILURE;
	}


	// STEP 3 : Calculate order price
	double dNewQuotePrice = 0.0, dBenchmark=0.0;
	dBenchmark = pStratParams->dBenchmark;
	dNewQuotePrice = GetStratQuotePrice(dBenchmark, mtick_quote, mtick_hedge, pStratParams->enSideQuote);


	// STEP 4 : Send new/replace
	if (QuoteOrder.GetWorkingSize() == 0)
	{
		if (pStratParams->eStratMode == STRAT_MODE_SILENT)
		{
			if ((pStratParams->enSideQuote == SIDE_BUY && (mtick_quote.GetAsk() - dNewQuotePrice > g_dEpsilon) ))
			{
				_STRAT_LOG_VERB_(CWARN << "StratID [" << pStratParams -> nStratId
										<< "] Quote Mode [" << StratModeToStr(pStratParams->eStratMode)
										<< "] Symbol [" << pStratParams->szSymbolQuote
										<< "] Buy order price  [" << dNewQuotePrice
										<< "] less than Market Ask [" <<  mtick_quote.GetAsk()
										<< "] : NOT sending new order!"
										<< std::endl);
				return SUCCESS;
			}
			else if ((pStratParams->enSideQuote == SIDE_SELL && dNewQuotePrice - mtick_quote.GetBid() > g_dEpsilon))
			{
				_STRAT_LOG_VERB_(CWARN << "StratID [" << pStratParams -> nStratId
										<< "] Quote Mode [" << StratModeToStr(pStratParams->eStratMode)
										<< "] Symbol [" << pStratParams->szSymbolQuote
										<< "] Sell order price  [" << dNewQuotePrice
										<< "] greater than Market Bid [" <<  mtick_quote.GetBid()
										<< "] : NOT sending new order!"
										<< std::endl);
				return SUCCESS;
			}
			else
			{
				_STRAT_LOG_VERB_(CWARN << "StratID [" << pStratParams -> nStratId
										<< "] Quote Mode [" << StratModeToStr(pStratParams->eStratMode)
										<< "] Symbol [" << pStratParams->szSymbolQuote
										<< "] Side [" << (pStratParams->enSideQuote==SIDE_BUY?"Buy":"Sell")
										<< "] Price [" << dNewQuotePrice
										<< "] Matches with Market " << (pStratParams->enSideQuote==SIDE_BUY?"Ask":"Bid")
										<< " [" << (pStratParams->enSideQuote==SIDE_BUY?mtick_quote.GetAsk():mtick_quote.GetBid())
										<< "] : Proceed to send new order!"
										<< std::endl);
			}
		}

		// Send new quote with qty => nNewQuoteQty
		bool bStreetSent = SendOutStreetOrder(QuoteOrder, pStratParams->szDest, nNewQuoteQty, dNewQuotePrice);
		if (!bStreetSent)
		{
			CERR << "StratID [" << pStratParams -> nStratId
				<< "] Quote Symbol [" << pStratParams->szSymbolQuote
				<< "] : FAILED to send new Quote at Price [" << dNewQuotePrice
				<< "] Quantity [" << nNewQuoteQty
				<< "]"
				<< std::endl;

			return FAILURE;
		}
		else
		{
			CINFO << "StratID [" << pStratParams -> nStratId
					<< "] Quote Symbol [" << pStratParams->szSymbolQuote
					<< "] : NEW Quote sent at Price [" << dNewQuotePrice
					<< "] Quantity [" << nNewQuoteQty
					<< "]"
					<< std::endl;

			return SUCCESS;
		}
	}
	else
	{
		// Replace existing quote by change to -> nNewQuoteQty
		STREET_ORDER_CONTAINER SOrdContainer;
		STREET_ORDER ExistingStreetOrder;
		double dOldQuotePrice = 0.0;
		int nOldQuoteQty = 0;

		if (pStratParams->eStratMode == STRAT_MODE_SILENT)
		{
			_STRAT_LOG_VERB_(CWARN << "StratID [" << pStratParams -> nStratId
									<< "] Quote Mode [" << StratModeToStr(pStratParams->eStratMode)
									<< "] : NOT replacing exiting order!"
									<< std::endl);
			return SUCCESS;
		}

		if(SOrdContainer.GetFirstActiveByClientOrderId(ExistingStreetOrder, QuoteOrder.GetHandle()))
		{
			do
			{
				if(!CanReplace(ExistingStreetOrder))
					continue;

				dOldQuotePrice = ExistingStreetOrder.GetPrice();
				nOldQuoteQty = ExistingStreetOrder.GetSize();

				if ((nNewQuoteQty == nOldQuoteQty) &&  (fabs(dNewQuotePrice - dOldQuotePrice) < g_dEpsilon))
				{
					CWARN	<< "StratID [" << pStratParams->nStratId
							<< "] QuoteSymbol [" << QuoteOrder.GetSymbol()
							<< "] StreetOrder ID [" << ExistingStreetOrder.GetHandle()
							<< "] OldQuotePrice [" << dOldQuotePrice
							<< "] OldQuoteQty [" << nOldQuoteQty
							<< "] : Qty and Price same as NewQuotePrice [" << dNewQuotePrice
							<< "] NewQuoteQty [" << nNewQuoteQty
							<< "] - Not Sending Replace Quote order!"
							<< std::endl;
					continue;
				}

				ExistingStreetOrder.SetPrice(dNewQuotePrice);
				ExistingStreetOrder.SetSize(nNewQuoteQty);
				ExistingStreetOrder.SetRefOrdId(ExistingStreetOrder.GetHandle());
				ExistingStreetOrder.SetWaveName(ExistingStreetOrder.GetWaveName());

				ExistingStreetOrder.SetAccount(ExistingStreetOrder.GetAccount());

				ExistingStreetOrder.SetUserMemo(ExistingStreetOrder.GetUserMemo());

				if (ExistingStreetOrder.GetClientId2() == LEGID_NKSP_Q)
				{
					ExistingStreetOrder.SetClientId2(LEGID_NKSP_Q);
					ExistingStreetOrder.SetClientId0(LEGID_NKSP_Q);

				}
				else if  (ExistingStreetOrder.GetClientId0() == LEGID_NKSP_Q)
				{
					ExistingStreetOrder.SetClientId2(LEGID_NKSP_Q);
					ExistingStreetOrder.SetClientId0(LEGID_NKSP_Q);
				}
				else if (strstr(ExistingStreetOrder.GetUserMemo(), LEG_QUOTE))
				{
					ExistingStreetOrder.SetClientId2(LEGID_NKSP_Q);
					ExistingStreetOrder.SetClientId0(LEGID_NKSP_Q);
				}
				else
				{
					CWARN << "Existing StreetOrder ID [" << ExistingStreetOrder.GetHandle()
							<< "] GetClientId0 [" << ExistingStreetOrder.GetClientId0()
							<< "] GetClientId2 [" << ExistingStreetOrder.GetClientId2()
							<< "] GetUserMemo [" << ExistingStreetOrder.GetUserMemo()
							<< "] : Failed to match ClientId to LegId (1/2) and Memo!"
							<< std::endl;
				}

				enErrorCode errStreetReplace = ExistingStreetOrder.Replace();

				if(errStreetReplace != RULES_EO_OK)
				{
					CERR << "Replace Fail for existing StreetOrder ID [" << ExistingStreetOrder.GetHandle()
							<<"] errStreetReplace [" << ErrorCodeToStr(errStreetReplace)
							<<"]"
							<< std::endl;

					return FAILURE;
				}
				else
				{
					CINFO << "Successfully sent replace request for [" << ExistingStreetOrder.GetHandle()
							<< "] Symbol [" << ExistingStreetOrder.GetSymbol()
							<< "] ClientOrder ID [" << ExistingStreetOrder.GetClientOrdId()
							<< "] GetAccount [" << ExistingStreetOrder.GetAccount()
							<< "] GetClientId0 [" << ExistingStreetOrder.GetClientId0()
							<< "] GetClientId2 [" << ExistingStreetOrder.GetClientId2()
							<< "] GetUserMemo [" << ExistingStreetOrder.GetUserMemo()
							<< "] with NewQuotePrice [" << dNewQuotePrice
							<< "] NewQuoteQty [" << nNewQuoteQty
							<< "] !"
							<< std::endl;

					return SUCCESS;
				}
			}
			while(SOrdContainer.GetNextActiveByClientOrderId(ExistingStreetOrder));
		}
		return SUCCESS;
	}
}

int	NikkeiSpread::HandleHedgeUponMarketData (StratParams* pStratParams, MTICK& mtick_hedge)
{
	// STEP 1 : Get client order
	CLIENT_ORDER HedgeOrder;
	GetHedgeClientOrder(HedgeOrder, pStratParams);

	// STEP 2 : Calculate order quantity
	int nNewHedgeQty = 0;
	nNewHedgeQty = CalculateNewHedgeQuantity(pStratParams);
	if (nNewHedgeQty <= 0)
	{
		CERR << "StratId [" << pStratParams->nStratId
				<<"] Invalid new OrdQty [" << nNewHedgeQty
				<<"] : Not sending new hedge order!"
				<< std::endl;
		return FAILURE;
	}


	// STEP 3 : Calculate order price
	double dNewHedgePrice = 0.0;
	dNewHedgePrice = GetStratHedgePrice (pStratParams, mtick_hedge,  pStratParams->enSideHedge);

	// STEP 4 : Send replace
	if (HedgeOrder.GetWorkingSize() == 0)
	{
		// Send new hedge with qty => nNewHedgeQty
		bool bStreetSent = SendOutStreetOrder(HedgeOrder, pStratParams->szDest, nNewHedgeQty, dNewHedgePrice);
		if (!bStreetSent)
		{
			CERR << "StratID [" << pStratParams -> nStratId
				<< "] Hedge Symbol [" << pStratParams->szSymbolHedge
				<< "] : FAILED to send new Hedge at Price [" << dNewHedgePrice
				<< "] Quantity [" << nNewHedgeQty
				<< "]"
				<< std::endl;

			return FAILURE;
		}
		else
		{
			CINFO << "StratID [" << pStratParams -> nStratId
					<< "] Hedge Symbol [" << pStratParams->szSymbolHedge
					<< "] : NEW Quote sent at Price [" << dNewHedgePrice
					<< "] Quantity [" << nNewHedgeQty
					<< "]"
					<< std::endl;

			return SUCCESS;
		}
	}
	else
	{
		// Replace existing hedge by change qty to -> nNewHedgeQty
		STREET_ORDER_CONTAINER SOrdContainer;
		STREET_ORDER ExistingStreetOrder;
		double dOldHedgePrice = 0.0;
		int nOldHedgeQty = 0;


		if(SOrdContainer.GetFirstActiveByClientOrderId(ExistingStreetOrder, HedgeOrder.GetHandle()))
		{
			do
			{
				if(!CanReplace(ExistingStreetOrder))
					continue;

				dOldHedgePrice = ExistingStreetOrder.GetPrice();
				nOldHedgeQty = ExistingStreetOrder.GetSize();

				if ((nNewHedgeQty == nOldHedgeQty) &&  (fabs(dNewHedgePrice - dOldHedgePrice) < g_dEpsilon))
				{
					_STRAT_LOG_VERB_(CWARN	<< "StratID [" << pStratParams->nStratId
											<< "] HedgeSymbol [" << HedgeOrder.GetSymbol()
											<< "] StreetOrder ID [" << ExistingStreetOrder.GetHandle()
											<< "] OldHedgePrice [" << dOldHedgePrice
											<< "] OldHedgeQty [" << nOldHedgeQty
											<< "] : Qty and Price as NewHedgePrice [" << dNewHedgePrice
											<< "] NewHedgeQty [" << nNewHedgeQty
											<< "] - Not Sending Replace Hedge order!"
											<< std::endl);
					continue;
				}

				ExistingStreetOrder.SetPrice(dNewHedgePrice);
				ExistingStreetOrder.SetSize(nNewHedgeQty);
				ExistingStreetOrder.SetRefOrdId(ExistingStreetOrder.GetHandle());
				ExistingStreetOrder.SetWaveName(ExistingStreetOrder.GetWaveName());

				ExistingStreetOrder.SetAccount(ExistingStreetOrder.GetAccount());

				ExistingStreetOrder.SetUserMemo(ExistingStreetOrder.GetUserMemo());

				if (ExistingStreetOrder.GetClientId2() == LEGID_NKSP_H)
				{
					ExistingStreetOrder.SetClientId2(LEGID_NKSP_H);
					ExistingStreetOrder.SetClientId0(LEGID_NKSP_H);
				}
				else if  (ExistingStreetOrder.GetClientId0() == LEGID_NKSP_H)
				{
					ExistingStreetOrder.SetClientId2(LEGID_NKSP_H);
					ExistingStreetOrder.SetClientId0(LEGID_NKSP_H);
				}
				else if (strstr(ExistingStreetOrder.GetUserMemo(), LEG_HEDGE))
				{
					ExistingStreetOrder.SetClientId2(LEGID_NKSP_H);
					ExistingStreetOrder.SetClientId0(LEGID_NKSP_H);
				}
				else
				{
					CWARN << "Existing StreetOrder ID [" << ExistingStreetOrder.GetHandle()
							<< "] GetClientId0 [" << ExistingStreetOrder.GetClientId0()
							<< "] GetClientId2 [" << ExistingStreetOrder.GetClientId2()
							<< "] GetUserMemo [" << ExistingStreetOrder.GetUserMemo()
							<< "] : Failed to match ClientId to LegId (1/2) and Memo!"
							<< std::endl;
				}

				enErrorCode errStreetReplace = ExistingStreetOrder.Replace();

				if(errStreetReplace != RULES_EO_OK)
				{
					CERR << "Replace Fail for existing StreetOrder ID [" << ExistingStreetOrder.GetHandle()
							<<"] errStreetReplace [" << ErrorCodeToStr(errStreetReplace)
							<<"]"
							<< std::endl;

					return FAILURE;
				}
				else
				{
					CINFO << "Successfully sent replace request for [" << ExistingStreetOrder.GetHandle()
							<< "] Symbol [" << ExistingStreetOrder.GetSymbol()
							<< "] ClientOrder ID [" << ExistingStreetOrder.GetClientOrdId()
							<< "] GetAccount [" << ExistingStreetOrder.GetAccount()
							<< "] GetClientId0 [" << ExistingStreetOrder.GetClientId0()
							<< "] GetClientId2 [" << ExistingStreetOrder.GetClientId2()
							<< "] GetUserMemo [" << ExistingStreetOrder.GetUserMemo()
							<< "] with NewHedgePrice [" << dNewHedgePrice
							<< "] NewHedgeQty [" << nNewHedgeQty
							<< "] !"
							<< std::endl;

					return SUCCESS;
				}
			}
			while(SOrdContainer.GetNextActiveByClientOrderId(ExistingStreetOrder));
		}
		return SUCCESS;
	}
}

int NikkeiSpread::HandleHedgeOnQuoteExecs(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder)
{
	// STEP 1 : Get StratParams and client order
	StratParams *pStratParams = NULL;
	pStratParams = GetActiveStratParamsByClientOrderId(ParentOrder.GetHandle());
	if (!pStratParams)
	{
		_STRAT_LOG_VERB_(CERR << "Failed to get StratParam with ClientOrderId [" << ParentOrder.GetHandle() << "]" << std::endl);
		return FAILURE;
	}

	CLIENT_ORDER HedgeOrder;
	GetHedgeClientOrder(HedgeOrder, pStratParams);

	// STEP 2 : Calculate order quantity
	int nNewHedgeQty = 0;
	nNewHedgeQty = CalculateNewHedgeQuantity(pStratParams);
	if (nNewHedgeQty <= 0)
	{
		CERR << "StratId [" << pStratParams->nStratId
				<<"] Invalid new OrdQty [" << nNewHedgeQty
				<<"] : Not sending new hedge order!"
				<< std::endl;
		return FAILURE;
	}

	// STEP 3 : Calculate order price
	MTICK  mtick_hedge;
	MarketDataGetSymbolInfo(pStratParams->szSymbolHedge, mtick_hedge);

	double dNewHedgePrice = 0.0;
	dNewHedgePrice = GetStratHedgePrice (pStratParams, mtick_hedge,  pStratParams->enSideHedge);

	// STEP 4 : Send replace
	if (HedgeOrder.GetWorkingSize() == 0)
	{
		// Send new hedge with qty => nNewHedgeQty
		bool bStreetSent = SendOutStreetOrder(HedgeOrder, pStratParams->szDest, nNewHedgeQty, dNewHedgePrice);
		if (!bStreetSent)
		{
			CERR << "StratID [" << pStratParams -> nStratId
				<< "] Hedge Symbol [" << pStratParams->szSymbolHedge
				<< "] : FAILED to send new Hedge at Price [" << dNewHedgePrice
				<< "] Quantity [" << nNewHedgeQty
				<< "]"
				<< std::endl;

			return FAILURE;
		}
		else
		{
			CINFO << "StratID [" << pStratParams -> nStratId
					<< "] Hedge Symbol [" << pStratParams->szSymbolHedge
					<< "] : NEW Hedge sent at Price [" << dNewHedgePrice
					<< "] Quantity [" << nNewHedgeQty
					<< "]"
					<< std::endl;

			return SUCCESS;
		}
	}
	else
	{
		// PROJ_NSP_v1.0.03.20130723 : Disabled auto-adjust price for hedging leg active order

		CWARN << "StratID [" << pStratParams->nStratId
				<< "] HedgeSymbol [" << HedgeOrder.GetSymbol()
				<< "] HedgeClientOrderId [" << HedgeOrder.GetHandle()
				<< "] has WorkingSize [" << HedgeOrder.GetWorkingSize()
				<< "] - Not changing Hedge order!"
				<< std::endl;


		// PROJ_NSP_v1.0.03.20130723 : Keeping the below code in case auto-adjust is later requested
/*
		// Replace existing hedge by change qty to -> nNewHedgeQty
		STREET_ORDER_CONTAINER SOrdContainer;
		STREET_ORDER ExistingStreetOrder;
		double dOldHedgePrice = 0.0;
		int nOldHedgeQty = 0;


		if(SOrdContainer.GetFirstActiveByClientOrderId(ExistingStreetOrder, HedgeOrder.GetHandle()))
		{
			do
			{
				if(!CanReplace(ExistingStreetOrder))
					continue;

				dOldHedgePrice = ExistingStreetOrder.GetPrice();
				nOldHedgeQty = ExistingStreetOrder.GetSize();

				if ((nNewHedgeQty == nOldHedgeQty) &&  (dOldHedgePrice == dOldHedgePrice))
				{
					_STRAT_LOG_VERB_(CWARN	<< "StratID [" << pStratParams->nStratId
											<< "] HedgeSymbol [" << HedgeOrder.GetSymbol()
											<< "] StreetOrder ID [" << ExistingStreetOrder.GetHandle()
											<< "] OldHedgePrice [" << dOldHedgePrice
											<< "] OldHedgeQty [" << nOldHedgeQty
											<< "] : Qty and Price as OldHedgePrice [" << dOldHedgePrice
											<< "] NewHedgeQty [" << nNewHedgeQty
											<< "] - Not Sending Replace Hedge order!"
											<< std::endl);
					continue;
				}

				ExistingStreetOrder.SetPrice(dOldHedgePrice);
				ExistingStreetOrder.SetSize(nNewHedgeQty);
				ExistingStreetOrder.SetRefOrdId(ExistingStreetOrder.GetHandle());
				ExistingStreetOrder.SetWaveName(ExistingStreetOrder.GetWaveName());

				ExistingStreetOrder.SetAccount(ExistingStreetOrder.GetAccount());

				ExistingStreetOrder.SetUserMemo(ExistingStreetOrder.GetUserMemo());

				if (ExistingStreetOrder.GetClientId2() == LEGID_NKSP_H)
				{
					ExistingStreetOrder.SetClientId2(LEGID_NKSP_H);
					ExistingStreetOrder.SetClientId0(LEGID_NKSP_H);
				}
				else if  (ExistingStreetOrder.GetClientId0() == LEGID_NKSP_H)
				{
					ExistingStreetOrder.SetClientId2(LEGID_NKSP_H);
					ExistingStreetOrder.SetClientId0(LEGID_NKSP_H);
				}
				else if (strstr(ExistingStreetOrder.GetUserMemo(), LEG_HEDGE))
				{
					ExistingStreetOrder.SetClientId2(LEGID_NKSP_H);
					ExistingStreetOrder.SetClientId0(LEGID_NKSP_H);
				}
				else
				{
					CWARN << "Existing StreetOrder ID [" << ExistingStreetOrder.GetHandle()
							<< "] GetClientId0 [" << ExistingStreetOrder.GetClientId0()
							<< "] GetClientId2 [" << ExistingStreetOrder.GetClientId2()
							<< "] GetUserMemo [" << ExistingStreetOrder.GetUserMemo()
							<<"] : Failed to match ClientId to LegId (1/2) and Memo!"
							<< std::endl;
				}

				enErrorCode errStreetReplace = ExistingStreetOrder.Replace();

				if(errStreetReplace != RULES_EO_OK)
				{
					CERR << "Replace Fail for existing StreetOrder ID [" << ExistingStreetOrder.GetHandle()
							<<"] errStreetReplace [" << ErrorCodeToStr(errStreetReplace)
							<<"]"
							<< std::endl;

					return FAILURE;
				}
				else
				{
					CINFO << "Successfully sent replace request for [" << ExistingStreetOrder.GetHandle()
							<< "] Symbol [" << ExistingStreetOrder.GetSymbol()
							<< "] ClientOrder ID [" << ExistingStreetOrder.GetClientOrdId()
							<< "] GetAccount [" << ExistingStreetOrder.GetAccount()
							<< "] GetClientId0 [" << ExistingStreetOrder.GetClientId0()
							<< "] GetClientId2 [" << ExistingStreetOrder.GetClientId2()
							<< "] GetUserMemo [" << ExistingStreetOrder.GetUserMemo()
							<< "] with OldHedgePrice [" << dOldHedgePrice
							<< "] NewHedgeQty [" << nNewHedgeQty
							<< "] !"
							<< std::endl;

					return SUCCESS;
				}
			}
			while(SOrdContainer.GetNextActiveByClientOrderId(ExistingStreetOrder));
		}
		return SUCCESS;
*/
	}

	return SUCCESS;
}

double NikkeiSpread::GetStratQuotePrice (double dBenchmark, MTICK& mtick_quote, MTICK& mtick_hedge, enOrderSide enSideQuote)
{
	double dQuotePrice = 0.0;

	if (enSideQuote == SIDE_BUY)
	{
		dQuotePrice = mtick_hedge.GetBid() + dBenchmark;
		if (dQuotePrice - mtick_quote.GetAsk() > g_dEpsilon)	{	dQuotePrice = mtick_quote.GetAsk();	}

	}
	else if (enSideQuote == SIDE_SELL)
	{
		dQuotePrice = mtick_hedge.GetAsk() + dBenchmark;
		if (mtick_quote.GetBid() - dQuotePrice > g_dEpsilon)	{	dQuotePrice = mtick_quote.GetBid();	}
	}
	else
	{
		return -dQuotePrice;
	}


	CINFO << "Quote Symbol [" << mtick_quote.GetSymbol()
			<< "] Benchmark [" << dBenchmark
			<< "] Side [" << enSideQuote
			<< "] - Quote Bid/Ask [" << mtick_quote.GetBid() << "/" << mtick_quote.GetAsk()
			<< "] Hedge Bid/Ask [" <<  mtick_hedge.GetBid() << "/" << mtick_hedge.GetAsk()
			<< "] : Setting New Price = [" << dQuotePrice
			<< "]"
			<< std::endl;

	return dQuotePrice;
}

double NikkeiSpread::GetStratHedgePrice (StratParams* pStratParams, MTICK& mtick_hedge,  enOrderSide enSideHedge)
{
	double dHedgePrice = 0.0;

	if (enSideHedge == SIDE_BUY)
	{
		dHedgePrice = mtick_hedge.GetAsk() + pStratParams->nPayupTicks * GetTickSizeForSymbol(pStratParams->szSymbolHedge) ;

	}
	else if (enSideHedge == SIDE_SELL)
	{
		dHedgePrice = mtick_hedge.GetBid() - pStratParams->nPayupTicks * GetTickSizeForSymbol(pStratParams->szSymbolHedge) ;
	}
	else
	{
		return -dHedgePrice;
	}

	CINFO << "Hedge Symbol [" << mtick_hedge.GetSymbol()
			<< "] Side [" << enSideHedge
			<< "] Hedge Bid/Ask [" <<  mtick_hedge.GetBid() << "/" << mtick_hedge.GetAsk()
			<< "] : Setting New Price = [" << dHedgePrice
			<< "]"
			<< std::endl;

	return dHedgePrice;
}

int	NikkeiSpread::CalculateNewQuoteQuantity(StratParams* pStratParams)
{
	int nOrdQty = 1, nNetStratExecs = 0;
	nNetStratExecs = CalculateNetStratExecs(pStratParams);

	nOrdQty = nNetStratExecs < 0 ?  abs(nNetStratExecs) : pStratParams->nOrdLot;

	_STRAT_LOG_VERB_(CWARN << "StratID [" << pStratParams -> nStratId
							<< "] QuoteSymbol [" << pStratParams->szSymbolQuote
							<< "] QuoteSide [" << pStratParams->enSideQuote
							<< "] nOrdLot [" << pStratParams->nOrdLot
							<< "] nNetStratExecs [" << nNetStratExecs
							<< "] -> Setting New Quote OrdQty = [" << nOrdQty
							<< "]"
							<< std::endl);
	return nOrdQty;
}

int	NikkeiSpread::CalculateNewHedgeQuantity(StratParams* pStratParams)
{
	int nOrdQty = 0, nNetStratExecs = 0;
	nNetStratExecs = CalculateNetStratExecs(pStratParams);

	nOrdQty = nNetStratExecs > 0 ? abs(nNetStratExecs) : 0;

	_STRAT_LOG_VERB_(CWARN << "StratID [" << pStratParams -> nStratId
							<< "] HedgeSymbol [" << pStratParams->szSymbolHedge
							<< "] HedgeSide [" << pStratParams->enSideHedge
							<< "] nOrdLot [" << pStratParams->nOrdLot
							<< "] nNetStratExecs [" << nNetStratExecs
							<< "] -> Setting New Hedge OrdQty = [" << nOrdQty
							<< "]"
							<< std::endl);
	return nOrdQty;
}

int	NikkeiSpread::OrderPriceCrossCheck(STREET_ORDER& NewStreetOrder)
{
	STREET_ORDER_CONTAINER SOC;
	STREET_ORDER so;

	if(SOC.GetFirstActiveBySymbol(so,NewStreetOrder.GetSymbol()))
	{
		do
		{
			// New Buy Order Price must be less than existing Sell Order Price
			// New Sell Order Price must be greater than existing Buy Order Price

			if (!strcmp(NewStreetOrder.GetDestination(), so.GetDestination()))
			{
				_STRAT_LOG_VERB_(CWARN << "NewStreetOrder ID [" << NewStreetOrder.GetHandle()
										<< "] to Destination [" << NewStreetOrder.GetDestination()
										<< "] Side [" << (NewStreetOrder.GetSide()==SIDE_BUY?"BUY":"SEL")
										<< "] at Price [" << NewStreetOrder.GetPrice()
										<< "] Comparing against previous StreetOrder ID [" << so.GetHandle()
										<< "] at Destination [" << so.GetDestination()
										<< "] Side [" <<  (so.GetSide()==SIDE_BUY?"BUY":"SEL")
										<< "] at Price [" << so.GetPrice()
										<< "]."
										<<std::endl);

				if ((NewStreetOrder.GetSide()==SIDE_BUY && so.GetSide()==SIDE_SELL
						&& (NewStreetOrder.GetPrice() - so.GetPrice() > -g_dEpsilon))
					|| (NewStreetOrder.GetSide()==SIDE_SELL && so.GetSide()==SIDE_BUY
						&& (NewStreetOrder.GetPrice() - so.GetPrice() < g_dEpsilon)))
				{
					NewStreetOrder.SetRejectMsg("Price SELF-CROSS!");

					_STRAT_LOG_VERB_(CERR << "NewStreetOrder ID [" << NewStreetOrder.GetHandle()
								<< "] to Destination [" << NewStreetOrder.GetDestination()
								<< "] Side [" << (NewStreetOrder.GetSide()==SIDE_BUY?"BUY":"SEL")
								<< "] at Price [" << NewStreetOrder.GetPrice()
								<< "] CORSSED with previous StreetOrder ID [" << so.GetHandle()
								<< "] at Destination [" << so.GetDestination()
								<< "] Side [" <<  (so.GetSide()==SIDE_BUY?"BUY":"SEL")
								<< "] at Price [" << so.GetPrice()
								<< "] : NOT sending new order!!!"
								<<std::endl);
					return FAILURE;
				}
			}
		}while(SOC.GetNextActiveBySymbol(so));
	}
	return SUCCESS;
}

/// Utils
/// UTILITY FUNCTION: Send a new street order from client order
int NikkeiSpread::SendOutStreetOrder(CLIENT_ORDER& ClientOrder, const char* pszDestination, int nOrderQty, double dOrderPrice)
{
    if(!pszDestination)
    {
    	CERR << "Cannot Send StreetOrder. Destination is not set" << std::endl;
    	return FAILURE;
    }
    else if(nOrderQty <= 0)
    {
    	CERR << "Cannot Send StreetOrder. Invalid OrderQty [" << nOrderQty << "]" << std::endl;
    	return FAILURE;
    }

    STREET_ORDER order;
    if(dOrderPrice > g_dEpsilon)
    {
    	order.SetOrderType(TYPE_LIMIT);
    	order.SetPrice(dOrderPrice);
    }
    else
    {
    	CERR << "Cannot Send StreetOrder. Invalid Price [" << dOrderPrice << "]" << std::endl;
    	return FAILURE;
    }

    order.SetSymbol(ClientOrder.GetSymbol());
    order.SetPortfolio(ClientOrder.GetPortfolio());
    order.SetClientOrdId(ClientOrder.GetHandle());
    order.SetSide(ClientOrder.GetSide());
    order.SetAccount(ClientOrder.GetAccount());

    order.SetDestination(pszDestination);
    order.SetSize(nOrderQty);
    order.SetOrderTimeInForce(TIF_DAY);

    order.SetClientId2(atoi(ClientOrder.GetFixTag(FIX_TAG_LEGID_NKSP)));
    order.SetClientId0(atoi(ClientOrder.GetFixTag(FIX_TAG_LEGID_NKSP)));

    char szMemo [64] = "";
    if (atoi(ClientOrder.GetFixTag(FIX_TAG_LEGID_NKSP)) == LEGID_NKSP_Q)
    {
    	sprintf(szMemo, "%s_%s", ClientOrder.GetFixTag(FIX_TAG_STRATPORT_NKSP), LEG_QUOTE);
    	order.SetUserMemo(szMemo);
    }
    else if (atoi(ClientOrder.GetFixTag(FIX_TAG_LEGID_NKSP)) == LEGID_NKSP_H)
    {
    	sprintf(szMemo, "%s_%s", ClientOrder.GetFixTag(FIX_TAG_STRATPORT_NKSP), LEG_HEDGE);
    	order.SetUserMemo(szMemo);
    }

    char szWave [64] = "";
    sprintf(szWave, "%s-%s", ClientOrder.GetAccount(), GetProductFromSymbol(ClientOrder.GetSymbol()));
    order.SetWaveName(szWave);

    enErrorCode eReturn = order.Send();
    if (eReturn != RULES_EO_OK)
	{
		CERR << "FAIL to send street order : Client OrderID [" << ClientOrder.GetHandle()
				<< "] Account [" << ClientOrder.GetAccount()
				<< "] Street Order ID [" << order.GetHandle()
				<< "] Symbol [" << ClientOrder.GetSymbol()
				<< "] Portfolio [" << ClientOrder.GetPortfolio()
				<< "] Side [" << ClientOrder.GetSide()
				<< "] Price [" << dOrderPrice
				<< "] Quantity [" << nOrderQty
				<< "] Dest [" << pszDestination
				<< "] Wave [" << order.GetWaveName()
				<< "] UserMemo [" << order.GetUserMemo()
				<< "] ErrorCode [" << ErrorCodeToStr(eReturn)
				<< "]"
				<< std::endl;
		return FAILURE;
	}
    else
    {
		CINFO << "Successfully sent street order : Client OrderID [" << ClientOrder.GetHandle()
				<< "] Account [" << ClientOrder.GetAccount()
				<< "] Street Order ID [" << order.GetHandle()
				<< "] Symbol [" << ClientOrder.GetSymbol()
				<< "] Portfolio [" << ClientOrder.GetPortfolio()
				<< "] Side [" << ClientOrder.GetSide()
				<< "] LegId [" << ClientOrder.GetFixTag(FIX_TAG_LEGID_NKSP)
				<< "] Price [" << dOrderPrice
				<< "] Quantity [" << nOrderQty
				<< "] Dest [" << pszDestination
				<< "] Wave [" << order.GetWaveName()
				<< "] UserMemo [" << order.GetUserMemo()
				<< "] ClientId0 [" << order.GetClientId0()
				<< "] ClientId2 [" << order.GetClientId2()
				<< "]"
				<< std::endl;
    	return SUCCESS;
    }
}

/// UTILITY FUNCTION: Cancel all street orders for a given client order
int NikkeiSpread::CancelAllOrdersForClientOrder(const char* pszClientOrderId)
{
    static STREET_ORDER_CONTAINER container;
    static STREET_ORDER TmpStreetOrder;
    if(!pszClientOrderId)
    	return FAILURE;

    CINFO << "Cancelling All Street orders for ClientOrderId [" << pszClientOrderId << "]" << std::endl;

    if(container.GetFirstActiveByClientOrderId(TmpStreetOrder, pszClientOrderId))
    {
    	do
    	{
    		if(!CanReplace(TmpStreetOrder))
    		{
    			CWARN << "Cannot Cancel Street Order [" << TmpStreetOrder.GetHandle()
    					<<"] , OrderState [" << StateToStr(TmpStreetOrder.GetOrderState())
    					<<"]"
    					<< std::endl;

    			continue;
    		}

    		TmpStreetOrder.SetClientOrdId(pszClientOrderId);

    		enErrorCode errStreetCancel = TmpStreetOrder.Cancel();
    		if(errStreetCancel != RULES_EO_OK)
    		{
    			CERR << "Cancel Fail for [" << TmpStreetOrder.GetHandle()
    					<<"] errStreetCancel [" << ErrorCodeToStr(errStreetCancel)
    					<<"]"
    					<< std::endl;
    		}
    		else
    		{
    			CINFO << "Cancel Success for [" << TmpStreetOrder.GetHandle() << "]" << std::endl;
    		}
    	}
    	while(container.GetNextActiveByClientOrderId(TmpStreetOrder));
    }
    return SUCCESS;
}

// UTILITY FUNCTION: Can we modify the existing street order?
bool NikkeiSpread::CanReplace(STREET_ORDER& StreetOrder)
{
    if(StreetOrder.isReplacePending())
    {
    	_STRAT_LOG_VERB_(CWARN << "Street Order [" << StreetOrder.GetHandle()
    							<<"] Cannot Rpl/Cxl : Pending Replace! "
    							<< std::endl);
    	return false;
    }

    if(StreetOrder.isCancelPending())
	{
		_STRAT_LOG_VERB_(CWARN << "Street Order [" << StreetOrder.GetHandle()
								<<"] Cannot Rpl/Cxl : Pending Cancel! "
								<< std::endl);
		return false;
	}

    if(StreetOrder.GetOrderType() == TYPE_MARKET)
    {
		_STRAT_LOG_VERB_(CWARN << "Street Order [" << StreetOrder.GetHandle()
								<<"] Cannot Rpl: Market Order! "
								<< std::endl);
		return false;
    }

    if(StreetOrder.GetOrderTimeInForce() == TIF_IOC)
    {
    	_STRAT_LOG_VERB_(CWARN << "Street Order [" << StreetOrder.GetHandle()
    							<<"] Cannot Rpl: Market Order! "
    							<< std::endl);
    	return false;
    }

    switch(StreetOrder.GetOrderState())
    {
		case STATE_OPEN:
		case STATE_PARTIAL:
		case STATE_REPLACED:
			return true;
		case STATE_UNACKED:
		case STATE_FILLED:
		case STATE_CANCELLED:
		case STATE_PENDING_RPL:
		case STATE_REJECTED:
		case STATE_DONE:
		case STATE_INVALID:
		default:
			return false;
    }
    return false;
}

/// UTILITY FUNCTION: Return order state
const char* NikkeiSpread::StateToStr(enOrderState e)
{
    static char *p[] = {"OPEN", "PART", "RPLD", "UNACKED", "FILLED", "CXLD", "PEND-RPL", "REJD", "DONE", "PEND-NEW", "INVALID"};
    switch(e)
    {
		case STATE_OPEN:
			return p[0];
		case STATE_PARTIAL:
			return p[1];
		case STATE_REPLACED:
			return p[2];
		case STATE_UNACKED:
			return p[3];
		case STATE_FILLED:
			return p[4];
		case STATE_CANCELLED:
			return p[5];
		case STATE_PENDING_RPL:
			return p[6];
		case STATE_REJECTED:
			return p[7];
		case STATE_DONE:
			return p[8];
		case STATE_PENDING_NEW:
			return p[9];
		case STATE_INVALID:
		default:
			return p[10];
    }
    return p[10];
}

/// UTILITY FUNCTION: Return error code
const char* NikkeiSpread::ErrorCodeToStr(enErrorCode e)
{
	static char *p[] = { "RULES_EO_OK",
						 "RULES_EO_FAIL",
						 "RULES_EO_NO_LVS",
						 "RULES_EO_NO_ORD",
						 "RULES_EO_NO_PORT",
						 "RULES_EO_OTHER_USER",
						 "RULES_EO_MAX_REJ",
						 "RULES_EO_NO_CLIENT_ORD",
						 "RULES_EO_NOT_ACTIVE",
						 "RULES_EO_REF_CXLD",
						 "RULES_EO_REF_RPLD",
						 "RULES_EO_INVALID_PRICE",
						 "RULES_EO_EXCEED_UNORD_SHRS",
						 "RULES_EO_DEST_NOT_UP",
						 "RULES_EO_SOCKET_FAIL" };
	switch (e)
	{
		case RULES_EO_OK:
			return p[0];
		case RULES_EO_FAIL:
			return p[1];
		case RULES_EO_NO_LVS:
			return p[2];
		case RULES_EO_NO_ORD:
			return p[3];
		case RULES_EO_NO_PORT:
			return p[4];
		case RULES_EO_OTHER_USER:
			return p[5];
		case RULES_EO_MAX_REJ:
			return p[6];
		case RULES_EO_NO_CLIENT_ORD:
			return p[7];
		case RULES_EO_NOT_ACTIVE:
			return p[8];
		case RULES_EO_REF_CXLD:
			return p[9];
		case RULES_EO_REF_RPLD:
			return p[10];
		case RULES_EO_INVALID_PRICE:
			return p[11];
		case RULES_EO_EXCEED_UNORD_SHRS:
			return p[12];
		case RULES_EO_DEST_NOT_UP:
			return p[13];
		case RULES_EO_SOCKET_FAIL:
			return p[14];
	}
	return p[1];
}


/// UTILITY FUNCTION: Return Strat mode
const char* NikkeiSpread::StratModeToStr(E_STRAT_MODE e)
{
	static char *p[] = {"INVALID",
						"ALWAYS",
						"SILENT"};

	switch(e)
	{
		case STRAT_MODE_INVALID:
			return p[0];
		case STRAT_MODE_ALWAYS:
			return p[1];
		case STRAT_MODE_SILENT:
			return p[2];
	};
	return p[0];
}

/// UTILITY FUNCTION: Get the default tick size for different symbols
double	NikkeiSpread::GetTickSizeForSymbol(const char* pszSymbol)
{
    if(!pszSymbol)
    {
    	CERR <<"Invalid Symbol ["<< pszSymbol << "]" << std::endl;
    	return g_dEpsilon/10.0;
    }

    if(strstr(pszSymbol, SYM_NAME_NKD))
    	return 5.0;
    else if(strstr(pszSymbol, SYM_NAME_NIY))
    	return 5.0;
    else if(strstr(pszSymbol, SYM_NAME_6J))
    	return 1.0;

    return g_dEpsilon/10.0;
}

/// UTILITY FUNCTION: Get the underlying product for different symbols
const char* NikkeiSpread::GetProductFromSymbol(const char* pszSymbol)
{
    if(!pszSymbol)
    {
    	CERR <<"Invalid Symbol ["<< pszSymbol << "]" << std::endl;
    	return "INVALID";
    }

    if(strstr(pszSymbol, SYM_NAME_NKD))
    	return SYM_NAME_NKD;
    else if(strstr(pszSymbol, SYM_NAME_NIY))
    	return SYM_NAME_NIY;
    else if(strstr(pszSymbol, SYM_NAME_6J))
    	return SYM_NAME_6J;

    return "INVALID";
}

/// UTILITY FUNCTION: Cancels all street orders of the strat
int NikkeiSpread::CancelAllStreetOrders(const StratParams *pStratParams)
{
	if (!pStratParams)
	{
		_STRAT_LOG_DBUG_(CDEBUG << "Invalid pStratParams!" << std::endl);
		return FAILURE;
	}

	_STRAT_LOG_DBUG_(CDEBUG << "StratID [" << pStratParams->nStratId
					<<"] Sending cancel!"
					<< std::endl);

	if ((CancelAllOrdersForClientOrder(pStratParams->szClientOrdIdQuote) == SUCCESS)
			&& (CancelAllOrdersForClientOrder(pStratParams->szClientOrdIdHedge) == SUCCESS))
		return SUCCESS;
	else
		return FAILURE;
}


/// UTILITY FUNCTION: Prints Client Order Map
void NikkeiSpread::PrintStratOrderMap(STRATMAP& mpStratMap, const long nStratId)
{
	CDEBUG << "Printing StratMap : StratID [" << nStratId << "]." << std::endl;

	if(nStratId < 1)
	{
		CDEBUG << "No StratID [" << nStratId
				<< "] passed; hence printing all." << std::endl;
	}
	if (mpStratMap.count(nStratId) <= 0)
	{
		CDEBUG << "StratID [" << nStratId
				<< "] is not found in the map!" << std::endl;
	}
	if(mpStratMap.count(nStratId))
	{
		STRATMAPITER miIter =  mpStratMap.find(nStratId);
		if (miIter != mpStratMap.end())
		{
			CDEBUG << "\n New Strat Order"
					<< " | nStratId=" << miIter->second->nStratId
					<< " | szStratPort=" << miIter->second->szStratPort
					<< " | szOMUser=" << miIter->second->szOMUser
					<< " | szDest=" << miIter->second->szDest
					<< " \n Flags: "
					<< " | bQuoteEntry=" << miIter->second->bQuoteEntry
					<< " | bHedgeEntry=" << miIter->second->bHedgeEntry
					<< " | bQuoteRpld=" << miIter->second->bQuoteRpld
					<< " | bHedgeRpld=" << miIter->second->bHedgeRpld
					<< " \n Params:"
					<< " | eStratMode=" << StratModeToStr(miIter->second->eStratMode)
					<< " | bStratReady=" << miIter->second->bStratReady
					<< " | bStratRunning=" << miIter->second->bStratRunning
					<< " | nOrdLot="  << miIter->second->nOrdLot
					<< " | nTotalLot=" <<  miIter->second->nTotalLot
					<< " | nPayupTicks=" << miIter->second->nPayupTicks
					<< " \n Quote: "
					<< " | szSymbolQuote=" << miIter->second->szSymbolQuote
					<< " | enSideQuote=" << miIter->second->enSideQuote
					<< " | szClientOrdIdQuote=" << miIter->second->szClientOrdIdQuote
					<< " | szPortfolioQuote=" << miIter->second->szPortfolioQuote
					<< "  \n Hedge: "
					<< " | szSymbolHedge=" << miIter->second->szSymbolHedge
					<< " | enSideHedge=" << miIter->second->enSideHedge
					<< " | szClientOrdIdHedge=" << miIter->second->szClientOrdIdHedge
					<< " | szPortfolioHedge=" << miIter->second->szPortfolioHedge
					<< "\n"
					<< std::endl;
		}
	}
	else
	{
		STRATMAPITER miIter;
		for(miIter = mpStratMap.begin(); miIter != mpStratMap.end() ; miIter++)
		{
			CDEBUG << "\n New Strat Order "
					<< " | nStratId=" << miIter->second->nStratId
					<< " | szStratPort=" << miIter->second->szStratPort
					<< " | szOMUser=" << miIter->second->szOMUser
					<< " | szDest=" << miIter->second->szDest
					<< " \n Flags: "
					<< " | bQuoteEntry=" << miIter->second->bQuoteEntry
					<< " | bHedgeEntry=" << miIter->second->bHedgeEntry
					<< " | bQuoteRpld=" << miIter->second->bQuoteRpld
					<< " | bHedgeRpld=" << miIter->second->bHedgeRpld
					<< " \n Params:"
					<< " | eStratMode=" << StratModeToStr(miIter->second->eStratMode)
					<< " | bStratReady=" << miIter->second->bStratReady
					<< " | bStratRunning=" << miIter->second->bStratRunning
					<< " | nOrdLot="  << miIter->second->nOrdLot
					<< " | nTotalLot=" <<  miIter->second->nTotalLot
					<< " | nPayupTicks=" << miIter->second->nPayupTicks
					<< " \n Quote: "
					<< " | szSymbolQuote=" << miIter->second->szSymbolQuote
					<< " | enSideQuote=" << miIter->second->enSideQuote
					<< " | szClientOrdIdQuote=" << miIter->second->szClientOrdIdQuote
					<< " | szPortfolioQuote=" << miIter->second->szPortfolioQuote
					<< "  \n Hedge: "
					<< " | szSymbolHedge=" << miIter->second->szSymbolHedge
					<< " | enSideHedge=" << miIter->second->enSideHedge
					<< " | szClientOrdIdHedge=" << miIter->second->szClientOrdIdHedge
					<< " | szPortfolioHedge=" << miIter->second->szPortfolioHedge
					<< "\n"
					<< std::endl;
		}
	}
}

/// UTILITY FUNCTION: Prints Client Order Rpl Map
void NikkeiSpread::PrintStratOrderMapRpl(STRATMAP& mpStratMapRpl)
{
	CDEBUG << "Printing StratOrderMapRpl - printing all Rpl orders." << std::endl;

	STRATMAPITER miIter;
	for(miIter = mpStratMapRpl.begin(); miIter != mpStratMapRpl.end() ; miIter++)
	{
		CDEBUG << "\n Rpl Strat Order "
				<< " | nStratId=" << miIter->second->nStratId
				<< " | szStratPort=" << miIter->second->szStratPort
				<< " | szOMUser=" << miIter->second->szOMUser
				<< " | szDest=" << miIter->second->szDest
				<< " \n Flags: "
				<< " | bQuoteEntry=" << miIter->second->bQuoteEntry
				<< " | bHedgeEntry=" << miIter->second->bHedgeEntry
				<< " | bQuoteRpld=" << miIter->second->bQuoteRpld
				<< " | bHedgeRpld=" << miIter->second->bHedgeRpld
				<< " \n Params:"
				<< " | eStratMode=" << StratModeToStr(miIter->second->eStratMode)
				<< " | bStratReady=" << miIter->second->bStratReady
				<< " | bStratRunning=" << miIter->second->bStratRunning
				<< " | nOrdLot="  << miIter->second->nOrdLot
				<< " | nTotalLot=" <<  miIter->second->nTotalLot
				<< " | nPayupTicks=" << miIter->second->nPayupTicks
				<< " \n Quote: "
				<< " | szSymbolQuote=" << miIter->second->szSymbolQuote
				<< " | enSideQuote=" << miIter->second->enSideQuote
				<< " | szClientOrdIdQuote=" << miIter->second->szClientOrdIdQuote
				<< " | szPortfolioQuote=" << miIter->second->szPortfolioQuote
				<< "  \n Hedge: "
				<< " | szSymbolHedge=" << miIter->second->szSymbolHedge
				<< " | enSideHedge=" << miIter->second->enSideHedge
				<< " | szClientOrdIdHedge=" << miIter->second->szClientOrdIdHedge
				<< " | szPortfolioHedge=" << miIter->second->szPortfolioHedge
				<< "\n"
				<< std::endl;
	}
}

/// UTILITY FUNCTION: Prints Client Order Cxl Map
void NikkeiSpread::PrintStratOrderMapCxl(STRATMAP& mpStratMapCxl)
{
	CDEBUG << "Printing StratOrderMapCxl - printing all Cxl orders." << std::endl;

	STRATMAPITER miIter;
	for(miIter = mpStratMapCxl.begin(); miIter != mpStratMapCxl.end() ; miIter++)
	{
		CDEBUG << "\n Cxl Strat Order "
				<< " | nStratId=" << miIter->second->nStratId
				<< " | szStratPort=" << miIter->second->szStratPort
				<< " | szOMUser=" << miIter->second->szOMUser
				<< " | szDest=" << miIter->second->szDest
				<< " \n Flags: "
				<< " | bQuoteEntry=" << miIter->second->bQuoteEntry
				<< " | bHedgeEntry=" << miIter->second->bHedgeEntry
				<< " | bQuoteRpld=" << miIter->second->bQuoteRpld
				<< " | bHedgeRpld=" << miIter->second->bHedgeRpld
				<< " \n Params:"
				<< " | eStratMode=" << StratModeToStr(miIter->second->eStratMode)
				<< " | bStratReady=" << miIter->second->bStratReady
				<< " | bStratRunning=" << miIter->second->bStratRunning
				<< " | nOrdLot="  << miIter->second->nOrdLot
				<< " | nTotalLot=" <<  miIter->second->nTotalLot
				<< " | nPayupTicks=" << miIter->second->nPayupTicks
				<< " \n Quote: "
				<< " | szSymbolQuote=" << miIter->second->szSymbolQuote
				<< " | enSideQuote=" << miIter->second->enSideQuote
				<< " | szClientOrdIdQuote=" << miIter->second->szClientOrdIdQuote
				<< " | szPortfolioQuote=" << miIter->second->szPortfolioQuote
				<< "  \n Hedge: "
				<< " | szSymbolHedge=" << miIter->second->szSymbolHedge
				<< " | enSideHedge=" << miIter->second->enSideHedge
				<< " | szClientOrdIdHedge=" << miIter->second->szClientOrdIdHedge
				<< " | szPortfolioHedge=" << miIter->second->szPortfolioHedge
				<< "\n"
				<< std::endl;
	}

}

/// UTILITY FUNCTION: Prints Information from Street execution report
void NikkeiSpread::PrintStreetExec(STREET_EXEC& StreetExec, CLIENT_ORDER& parentOrder)
{
	CINFO << "StreetOrder :"
			<< " StreetOrder Handle=[" <<  (StreetExec.GetStreetOrder()).GetHandle()
			<< "] | StreetOrder State [" << StateToStr((StreetExec.GetStreetOrder()).GetOrderState())
			<< "] | StreetOrder GetClientId0=[" << (StreetExec.GetStreetOrder()).GetClientId0()
			<< "] | StreetOrder GetClientId2=[" << (StreetExec.GetStreetOrder()).GetClientId2()
			<< "] | StreetOrder GetUserMemo=[" << (StreetExec.GetStreetOrder()).GetUserMemo()
			<< "] - StreetExecs : GetClientOrdId=[" << StreetExec.GetClientOrdId()
			<< "] | GetSymbol=[" << StreetExec.GetSymbol()
			<< "] | GetLastFillQty=[" << StreetExec.GetLastFillQty()
			<< "] | GetLastFillPrice=[" << StreetExec.GetLastFillPrice()
			<< "] | GetFilledSize=[" << StreetExec.GetFilledSize()
			<< "] | GetSize=[" << StreetExec.GetSize()
			<< "] | GetPrice=[" << StreetExec.GetPrice()
			<< "] | GetTotalLvs=[" << StreetExec.GetTotalLvs()
			<< "] | GetOrderState=[" << StateToStr(StreetExec.GetOrderState())
			<< "] | GetSide=[" << StreetExec.GetSide()
			<< "] | GetExecBroker=[" << StreetExec.GetExecBroker()
			<< "] | GetBrokerOrdId=[" << StreetExec.GetBrokerOrdId()
			<< "] | GetCapacity=[" << StreetExec.GetCapacity()
			<< "] | GetTradingAccount=[" << StreetExec.GetTradingAccount()
			<< "] | GetExecId=[" << StreetExec.GetExecId()
			<< "] | GetExecRefId=[" << StreetExec.GetExecRefId()
			<< "] | GetHandle=[" << StreetExec.GetHandle()
			<< "] | GetDestination=[" << StreetExec.GetDestination()
			<< "] | GetDestinationSubId=[" << StreetExec.GetDestinationSubId()
			<< "] | GetSender=[" << StreetExec.GetSender()
			<< "] | GetSenderSubId=[" << StreetExec.GetSenderSubId()
			<< "] | GetClientId2=[" << StreetExec.GetClientId2()
			<< "] | GetTradeDate=[" << StreetExec.GetTradeDate()
			<< "]"
			<< std::endl;
}

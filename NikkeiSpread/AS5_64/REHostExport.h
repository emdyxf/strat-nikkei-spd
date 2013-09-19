#define FLEX_INSIDE_CLIENT_RULES 1
#ifdef USING_FT_SIZE_TYPE

#ifndef __FLEXDECIMAL_H__
#define __FLEXDECIMAL_H__

#include <iostream>
#include <sstream>
#include <string.h>
#include <cmath>
#include <math.h>

#ifndef FT_DLL
	#ifdef PLATFORM_WIN
		#define	FT_DLL __declspec(dllexport)
	#else
		#define	FT_DLL
	#endif
#endif

namespace FlexDecimal
{
class FT_DLL Decimal
{
	public:
		enum RoundingMethod {Normal, AllUp, AllDown};

		Decimal():m_data(0)									{}
		Decimal(const int value)							{ m_data = (long long)(value) * m_multiplier; }
		Decimal(const double value)							{ setFromDouble(value); }
		Decimal(const long value)							{ m_data = (long long)(value) * m_multiplier; }
		Decimal(const long long value)						{ m_data = value * m_multiplier; }

		Decimal & operator=(const Decimal & rhs)			{ m_data = rhs.m_data; return *this; }
		Decimal & operator=(const int & rhs)				{ m_data = (long long)(rhs) * m_multiplier; return *this; }
		Decimal & operator=(const long int & rhs)			{ m_data = (long long)(rhs) * m_multiplier; return *this; }
		Decimal & operator=(const double & rhs)				{ setFromDouble(rhs); return *this; }

		operator bool() const								{ return m_data != 0; }
		bool operator ! () const							{ return m_data == 0; }
		bool operator && (const bool X) const				{ return m_data && X; }
		bool operator || (const bool X) const				{ return m_data || X; }
		const Decimal operator - () const					{ Decimal d(*this); d.m_data *= -1; return d;}

		const Decimal operator + (const Decimal & rhs) const;
		const Decimal operator + (const int & rhs) const;
		Decimal& operator += (const Decimal & rhs);
		Decimal& operator += (const int & rhs);
		const Decimal operator - (const Decimal & rhs) const;
		const Decimal operator - (const int & rhs) const;
		const Decimal operator - (const long int & rhs) const;
		Decimal& operator -= (const Decimal & rhs);
		Decimal& operator -= (const int & rhs);
		const Decimal operator * (const Decimal & rhs) const;
		const Decimal operator * (const int & rhs) const;
		const Decimal& operator *= (const Decimal & rhs);
		const Decimal& operator *= (const int & rhs);
		const Decimal operator / (const Decimal & rhs) const;
		const Decimal operator / (const int & rhs) const;
		const Decimal& operator /= (const Decimal & rhs);
		const Decimal& operator /= (const int & rhs);

		bool operator>(const Decimal & rhs) const			{ return m_data > rhs.m_data; }
		bool operator>(const int & rhs) const				{ return m_data > ((long long)(rhs) * m_multiplier); }
		bool operator<(const Decimal & rhs) const			{ return m_data < rhs.m_data; }
		bool operator<(const int & rhs) const				{ return m_data < ((long long)(rhs) * m_multiplier); }
		bool operator==(const Decimal & rhs) const			{ return m_data == rhs.m_data; }
		bool operator==(const int & rhs) const				{ return m_data == ((long long)(rhs) * m_multiplier); }
		bool operator>=(const Decimal & rhs) const			{ return m_data >= rhs.m_data; }
		bool operator>=(const int & rhs) const				{ return (m_data >= ((long long)(rhs)* m_multiplier)); }
		bool operator<=(const Decimal & rhs) const			{ return m_data <= rhs.m_data; }
		bool operator<=(const int & rhs) const				{ return (m_data <= ((long long)(rhs)* m_multiplier)); }
		bool operator != (const Decimal & X) const			{ return (X.m_data != m_data); }
		bool operator!=(const int & rhs) const				{ return (m_data != ((long long)(rhs)* m_multiplier)); }
		const Decimal operator % (const Decimal & rhs) const{ return Decimal((double)(m_data % rhs.m_data) / m_multiplier); }
		const Decimal operator % (const int & rhs) const	{ return Decimal((double)(m_data % ((long long)(rhs) * m_multiplier)) / m_multiplier); }

		double operator + (const double & rhs) const		       { return rhs + m_data / double(m_multiplier); }
		double operator - (const double & rhs) const		       { return m_data / double(m_multiplier) - rhs; }
		double operator * (const double & rhs) const		       { return rhs * m_data / m_multiplier; }
		double operator / (const double & rhs) const		       { return (m_data / rhs) / m_multiplier; }
		bool operator>(const double & rhs) const                    { return *this >  Decimal(rhs); }
		bool operator<(const double & rhs) const                    { return *this <  Decimal(rhs); }
		bool operator==(const double & rhs) const                   { return *this == Decimal(rhs); }
		bool operator != (const double & rhs) const                 { return *this != Decimal(rhs); }
		bool operator >= (const double & rhs) const		       { return *this >= Decimal(rhs); }
		bool operator <= (const double & rhs) const		       { return *this <= Decimal(rhs); }

		inline operator double() const					{ return to_double(); }
		inline operator float() const					{ return to_double(); }

		Decimal & operator++(); //prefix
		Decimal operator++(int); //postfix
		Decimal & operator--(); // prefix
		Decimal operator--(int); // postfix

		// conversion helper functions:
		double to_double() const							{ return m_data/double(m_multiplier); }
		int to_int() const									{ return (int)(m_data/m_multiplier); }
		long to_long() const								{ return (long)(m_data/m_multiplier); }
		char* to_string(char *p = NULL) const;
		const char* to_comma_string() const;
		long long int GetData()								{ return m_data; }
		void SetData(const long long data)					{ m_data = data; }

		friend std::ostream& operator<< (std::ostream&os, const Decimal& d);

		static unsigned short int GetPrecision()			{ return m_precision; }
		static void changePrecision(unsigned short int precision);
		static unsigned int GetMultiplier()				{  return m_multiplier; }

		static inline Decimal ConvertFTSize(int X)
		{
			Decimal Y;
			Y = X;
			return Y;
		}
		static inline Decimal ConvertFTSize(double X)
		{
			Decimal Y;
			Y = X;
			return Y;
		}

		// Only cast an instance of this class if in app rules.
#ifdef FLEX_INSIDE_CLIENT_RULES
	public:
#else
	private:
#endif
		inline operator int() const { return to_int(); }
		inline operator long() const { return to_int(); }
		inline operator long long() const { return m_data/m_multiplier; }
		inline operator char() const { return (char)(m_data/m_multiplier); }
		inline operator short() const { return (short)(m_data/m_multiplier); }
	//do not add any functions here.  or first add the #ifdef FLEX_INSIDE_CLIENT_RULES to change the scope

	private:
		long long int m_data;

		static unsigned short int m_precision;
		static unsigned int m_multiplier;
		static  RoundingMethod m_round;

		void setFromDouble(double value = 0.00);

		static void add(Decimal & dec, const Decimal & value)	{ dec.m_data += value.m_data; }
		static void add(Decimal & dec, const int & value)		{ dec.m_data += (long long)(value) * m_multiplier; }

		static void subtract(Decimal & dec, const Decimal & value)	{ dec.m_data -= value.m_data; }
		static void subtract(Decimal & dec, const int & value)		{ dec.m_data -= (long long)(value) * m_multiplier; }
		static void subtract(Decimal & dec, const long int & value) { dec.m_data -= (long long)(value) * m_multiplier; }

		static void multiply(Decimal & dec, const Decimal & value);
		static void multiply(Decimal & dec, const int & value)		{ dec.m_data *= (long long)value; }

		static void divide(Decimal & dec, const Decimal & value)	{ dec.setFromDouble(double(dec.m_data) / value.m_data); }
		static void divide(Decimal & dec, const int & value)		{ dec.m_data /= (long long)(value); }

};

inline Decimal abs(const Decimal& X)  { return (X < 0) ? -X : X; }

inline const Decimal operator + (const int & X, const Decimal& Y) { return Decimal(X) + Y; }
inline const Decimal operator - (const int & X, const Decimal& Y) { return Decimal(X) - Y; }
inline const Decimal operator * (const int & X, const Decimal& Y) { return Decimal(X) * Y; }
inline const Decimal operator / (const int & X, const Decimal& Y) { return Decimal(X) / Y; }
inline const Decimal operator % (const int & X, const Decimal& Y) { return X % Y.to_int(); }
inline const Decimal operator + (const long & X, const Decimal& Y) { return Decimal(X) + Y; }
inline const double operator * (const double & X, const Decimal& Y) { return X * Y.to_double(); }
inline const double operator / (const double & X, const Decimal& Y) { return X / Y.to_double(); }
inline const double operator + (const double & X, const Decimal& Y) { return X + Y.to_double(); }
inline const double operator - (const double & X, const Decimal& Y) { return X - Y.to_double(); }

inline bool operator == (const int & X, const Decimal& Y) { return Decimal(X) == Y; }
inline bool operator != (const int & X, const Decimal& Y) { return Decimal(X) != Y; }
inline bool operator <  (const int & X, const Decimal& Y) { return Decimal(X) <  Y; }
inline bool operator >  (const int & X, const Decimal& Y) { return Decimal(X) >  Y; }
inline bool operator <= (const int & X, const Decimal& Y) { return Decimal(X) <= Y; }
inline bool operator >= (const int & X, const Decimal& Y) { return Decimal(X) >= Y; }
inline bool operator == (const long & X, const Decimal& Y) { return Decimal(X) == Y; }
inline bool operator == (const double & X, const Decimal& Y) { return Decimal(X) == Y; }
inline bool operator != (const double & X, const Decimal& Y) { return Decimal(X) != Y; }
inline bool operator <  (const double & X, const Decimal& Y) { return Decimal(X) <  Y; }
inline bool operator >  (const double & X, const Decimal& Y) { return Decimal(X) >  Y; }
inline bool operator <= (const double & X, const Decimal& Y) { return Decimal(X) <= Y; }
inline bool operator >= (const double & X, const Decimal& Y) { return Decimal(X) >= Y; }

inline double operator -= (double& X, const Decimal& Y) { X = X - Y.to_double(); return X; }
inline double operator += (double& X, const Decimal& Y) { X = X + Y.to_double(); return X; }
inline double operator *= (double& X, const Decimal& Y) { X = X * Y.to_double(); return X; }
inline double operator /= (double& X, const Decimal& Y) { X = X / Y.to_double(); return X; }

inline bool operator && (const Decimal& X, const Decimal& Y) { return (X.to_int() != 0) && (Y.to_int() != 0); }
template<typename T> inline bool operator && (const T& other, const Decimal& Y) { return other && (Y.to_int() != 0); }
template<typename T> inline bool operator && (const Decimal& Y, const T& other) { return (Y.to_int() != 0) && other; }
inline bool operator || (const Decimal& X, const Decimal& Y) { return (X.to_int() != 0) || (Y.to_int() != 0); }
template<typename T> inline bool operator || (const T& other, const Decimal& Y) { return other || (Y.to_int() != 0); }
template<typename T> inline bool operator || (const Decimal& Y, const T& other) { return (Y.to_int() != 0) || other; }

#ifdef FLEX_INSIDE_CLIENT_RULES
inline int operator -= (int& X, const Decimal& Y) { X = X - Y.to_int(); return X; }
inline int operator += (int& X, const Decimal& Y) { X = X + Y.to_int(); return X; }
inline int operator *= (int& X, const Decimal& Y) { X = X * Y.to_int(); return X; }
inline int operator /= (int& X, const Decimal& Y) { X = X / Y.to_int(); return X; }
inline int operator %= (int& X, const Decimal& Y) { X = X % Y.to_int(); return X; }
#endif
};

#endif
#endif
#ifndef _OMRulesExport_h_
#define _OMRulesExport_h_
#ifdef USING_FT_SIZE_TYPE
#ifndef FLEX_INSIDE_CLIENT_RULES
#include "Decimal.h"
#endif
namespace FlexDecimal { class Decimal; };
typedef FlexDecimal::Decimal FT_SIZE_TYPE;
#else
typedef int FT_SIZE_TYPE;
#endif

#include <vector>

/*!
 * @mainpage Writing rules in the Flextrade OM.
 *
 * @section intro_sec Introduction
 *
 * There is a single function "int OnEvent(int nReason, HEVENTDATA data)" that is called for ever event. The data pointer can 
 * be converted into one of the OMRULESEXPORT classes based on the event type.
 * 
 * @see OMRULESEXPORT
 * 
 * @section Events
 * 
 * There are many different event calls each has a specific class for the data passed it, it's very important that the correct
 * casting is carried out for each event.
 * @see enEvents, for a full list of the OM events.
 * 
 * The events for all street and client interactions are paired with a Validation event called, returning false from this will 
 * stop the action from being carried out and the second event in the pair from being called.
 * 
 *
 * @section Rules
 *
 * Every rules should be created within a XXX.rule file with the filename matching the name of the function inside that file.
 * However header files can also be written and included in projects, to contain any inline classes that may be required.
 * 
 * MyFile.rule
 * 
 * <pre>
 * void MyFile()
 * {
 * 		DoStuff();
 * }
 * 
 * </pre>
 * 
 * @section Containers
 * 
 * There are several containers that can be used to retrieve the information on Client, Street, IOI details inside the OM
 * 
 * <pre>
 * CONTAINER 	coc;
 * ITEM 		co;
 * for(bool b = coc.GetFirstActive(co); b; b = coc.GetNextActive(co))
 * {
 * 		m_Log << "Got ITEM = " << co << std::endl;
 * }
 * </pre>
 *  
 */

/**
 * @file OMRulesExport.h
 *
 * @brief This is the interface available between the OMRules and the OM.
 *
 * @author Flextrade
 */
#include <string>
#include <map>
using namespace std;

class FillOrder;

/**
 * @brief All available classes and functions are inside the OMRULESEXPORT namespace.
 * 
 */
namespace OMRULESEXPORT
{
const double PRICE_MARKET_OPEN = -1000000;
const double PRICE_MARKET_DAY = -1000001;
const double PRICE_MARKET_CLOSE = -1000002;
const int MAX_STRAT_NAME_LEN_CONST = 4;
const int MAX_CONFIG_BUFFER_LEN_CONST = 4096;
const int MAX_NUMBER_OF_LEGS = 4;

/**	
 * @brief The Different sides that an order can be
 *
 */
enum enOrderSide
{
	SIDE_INVALID,
	SIDE_BUY,
	SIDE_SELL,
    SIDE_BM,
    SIDE_SP,
	SIDE_SHORT,
    SIDE_SSE,
    SIDE_SYSTEM,
    SIDE_CROSS,
    SIDE_BL_CROSS,
    SIDE_SL_CROSS,
    SIDE_B2C,
    SIDE_ALL
};

/**
 * @brief How long an order is valid for.
 * 
 */
enum enOrdTimeInForce
{
	TIF_INVALID,
	TIF_DAY,
	TIF_IOC,
	TIF_FOK,
	TIF_GTX,
	TIF_GTD,
	TIF_GTC,
	TIF_OPG
};

/**
 * @brief What is the base fix order type for this order.
 * 
 */
enum enOrdType
{
	TYPE_INVALID,
	TYPE_MARKET,
	TYPE_LIMIT,
	TYPE_STOP,
	TYPE_STOPLIMIT,
	TYPE_MKTCLO,
	TYPE_MKT_LEFTOVER,
	TYPE_PREVIOUSLY_QUOTED,
	TYPE_FX_PREVIOUSLY_QUOTED,
	TYPE_LIMCLO
};

/**
 * @brief What is the current state of this order.
 * 
 */
enum enOrderState
{
	STATE_UNACKED,
	STATE_OPEN,
	STATE_PARTIAL,
	STATE_FILLED,
	STATE_CANCELLED,
	STATE_REPLACED,
	STATE_PENDING_RPL,
	STATE_REJECTED,
	STATE_DONE,
	STATE_PENDING_NEW,
	STATE_INVALID
};

/**
 * @brief What is the transaction type of the execution report
 *
 */
enum enExecTransactionType
{
	EXEC_INVALID,
	EXEC_NEW,
	EXEC_CXL,
	EXEC_CORRECT,
	EXEC_STAT,
	EXEC_OUT_CXL,
	EXEC_CXL_FLEX,
	EXEC_CORR_FLEX,
};

enum enLegCFICode
{
	ELEG_CFI_CODE_ES,
	ELEG_CFI_CODE_OC,
	ELEG_CFI_CODE_OP,
	INVALID_LEG_CFI_CODE
};


/**
 * @brief What are possible dispositions for a client order. Usually used for HELD orders
 * being dealt with automatically
 *
 */
enum enExecOption
{
    EXEC_OPTION_CROSS  = 0x0001,
  	EXEC_OPTION_AUTOEX = 0x0002,
  	EXEC_OPTION_ROUTE  = 0x0004,
  	EXEC_OPTION_MANUAL = 0x0008,
  	EXEC_OPTION_PING_EXTSYS = 0x0010,
  	EXEC_OPTION_PING_NATIVE = 0x0020,
};

/**
 * @brief Symbolic constants for database tables. This file must matche with internal enums in flexdb/include/FtRefData.h
 *
 */
enum enRefDataTable
{
  REF_DATA_BROKER = 0
  , REF_DATA_CLIENT_ACCT
  , REF_DATA_EXCHANGE
  , REF_DATA_FIRM_ACCT
  , REF_DATA_FIRM_ACCT_XS
  , REF_DATA_HOLIDAYS
  , REF_DATA_OASYS_XREF
  , REF_DATA_SEC_MASTER
  , REF_DATA_SEC_PRICES
  , REF_DATA_SEC_ROUTING
  , REF_DATA_TRADER_SYMBOL  //10
  , REF_DATA_USER
  , REF_DATA_PNL
  , REF_DATA_RULES_MATRIX
  , REF_DATA_TRADING_RULES
  , REF_DATA_MANNING_RULES
  , REF_DATA_CONFIG
  , REF_DATA_EXCHANGE_PARAMS
  , REF_DATA_ORDER_ROUTING
  , REF_DATA_CUST_DEFAULTS
  , REF_DATA_FX_RATE        // 20
  , REF_DATA_ETF_CREATE_REDEEM
  , REF_DATA_QUOTE_PARAM
  , REF_DATA_SHORT_LOCATE
  , REF_DATA_CUST_COVERAGE
  , REF_DATA_LIQUIDITY_MATRIX
  , REF_DATA_COUNT
};

/**
 * @brief These are the events that are called in order for the OMRules to be executed.
 * 
 */
enum enEvents
{
	EVENT_VALID_CLIENT_ORDER,		///< Data is CLIENT_ORDER
	EVENT_VALID_CLIENT_REPLACE,     ///< Data is CLIENT_ORDER
	EVENT_VALID_CLIENT_CANCEL,		///< Data is CLIENT_ORDER
	EVENT_CLIENT_NEW_ORDER,			///< Data is CLIENT_ORDER
	EVENT_CLIENT_REPLACE_ORDER,		///< Data is CLIENT_ORDER
	EVENT_CLIENT_CANCEL_ORDER,		///< Data is CLIENT_ORDER

	EVENT_CLIENT_SEND_ACK,			///< Data is CLIENT_ORDER
	EVENT_CLIENT_SEND_OUT,			///< Data is CLIENT_ORDER
	EVENT_CLIENT_SEND_REJECT,		///< Data is CLIENT_ORDER
	EVENT_CLIENT_SEND_FILLS,		///< Data is CLIENT_EXEC
	EVENT_CLIENT_SEND_PENDING_RPL,   ///< Data is CLIENT_EXEC

	EVENT_VALID_STREET_ORDER,		///< Data is STREET_ORDER
	EVENT_VALID_STREET_REPLACE,		///< Data is STREET_ORDER
	EVENT_VALID_STREET_CANCEL,		///< Data is STREET_ORDER
	EVENT_STREET_NEW_ORDER,			///< Data is STREET_ORDER
	EVENT_STREET_REPLACE_ORDER,		///< Data is STREET_ORDER
	EVENT_STREET_CANCEL_ORDER,		///< Data is STREET_ORDER
	EVENT_STREET_STATUS_REPORT,

	EVENT_STREET_ACK,				///< Data is STREET_ORDER
	EVENT_STREET_OUT,				///< Data is STREET_ORDER
	EVENT_STREET_REJECT,			///< Data is STREET_ORDER
	EVENT_STREET_FILLS,				///< Data is STREET_EXEC
	EVENT_STREET_PENDING_RPL,		///< Data is STREET_EXEC

	EVENT_NEW_IOI_FROM_CLIENT,		///< Data is IOI
	EVENT_RPL_IOI_FROM_CLIENT,		///< Data is IOI
	EVENT_CXL_IOI_FROM_CLIENT,		///< Data is IOI
	EVENT_NEW_IOI_TO_CLIENT,		///< Data is IOI
	EVENT_RPL_IOI_TO_CLIENT,		///< Data is IOI
	EVENT_CXL_IOI_TO_CLIENT,		///< Data is IOI

	EVENT_CLIENT_CONNECT,			///< const char*
	EVENT_CLIENT_DISCONNECT,		///< const char*
	EVENT_STREET_CONNECT,			///< const char*
	EVENT_STREET_DISCONNECT,		///< const char*
	EVENT_TRADER_CONNECT,			///< Data is TRADER
	EVENT_TRADER_DISCONNECT,		///< Data is TRADER

	EVENT_TIMER,
	EVENT_TIMER_NO_DATA,
	EVENT_MARKET_DATA,				///< Data is MTICK
	EVENT_ONLOAD,					///< NULL
	EVENT_BEGIN_RECOVERY,			///< NULL
	EVENT_END_RECOVERY,				///< NULL
	EVENT_CONFIG_UPDATE,			///< Data is STRAT_CONFIG

	EVENT_CLIENT_COMMAND,			///< Data is STRAT_COMMAND
	EVENT_INTERNAL_EXEC,			///< Data is INTERNAL_EXEC

	EVENT_PRICE_SERVER_CONN_UP,		///< const char*
	EVENT_PRICE_SERVER_CONN_DOWN,	///< const char*

	EVENT_STREET_DISCARD_NEWORD,
	EVENT_STREET_DISCARD_RPLORD,
	EVENT_STREET_DISCARD_CXLORD,

	EVENT_CLIENT_ORDER_ENRICHMENT,  // This event is invoked first for a client order. The objective
	                                // is to fill defaults/computed values in the order so that 
	                                // we can route it appropriately.

	EVENT_CLIENT_ORDER_AUTOEX,
	EVENT_STREET_NEW_REPORT_STAT,	// event for FtStOrdsHndlr::processExecNewRepStats
	EVENT_CLIENT_ORIGINATED_EXEC,		///< Data is STREET_EXEC

	EVENT_MARKET_OPEN, // Data is EVENT_DATA.
	EVENT_MARKET_CLOSE, // Data is EVENT_DATA

	EVENT_CLIENT_MLEG_ORDER_ENRICHMENT,
	EVENT_VALID_CLIENT_NEW_MLEG_ORDER,		///< Data is CLIENT_MLEG_ORDER
	EVENT_VALID_CLIENT_REPLACE_MLEG_ORDER,	///< Data is CLIENT_MLEG_ORDER
	EVENT_VALID_CLIENT_CANCEL_MLEG_ORDER,	///< Data is CLIENT_MLEG_ORDER
	EVENT_CLIENT_NEW_MLEG_ORDER,			///< Data is CLIENT_MLEG_ORDER
	EVENT_CLIENT_REPLACE_MLEG_ORDER,		///< Data is CLIENT_MLEG_ORDER
	EVENT_CLIENT_CANCEL_MLEG_ORDER,			///< Data is CLIENT_MLEG_ORDER

    EVENT_STREET_MLEG_ORDER_ENRICHMENT,
    EVENT_VALID_STREET_NEW_MLEG_ORDER,      ///< Data is STREET_MLEG_ORDER
    EVENT_VALID_STREET_REPLACE_MLEG_ORDER,  ///< Data is STREET_MLEG_ORDER
    EVENT_VALID_STREET_CANCEL_MLEG_ORDER,   ///< Data is STREET_MLEG_ORDER
    EVENT_STREET_NEW_MLEG_ORDER,            ///< Data is STREET_MLEG_ORDER
    EVENT_STREET_REPLACE_MLEG_ORDER,        ///< Data is STREET_MLEG_ORDER
    EVENT_STREET_CANCEL_MLEG_ORDER,	        ///< Data is STREET_MLEG_ORDER

    EVENT_CLIENT_EXEC_CORR,					// Data is CLIENT_EXEC

    EVENT_STREET_FAKE_REP,					// Data is STREET_EXEC
    EVENT_STREET_BUST_REP					// Data is STREET_BUST_EXEC
};


/**
 * @brief Enumeration of the different types of class available through the om interface.
 * 
 */
enum enClassType
{
	CLASS_NONE,
	CLASS_TRADER,
	CLASS_MTICK,
	CLASS_ORDER,
	CLASS_CLIENT_ORDER,
	CLASS_STREET_ORDER,
	CLASS_STREET_EXEC,
	CLASS_CLIENT_EXEC,
	CLASS_CUSTOMER_INFO,
	CLASS_STRAT_COMMAND,
	CLASS_STRAT_CONFIG,
	CLASS_MKT_DEPTH_INFO,
	CLASS_PNL_INSTANCE_INFO,
	CLASS_POSITION_INFO,
	CLASS_EVENT_DATA,
	CLASS_IOI,
	CLASS_POINTER,
	CLASS_SECURITY_INFO,
	CLASS_USER_INFO,
	CLASS_CLIENT_MLEG_ORDER,
	CLASS_STREET_MLEG_ORDER
};

/**
 * @brief Enumeration of error codes
 *
 */
enum enErrorCode
{
	RULES_EO_OK = 1,
	RULES_EO_FAIL = -99,
	RULES_EO_NO_LVS,
	RULES_EO_NO_ORD,
	RULES_EO_NO_PORT,
	RULES_EO_OTHER_USER,
	RULES_EO_MAX_REJ,
	RULES_EO_NO_CLIENT_ORD,
	RULES_EO_NOT_ACTIVE,
	RULES_EO_REF_CXLD,
	RULES_EO_REF_RPLD,
	RULES_EO_INVALID_PRICE,
	RULES_EO_EXCEED_UNORD_SHRS,
	RULES_EO_DEST_NOT_UP,
	RULES_EO_SOCKET_FAIL,
};

enum enPorSide
{
	RULES_PS_NIL= 1,
	RULES_PS_B	= 2,
	RULES_PS_S	= 3,
	RULES_PS_H	= 4,
	RULES_PS_C	= 5,
	RULES_PS_ALL=99
};

/**
 * @brief Contains data when a Trader logs in to the OM, EVENT_TRADER_CONNECT or EVENT_TRADER_DISCONNECT
 * 
 */
class TRADER
{
public:
	TRADER(const char* szLogin, int nMode);

	const char* GetLogin();
	int GetMode();

private:
	char m_szLogin[8];
	int m_nMode;
};

/**
 * @brief Contains data about a new EVENT_MARKET_DATA Event.
 * 
 * @see enEvents
 * 
 */
class MTICK
{
public:
	MTICK(void *pData = NULL);
	virtual ~MTICK();

	const char * GetSymbol() const;
	const char * GetExchange() const;
	double GetBid() const;
	double GetAsk() const;
	double GetLast() const;
	int GetBidSize() const;
	int GetAskSize() const;
	int GetLastSize() const;
	double GetVolume() const;
	double GetVwap() const;
	char GetBidEx() const;
	char GetAskEx() const;
	double GetTickSize() const;

	int GetFlid(int nFlid, int iDefaultValue) const;
	double GetFlid(int nFlid, double dDefaultValue) const;
	char GetFlid(int nFlid, char cDefaultValue) const;
	bool GetFlid(int nFlid, char* szBuffer, int iBufferSize) const;
	bool IsTradeTick() const;

	bool IsTopic() const;
	const char *GetSymbolFromTopic() const;
	const char *GetPortfolioFromTopic() const;

protected:
	void *m_pData;

	friend bool MarketDataGetSymbolInfo(const char* szSymbol, MTICK &tickInfo);
};

enum enOrderReqMsgType
{
    UNKNOWN_REQ,
    NEWORD_REQ,
    RPLORD_REQ,
    CXLORD_REQ
};


//#define SETTABLE_FIELD(FIELD) FIELD; bool FIELD##_set;
//#define IS_FIELD_SET(FIELD) FIELD##_set

/**
 * @brief Base class for Client and Street orders
 * 
 * @see CLIENT_ORDER, STREET_ORDER
 */
class ORDER
{
public:
    /**
     * Constructor
     */
	ORDER(const int isRecoveredObject = 0);

    /**
     * Destructor
     */
	virtual ~ORDER() { }

    /**
     * Get order handle (ID)
     */
	const char * GetHandle() const;

    /**
     * Get order handle (ID)
     */
	void GetHandle(void*& handle) const;

    /**
     * Set order handle (ID)
     */
	bool SetHandle(const char*);

    /**
     * Get order symbol
     */
	const char * GetSymbol() const;

    /**
     * Set order symbol
     */
	bool SetSymbol(const char*);

    /**
     * Get client symbol
     */
	const char * GetClientSymbol() const;

    /**
     * Set client symbol
     */
    bool  SetClientSymbol(const char *);

    /**
     * Get time of the order
     */
	int GetTime() const;

    /**
     * Get average price
     */
	double GetAvgPrice() const;

    /**
     * Set average price
     */
	bool SetAvgPrice(double dblAgvPrice);

    /**
     * Get order price
     */
	double GetPrice() const;

    /**
     * Set order price
     */
	bool SetPrice(double dblPrice);

    /**
     * Get stop price
     */
	double GetStopPrice() const;

    /**
     * Set stop price
     */
	bool SetStopPrice(double dblStopPrice);

    /**
     * Get order size
     */
	FT_SIZE_TYPE GetSize() const;

    /**
     * Set order size
     */
	bool SetSize(FT_SIZE_TYPE nSize);

    /**
     * Get order bench price
     */
	double GetBenchPrice() const;

    /**
     * Set order bench price
     */
	void SetBenchPrice(const double dBenchPrice);

    /**
     * Get order filled size
     */
	FT_SIZE_TYPE GetFilledSize() const;

    /**
     * Get order outed size
     */
	FT_SIZE_TYPE GetOutedSize() const;

    /**
     * Get order number of leaves
     */
	FT_SIZE_TYPE GetLeaves() const;

    /**
     * Get order type
     */
	enOrdType GetOrderType() const;

    /**
     * Get order Time in Force 
     */
	enOrdTimeInForce GetOrderTimeInForce();

    /**
     * Set order Time in Force
     */
	bool SetOrderTimeInForce(enOrdTimeInForce tif);

    /**
     * Set order type
     */
	bool SetOrderType(OMRULESEXPORT::enOrdType type);

    /**
     * Get order side
     */
	enOrderSide GetSide() const;

    /**
     * Set order size
     */
	bool SetSide(enOrderSide side);

    /**
     * Get client ID 0
     */
	long GetClientId0() const;

    /**
     * Set client ID 0
     */
	bool SetClientId0(long nClientId);

    /**
     * Get client ID 2
     */
	int GetClientId2() const;

    /**
     * Set client ID 2
     */
	bool SetClientId2(int nClientId);

    /**
     * Get order destination
     */
	const char * GetDestination() const;

    /**
     * Set order destination
     */
	bool SetDestination(const char * szDestination);

    /**
     * Get order sender
     * 
     * @param bOnBehalfIfAvail get OnBehalfOfCompID if available
     * @return OnBehalfOfCompID tag if bOnBehalfIfAvail is set, otherwise returns sender
	 *
     */
	const char * GetSender(bool bOnBehalfIfAvail = false);

    /**
     * Set order sender
     */
	bool SetSender(const char * szSender);

    /**
     * Get order sender sub ID
     * 
     * @param bOnBehalfIfAvail get OnBehalfOfSubID if available
     * @return OnBehalfOfSubID tag if bOnBehalfIfAvail is set, otherwise returns sender sub ID
     *
     */
	const char * GetSenderSubId(bool bOnBehalfIfAvail = false);

    /**
     * Set order sender sub ID
     */
	bool SetSenderSubId(const char * szSenderSubId);

    /**
     * Get destination sub ID
     */
	const char * GetDestinationSubId() const;

    /**
     * Set destination sub ID
     */
	bool SetDestinationSubId(const char * szDestinationSubId);

    /**
     * Get account of the order
     */
	const char * GetAccount() const;

    /**
     * Set account of the order
     */
	bool SetAccount(const char * szAccount);

    /**
     * Get execution destination
     */
	const char * GetExecDest() const;

    /**
     * Set execution destination
     */
	bool SetExecDest(const char * szExecDest);

    /**
     * Get commission
     */
	double GetCommission() const;

    /**
     * Set commission
     */
	bool SetCommission(double dblCommission);

    /**
     * Get commission type
     */
	short GetCommissionType() const;

    /**
     * Set commission type
     */
	bool SetCommissionType(short iCommissionType);

    /**
     * Get (extension) commission type
     */
	char GetCommType() const;

    /**
     * Set (extension) commission type
     */
	bool SetCommType(char cCommissionType);

    /**
     * Get rule 80
     */
	char GetRule80() const;

    /**
     * Set rule 80
     */
	bool SetRule80(char cRule80);

    /**
     * Get fix tags
     */
	const char* GetFixTags() const;

    /**
     * Get fix tag
     */
	const char* GetFixTag(int nTag);

    /**
     * Set fix tag
     *
     * @param nTag fix tag
     * @param szValue tag value string
     * @return Whether or not the tag has been set correctly
	 *
     */
	virtual bool SetFixTag(const int nTag, const char * const szValue);

    /**
     * Set fix tag string
     */
	bool SetFixTagStr(const char * szFixTags);

    /**
     * Update fix tag
     *
     * @param tag fix tag
     * @param value tag value string
     * @return Whether or not the tag has been updated correctly
     *
     */
	bool UpdateFixTag(const int tag, const char* value);

    /**
     * Remove fix tag
     */
	bool RemoveFixTag(const int tag) { return UpdateFixTag(tag, NULL); }

    /**
     * Get customer ID
     */
	const char * GetCustomerId() const;

    /**
     * Set customer ID
     */
	bool SetCustomerId(const char* szValue);

    /**
     * Get trading account
     */
	const char * GetTradingAccount() const;

    /**
     * Set trading account
     */
	bool SetTradingAccount(const char* szValue);

    /**
     * Get user memo
     */
	const char * GetUserMemo() const;

    /**
     * Set user memo
     */
	bool SetUserMemo(const char* szValue);

    /**
     * Get wave name
     */
	const char * GetWaveName() const;

    /**
     * Set wave name
     */
	bool SetWaveName(const char* szValue);

    /**
     * Check whether the order is valid or not
     */
	bool IsValidOrder() const;

    /**
     * Get order reject message
     */
	const string& GetRejectMsg() const;

    /**
     * Set order reject message
     */
	void SetRejectMsg(const string& szRejectMsg);

    /**
     * Set whether the order is valid or not
     */
	void SetValidOrder(const bool yesNo);

    /**
     * Check whether the order is active or not
     */
	bool IsOrdActive() const;

    /**
     * Set whether the order is active or not
     */
	void SetIsOrdActive(bool bIsActive);

    /**
     * Update destination sub ID
     */
	void UpdateDestinationSubId(const char* trader);

    /**
     * Update commission
     */
	void UpdateCommission(const double commission);

    /**
     * Get source
     */
	const char * GetSource() const;

    /**
     * Set source
     */
	bool SetSource(const char* szValue);

    /**
     * Get locate information
     */
	const char* GetLocateInfo() const;

    /**
     * Set locate information
     */
	void SetLocateInfo(const char* value);

    /**
     * Get client sub ID
     */
	const char* GetClientSubid() const;

    /**
     * Set client sub ID
     */
	void SetClientSubid(const char* value);

    /**
     * Get preborrow quantity
     */
	const FT_SIZE_TYPE GetPreborrowQty() const;

    /**
     * Set preborrow quantity
     */
	void SetPreborrowQty(const FT_SIZE_TYPE value);

    /**
     * Get preborrow code
     */
	const char* GetPreborrowCode() const;

    /**
     * Set preborrow code
     */
	void SetPreborrowCode(const char* value);

    /**
     * Get new order ID (of the replace/cancel order)
     */
	const string& GetNewOrderId() const;

    /**
     * Set new order ID (of the replace/cancel order)
     */
	void SetNewOrderId(const string& newOrdId);

    /**
     * Get common order ID
     */
	const char* GetCommonOrderId();

    /**
     * Set common order ID
     */
	void SetCommonOrderId(const char* value);

    /**
     * Check whether the replace is trivial or not
     */
	bool trivialReplace() const;

    /**
     * Set whether the replace is trivial or not
     */
	void SetTrivialReplace(const bool yn);

    /**
     * Set whether the order is recovered or not
     */
	void SetRecoveredObject(const int yn);

    /**
     * Check whether the order is recovered or not
     */
	int isRecoveredObject() const;

    /**
     * Set whether the order is replace pending or not
     */
	void SetReplacePending(const bool yn);

    /**
     * Check whether the order is replace pending or not
     */
	bool isReplacePending() const;

    /**
     * Set whether the order is cancel pending or not
     */
	void SetCancelPending(const bool yn);

    /**
     * Check whether the order is cancel pending or not
     */
	bool isCancelPending() const;

    /**
     * Set whether the order has been ACKed or not
     */
	void SetAcked(const bool yn);

    /**
     * Check whether the order has been ACKed or not
     */
	bool isAcked() const;

    /**
     * Set order portfolio
     */
	void SetPortfolio(const char* port);

    /**
     * Get order portfolio
     */
	const char* GetPortfolio() const;

    /**
     * Get strategy name
     */
	const string& GetStrategyName() const;

    /**
     * Set strategy name
     */
	void SetStrategyName(const string& stratName);

    /**
     * Get reference order ID
     */
	const string& GetRefOrdId() const;

    /**
     * Set reference order ID
     */
	void SetRefOrdId(const string& refOrdId);

    /**
     * Set order information from the NewOrd pointer
     */
	void SetOrderPtr(void *pn);

    /**
     * Set original size of the order
     */
	void SetOrigSize(const FT_SIZE_TYPE val);

    /**
     * Get original size of the order
     */
	FT_SIZE_TYPE GetOrigSize() const;

    /**
     * Set original price of the order
     */
	void SetOrigPrice(const double val);

    /**
     * Get original price of the order
     */
	double GetOrigPrice() const;

    /**
     * Check whether the order has been fully filled
     */
	bool IsFullyFilled() const;

    /**
     * Check whether the order has been canceled
     */
	bool IsCanceled() const;

    /**
     * Set trade date
     */
	void SetTradeDate(const int val);

    /**
     * Get trade date
     */
	int GetTradeDate() const;

    /** 
     * Set settlement type
     */
	void SetSettleType(const char val);

    /**  
     * Get settlement type
     */
	char GetSettleType() const;

    /**  
     * Set settlement date
     */
	void SetSettleDate(const int val);

    /**  
     * Get settlement date
     */
	int GetSettleDate() const;

    /**  
     * Set execution instruction
     */
	void SetExecInst(const string& val);

    /**  
     * Set execution instruction
     */
	void SetExecInst(int val);

    /**  
     * Get execution instruction
     */
	string GetExecInstStr() const;

    /**  
     * Get execution instruction
     */
	int GetExecInst() const;

    /**  
     * Set execution instruction
     */
	void SetExecInst(const char val);

    /**  
     * Check whether cancel request reject has been set
     */
	bool IsCxlReqRejectSet();

    /**  
     * Check whether replace request reject has been set
     */
	bool IsRplReqRejectSet();

    /**  
     * Set system memo
     */
	void SetSystemMemo(const char* val);

    /**  
     * Get system memo
     */
	const char* GetSystemMemo() const;

    /**  
     * Set system order type
     */
	void SetSystemOrdType(int systemOrdType);

    /**  
     * Get system order type
     */
	int GetSystemOrdType() const;

    /**  
     * Check whether the order is multileg order or not
     */
	bool IsMultilegOrder() const;

    /**  
     * Check whether the order is multileg parent order or not
     */
	bool IsMultilegParent() const;

    /**  
     * Check whether the order is multileg child order or not
     */
	bool IsMultilegChild() const;

protected:
    /**
     * Set time of the order
     */
	bool SetTime(int);

    /**
     * Set filled size
     */
	bool SetFilledSize(const FT_SIZE_TYPE nSize);

    /**
     * Set outed size
     */
	bool SetOutedSize(const FT_SIZE_TYPE nSize);

    /**
     * Set number of leaves
     */
	bool SetLeaves(const FT_SIZE_TYPE nLeaves);

    /**
     * Set whether the order is cancel request reject or not
     */
	void SetCxlReqReject(bool val);

    /**
     * Set whether the order is replace request reject or not
     */
	void SetRplReqReject(bool val);

	FT_SIZE_TYPE m_iOutedSize;
	FT_SIZE_TYPE m_iFilledSize;
	FT_SIZE_TYPE m_iLeaves;
	bool m_bOrdIsActive;
	double m_dblStopPrice;
	double m_dblBenchPrice;
	string m_newOrderId; // for replace/cancel the Order ID of the New Order
	bool m_bValidOrder;
	double m_dblAvgPrice;
	bool m_bTrivialReplace;
	int m_iRecoveredObject;
	bool m_bReplacePending;
	bool m_bCancelPending;
	bool m_bIsAcked;
	string m_szRejectMsg;
	string m_strategyName;
	string m_refOrdId;
	FT_SIZE_TYPE m_iOrigSize;
	double m_dblOrigPrice;
	long m_client_id;

	std::multimap<int, std::string> m_customFixtags;

	//// NewOrd fields /////////////////////////////////////////////////////////////////////
	string m_handle; // oprice.id
	string m_symbol; // odesc.sy
	string m_clientSym; 
	time_t m_time;   // oprice.tim
	double m_price;  // oprice.lim
	FT_SIZE_TYPE m_size;      // osiz
	enOrderSide m_side; // oprice.osid
	enOrdTimeInForce m_tif; // oprice.tif
	enOrdType m_type; // oprice.otyp
	long m_clientId2; // client_id2
	string m_dest;    // odesc.Dest
	string m_sender;  // odesc.Sndr
	string m_senderSubId; // odesc.SndrSubid
	string m_destSubId;   // odesc.DestSubid
	string m_account;     // acct
	string m_execDest;    // oprice.sExDest
	double m_commission;  // extension.commission
	short m_commissionType; // comm_typ
	char m_commType;  // extension.comm_type
	char m_rule80;    // rule80
	char m_fixtags[512]; // fixtags
	string m_customerId; // extension.client_id
	string m_tradingAcct; // extension.trading_acct
	string m_userMemo; // ordMemo
	string m_waveName; // waveName
	string m_locateInfo; // extension.locate_info
	string m_clientSubId; // extension.client_subid
	FT_SIZE_TYPE m_preborrowQty; // extension.preborrow_qty
	string m_preborrowCode; // extension.preborrow_code
	string m_commonOrderId; // extension.common_order_id
	string m_source; // extension.source
	string m_systemMemo; // sSystemMemo
	////////////////////////////////////////////////////////////////////////////////////////
	string m_portfolio; 
	bool m_customFixtagsChanged;
	int m_trade_date; // extension.trade_date
	char m_settlement_type; // extension.settlement_type
	int  m_settlement_date; // extension.settlement_date
	int  m_exec_inst; // Held ==> '5' / Nheld ==> '1'
	bool m_cxlReqRej;
	bool m_rplReqRej;
	int m_systemOrdType;
};

/**
 * @brief Contains information about an incoming client order.
 * 
 * 
 */
class CLIENT_ORDER : public ORDER
{
public:
    /**
     * Constructor
     */
	CLIENT_ORDER(const int isRecoveredObject = 0);

    /**
     * Set fix tag
     *
     * @param nTag fix tag
     * @param szValue tag value string
     * @return Whether or not the tag has been set correctly
	 *
     */
	bool SetFixTag(const int nTag, const char * const szValue);

    /**
     * Get unsent size of the client order
     */
	FT_SIZE_TYPE GetUnOrdSize() const;

    /**
     * Get number of total outs 
     */
	FT_SIZE_TYPE GetTotOuts() const;

    /**
     * Get working size
     */
	FT_SIZE_TYPE GetWorkingSize() const;

    /**
     * Get street execution size
     */
	FT_SIZE_TYPE GetStreetExecs() const;

    /**
     * Get replace request order 
     */
	int GetRplReqOrd(CLIENT_ORDER& clOrd) const;

    /**
     * Check whether the order is part of a bunch order or not 
     */
	bool IsPartOfBunch() const;

    /**
     * Send client order out message
     */
	bool SendOut(const char* szOutReason = NULL);

    /**
     * Send client order ACK message
     */
	bool SendAck();

    /**
     * Send client order pending replace message
     */
	bool SendPendingReplace();

    /**
     * Send client order replaced message
     */
	bool SendReplaced();

	/**  
     * Send client order suspended message
     *
     * @param fixtags Fix tags
     * @return Whether or not the message has been sent successfully
     *
     */
	bool SendSuspended(const char* fixtags = NULL);

    /**
     * Send client order triggered message
     *
     * @param fixtags Fix tags
     */
	bool SendTriggered();
	bool SendTriggered(const char* fixtags);

    /**
     * Send client order rejected message
	 *
     * @param szRejectReason Reason of the reject
     * @param pendRplOrder Whether the order is pending replace or not
     * @return Whether or not the message has been sent successfully
	 *
     */
	bool SendReject(const char* szRejectReason, bool pendRplOrder = false);

    /** 
     * Send client order cancel request reject message
     *
     * @param szRejectReason Reason of the reject
     * @return Whether or not the message has been sent successfully
     *
     */
	bool SendCancelReqReject(const char *szRejectReason);

    /**  
     * Send client order replace request reject message
     *
     * @param szRejectReason Reason of the reject
     * @return Whether or not the message has been sent successfully
     *
     */
	bool SendReplaceReqReject(const char* szRejectReason);

    /**
     * Send client order restated message
     *
     * @param oshrs Number of shares restated
     * @param fixtags fixtags to send
     * @return Whether or not the message has been sent successfully
     *
     */
	bool SendRestated(FT_SIZE_TYPE oshrs, const char* fixtags);

    /**
     * Set primary order ID
     */
	void SetPrimOrdId(const string& primOrdId);

    /**
     * Get primary order ID
     */
	const string& GetPrimOrdId() const;

    /**
     * Get original order ID
     */
	const string& GetOrigOrderId() const;

    /**
     * Set original order ID
     */
	void SetOrigOrderId(const string& origOrdId);

    /**  
     * Cross the client order with another client order
     *
     * @param co Client order to cross
	 * @param numShares Number of shares to cross
	 * @param crossPrice Price to cross
	 * @param crossId Cross ID
     * @return Whether or not the two client orders have been successfully crossed
     *
     */
	bool Cross(CLIENT_ORDER& co, const int numShares, const double crossingPrice, const char *crossId = NULL);

    /**
     * Get assigned trader
     */
	const string GetAssignedTrader() const;

    /**
     * Assign the client order to the trader
     */
	bool AssignTrader(const string& trdr);

    /**
     * Automatically cross the client order with other orders in the order book
     */
	void AutoCrossHeldOrder();

    /**
     * Automatically execute the client order
	 *
     * @param execQty Quantity of the execution
     * @param execPrice Price of the execution
	 *
     */
	void AutoExHeldOrder(const FT_SIZE_TYPE& execQty, const double execPrice);

    /**
     * Get execution options
     */
	int GetExecOptions() const;

    /** 
     * Add execution option
     */
	void AddExecOption(const enExecOption option);

    /** 
     * Delete execution option
     */
	void DelExecOption(const enExecOption option);

    /** 
     * Clear execution options
     */
	void ClearExecOptions();

    /**
     * Request cancel the client order
     */
	bool RequestCancel();

private:
	FT_SIZE_TYPE m_iWorkingSize;
	// for replace/cancel requests which are morphed into a new ord
	string m_origOrderid;
	string m_clientPrimOrdId; 
	friend class ::FillOrder;
	int m_execOptions; 
};

/**
 * @brief Contains information about an outgoing street order.
 * 
 * 
 */
class STREET_EXEC;
class STREET_ORDER : public ORDER
{
public:
    /**
     * Constructor
     */
	STREET_ORDER(const int isRecoveredObject = 0);

    /**
     * Get client order ID
     */
	const string& GetClientOrdId() const;

    /**
     * Set client order ID
     */
	void SetClientOrdId(const string& clientOrdId);

    /**
     * Get order state
     */
	enOrderState GetOrderState() const;

    /**
     * Get number of total leaves
     */
	FT_SIZE_TYPE GetTotalLvs() const;

    /**
     * Set number of total leaves
     */
	void SetTotalLvs(const FT_SIZE_TYPE nTotLvs);

    /**
     * Get last report time
     */
	time_t GetLastRepTime() const;

    /**
     * Set last report time
     */
	void SetLastRepTime(time_t tLastRep);

    /**
     * Set order state
     */
	bool SetOrderState(enOrderState state);

    /**
     * Set primary order ID
     */
	void SetPrimOrdId(const string& primOrdId);

    /**
     * Get primary order ID
     */
	const string& GetPrimOrdId() const;

    void SetBrokerOrdId(const char* brokerOrdId);
    const string& GetBrokerOrdId() const;

    /**
     * Send the street order
     */
	enErrorCode Send();

    /**
     * Send street order status request message
     */
	bool SendRequest();

    /**
     * Send street order cancel message
     */
	enErrorCode Cancel();

    /**
     * Send street order replace message
     */
	enErrorCode Replace();

    /**
     * Send manual street order message
     * @param bIsCrossPnL Whether the manual street order is pipped cross or not
	 *
     */
	bool CreateAndFillManual(bool bIsCrossPnL);

    /**
     * Get order request message type of the street order
     * @param enOrdReq Output order request message type
     * @return whether the API is successful called or not
     *
	 */
	bool GetOrderReqMsgType(enOrderReqMsgType &enOrdReq);

    /**
     * Get street execution by ID
     * @param execId Input street execution ID
	 * @param sexec Output Street execution
	 * @return whether the street exec has been found or not
     *
     */
	bool GetStreetExecById(const char *execId, STREET_EXEC &sexec);

protected:
	string m_clientOrdId;
	string m_streetPrimOrdId;
    string m_brokerOrdId;
	enOrderState m_enOrderState;

	time_t m_tLastRep;
	FT_SIZE_TYPE m_iTotLvs;
	friend class ::FillOrder;
};

/**
 * @brief Contains information about an incoming street execution
 * 
 * 
 */
class STREET_EXEC
{
public:
	STREET_EXEC(void* pOMSExecRep, const int isRecoveredObject = 0);
	STREET_EXEC();
	virtual ~STREET_EXEC();

	const FT_SIZE_TYPE GetLastFillQty() const;
	const double GetLastFillPrice() const;
	const char * GetExecBroker() const;
	void SetExecBroker(const char* szVal);
    const char * GetBrokerOrdId() const;
	const char * GetLastMkt() const;
	void SetLastMkt(const char* szVal);
	char GetCapacity() const;
	void SetCapacity(const char cCapacity);
	const char * GetTradingAccount() const;
	void SetTradingAccount(const char* szTrdAcct);
	const char* GetExecId() const;
	const char* GetExecRefId() const;
	const char * GetHandle() const;
	const string& GetClientOrdId() const;
	FT_SIZE_TYPE GetFilledSize() const;
	FT_SIZE_TYPE GetSize() const;
	double GetPrice() const;
	enOrderSide GetSide() const;
	enExecTransactionType GetExecTransType() const;
	FT_SIZE_TYPE GetTotalLvs() const;
	const char * GetDestination() const;
	const char * GetDestinationSubId() const;
	const char * GetSender() const;
	const char * GetSenderSubId() const;
	enOrderState GetOrderState() const;
	const char * GetAccount() const;
	const char * GetExecDest() const;
	int GetClientId2() const;
	char GetRule80() const;
	double GetCommission() const;
	short GetCommissionType() const;
	const char * GetFixTag(int nTag);
	const char * GetSymbol() const;
	const char * GetExecSymbol() const;
	STREET_ORDER& GetStreetOrder();
	bool SetFixTags(const char* value);
	bool SetFixTag(int tag, const char* value);
	void SetExecPtr(void* pExec);
	bool SetLastFillPrice(double dNewFillPx);
	bool isMultiLegExec();
	void setMultiLegExec();
	bool SendMLExecToClient();
	const char *GetMLStategyName();
    char GetReportStatus();
	bool UpdateFixTag(int nTag, const char * szValue);
	time_t GetTransactionTime() const;
	int GetTradeDate() const;

protected:

	virtual char* GetFixTagPtr(size_t* nMaxLength = NULL);
	virtual const char* GetFixTagPtr(size_t* nMaxLength = NULL) const;
	STREET_ORDER m_Order;
	void* m_pOMSExecRep;
	void *m_CustomFixTags;
	bool SetOrderState(enOrderState state);
	bool multilegExec;
	friend class FillOrder;
};

class STREET_BUST_EXEC
{
public:
	STREET_BUST_EXEC(void* pBustExecRep, const int isRecoveredObject = 0);
	const char* GetBustOrdId() const;
	const char* GetBustRepId() const;
	STREET_ORDER& GetStreetOrder();

protected:
	STREET_ORDER m_Order;
	void* m_pBustExecReq;
	int m_isRecoveredObject;
};


/**
 * @brief Contains information about an outgoing client order.
 * 
 * 
 */
class CLIENT_EXEC
{
public:
	CLIENT_EXEC(void* pOMSExecRep, FT_SIZE_TYPE nLeaves, void* pClientOrder,
			const char* szStreetOrderDestination = 0, const int isRecoveredObject = 0);
	CLIENT_EXEC(void* pOMSExecRep, const int isRecoveredObj);
	virtual ~CLIENT_EXEC();
	const char * GetSymbol() const;
	enOrderSide GetSide() const;
	const char * GetDestination() const;
	const char * GetSender() const;
	const char * GetSndrSubId() const;
	const char * GetDestSubId() const;
	const char * GetOrderID() const;
	const char* GetExecId() const;
	const char* GetExecRefId() const;
	enExecTransactionType GetExecTransType() const;
	double GetLastPrice() const;
	void SetLastPrice(double dblLastPrice);
	double GetAvgPrice() const;
	void SetAvgPrice(const double avgPrice);
	FT_SIZE_TYPE GetLastAmount() const;
	FT_SIZE_TYPE GetCumQty() const;
	FT_SIZE_TYPE GetLeaves() const;
	bool IsPippedCross() const;
	bool SetFixTag(int nTag, const char * szValue);
	CLIENT_ORDER& GetClientOrder();
	char GetCapacity() const;
	const char* GetTradingAccount() const;
	const char * GetFixTags();
	const char* GetFixTag(int nTag);
	bool UpdateFixTag(const int tag, const char* value);
	bool RemoveFixTag(int nTag);
	const string& GetStrategyName() const;

	void SetRecoveredObject(const int yn);
	int isRecoveredObject() const;
	enOrderState GetReportStatus();
	const char* GetHandle() const;
	const string GetStreetExecID() const;
	bool GetStreetExec(STREET_EXEC &exec) const;
	double GetFXRate() const;
	int GetTradeDate() const;
protected:
	bool m_bPippedCross;
	CLIENT_ORDER m_Order;
	void* m_pOMSExecRep;
	FT_SIZE_TYPE m_nLeaves;
	int m_iRecoveredObject;
	void *m_CustomFixTags;

	// don't allow copying
	CLIENT_EXEC(const CLIENT_EXEC&);
	const CLIENT_EXEC& operator=(const CLIENT_EXEC*);
};

/**
 * @brief This is the data in the EVENT_INTERNAL_EXEC event
 * 
 * 
 */
class INTERNAL_EXEC
{
public:
	INTERNAL_EXEC(void* pOMSExecRep);
	INTERNAL_EXEC();
	virtual ~INTERNAL_EXEC();

	const char * GetExecBroker() const;
	void SetExecBroker(const char* szVal);
	const char * GetLastMkt() const;
	void SetLastMkt(const char* szVal);

	const char * GetFixTag(int nTag);

	bool SetFixTags(const char* value);
	bool SetFixTag(int tag, const char* value);

	double GetLastPrice() const;
	FT_SIZE_TYPE GetLastShares() const;

	void SetAvgPrice(const double avgPrice);
	void SetOrderQty(const FT_SIZE_TYPE qty);
	void SetOrderPrice(const double price);
	void SetCumQty(const FT_SIZE_TYPE qty);
	
	char GetCapacity() const;
	void SetCapacity(const char cCapacity);
	const char * GetTradingAccount() const;
	void SetTradingAccount(const char* szTrdAcct);
	
	int GetTradeDate() const;
	
	const char* GetExecId() const;
	const char* GetExecRefId() const;
	enExecTransactionType GetExecTransType() const;

protected:

	virtual char* GetFixTagPtr(size_t* nMaxLength = NULL);
	virtual const char* GetFixTagPtr(size_t* nMaxLength = NULL) const;
	void* m_pOMSExecRep;
	void *m_CustomFixTags;
};

/**
 * @brief Contains information about a LEG in multileg orders.
 */
class LEG
{
public:
    /**
     * Constructor
     */
	LEG();

    /**
     * Copy constructor
     */
    LEG(const LEG&);

    /**
     * Destructor
     */
	virtual ~LEG();

    /**
     * Get LEG implementation object 
     */
	void* GetLegImpl() const;

    /**
     * Assignment operator
     */
    LEG& operator=(const LEG&);

    /**
     * Get order portfolio
     */
	const string& GetOrdPor() const;

    /**
     * Get portfolio side information
     */
	enPorSide	GetPorSide() const;

    /**
     * Get symbol of the leg order
     */
	const string&	GetSymbol() const;

    /**
     * Get leg order side
     */
	enOrderSide	GetOrderSide() const;

    /**
     * Get leg CFI code
     */
	enLegCFICode	GetLegCFICode() const;

    /**
     * Get leg ratio quantity
     */
	double		GetRatioQty() const;

    /**
     * Get leg strike price
     */
	double		GetStrikePrice() const;

    /**
     * Get leg bench price
     */
	double		GetBenchPrice() const;

    /**
     * Add leg fix tag
     *
     * @param nTag fix tag
     * @param szValue tag value string
     * @return Whether or not the tag has been added correctly
     */
	bool		AddFixTag(const int nTag, const char * const szValue);

    /**
     * Get leg fix tag
     */
	const char*	GetFixTag(int nTag);

    /**
     * Update leg fix tag
     *
     * @param nTag fix tag
     * @param szValue tag value string
     * @return Whether or not the tag has been updated correctly
     */
    bool        UpdateFixTag(const int nTag, const char * const szValue);

    /**
     * Set leg fix tag string
     */
	void		SetFixTagStr(const char* fixStr);

    /**
     * Get leg fix tag string
     */
	const char*	GetFixTagStr() const;

    /**
     * Get leg client ID
     */
	long		GetClientId() const;

    /**
     * Get leg client order ID
     */
	const string&	GetClientOrdId() const;

    /**
     * Get leg reference ID
     */
	const string&	GetRefId() const;

    /**
     * Get leg quantity
     */
    FT_SIZE_TYPE GetLegQty() const;

    /**
     * Get leg forward points
     */
    double      GetFwdPoints() const;

    /**
     * Get leg spot price
     */
    double      GetSpotPrice() const;

    /**
     * Get leg expiry date time
     * @param datestr date string
     * @param timestr time string
     * @return whether the date and time has been retrieved successfully
     */
    bool		GetExpiryDateTime(string &datestr, string &timestr) const;

    /**
     * Get leg settlement date
     */
    const string& GetSettlementDate() const;

private:
	void *m_pImpl;
};

/**
 * @brief Contains information about all legs in a multileg orders.
 */
class MLEG_ORDER 
{
public:
    /**
     * Constructor
     */
	MLEG_ORDER() :
		m_UsrInit(""),
		m_nLegCount(0)
	{
	}

    /**
     * Set leg count
     */
	void	SetLegCount(int nLegCount);

    /**
     * Get leg count
     */
	int		GetLegCount() const;

    /**
     * Set user init
     */
	void		SetUsrInit(const char* pUsrInit);

    /**
     * Get user init
     */
	const char* GetUsrInit() const;

    /**
     * Get leg information by leg index
     */
	LEG&		GetLegInfo(size_t i);

    /**
     * Get leg information by leg index
     */
	const LEG&	GetLegInfo(size_t i) const;

private:
	string m_UsrInit;
	int m_nLegCount;
	LEG	m_Legs[MAX_NUMBER_OF_LEGS];
};

/**
 * @brief Contains information about a client multileg order.
 */
class CLIENT_MLEG_ORDER : virtual public MLEG_ORDER,
						  virtual public CLIENT_ORDER
{
public:
    /**
     * Constructor
     */
	CLIENT_MLEG_ORDER(const int recovering):
		MLEG_ORDER(),
		CLIENT_ORDER(recovering)
	{
	}

    /**
     * Get leg order size by leg index
     */
	FT_SIZE_TYPE GetLegOrderSize(size_t i) const;

    /**
     * Out the client multileg order
     */
	bool SendOut(const char* szOutReasons = NULL);

    /**
     * Check whether the client multileg order has been fully filled or not
     */
	bool IsFullyFilled() const;

    /**
     * Check whether the client multileg order has been canceled or not
     */
	bool IsCanceled() const;
};

/**
 * @brief Contains information about a street multileg order.
 */
class STREET_MLEG_ORDER : virtual public MLEG_ORDER,
						  virtual public STREET_ORDER
{
public:
    /**
     * Constructor
     */
	STREET_MLEG_ORDER(const int recovering) :
		MLEG_ORDER(),
		STREET_ORDER(recovering)
	{
	}

    /**
     * Get leg order size by leg index
     */
	FT_SIZE_TYPE GetLegOrderSize(size_t i) const;

    /**
     * Fill street multileg order from the client multileg order
     */
	void Fill(const CLIENT_MLEG_ORDER &order);

    /**
     * Send the multileg order to the street
     */
	enErrorCode Send();

    /**
     * Send mutileg street order replace message
     */
	enErrorCode Replace();
};

/**
 * @brief When constructed this class will retrieve information regarding the given id.
 * 
 * 
 */
class CUSTOMER_INFO
{
public:
	CUSTOMER_INFO(const char* id);
	virtual ~CUSTOMER_INFO();

	const char* GetId() const;
	const char* GetName() const;
	const double GetDefaultCommission() const;
	char* GetDefaultValue(const char* subid, const char* defaultName, char* value) const;
	char* GetRRNumber(char* rv) const;
	char* GetClrAcctId(char* rv) const;

private:
	char m_id[20];
	char m_name[128];
	double m_commission;
	char m_rrNumber[32];
	char m_clrAcctId[32];
};

/**
 * @brief This is the data passed in EVENT_CLIENT_COMMAND
 * 
 * @see STRAT_RESPONSE
 */
class STRAT_COMMAND
{
	friend class STRAT_RESPONSE;
public:
	STRAT_COMMAND(void* pCommand);
	~STRAT_COMMAND();

	bool GetStratName(char* szStratName, int iBufferSize);
	bool GetCommandName(char* szCommandName, int iBufferSize);
	bool GetParam(const char* szParamName, char* szParamValue, int iBufferSize);
	int GetParam(const char* szParamName, int iDefaultValue);
	double GetParam(const char* szParamName, double dDefaultValue);
	char GetParam(const char* szParamName, char cDefaultValue);

private:
	void *m_pCommand;
};

/**
 * @brief This class can be used to pass data back to the application call the RES_OM_RESPONSE analytic.
 * 
 * @see STRAT_COMMAND
 */
class STRAT_RESPONSE
{
public:
	STRAT_RESPONSE(const STRAT_COMMAND&);
	STRAT_RESPONSE();

	~STRAT_RESPONSE();

	bool GetStratName(char* szStratName, int iBufferSize);
	bool GetCommandName(char* szCommandName, int iBufferSize);

	bool GetParam(const char* szParamName, char* szParamValue, int iBufferSize);
	bool SetParam(const char* szParamName, const char* szParamValue);

	int GetParam(const char* szParamName, int iDefaultValue);
	bool SetParam(const char* szParamName, int iValue);

	double GetParam(const char* szParamName, double dDefaultValue);
	bool SetParam(const char* szParamName, double dValue);

	char GetParam(const char* szParamName, char cDefaultValue);
	bool SetParam(const char* szParamName, char cValue);

	void SetStatus(unsigned nStatus);
	unsigned int GetStatus() const;
	static const char* StatusName(unsigned nStatus);

	/**
	 * Send the information to the named user and call the RES_OM_RESPONSE analytic.
	 *
	 * @param Username
	 * @return Was the response sent
	 */
	bool Send(const char* user) const;
private:
	void *m_pResponse;
};

/**
 * @brief This is the data passed in EVENT_CONFIG_UPDATE
 * 
 * 
 */
class STRAT_CONFIG
{
public:
	STRAT_CONFIG(void* sConfig);
	virtual ~STRAT_CONFIG();

	const char* GetStratName();
	const char* GetConfigBuf();

private:
	char m_szStrategy[MAX_STRAT_NAME_LEN_CONST + 1];
	char m_szConfigBuffer[MAX_CONFIG_BUFFER_LEN_CONST + 1];
};

/**
 * @brief This will be filled after a call to GetMarketDepthInfo
 * 
 * @see GetMarketDepthInfo
 */
class MKT_DEPTH_INFO
{
public:
	MKT_DEPTH_INFO();
	virtual ~MKT_DEPTH_INFO();

	double GetBidAt(int level);
	int GetBidSizeAt(int level);
	double GetAskAt(int level);
	int GetAskSizeAt(int level);
	void SetDepthData(void* data);
	const char* GetBidIDAt(int level);
	const char* GetAskIDAt(int level);

private:

	void* m_pDepthInfo;
};

/**
 * @brief Not sure if this is used for anything
 * 
 * 
 */
class PNL_INSTANCE_INFO
{
public:
	char account[64];
	char symbol[64];
	bool makeInverse;
};

/**
 * @brief Can be used to specify a requested account to recover from GetPositions function calls.
 * 
 * @see GetPositions
 */
class POSITION_INFO
{
public:
	char account[64];
	char symbol[64];
	double dblPosition;
	double dblRealPnl;
	double dblAvgCost;
	double dblTotBought;
	double dblTotSold;
	double dblAvgBuyCost;
	double dblAvgSellCost;
	double dblNominalPosition;
	double dblOpenBuyShares;
	double dblOpenSellShares;
	double dblOrderCount;
	double dblMiscFee;
	
	bool GetSodPositionInfo(POSITION_INFO& positionInfo);
	void SetSodPositionInfo(void* pSod){m_pSod = pSod;}

private:
	void* m_pSod;
	
};

class POSITION_INFO_CONTAINER
{
public:

	POSITION_INFO_CONTAINER();
	virtual ~POSITION_INFO_CONTAINER(){};

	bool GetPositionByAccountSymbol(POSITION_INFO& positionInfo,
									const char* tradingAcc,
									const char* sym);

	bool GetFirst(POSITION_INFO& positionInfo);
	bool GetNext(POSITION_INFO& positionInfo);

	bool GetFirstByTradingAccount(POSITION_INFO& positionInfo, const char* tradingAcc);
	bool GetNextByTradingAccount(POSITION_INFO& positionInfo, const char* tradingAcc);

	bool GetFirstBySymbol(POSITION_INFO& positionInfo, const char* sym);
	bool GetNextBySymbol(POSITION_INFO& positionInfo, const char* sym);

private:

	void* m_pImpl;
};

class STREET_EXECUTION_CONTAINER
{
public:

	STREET_EXECUTION_CONTAINER();
	virtual ~STREET_EXECUTION_CONTAINER(){};

	bool GetFirst(STREET_EXEC& streetExec);
	bool GetNext(STREET_EXEC& streetExec);

private:
	void* m_pImpl;
};

/**
 * @brief This is the event type that will be received if a TimedEvent is set via the FtTimedEventStrategy 
 * 
 * 
 */
class EVENT_DATA
{
public:
	EVENT_DATA(const char *eventData = NULL, const int eventDataClass = OMRULESEXPORT::CLASS_NONE,
			const char *eventDataSource = NULL);

	EVENT_DATA(const EVENT_DATA& rhs);

	virtual ~EVENT_DATA();

	const char *getEventData() const;
	const int getEventDataClass() const;
	const char *getEventDataSource() const;

	void setEventData(const char *eventData);
	void setEventDataClass(int eventDataClass);
	void setEventDataSource(const char *eventDataSource);

	static bool equals(const void* lhs, const void* rhs);

	virtual bool operator ==(const EVENT_DATA& rhs);
	virtual EVENT_DATA& operator =(const EVENT_DATA& rhs);

private:
	void clearEventData();
	void clearEventDataSource();

private:
	char *m_eventData;
	int m_eventDataClass;
	char *m_eventDataSource;
};

/**
 * @brief This class used in conjuction with all indication of interest events and functions
 * 
 * 
 */
class IOI
{
public:
	IOI();
	IOI(void* pIOI);
	IOI(const IOI& rhs);
	virtual ~IOI();

	void SetDest(const char* val);
	const char* GetDest() const;

	void SetSndr(const char* val);
	const char* GetSndr() const;

	void SetSndrSubid(const char* val);
	const char* GetSndrSubid() const;

	void SetDestSubid(const char* val);
	const char* GetDestSubid() const;

	void SetSymbol(const char* val);
	const char* GetSymbol() const;

	void SetExchange(const char* val);
	const char* GetExchange() const;

	void SetSecurityId(const char* val);
	const char* GetSecurityId() const;

	void SetSide(enOrderSide val);
	enOrderSide GetSide() const;

	void SetShares(const FT_SIZE_TYPE val);
	FT_SIZE_TYPE GetShares() const;

	void SetId(const char* val);
	const char* GetId() const;

	void SetPrice(const double val);
	double GetPrice() const;

	void SetValidUntil(const char* val);
	const char* GetValidUntil() const;

	void SetInputOutput(const char val);
	char GetInputOutput() const;

	void SetQualityInd(const char val);
	char GetQualityInd() const;

	void SetService(const char* val);
	const char* GetService() const;

	void SetArchSeq(const int val);
	const int GetArchSeq() const;

	void SetComments(const char* val);
	const char* GetComments() const;

	void SetQualifier(const char val);
	char GetQualifier() const;

	void SetNatural(const char val);
	char GetNatural() const;

	void SetRefId(const char* val);
	const char* GetRefId() const;

	bool SendToClient();
	bool ReplaceToClient();
	bool CancelToClient();
	bool Discard();

private:

	void* m_pData;
	bool m_bIsAlloc;
	char* m_szRefId;

	void* GetIOIPtr();
	void fillIOIStruct(void* hdr, const IOI& ioi) const;

};

/**
 * @brief An instance of this class should be created to look for client orders.
 * 
 * 
 */
class CLIENT_ORDER_CONTAINER
{
public:
	CLIENT_ORDER_CONTAINER(const char* szStratName = NULL);
	virtual ~CLIENT_ORDER_CONTAINER();

	bool GetFirstActive(CLIENT_ORDER& clientOrder);
	bool GetNextActive(CLIENT_ORDER& clientOrder);

	bool GetFirstActiveByClient(CLIENT_ORDER& clientOrder, const char *szClient);
	bool GetNextActiveByClient(CLIENT_ORDER& clientOrder);

	bool GetFirstActiveBySymbol(CLIENT_ORDER& clientOrder, const char *szSymbol);
	bool GetNextActiveBySymbol(CLIENT_ORDER& clientOrder);

	static bool GetActiveByClientOrderId(CLIENT_ORDER& order, const char *clientOrderId);
	static bool GetByClientOrderId(CLIENT_ORDER& order, const char *clientOrderId);

private:
	void *m_pImpl;
};

/**
 * @brief An instance of this class should be created to look for street orders.
 * 
 * 
 */
class STREET_ORDER_CONTAINER
{
public:
	STREET_ORDER_CONTAINER(const char* szStratName = NULL);
	virtual ~STREET_ORDER_CONTAINER();

	bool GetFirstActive(STREET_ORDER& streetOrder);
	bool GetNextActive(STREET_ORDER& streetOrder);

	bool GetFirstActiveByDest(STREET_ORDER& streetOrder, const char *szDest);
	bool GetNextActiveByDest(STREET_ORDER& streetOrder);

	bool GetFirstActiveBySymbol(STREET_ORDER& streetOrder, const char *szSymbol);
	bool GetNextActiveBySymbol(STREET_ORDER& streetOrder);

	bool GetFirstActiveByPortfolioSymbol(STREET_ORDER& streetOrder, const char* port, const char* szSymbol);
	bool GetNextActiveByPortfolioSymbol(STREET_ORDER& streetOrder);

	bool GetFirstActiveByClientOrderId(STREET_ORDER& streetOrder, const char *clientOrderId);
	bool GetNextActiveByClientOrderId(STREET_ORDER& streetOrder);

	bool GetByStreetOrderId(STREET_ORDER& streetOrder, const char *streetOrderId);

	static bool GetActiveInChain(STREET_ORDER& order, const char *orderId);

private:
	void *m_pImpl;
};

/**
 * @brief This can be used to see what IOIs have been received and set by the OM.
 * 
 * 
 */
class IOI_CONTAINER
{
public:
	static bool ClientReceivedIoiGetById(const char* ioiId, IOI& ioi);
	static bool ClientSentIoiGetById(const char* ioiId, IOI& ioi);
};

/**
 * @brief An instance of this class should be created to look for client multi-leg orders.
 * 
 * 
 */
class CLIENT_MLEG_ORDER_CONTAINER
{
public:
	CLIENT_MLEG_ORDER_CONTAINER(const char* szStratName = NULL);
	virtual ~CLIENT_MLEG_ORDER_CONTAINER();

	bool GetFirstActive(CLIENT_MLEG_ORDER& clientOrder);
	bool GetNextActive(CLIENT_MLEG_ORDER& clientOrder);

	bool GetFirstActiveByClient(CLIENT_MLEG_ORDER& clientOrder, const char *szClient);
	bool GetNextActiveByClient(CLIENT_MLEG_ORDER& clientOrder);

	bool GetFirstActiveBySymbol(CLIENT_MLEG_ORDER& clientOrder, const char *szSymbol);
	bool GetNextActiveBySymbol(CLIENT_MLEG_ORDER& clientOrder);

	static bool GetActiveByClientOrderId(CLIENT_MLEG_ORDER& order, const char *clientOrderId);

private:
	void *m_pImpl;
};

/**
 * @brief An instance of this class should be created to look for street multi-leg orders.
 * 
 * 
 */
class STREET_MLEG_ORDER_CONTAINER
{
public:
	STREET_MLEG_ORDER_CONTAINER(const char* szStratName = NULL);
	virtual ~STREET_MLEG_ORDER_CONTAINER();

	bool GetFirstActive(STREET_MLEG_ORDER& streetOrder);
	bool GetNextActive(STREET_MLEG_ORDER& streetOrder);

	bool GetFirstActiveByDest(STREET_MLEG_ORDER& streetOrder, const char *szDest);
	bool GetNextActiveByDest(STREET_MLEG_ORDER& streetOrder);

	bool GetFirstActiveBySymbol(STREET_MLEG_ORDER& streetOrder, const char *szSymbol);
	bool GetNextActiveBySymbol(STREET_MLEG_ORDER& streetOrder);

	bool GetFirstActiveByPortfolioSymbol(STREET_MLEG_ORDER& streetOrder, const char* port, const char* szSymbol);
	bool GetNextActiveByPortfolioSymbol(STREET_MLEG_ORDER& streetOrder);

	bool GetFirstActiveByClientOrderId(STREET_MLEG_ORDER& streetOrder, const char *clientOrderId);
	bool GetNextActiveByClientOrderId(STREET_MLEG_ORDER& streetOrder);

	bool GetByStreetOrderId(STREET_MLEG_ORDER& streetOrder, const char *streetOrderId);

	static bool GetActiveInChain(STREET_MLEG_ORDER& order, const char *orderId);

private:
	void *m_pImpl;
};

/**
 * @brief This will return the symbol name given an id from the Security Master Table in the OM.
 * 
 * 
 */
class SECURITY_INFO
{
public:
	SECURITY_INFO(const char* id);
	virtual ~SECURITY_INFO();

	char* GetRegionCode(char* rv) const;
	char* GetCountryCode(char* rv) const;
	char* GetSector(char* rv) const;
	char* GetUnderlyingSymbol(char* rv) const;
	char* GetSecurityType(char* rv) const;
	bool IsRegNMSExempt() const;
	bool IsValidSecurity() { return (m_security != 0); }

private:
	void* m_security;
};

class USER_INFO
{
public:
	USER_INFO(const char* id);
	virtual ~USER_INFO();

	char* GetRRNumber(char* rv) const;

private:
	void* m_user;
};

/**
 * @brief An instance of this class should be created to get orders from order book
 * 
 * 
 */
class ORDER_BOOK_CONTAINER
{
public:
	ORDER_BOOK_CONTAINER(const char* symbol);
	virtual ~ORDER_BOOK_CONTAINER();

	bool GetBestBuySizeAtPrice(FT_SIZE_TYPE& size, double& price);
	bool GetNextBuySizeAtPrice(FT_SIZE_TYPE& size, double& price);

	bool GetBestSellSizeAtPrice(FT_SIZE_TYPE& size, double& price);
	bool GetNextSellSizeAtPrice(FT_SIZE_TYPE& size, double& price);

	bool GetBestBuyOrder(CLIENT_ORDER& clientOrder);
	bool GetNextBuyOrder(CLIENT_ORDER& clientOrder);

	bool GetBestSellOrder(CLIENT_ORDER& clientOrder);
	bool GetNextSellOrder(CLIENT_ORDER& clientOrder);

private:
	void *m_pImpl;
};

class DTAG_PARAMS
{
public:
	DTAG_PARAMS(const char *dest, const char *strat);
	~DTAG_PARAMS();
	bool GetParam(const char *param, string& val);
	bool GetParam(const char *param, double &val);
	bool SetParam(const char *param, const string& val);
	bool SetParam(const char *param, const double val);

	bool GetParamString(string &str);
	bool SetParamString(const string& str);

private:
	void* m_pParams;
};

/**
 * @brief Interface to the in-memory reference database loaded by the OM on startup
 *
 */

class REFERENCE_DATABASE
{
public:
	REFERENCE_DATABASE();
	virtual ~REFERENCE_DATABASE();

	bool GetRow(const enRefDataTable code, const string& key, map<string, string>& row);
};

class MARKET_DATA_CONNECTION
{
public:
	MARKET_DATA_CONNECTION();
	~MARKET_DATA_CONNECTION();

	static bool Subscribe(const char *topic);
	static bool Unsubscribe(const char *topic);

	static bool GetTick(const char *topic, MTICK& tickInfo);

	static void CreatePortSymbolTopic(const char *symbol, const char *portfolio, char *topic);
	static bool GetSymbolFromTopic(const char *topic, char *symbol);
	static bool GetPortFromTopic(const char *topic, char *portfolio);

	static bool GetNBBO(const char *symbol, double& bidPrice, int& bidSize, double& askPrice, int& askSize, double &last, double& close);

	static int GetDepthInfo(const char *symbol, MKT_DEPTH_INFO& mktDepthInfo);
	static int GetDepthTOBInfo(const char *symbol, MKT_DEPTH_INFO& mktDepthInfo, const long int milliSecondWaitTime = 0);

	static int GetDepthSource(const char *sourceName);
	static const char *GetDepthSourceName(char cSource, int *pLen = 0);
};


/** 
 * @brief Get the current depth info for a symbol.
 * 
 */
int GetMarketDepthInfo(const char* sym, MKT_DEPTH_INFO& mktDepthInfo);

/**
 * @brief Get the Top of the Book Level 2 for a symbol
 *
 */
int GetMarketDepthTOBInfo(const char* symbol, MKT_DEPTH_INFO& mktDepthInfo, const long int milliSecondWaitTime = 0);

/**
 * @brief Get the source id for a given market data source name
 *
 */
int GetMarketDepthSource(const char *sourceName);

/**
 * @brief Get the market data source name given the source as a character
 *
 */
const char* GetMarketDepthSourceName(char cSource, int *pLen = 0);

/**
 * @brief Ask the OM to Subscribe to the given symbol
 */
bool MarketDataSubscribeSymbol(const char* szSymbol);

/**
 * @brief Ask the OM to Unsubscribe to the given symbol
 * 
 */
bool MarketDataUnsubscribeSymbol(const char* szSymbol);

/**
 * @brief Get the current top of book and last data for the given symbol
 * 
 */
bool MarketDataGetSymbolInfo(const char* szSymbol, MTICK &tickInfo);

/**
 * @brief Get the NBBO using SMC
 *
 */
bool MarketDataGetNBBO(const char* symbol, double& bidPrice, int& bidSize, double& askPrice, int& askSize, double &last, double& close);

/**
 * @brief Get the OPEN using SMC
 *
 */
double MarketDataGetOpenPrice(const char* symbol);


/**
 * @brief Get the CLOSE using SMC
 *
 */
double MarketDataGetClosePrice(const char* symbol);


/**
 * @brief Is the given user connected to the OM in flextrader?
 * 
 */
bool IsUserUp(const char* szUser);

/**
 * @brief List all connected users
 *
 */
void GetUserList(std::vector<std::string>& users);

/**
 * @brief Is the given street side broker currently connected?
 * 
 */
bool IsDestinationUp(const char* szDest);

/**
 * @brief Is the given client broker currently connected?
 * 
 */
bool IsClientUp(const char* szClient);

/**
 * @brief Is the OM currently in recovery?
 * 
 */
bool IsOMRecovering();

/**
 * @brief Used to sent data to the MsgProcessor thread
 * 
 */
bool SetParamsValue(const char* szHandle, void* pData);

/**
 * @brief Used to retrieve data from the MsgProcessor thread
 * 
 */
void *GetParamsValue(const char* szHandle);

/**
 * @brief This call can be used to send out messages to all OMAPI listeners into the OnErrorMessage callback.
 * 
 */
bool SendErrorMsg(const char* szErrorMsg, bool bSave = true);

/**
 * @brief This can be used to send an order into the OM as if it was received over a client fix connection.
 * 
 */
bool SendClientOrder(ORDER & oinfo);

/**
 * @brief Get all known position information from the system and put it into the given array, upto nArraySize elements. 
 * 
 */
bool GetPositions(POSITION_INFO arrPositionInfo[], int nArraySize, int* pnNumReturned);

/**
 * @brief Get the position information from the system for a given account and symbol only.
 * 
 */
bool GetPositions(POSITION_INFO &positionInfo, const char *szAccount, const char *szSymbol);

/**
 * @brief Get the position information from the system for a given account and symbol only.
 * 
 */
bool GetAggPosition(POSITION_INFO &positionInfo, const char *szAccount, const char *szSymbol);

/**
 * @brief Add a combo mode portfolio into the om.
 */
bool AddComboPortfolio(const char* szName);

/**
 * @brief See if a given portfolio is a combo portfolio in the OMs list.
 */
bool IsComboPortfolioLoaded(const char* szName);

/**
 * @brief Allow a StrategyBase object to call custom methods in StrategyValidatingBase
 */
int CallValidatingCustomMethod(const char *stratName, const short num, void *data);

/**
 * @brief Deprecated
 * @deprecated Use calls inside STREET_ORDER class.
 */
bool SendStreetOrder(STREET_ORDER & oinfo);
bool SendMLegStreetOrder(STREET_MLEG_ORDER & oinfo);

/**
 * @brief Deprecated
 * @deprecated Use calls inside STREET_ORDER class.
 */
bool SendStreetStatusRequest(STREET_ORDER & oinfo);


/**
 * @brief Deprecated
 * @deprecated Use calls inside STREET_ORDER class.
 */
bool CancelStreetOrder(const char * szHandle, const char* const fixtags = NULL);

/**
 *Call to  cancel street order
 *
 */
bool CancelStreetOrder(STREET_ORDER & oinfo);


/**
 * @brief Deprecated
 * @deprecated Use calls inside STREET_ORDER class.
 */
bool ReplaceStreetOrder(STREET_ORDER& rplOrder);

/**
 * @brief Deprecated
 * @deprecated Use calls inside STREET_ORDER class.
 */
bool StreetOrderSendOut(const char * szHandle);

/**
 * @brief Deprecated
 * @deprecated use STREET_ORDER_CONTAINER
 */
bool GetFirstStreetOrder(STREET_ORDER & sinfo);

/**
 * @brief Deprecated
 * @deprecated use STREET_ORDER_CONTAINER
 */
bool GetFirstActiveStreetOrder(STREET_ORDER & sinfo);

/**
 * @brief Deprecated
 * @deprecated use STREET_ORDER_CONTAINER
 */
bool GetNextStreetOrder(STREET_ORDER & sinfo);

/**
 * @brief Deprecated
 * @deprecated use STREET_ORDER_CONTAINER
 */
bool GetNextActiveStreetOrder(STREET_ORDER & sinfo);

/**
 * @brief Deprecated
 * @deprecated use STREET_ORDER_CONTAINER
 */
bool GetFirstStreetOrderByClientOrder(STREET_ORDER & sinfo, const char *szParentHandle);

/**
 * @brief Deprecated
 * @deprecated use STREET_ORDER_CONTAINER
 */
bool GetNextStreetOrderByClientOrder(STREET_ORDER & sinfo, const char *szParentHandle);

/**
 * @brief Deprecated
 * @deprecated use STREET_ORDER_CONTAINER
 */
bool GetFirstStreetOrderBySymbol(STREET_ORDER & sinfo, const char *szSymbol);

/**
 * @brief Deprecated
 * @deprecated use STREET_ORDER_CONTAINER
 */
bool GetNextStreetOrderBySymbol(STREET_ORDER & sinfo, const char *szSymbol);

/**
 * @brief Deprecated
 * @deprecated use STREET_ORDER_CONTAINER
 */
bool GetFirstStreetOrderByDest(STREET_ORDER & sinfo, const char *szDest);

/**
 * @brief Deprecated
 * @deprecated use STREET_ORDER_CONTAINER
 */
bool GetNextStreetOrderByDest(STREET_ORDER & sinfo, const char *szDest);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetFirstClientOrder(CLIENT_ORDER & oinfo);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetFirstActiveClientOrder(CLIENT_ORDER & oinfo);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetNextClientOrder(CLIENT_ORDER & oinfo);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetNextActiveClientOrder(CLIENT_ORDER & oinfo);


/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetFirstClientOrderByClient(CLIENT_ORDER & sinfo, const char *szClient);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetNextClientOrderByClient(CLIENT_ORDER & sinfo, const char *szClient);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetFirstClientOrderBySymbol(CLIENT_ORDER & sinfo, const char *szSymbol);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetNextClientOrderBySymbol(CLIENT_ORDER & sinfo, const char *szSymbol);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
int GetActiveStreetOrdersForClientOrder(const char* clientOrderId, STREET_ORDER**& sinfo);
/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
int GetFutureActiveStreetOrdersForClientOrder(const char* clientOrderId, STREET_ORDER**& sinfo);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER
 */
bool OutClientOrder(const char *szHandle);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER
 */
bool RejectClientOrder(const char *szHandle, const char* szRejectReason);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER
 */
bool AckClientOrder(const char *szHandle);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER
 */
bool SendPendingRpl(const char *szHandle);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER
 */
bool AckClientReplace(const char *szHandle);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER
 */
bool ClientOrderSendOut(const char* szHandle);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER
 */
bool ClientOrderSendAck(const char* szHandle);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER
 */
bool ClientOrderSendPendingReplace(const char* szHandle);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER
 */
bool ClientOrderSendReplaced(const char* szHandle);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER
 */
bool ClientOrderSendReject(const char* szHandle, const char* szRejectReason);

/**
 * @brief Start a timed event.
 */
bool SetTimedEvent(const enEvents eventCode, const ORDER & oinfo, const int howOften,
		const int howManyTimes = 1);

/**
 * @brief Stop a timed event.
 */
bool RemoveTimedEvent(const enEvents eventCode, const ORDER & oinfo);

/**
 * @brief Start a timed event.
 */
bool SetTimedEvent(const enEvents eventCode, const char* oid, const int howOften,
		const int howManyTimes = 1);

/**
 * @brief Stop a timed event.
 */
bool RemoveTimedEvent(const enEvents eventCode, const char* oid);

/**
 * @brief Start a timed event.
 */
bool SetTimedEvent(const enEvents eventCode, const STREET_ORDER & oinfo, const int howOften,
		const int howManyTimes = 1);

/**
 * @brief Stop a timed event.
 */
bool RemoveTimedEvent(const enEvents eventCode, const STREET_ORDER & oinfo);

/**
 * @brief Start a generic timed event, without any event data.
 */
bool SetGenTimedEvent(const enEvents eventCode, const int howOften, const int howManyTimes);

/**
 * @brief Start a timed event.
 */
bool SetTimedEvent(const enEvents eventCode, EVENT_DATA* ed, const int howOften,
		const int howManyTimes = 1);

/**
 * @brief Stop a timed event.
 */
bool RemoveTimedEvent(const enEvents eventCode, EVENT_DATA* ed);

/**
 * @brief Add a manual fill to an order.
 */
bool CreateAndFillManualStreetOrder(STREET_ORDER & oinfo, bool bIsCrossPnL);

/**
 * @brief Add a manual fill to multi-leg order.
 */
bool CreateAndFillManualMLegStreetOrder(STREET_MLEG_ORDER & oinfo, bool bIsCrossPnL);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetFirstClientOrderByClient_All(ORDER & sinfo, const char *szClient);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetNextClientOrderByClient_All(ORDER & sinfo, const char *szClient);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetFirstClientOrderBySymbol_All(CLIENT_ORDER & sinfo, const char *szSymbol);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetNextClientOrderBySymbol_All(CLIENT_ORDER & sinfo, const char *szSymbol);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetClientOrderById(OMRULESEXPORT::CLIENT_ORDER& order, const string& orderId);

/**
 * @brief Deprecated
 * @deprecated use CLIENT_ORDER_CONTAINER
 */
bool GetActiveClientOrderById(OMRULESEXPORT::CLIENT_ORDER& order, const string& orderId);

/**
 * @brief Deprecated
 * @deprecated use STREET_ORDER_CONTAINER
 */
bool GetActiveStreetOrdInChain(const string& orderId, OMRULESEXPORT::STREET_ORDER& order);

/**
 * @brief Get all indication of interest information that has been received and matches the given id.
 */
bool ClientReceivedIoiGetById(const char* ioiId, IOI& ioi);

/**
 * @brief Get all indication of interest information that has been sent out for a given id.
 */
bool ClientSentIoiGetById(const char* ioiId, IOI& ioi);

/**
 * @brief Request an event to be started and associate it with a strategy.
 * @pre Rel-9-2-0
 */
bool SetMsTimer(EVENT_DATA& eventData, int nMilliSeconds, int nRepetitions, const char* szStratName);

/**
 * @brief Request an event that's associated with a strategy to be stopped.
 * @pre Rel-9-2-0
 */
bool RemoveMsTimer(EVENT_DATA& eventData, const char* szStratName);

bool GetMLClientOrderById(CLIENT_MLEG_ORDER& order, const string& orderId);
bool GetMLStreetOrderById(STREET_MLEG_ORDER& order, const string& orderId);
bool GetMLClientOrderByLegId(CLIENT_MLEG_ORDER& order, const string& legOrderId);
bool GetMLStreetOrderByLegId(STREET_MLEG_ORDER& order, const string& legOrderId);

} //namespace OMRULESEXPORT

#endif
#ifndef _CSmcData_h_
#define _CSmcData_h_

#include <iostream>

/**
 * Class represents data that is stored in shared memory for a given symbol. This is largely
 * taken from fields in the sym server.
 *
 * IMPORTANT NOTE!!!! Please do not add any includes into this file as it
 * is used for building rules
 */

class CSmcData
{

public:

	CSmcData();
	void	Clear();
	void operator=(const CSmcData &);
	friend ostream & operator<<(ostream & os, const CSmcData & data);
	bool operator==(const CSmcData &) const;
	
	double	GetBid();
	double	GetAsk();
	double	GetLast();
	double	GetVwap();
	double	GetPrimBid();
	double	GetPrimAsk();
	double	GetPrimLast();
	double	GetPrimVwap();
	void	SetBid(double dblPrice);
	void	SetAsk(double dblPrice);
	void	SetLast(double dblPrice);
	void	SetVwap(double dblPrice);
	void	SetPrimBid(double dblPrice);
	void	SetPrimAsk(double dblPrice);
	void	SetPrimLast(double dblPrice);
	void	SetPrimVwap(double dblPrice);

	double	GetOpen();
	double 	GetClose();
	double	GetHigh();
	double	GetLow();
	double	GetVolume();
	double	GetPrimVolume();
	double	GetPrimOpen();
	double 	GetPrimClose();
	void	SetOpen(double dblPrice);
	void	SetClose(double dblPrice);
	void	SetHigh(double dblPrice);
	void	SetLow(double dblPrice);
	void	SetVolume(double dblVolume);
	void	SetPrimVolume(double dblVolume);
	void	SetPrimOpen(double dblPrice);
	void	SetPrimClose(double dblPrice);
	
	int	GetBidSize();
	int	GetAskSize();
	int	GetTradeSize();
	int	GetPrimBidSize();
	int	GetPrimAskSize();
	int	GetPrimTradeSize();
	int	GetUpdateTime();
	int	GetTradeTime();
	void	SetBidSize(int nSize);
	void	SetAskSize(int nSize);
	void	SetTradeSize(int nSize);
	void	SetPrimBidSize(int nSize);
	void	SetPrimAskSize(int nSize);
	void	SetPrimTradeSize(int nSize);
	void	SetUpdateTime(int nTime);
	void	SetTradeTime(int nTime);
	

	double	GetUserValue(int nIndex, bool* pbIsSet = 0);
	void	SetUserValue(int nIndex, double dblValue);

	const char*	GetUserString(int nIndex, bool* pbIsSet = 0);
	void		SetUserString(int nIndex, const char * szString);
	
	double 		GetDepthBid(int nLevel);
	void		SetDepthBid(int nLevel, double dblPrice);

	int			GetDepthBidSize(int nLevel);
	void		SetDepthBidSize(int nLevel, int nSize);

	const char*	GetDepthBidQuoter(int nLevel);
	void		SetDepthBidQuoter(int nLevel, const char *szQuoter);

	const char*	GetDepthBidValueDate(int nLevel);
	void		SetDepthBidValueDate(int nLevel, const char *szValueDate);

	const char*	GetDepthBidQuoteId(int nLevel);
	void		SetDepthBidQuoteId(int nLevel, const char *szQuoteId);

	bool		GetDepthBidIndicative(int nLevel);
	void		SetDepthBidIndicative(int nLevel, bool bIndicative);
	
	double 		GetDepthAsk(int nLevel);
	void		SetDepthAsk(int nLevel, double dblPrice);

	int			GetDepthAskSize(int nLevel);
	void		SetDepthAskSize(int nLevel, int nSize);

	const char* GetDepthAskQuoter(int nLevel);
	void		SetDepthAskQuoter(int nLevel, const char *szQuoter);

	const char*	GetDepthAskValueDate(int nLevel);
	void		SetDepthAskValueDate(int nLevel, const char *szValueDate);

	const char*	GetDepthAskQuoteId(int nLevel);
	void		SetDepthAskQuoteId(int nLevel, const char *szQuoteId);

	bool		GetDepthAskIndicative(int nLevel);
	void		SetDepthAskIndicative(int nLevel, bool bIndicative);


	int			GetDepthBidMinSize(int nLevel);
	void		SetDepthBidMinSize(int nLevel, int nSize);

	double		GetDepthBidForwardPoints(int nLevel);
	void		SetDepthBidForwardPoints(int nLevel, double dblPoints);
	
	int			GetDepthAskMinSize(int nLevel);
	void		SetDepthAskMinSize(int nLevel, int nSize);

	double		GetDepthAskForwardPoints(int nLevel);
	void		SetDepthAskForwardPoints(int nLevel, double dblPoints);
	
	int			GetMaxDepthLevel() { return 2 * CONST_OCTET; };

	bool 		SwapDepthBidLevels(int nLevelA, int nLevelB);
	
	bool 		SwapDepthAskLevels(int nLevelA, int nLevelB);

	const char* GetTradingStatus();
	char		GetSHOIndicator();
	void		SetTradingStatus(const char* szTradingStatus);
	void		SetSHOIndicator(char cSHOIndicator);

	void 		SetDepthBidNearAllIn(int nLevel, double value);
	void 		SetDepthBidFarAllIn(int nLevel, double value);
	void 		SetDepthBidFarPoints(int nLevel, double value);
	void 		SetDepthBidFarSize(int nLevel, int value);
	void 		SetDepthAskNearAllIn(int nLevel, double value);
	void 		SetDepthAskFarAllIn(int nLevel, double value);
	void 		SetDepthAskFarPoints(int nLevel, double value);
	void 		SetDepthAskFarSize(int nLevel, int value);

	void		SetDepthBidReqID(int nLevel, int value);
	void		SetDepthAskReqID(int nLevel, int value);

	double		GetDepthBidNearAllIn(int nLevel);
	double		GetDepthBidFarAllIn(int nLevel);
	double		GetDepthBidFarPoints(int nLevel);
	int			GetDepthBidFarSize(int nLevel);
	double		GetDepthAskNearAllIn(int nLevel);
	double		GetDepthAskFarAllIn(int nLevel);
	double		GetDepthAskFarPoints(int nLevel);
	int			GetDepthAskFarSize(int nLevel);

	int			GetDepthBidReqID(int nLevel);
	int			GetDepthAskReqID(int nLevel);

protected:

	static const int CONST_QUOTER 	= 8;
	static const int CONST_DATE 	= 12;
	static const int CONST_ID 	= 48;
	static const int CONST_OCTET	= 8;
	static const int CONST_STRING 	= 64;
		
	char	m_cBitMask[58];

	//octet 0
	double	m_dblBid;
	double	m_dblAsk;
	double	m_dblLast;
	double	m_dblVwap;
	double	m_dblPrimBid;
	double	m_dblPrimAsk;
	double	m_dblPrimLast;
	double	m_dblPrimVwap;

	//octet 1
	double	m_dblOpen;
	double 	m_dblClose;
	double	m_dblHigh;
	double	m_dblLow;
	double	m_dblVolume;
	double	m_dblPrimVolume;
	double	m_dblPrimOpen;
	double 	m_dblPrimClose;

	//octet 2
	int	m_nBidSize;
	int	m_nAskSize;
	int	m_nTradeSize;
	int	m_nPrimBidSize;
	int	m_nPrimAskSize;
	int	m_nPrimTradeSize;
	int	m_nUpdateTime;
	int	m_nTradeTime;
	
	//octet 3
	double	m_dblUser[CONST_OCTET];
	//octet 4
	char	m_szUser[CONST_OCTET][CONST_STRING];
	
	//octet 5-6
	double 	m_dblDepthBid[2*CONST_OCTET];
	//octet 7-8
	int	m_nDepthBidSize[2*CONST_OCTET];
	//octet 9-10
	char	m_szDepthBidQuoter[2*CONST_OCTET][CONST_QUOTER];
	//octet 11-12
	char	m_szDepthBidValueDate[2*CONST_OCTET][CONST_DATE];
	//octet 13-14
	char	m_szDepthBidQuoteId[2*CONST_OCTET][CONST_ID];
	//octet 15-16
	char	m_cDepthBidIndicative[2*CONST_OCTET];
	
	//octet 17-18
	double 	m_dblDepthAsk[2*CONST_OCTET];
	//octet 19-20
	int	m_nDepthAskSize[2*CONST_OCTET];
	//octet 21-22
	char	m_szDepthAskQuoter[2*CONST_OCTET][CONST_QUOTER];
	//octet 23-24
	char	m_szDepthAskValueDate[2*CONST_OCTET][CONST_DATE];
	//octet 25-26
	char	m_szDepthAskQuoteId[2*CONST_OCTET][CONST_ID];
	//octet 27-28
	char	m_cDepthAskIndicative[2*CONST_OCTET];

	//octet 29-30
	int	m_nDepthBidMinSize[2*CONST_OCTET];
	//octet 31-32
	double	m_dblDepthBidForwardPoints[2*CONST_OCTET];
	
	//octet 33-34
	int	m_nDepthAskMinSize[2*CONST_OCTET];
	//octet 35-36
	double	m_dblDepthAskForwardPoints[2*CONST_OCTET];

	//octet 37
	char	m_szTradeStatus[CONST_STRING];
	char	m_cSHOIndicator;
	
	// octet 38-39
	double 	m_dblDepthBidNearAllIn[2*CONST_OCTET];
	// octet 40-41
	double 	m_dblDepthBidFarAllIn[2*CONST_OCTET];
	// octet 42-43
	double 	m_dblDepthBidFarPoints[2*CONST_OCTET];
	// octet 44-45
	int 	m_nDepthBidFarSize[2*CONST_OCTET];
	
	// octet 46-47
	double 	m_dblDepthAskNearAllIn[2*CONST_OCTET];
	// octet 48-49
	double 	m_dblDepthAskFarAllIn[2*CONST_OCTET];
	// octet 50-51
	double 	m_dblDepthAskFarPoints[2*CONST_OCTET];
	// octet 52-53
	int 	m_nDepthAskFarSize[2*CONST_OCTET];

	// octet 54-55
	int		m_nDepthBidReqID[2*CONST_OCTET];
	// octet 56-57
	int 	m_nDepthAskReqID[2*CONST_OCTET];
};

/**
 * Global function lets a reader read against multiple smc servers by simply using a different key
 * @param szTopic topic/symbol who's data we're querying
 * @param data CSmcData object in which we will retain data
 * @param nKey shared memory/semaphore key that we will query against. By default this is 1000 and
 * is the same as the command line parameter passed to smcd (in start_smc.csh)
 * @return < 0 in case shared memory could not be initialized, 0 if it could but we couldn't find the symbol, 1 if we found the symbol
 */
int GetTopicData(const char * szTopic, CSmcData & data, int nKey);

#endif
#ifndef _EXEC_INSTR_H_
#define _EXEC_INSTR_H_

/*
** Bit masks for exec instruction
*/
#define EXEC_INSTR_NOT_HELD		0x00000001		// '1'  	
#define EXEC_INSTR_WORK			0x00000002		// '2' 	
#define EXEC_INSTR_GO_ALONG		0x00000004		// '3' 	
#define EXEC_INSTR_OVER_THE_DAY		0x00000008		// '4' 	
#define EXEC_INSTR_HELD			0x00000010		// '5' 	
#define EXEC_INSTR_PARTICIPATE		0x00000020		// '6' 	
#define EXEC_INSTR_STRICT_SCALE	 	0x00000040		// '7' 	
#define EXEC_INSTR_TRY_TO_SCALE	 	0x00000080		// '8' 	
#define EXEC_INSTR_STAY_ON_BIDSIDE 	0x00000100		// '9' 	
#define EXEC_INSTR_STAY_ON_OFFERSIDE 	0x00000200		// '0' 	
#define EXEC_INSTR_NO_CROSS    		0x00000400		// 'A' 	
#define EXEC_INSTR_OK_TO_CROSS	   	0x00000800	   	// 'B' 	
#define EXEC_INSTR_Call_first	   	0x00001000	   	// 'C' 	
#define EXEC_INSTR_PERCENT_OF_VOLUME	0x00002000		// 'D' 	
#define EXEC_INSTR_DNI		   	0x00004000		// 'E' 	
#define EXEC_INSTR_DNR			0x00008000		// 'F' 	
#define EXEC_INSTR_AON			0x00010000		// 'G' 	
#define EXEC_INSTR_INSTITUTIONS_ONLY	0x00020000		// 'I' 	
#define EXEC_INSTR_LAST_PEG	   	0x00040000		// 'L' 	
#define EXEC_INSTR_MID_PRICE_PEG	0x00080000		// 'M' 	
#define EXEC_INSTR_NON_NEGOTIABLE	0x00100000		// 'N' 	
#define EXEC_INSTR_OPENING_PEG		0x00200000		// 'O' 	
#define EXEC_INSTR_MARKET_PEG		0x00400000		// 'P' 	
#define EXEC_INSTR_PRIMARY_PEG		0x00800000		// 'R' 	
#define EXEC_INSTR_SUSPEND 		    0x01000000		// 'S' 	
#define EXEC_INSTR_CDI	   		    0x02000000		// 'U' 	
#define EXEC_INSTR_NETTING		    0x04000000		// 'V' 
#define EXEC_INSTR_PEG_TO_VWAP	    0x08000000		// 'W' 
#define EXEC_INSTR_TRADE_ALONG	    0x10000000		// 'X' 
#define EXEC_INSTR_ISO		        0x20000000		// 'f' 

#endif //_EXEC_INSTR_H_
#if (!defined PLATFORM_FORTE10 && !defined PLATFORM_FORTE) 
#ifndef CXMLCONFIG_H_
#define CXMLCONFIG_H_

#include <string>


using std::basic_string;

/***
 * Wrapper class defining a dom.
 * 
 ***/
class CXMLDomNode
{
	// ensure objects can only be created by the Loader.
	friend class CXMLConfigLoader;
	
	public :
	
						/**
						 * Return the first child in this node.
						 * 
						 */
		CXMLDomNode		getFirstChild();
		
						/**
						 * Return the next sibling from this node.
						 * 
						 */
		CXMLDomNode		getNextSibling();
		
						/**
						 * Get the name of this node
						 * 
						 */
		bool			getNodeName(char* szOutStr, int nMaxLength);
		
						/**
						 * Get the text of this node
						 * 
						 */
		bool		 	getTextContent(char* szOutStr, int nMaxLength);
		
						/**
						 * Get the type of node that this is
						 * 
						 */
		short			getNodeType();
				
						/**
						 * Get the value of a name attribute of this node.
						 * 
						 */
		bool		 	getAttribute(const char* szName, char* szOutStr, int nMaxLength);
		
						/**
						 * Is this a valid node object?
						 * 
						 */
		bool			isValid();
		
	private : 
		

						/**
						 * Default constructor
						 * 
						 */
						CXMLDomNode();
		
						/**
						 * Construct from a DOMNode pointer
						 * 
						 */
						CXMLDomNode(void* DomNode);
		
		void* 			m_pNode;
	
};


/****
 * Class to load a dom from an XML file.
 * 
 ****/
class CXMLConfigLoader
{
public:
					CXMLConfigLoader();
					
					
	virtual 		~CXMLConfigLoader();
	
					/**
					 * Setup the values used to load in XML data.
					 * 
					 */
	void			InitialiseSettings(const char* szSettingsPath, const char* szSettingsName);
	
					/**
					 * Get the current path set to load in XML data.
					 * 
					 */
	const char* 	GetSettingsPath();
	
					/**
					 * See if there is new data to load, if the file has changed
					 * then OnUpdateSettings will be called with the root node.
					 * 
					 */
	void 			PollSettingsNow();
	
					/**
					 * Utility function to check that a sting is a valid double value
					 * 
					 */
	static bool 	ValidateDouble(const char* szDblString);
	
					/**
					 * Utility function to check that a sting is a valid integer value
					 * 
					 */
	static bool 	ValidateInt(const char* szIntString);
	
protected:
	
					/**
					 * Given the data in a file, create the XML structure to pass back
					 * 
					 */
	bool 			LoadSettings(const char* szData);
	
	virtual void 	OnUpdateSettings(CXMLDomNode* pCurNode) = 0;
	
	std::string 		m_sSettingsPath;
	std::string			m_sSettingsName;
	
	int 				m_nLastPollTime;
	char* 				m_pTempConfigBuf;
	char* 				m_pConfigBuf;
	int				 	m_nConfigBufSize;
	int 				m_nTempConfigBufSize;

};

#endif /*CXMLCONFIG_H_*/
#endif
#ifndef _StrategyBase_h_
#define _StrategyBase_h_
#include <fstream>
#include <vector>

using namespace OMRULESEXPORT;

#define ERROR_LOG (m_nLogLevel >= LOG_ERROR)
#define DEBUG_LOG (m_nLogLevel >= LOG_DEBUG)
#define VERBOSE_LOG (m_nLogLevel >= LOG_VERBOSE)

class StratletBase;
struct FLID_MAP;

class StrategyBase
{
public:
	static const size_t MAX_STRAT_NAME_LEN_ = 8;
	static const int SUCCESS;
	static const int FAILURE;
	std::ofstream m_Log;

	enum eLogLevel
	{
		LOG_ERROR = 0,
		LOG_DEBUG,
		LOG_VERBOSE
	};

	StrategyBase(const char* szStratName, short nLogLevel = 0);

	virtual ~StrategyBase();

	const char* GetStratName();

	bool IsSignalEnabled() const;
	void SetSignalEnabled(bool bSignalEnabled);

	/////////////////////////////////////////////////////////////////////////
	// Timers
	// different ways of using the SetTimer
	//	EVENT_DATA ed;
	//	SetTimer(ed, 5); // Set a timer with no data repeating every 5 seconds indefinitely
	//	RetTimer(ed); // Remove a timer that was set with data ed
	//
	//	EVENT_DATA ed2("QWERTY");
	//	SetTimer(ed2, 10); // Set a timer that returns with this data repeating every 10 seconds indefinitely
	//
	//	EVENT_DATA ed3("JA0001", CLASS_CLIENT_ORDER);
	//	SetTimer(ed3, 10);// Set a timer that returns with this data repeating every 10 seconds indefinitely
	//
	//	EVENT_DATA ed4("FOOBAR", MY_OWN_INT);
	//	SetTimer(ed4, 10);// Set a timer that returns with this data repeating every 10 seconds indefinitely

	bool SetTimer(EVENT_DATA& eventData, int nSeconds, int nRepetitions = -1);
	bool SetTimerInMilliSecond(EVENT_DATA& eventData, int nMilliSeconds, int nRepetitions = -1);
	bool RemoveTimer(EVENT_DATA& eventData);

	/////////////////////////////////////////////////////////////////////////
	// Market data
	virtual int OnMarketData(CLIENT_ORDER& ClientOrder, MTICK& MTick) = 0;
	virtual int OnMarketData(MTICK& MTick);
	virtual int OnLoad(const char* szConfigFile = NULL);
	virtual int OnMarketDataUp();
	virtual int OnMarketDataDown();

	//////////////////////////////////////////////////////////////////////
	// Connection call backs
	virtual int OnClientConnect(const char* szDest);
	virtual int OnClientDisconnect(const char* szDest);

	virtual int OnStreetConnect(const char* szDest);
	virtual int OnStreetDisconnect(const char* szDest);

	//////////////////////////////////////////////////////////////////////
	// OM Recovery
	virtual int OnEndOfRecovery();

	//////////////////////////////////////////////////////////////////////
	// Config/commands
	virtual int OnConfigUpdate(const char* szConfigBuf);
	virtual int OnClientCommand(STRAT_COMMAND &pCommand);

	/////////////////////////////////////////////////////////////////////////
	// Street Orders
	virtual int OnStreetOrdNew(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder) = 0;
	virtual int OnStreetOrdNew(STREET_ORDER& StreetOrd);
	virtual int OnStreetOrdRpl(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder) = 0;
	virtual int OnStreetOrdRpl(STREET_ORDER& StreetOrd);
	virtual int OnStreetOrdCancel(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder) = 0;
	virtual int OnStreetOrdCancel(STREET_ORDER& StreetOrd);

	virtual int OnStreetDiscardOrdNew(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	virtual int OnStreetDiscardOrdRpl(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	virtual int OnStreetDiscardOrdCancel(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	virtual int OnStreetExecPendingRpl(STREET_ORDER& StreetOrd);
	virtual int OnStreetOrdAck(STREET_ORDER& StreetOrd) = 0;
	virtual int OnStreetOrdRej(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder) = 0;
	virtual int OnStreetOrdRej(STREET_ORDER& StreetOrd);
	virtual int OnStreetOrdOut(STREET_ORDER& StreetOrd) = 0;
	virtual int OnStreetExec(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder) = 0;
	virtual int OnStreetExec(STREET_EXEC& StreetExec);
	virtual int OnStreetStatusReport(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder) = 0;
	virtual int OnStreetNewReportStat(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder);

	virtual int OnStreetFakeExec(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrde);
	virtual int OnStreetFakeExec(STREET_EXEC& StreetExec);
	virtual int OnStreetBustExec(STREET_BUST_EXEC& StreetExec);

	virtual int OnStreetOrdValidNew(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder) = 0;
	virtual int OnStreetOrdValidNew(STREET_ORDER& StreetOrd);
	virtual int OnStreetOrdValidRpl(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder) = 0;
	virtual int OnStreetOrdValidRpl(STREET_ORDER& StreetOrd);
	virtual int OnStreetOrdValidCancel(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder) = 0;
	virtual int OnStreetOrdValidCancel(STREET_ORDER& StreetOrd);

	/////////////////////////////////////////////////////////////////////////
	// Client Orders
	virtual int OnClientOrdNew(CLIENT_ORDER& ClientOrd) = 0;
	virtual int OnClientOrdRpl(CLIENT_ORDER& ClientOrd) = 0;
	virtual int OnClientOrdCancel(CLIENT_ORDER& ClientOrd) = 0;
	virtual int OnClientSendAck(CLIENT_EXEC& ClientExec) = 0;
	virtual int OnClientSendOut(CLIENT_EXEC& ClientOrd) = 0;
	virtual int OnClientSendReject(CLIENT_EXEC& ClientExec) = 0;
	virtual int OnClientSendFills(CLIENT_EXEC& ClientExec) = 0;
	virtual int OnClientOrdValidNew(CLIENT_ORDER& ClientOrd) = 0;
	virtual int OnClientOrdValidRpl(CLIENT_ORDER& ClientOrd) = 0;
	virtual int OnClientOrdValidCancel(CLIENT_ORDER& ClientOrd) = 0;
	virtual int OnClientSendPendingRpl(CLIENT_EXEC& ClientOrd)=0;

	////////////////////////////////////////////////////////////////////////
	// Timer callback
	virtual int OnTimer(EVENT_DATA& eventData) = 0;

	////////////////////////////////////////////////////////////////////////
	// IOIs
	virtual int OnClientIoiNew(OMRULESEXPORT::IOI& ioi);
	virtual int OnClientIoiCxl(OMRULESEXPORT::IOI& ioi);
	virtual int OnIoiNewToClient(OMRULESEXPORT::IOI& ioi);
	virtual int OnIoiCxlToClient(OMRULESEXPORT::IOI& ioi);

	////////////////////////////////////////////////////////////////////////
	// Calls for adding street order data into sym proxy so that the app can see it in Level 2 data screen
	bool InsertSymEntry(const STREET_ORDER& streetOrder);
	bool DeleteSymEntry(const STREET_ORDER& streetOrder);
	bool DeleteSymEntry(STREET_EXEC& streetExec);
	bool UpdateSymEntry(const STREET_ORDER& streetOrder);
	bool UpdateSymEntry(STREET_EXEC& streetExec);

	bool InsertSymEntry(std::string symbol, std::string orderId, double dblPrice, FT_SIZE_TYPE nQuantity,
			OMRULESEXPORT::enOrderSide side, std::string destination);
	bool DeleteSymEntry(std::string symbol, std::string orderId, bool bIsExec = false);
	bool UpdateSymEntry(std::string symbol, std::string orderId, double dblPrice, FT_SIZE_TYPE nQuantity,
			std::string destination, bool bIsExec = false);

	static bool UpdateOrderStats(const char* szTopic, FLID_MAP * hFLIDMap);

	/////////////////////////////////////////////////////////////////////////
	// Client MultiLeg Orders
	virtual int OnClientNewMLegOrd(CLIENT_MLEG_ORDER& ClientOrd);
	virtual int OnClientRplMLegOrd(CLIENT_MLEG_ORDER& ClientOrd);
	virtual int OnClientCancelMLegOrd(CLIENT_MLEG_ORDER& ClientOrd);
	virtual int OnClientValidNewMLegOrd(CLIENT_MLEG_ORDER& ClientOrd);
	virtual int OnClientValidRplMLegOrd(CLIENT_MLEG_ORDER& ClientOrd);
	virtual int OnClientValidCancelMLegOrd(CLIENT_MLEG_ORDER& ClientOrd);

    /////////////////////////////////////////////////////////////////////////
    // Street MultiLeg Orders
	virtual int OnStreetNewMLegOrd(STREET_MLEG_ORDER& StreetOrd, CLIENT_MLEG_ORDER& ClientOrd);
	virtual int OnStreetRplMLegOrd(STREET_MLEG_ORDER& StreetOrd, CLIENT_MLEG_ORDER& ClientOrd);
	virtual int OnStreetCancelMLegOrd(STREET_MLEG_ORDER& StreetOrd, CLIENT_MLEG_ORDER& ClientOrd);
	virtual int OnStreetValidNewMLegOrd(STREET_MLEG_ORDER& StreetOrd, CLIENT_MLEG_ORDER& ClientOrd);
	virtual int OnStreetValidRplMLegOrd(STREET_MLEG_ORDER& StreetOrd, CLIENT_MLEG_ORDER& ClientOrd);
	virtual int OnStreetValidCancelMLegOrd(STREET_MLEG_ORDER& StreetOrd, CLIENT_MLEG_ORDER& ClientOrd);

	friend class StrategyMaster;
	friend class StrategyValidatingBase;
	friend class StratletBase;

private:
	int OnMarketDataPre(CLIENT_ORDER& ClientOrder, MTICK& MTick);
	int OnMarketDataPre(MTICK& MTick);
	int OnLoadPre(const char* szConfigFile = NULL);
	int OnMarketDataUpPre();
	int OnMarketDataDownPre();

	// Connection call backs
	int OnClientConnectPre(const char* szDest);
	int OnClientDisconnectPre(const char* szDest);

	int OnStreetConnectPre(const char* szDest);
	int OnStreetDisconnectPre(const char* szDest);

	// OM Recovery
	int OnEndOfRecoveryPre();

	// Config/commands
	int OnConfigUpdatePre(const char* szConfigBuf);
	int OnClientCommandPre(STRAT_COMMAND &pCommand);

	// Street Orders
	int OnStreetOrdNewPre(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdNewPre(STREET_ORDER& StreetOrd);
	int OnStreetOrdRplPre(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdRplPre(STREET_ORDER& StreetOrd);
	int OnStreetOrdCancelPre(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdCancelPre(STREET_ORDER& StreetOrd);

	int OnStreetDiscardOrdNewPre(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetDiscardOrdRplPre(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetDiscardOrdCancelPre(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);


	int OnStreetOrdAckPre(STREET_ORDER& StreetOrd);
	int OnStreetOrdRejPre(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdRejPre(STREET_ORDER& StreetOrd);
	int OnStreetOrdOutPre(STREET_ORDER& StreetOrd);
	int OnStreetExecPre(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder);
	int OnStreetExecPre(STREET_EXEC& StreetExec);
	int OnStreetStatusReportPre(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder);
	int OnStreetNewReportStatPre(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder);

	int OnStreetFakeExecPre(STREET_EXEC& StreetExec, CLIENT_ORDER& ParentOrder);
	int OnStreetFakeExecPre(STREET_EXEC& StreetExec);
	int OnStreetBustExecPre(STREET_BUST_EXEC& StreetExec);

	int OnStreetOrdValidNewPre(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdValidNewPre(STREET_ORDER& StreetOrd);
	int OnStreetOrdValidRplPre(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdValidRplPre(STREET_ORDER& StreetOrd);
	int OnStreetOrdValidCancelPre(STREET_ORDER& StreetOrd, CLIENT_ORDER& ParentOrder);
	int OnStreetOrdValidCancelPre(STREET_ORDER& StreetOrd);

	// Client Orders
	int OnClientOrdNewPre(CLIENT_ORDER& ClientOrd);
	int OnClientOrdRplPre(CLIENT_ORDER& ClientOrd);
	int OnClientOrdCancelPre(CLIENT_ORDER& ClientOrd);
	int OnClientSendAckPre(CLIENT_EXEC& ClientExec);
	int OnClientSendOutPre(CLIENT_EXEC& ClientOrd);
	int OnClientSendRejectPre(CLIENT_EXEC& ClientExec);
	int OnClientSendFillsPre(CLIENT_EXEC& ClientExec);
	int OnClientOrdValidNewPre(CLIENT_ORDER& ClientOrd);
	int OnClientOrdValidRplPre(CLIENT_ORDER& ClientOrd);
	int OnClientOrdValidCancelPre(CLIENT_ORDER& ClientOrd);

	// Timer callback
	int OnTimerPre(EVENT_DATA& eventData);

	// IOIs
	int OnClientIoiNewPre(OMRULESEXPORT::IOI& ioi);
	int OnClientIoiCxlPre(OMRULESEXPORT::IOI& ioi);
	int OnIoiNewToClientPre(OMRULESEXPORT::IOI& ioi);
	int OnIoiCxlToClientPre(OMRULESEXPORT::IOI& ioi);

	typedef std::vector<StratletBase*> STRATLET_LIST;
	typedef std::vector<STRATLET_LIST> EVENT_STRATLET_MAP;		// Notes it is not a map but a vector but MAP is more appropriate name for such a data structure

	EVENT_STRATLET_MAP m_eventStratletMap;
	bool m_bSuspend;

protected:

	short m_nLogLevel;
	bool m_bSysTimerEnabled;
	bool m_bSignalEnabled;
	char m_sStratName[MAX_STRAT_NAME_LEN_ + 1];

	static char *m_LogFilePath;
	static char *m_LogFileName;
};

std::ostream& operator <<(std::ostream& os, STREET_ORDER& s);
std::ostream& operator <<(std::ostream& os, CLIENT_ORDER& s);
#endif//_StrategyBase_h_
#ifndef _StratletBase_h_
#define _StratletBase_h_
#include <string>

const int MAX_EVENTS = 128;	// TODO: Can be configurable -SK 07/06/2010

/************************************************************************
   The actual interface for all Stratlets, ideally it has to have
   all StrategyBase event callbacks but may have more than that.
   Derived (implemented) stratlet are supposed to call registerEvent
   in order to push itself to the list of stratlet per event per strategy.
   Also for strategies needing more control can disable the automatic 
   execution, and only call when needed.
************************************************************************/
class StrategyBase;

class StratletBase
{
public:
	StratletBase(std::string stratletName, StrategyBase* parentStrategy, bool autoExec);
	virtual ~StratletBase() = 0;

	/////////////////////////////////////////////////////////////////////////
	// Market data
	virtual int OnMarketData(OMRULESEXPORT::CLIENT_ORDER& ClientOrder, OMRULESEXPORT::MTICK& MTick);
	virtual int OnMarketData(OMRULESEXPORT::MTICK& MTick);
	virtual int OnLoad(const char* szConfigFile = NULL);
	virtual int OnMarketDataUp();
	virtual int OnMarketDataDown();

	//////////////////////////////////////////////////////////////////////
	// Connection call backs
	virtual int OnClientConnect(const char* szDest);
	virtual int OnClientDisconnect(const char* szDest);

	virtual int OnStreetConnect(const char* szDest);
	virtual int OnStreetDisconnect(const char* szDest);

	//////////////////////////////////////////////////////////////////////
	// OM Recovery
	virtual int OnEndOfRecovery(void*);

	//////////////////////////////////////////////////////////////////////
	// Config/commands
	virtual int OnConfigUpdate(const char* szConfigBuf);
	virtual int OnClientCommand(OMRULESEXPORT::STRAT_COMMAND &pCommand);

	/////////////////////////////////////////////////////////////////////////
	// Street Orders
	virtual int OnStreetOrdNew(OMRULESEXPORT::STREET_ORDER& StreetOrd, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);
	virtual int OnStreetOrdNew(OMRULESEXPORT::STREET_ORDER& StreetOrd);
	virtual int OnStreetOrdRpl(OMRULESEXPORT::STREET_ORDER& StreetOrd, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);
	virtual int OnStreetOrdRpl(OMRULESEXPORT::STREET_ORDER& StreetOrd);
	virtual int OnStreetOrdCancel(OMRULESEXPORT::STREET_ORDER& StreetOrd, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);
	virtual int OnStreetOrdCancel(OMRULESEXPORT::STREET_ORDER& StreetOrd);

	virtual int OnStreetOrdRej(OMRULESEXPORT::STREET_ORDER& StreetOrd, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);
	virtual int OnStreetOrdRej(OMRULESEXPORT::STREET_ORDER& StreetOrd);
	virtual int OnStreetOrdOut(OMRULESEXPORT::STREET_ORDER& StreetOrd);
	virtual int OnStreetExec(OMRULESEXPORT::STREET_EXEC& StreetExec, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);
	virtual int OnStreetExec(OMRULESEXPORT::STREET_EXEC& StreetExec);

	virtual int OnStreetFakeExec(OMRULESEXPORT::STREET_EXEC& StreetExec, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);
	virtual int OnStreetFakeExec(OMRULESEXPORT::STREET_EXEC& StreetExec);
	virtual int OnStreetBustExec(OMRULESEXPORT::STREET_BUST_EXEC& StreetExec);


	virtual int OnStreetOrdAck(OMRULESEXPORT::STREET_ORDER& StreetOrd);
	virtual int OnStreetStatusReport(OMRULESEXPORT::STREET_EXEC& StreetExec, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);
	virtual int OnStreetNewReportStat(OMRULESEXPORT::STREET_EXEC& StreetExec, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);

	virtual int OnStreetDiscardOrdNew(OMRULESEXPORT::STREET_ORDER& StreetOrd, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);
	virtual int OnStreetDiscardOrdRpl(OMRULESEXPORT::STREET_ORDER& StreetOrd, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);
	virtual int OnStreetDiscardOrdCancel(OMRULESEXPORT::STREET_ORDER& StreetOrd, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);

	virtual int OnStreetOrdValidNew(OMRULESEXPORT::STREET_ORDER& StreetOrd, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);
	virtual int OnStreetOrdValidNew(OMRULESEXPORT::STREET_ORDER& StreetOrd);
	virtual int OnStreetOrdValidRpl(OMRULESEXPORT::STREET_ORDER& StreetOrd, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);
	virtual int OnStreetOrdValidRpl(OMRULESEXPORT::STREET_ORDER& StreetOrd);
	virtual int OnStreetOrdValidCancel(OMRULESEXPORT::STREET_ORDER& StreetOrd, OMRULESEXPORT::CLIENT_ORDER& ParentOrder);
	virtual int OnStreetOrdValidCancel(OMRULESEXPORT::STREET_ORDER& StreetOrd);

	/////////////////////////////////////////////////////////////////////////
	// Client Orders
	virtual int OnClientOrdNew(OMRULESEXPORT::CLIENT_ORDER& ClientOrd);
	virtual int OnClientOrdRpl(OMRULESEXPORT::CLIENT_ORDER& ClientOrd);
	virtual int OnClientOrdCancel(OMRULESEXPORT::CLIENT_ORDER& ClientOrd);
	virtual int OnClientSendAck(OMRULESEXPORT::CLIENT_EXEC& ClientExec);
	virtual int OnClientSendOut(OMRULESEXPORT::CLIENT_EXEC& ClientOrd);
	virtual int OnClientSendReject(OMRULESEXPORT::CLIENT_EXEC& ClientExec);
	virtual int OnClientSendFills(OMRULESEXPORT::CLIENT_EXEC& ClientExec);
	virtual int OnClientOrdValidNew(OMRULESEXPORT::CLIENT_ORDER& ClientOrd);
	virtual int OnClientOrdValidRpl(OMRULESEXPORT::CLIENT_ORDER& ClientOrd);
	virtual int OnClientOrdValidCancel(OMRULESEXPORT::CLIENT_ORDER& ClientOrd);

	////////////////////////////////////////////////////////////////////////
	// Timer callback
	virtual int OnTimer(OMRULESEXPORT::EVENT_DATA& eventData);

	////////////////////////////////////////////////////////////////////////
	// IOIs
	virtual int OnClientIoiNew(OMRULESEXPORT::IOI& ioi);
	virtual int OnClientIoiCxl(OMRULESEXPORT::IOI& ioi);
	virtual int OnIoiNewToClient(OMRULESEXPORT::IOI& ioi);
	virtual int OnIoiCxlToClient(OMRULESEXPORT::IOI& ioi);

	bool autoExec() { return m_autoExec; }

protected:
	void registerEvent(int eventId);
	void unregisterEvent(int eventId);

	std::string m_stratletName;
	StrategyBase* m_parentStrategy;
	bool m_autoExec;
};

#endif//_StratletBase_h_
#ifndef STRATEGYVALIDATINGBASE_H_
#define STRATEGYVALIDATINGBASE_H_

#include <fstream>

using namespace OMRULESEXPORT;

class StrategyValidatingBase
{
public:
	StrategyValidatingBase();
	virtual ~StrategyValidatingBase();

	virtual bool OnStreetOrdValidNew(STREET_ORDER& streetOrd, CLIENT_ORDER& parentOrder) = 0;
	virtual bool OnStreetOrdValidRpl(STREET_ORDER& streetOrd, CLIENT_ORDER& parentOrder) = 0;
	virtual bool OnStreetOrdValidCancel(STREET_ORDER& streetOrd, CLIENT_ORDER& parentOrder) = 0;

	virtual bool OnStreetOrdRej(STREET_ORDER& streetOrd, CLIENT_ORDER& parentOrder) = 0;
	virtual bool OnStreetOrdOut(STREET_ORDER& streetOrd) = 0;
	virtual bool OnStreetExec(STREET_EXEC& streetExec, CLIENT_ORDER& parentOrder) = 0;
	virtual bool OnStreetFakeExec(STREET_EXEC& streetExec, CLIENT_ORDER& parentOrder) { return true; }
	virtual bool OnStreetBustExec(STREET_BUST_EXEC& streetExec) { return true; }

	virtual bool OnCustom1(void *customObject, const char *stratName);
	friend class StrategyMaster;
  protected:
	std::ofstream m_Log;
};

typedef void *(*_GetStrategyValidatingObject)();

#endif /* STRATEGYVALIDATINGBASE_H_ */
#ifndef _StrategyMaster_h_
#define _StrategyMaster_h_
#include <map>
#include <setjmp.h>
#include <string>

class StrategyMaster
{
public:
	enum
	{
		NO_SIG = 0,
		SIG_SEGV_HANDLED_IND = 1,
		SIG_FPE_HANDLED_IND  = 2,
		SIG_ILL_HANDLED_IND  = 3,
		SIG_BUS_HANDLED_IND  = 4
	};

	static const int TAG_58 = 58;

	static StrategyMaster& GetInstance();

	static void sigSEGV(int nSignal);
	static void sigFPE(int nSignal);
	static void sigILL(int nSignal);
	static void sigBUS(int nSignal);

	void DisableSignals();

	////////////////////////////////////
	// Destructor
	virtual ~StrategyMaster();

	bool SetRoutingTag(int nTag);

	bool LoadStrategy(StrategyBase* pStrat, const char* szConfigFile);
	void UnLoadStrategies();

	bool SetDefaultStrategyPath(const string& sFilePath);

	int callValidatingCustomMethod(const char *stratName, const short num, void *data);

	////////////////////////////////////
	// Main Event Handler - Entry point
	int HandleEvent(int nReason, void* data, bool bRej);
protected:
private:
	////////////////////////////////////
	// Constructor
	StrategyMaster(int nRouteTag = 9500);

	int handleEventStreetNewOrder(void* data);

	int handleEventStreetCancelOrder(void* data);

	int handleEventStreetReplaceOrder(void* data);

	int handleEventStreetFills(void* data);

	int handleEventStreetReject(void* data);

	int handleEventStreetAck(void* data);

	int handleEventStreetPendingRpl(void *data);

	int handleEventStreetOut(void* data);

	int handleEventStreetStatusReport(void* data);

	int handleEventStreetNewReportStat(void* data);

	int handleEventClientNewOrder(void* data);

	int handleEventClientCancelOrder(void* data);

	int handleEventClientReplaceOrder(void* data);

	int handleEventClientSendAck(void* data);

	int handleEventClientSendOut(void* data);

	int handleEventClientSendPendingRpl(void *data);

	int handleEventMarketData(void* data);

	int handleEventTimer(void* data);

	int handleEventTimerNoData(void* data);

	int handleEventValidClientOrder(void* data);

	int handleEventValidClientReplace(void* data);

	int handleEventValidClientCancel(void* data);

	int handleEventValidStreetOrder(void* data);

	int handleEventValidStreetReplace(void* data);

	int handleEventValidStreetCancel(void* data);

	int handleEventOnload(void* data);

	int handleEventClientSendFills(void* data);

	int handleEventClientSendReject(void* data);

	int handleEventBeginRecovery(void* data);

	int handleEventEndRecovery(void* data);

	int handleEventClientConnect(void* data);

	int handleEventClientDisconnect(void* data);

	int handleEventStreetConnect(void* data);

	int handleEventStreetDisconnect(void* data);

	int handleEventTraderConnect(void* data);

	int handleEventClientCommand(void* data);

	int handleEventConfigUpdate(void* data);

	int handleEventNewIoiFromClient(void* data);

	int handleEventRplIoiFromClient(void* data);

	int handleEventCxlIoiFromClient(void* data);

	int handleEventNewIoiToClient(void* data);

	int handleEventCxlIoiToClient(void* data);

	int handleEventPriceServerConnUp(void* data);

	int handleEventPriceServerConnDown(void* data);

	int handleEventStreetDiscardNewOrder(void* data);

	int handleEventStreetDiscardReplaceOrder(void* data);

	int handleEventStreetDiscardCancelOrder(void* data);

	int	handleEventClientOrderEnrichment(void *data);

	int handleEventClientNewMLegOrder(void* data);

	int handleEventClientCancelMLegOrder(void* data);

	int handleEventClientReplaceMLegOrder(void* data);

	int handleEventValidClientNewMLegOrder(void* data);

	int handleEventValidClientReplaceMLegOrder(void* data);

	int handleEventValidClientCancelMLegOrder(void* data);

    int handleEventStreetNewMLegOrder(void* data);

    int handleEventStreetCancelMLegOrder(void* data);

    int handleEventStreetReplaceMLegOrder(void* data);

    int handleEventValidStreetNewMLegOrder(void* data);

    int handleEventValidStreetReplaceMLegOrder(void* data);

    int handleEventValidStreetCancelMLegOrder(void* data);

    int handleEventStreetFakeRep(void* data);

    int handleEventStreetBustRep(void* data);

	bool loadValidatingSO();

	void ensureLinkage(int i);

	static void connectSymProxy();

private:

	StrategyBase* getStrategyByStreetOrder(void* data, const char* szEvent, STREET_ORDER** pStreetOrder, CLIENT_ORDER* pClientOrder);
	StrategyBase* getStrategyByStreetExec(void* data, const char* szEvent, STREET_EXEC** pStreetExec, CLIENT_ORDER* pClientOrder);
	StrategyBase* getStrategyByStreetBustExec(void* data, const char* szEvent, STREET_BUST_EXEC** pStreetBustExec);
	StrategyBase* getStrategyByClientExec(void* data, const char* szEvent, CLIENT_EXEC** pClientExec);
	StrategyBase* getStrategyByClientOrder(void* data, const char* szEvent, CLIENT_ORDER** pClientOrder);
	StrategyBase* getStrategyByName(const char*szStrat, const char* orderId, const char* szEvent);

	int							m_nRoutingTag;
	typedef std::map<std::string, StrategyBase*> STRATEGY_MAP;
	typedef STRATEGY_MAP::iterator STRATEGY_MAP_ITER;

	STRATEGY_MAP m_StrategyMap;

	static sigjmp_buf 			m_jmpBuf;
	bool						m_bTimerSet;

	void *m_ValidatingSO;
	StrategyValidatingBase *m_pStrategyValidatingBase;
};

#define SET_ROUTING_TAG(tag)			static bool g_bSetTag __attribute__((unused)) = (StrategyMaster::GetInstance()).SetRoutingTag((tag))
#define LOAD_FLEX_STRATEGY(Strat,Conf)	static bool g_bLd##Strat __attribute__((unused)) = (StrategyMaster::GetInstance()).LoadStrategy((new Strat()),(Conf))
#define SET_DEFAULT_STRAT_PATH(Filepath)	static bool g_bSetDef __attribute__((unused)) = (StrategyMaster::GetInstance()).SetDefaultStrategyPath((Filepath))

int FLEX_HANDLE_EVENT(int reason, void* data, bool bRej = true);

#define STRAT_SO_INIT extern "C" \
	{ \
		void __attribute__ ((constructor)) strat_so_init(void) \
		{ \
			cout << __PRETTY_FUNCTION__ << endl; \
		} \
	}

#define STRAT_SO_FINI extern "C" \
	{ \
		void __attribute__ ((destructor)) strat_so_fini(void) \
		{ \
			cout << __PRETTY_FUNCTION__ << endl; \
			extern int g_atexit_flag; \
			if (!g_atexit_flag) \
			(StrategyMaster::GetInstance()).UnLoadStrategies(); \
		} \
	}

#endif//_StrategyMaster_h_
#ifndef _FLID_MAP_H_
#define _FLID_MAP_H_

#include <map>

union UNION_VALUE
{
	char str[32];
	double d;
	int i;
};

enum FLID_VALUE_TYPE { FLID_TYPE_STRING, FLID_TYPE_DOUBLE, FLID_TYPE_INT };

struct FLID_VALUE
{
	FLID_VALUE_TYPE eType;
	UNION_VALUE val;

	FLID_VALUE(FLID_VALUE_TYPE typ, const char* szVal)
		: eType(typ)
	{
		strcpy(val.str, szVal);
	}

	FLID_VALUE(FLID_VALUE_TYPE typ, double dVal)
		: eType(typ)
	{
		val.d = dVal;
	}

	FLID_VALUE(FLID_VALUE_TYPE typ, int iVal)
		: eType(typ)
	{
		val.i = iVal;
	}
};

struct FLID_MAP {
    typedef std::map<int, FLID_VALUE> self_type;
    typedef self_type::value_type value_type;
    self_type dummy;
};

#endif //_FLID_MAP_H_
#ifndef FtVersion_h
#define FtVersion_h

#define FLEX_MAJOR_VERSION Rel-11-1-0
#define FLEX_MINOR_VERSION Rel-11-1-0
#define FLEX_REVISION_VERSION Rel-11-1-0
#define FLEX_PROJECT_VERSION 20130118.3.0.7

/*
 * These functions provide version information
 */

#ifdef	__cplusplus
extern "C" {
#endif

  /*
   * Get the version number
   * @return version number as string
   */
  const char* FtGetVersion();

  /*
   * Get the build date
   * @return build date as string
   */
  const char* FtGetBuildDate();

  /*
   * Get the build host
   * @return build host name
   */
  const char* FtGetBuildHost();


  /*
   * Get the major version number; for 1.22.333 - you get 1
   * @return version number as string
   */
  const char *FtGetVersionMajor();

  /*
   * Get the minor version number; for 1.22.333 - you get 22
   * @return version number as string
   */
  const char *FtGetVersionMinor();

  /*
   * Get the revision version number; for 1.22.333 - you get 333
   * @return version number as string
   */
  const char *FtGetVersionRevision();

  /*
   * Get the project version
   * @return project version
   */
  const char* FtGetProjectVersion();

#ifdef	__cplusplus
}
#endif

#endif // FtVersion_h



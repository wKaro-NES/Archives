#ifndef __GSMPOSLOGGER_H_
#define __GSMPOSLOGGER_H_



#define GSMPOS_LOG

#define GSMPOS_LOG_ERROR

#define GSMPOS_LOG_FUNCTION

#define GSMPOS_LOG_INFOPRINT

#define GSMPOS_LOG_TIME_PROFILE

#include <e32std.h>
#include <e32base.h>

class TGsmPosLogger
	{
	public:
		static void Init(TBool aClear, const char* aFunction);
		static void Log(TRefByValue<const TDesC16> aFmt, ...);
		static void Log(TRefByValue<const TDesC8> aFmt, ...);
		static void LogDump(const TDesC16& aText);
		static void LogMultiline(const TDesC8& aText);
		static void LogMultiline(const TDesC16& aText);
		static void LogDump(const TDesC8& aText);
		static void LogFault(TRefByValue<const TDesC16> aFmt, ...);
		static void LogFault(TRefByValue<const TDesC8> aFmt, ...);
		
	private:
		static void Log(TRefByValue<const TDesC16> aFmt, TBool aError, VA_LIST& aList);
		static void Log(TRefByValue<const TDesC8> aFmt, TBool aError, VA_LIST& aList);
	};

class GsmPosLoggerScope
	{
	public:
		GsmPosLoggerScope(const char* aFunction, TBool aLeaveCheck);
		~GsmPosLoggerScope();
		void Close();
	
	private:
		TBool iLeaveOccured;
		TBool iLeaveCheck;
		TPtrC8 iFunction;
	};

class TGsmPosLoggerOverflowHandler16 : public TDes16Overflow
	{
	public:
		TGsmPosLoggerOverflowHandler16();
		virtual ~TGsmPosLoggerOverflowHandler16();
		TBool IsOverflow();
		
		//from TDes16Overflow
		virtual void Overflow(TDes16 &aDes);
	private:
		TBool iOverflow;
	};

class TGsmPosLoggerOverflowHandler8 : public TDes8Overflow
	{
	public:
		TGsmPosLoggerOverflowHandler8();
		virtual ~TGsmPosLoggerOverflowHandler8();
		TBool IsOverflow();
		
		//from TDes8Overflow
		virtual void Overflow(TDes8 &aDes);
	private:
		TBool iOverflow;		
	};

//--------------------------------------------------------------------------------------------

#ifdef GSMPOS_LOG_ERROR														
	#define LOG_ERROR                 			TGsmPosLogger::LogFault
#else
	#define LOG_ERROR
#endif	

#ifdef GSMPOS_LOG																
	#undef LOG_ERROR
	#define LOG_ERROR                 			TGsmPosLogger::LogFault
	#define LOG_DUMP(x)         				TGsmPosLogger::LogDump(x)
	#define LOG_MULTILINE(x)         			TGsmPosLogger::LogMultiline(x)
	#define LOG		                			TGsmPosLogger::Log
#else
	#define LOG_DUMP(x)
	#define LOG_MULTILINE(x)
	#define LOG
#endif

#ifdef GSMPOS_LOG_FUNCTION														
	#define LOG_FUNCTION						GsmPosLoggerScope _t_scope(__PRETTY_FUNCTION__ , EFalse); 

	#define LOG_FUNCTION_LEAVE    				GsmPosLoggerScope _t_scope(__PRETTY_FUNCTION__ , ETrue); \
												CleanupClosePushL(_t_scope);
#else
	#define LOG_FUNCTION
	#define LOG_FUNCTION_LEAVE
#endif

	

#ifdef GSMPOS_LOG_INFOPRINT														
    #define LOG_INFOPRINT(msg)      User::InfoPrint(msg);
#else
    #define LOG_INFOPRINT(msg)
#endif


#ifdef GSMPOS_LOG_TIME_PROFILE														
	#define LOG_TIME_INIT \
	TPtrC _t_message; \
	TTime _t_start, _t_end; \
	_t_start.HomeTime();

	#define LOG_TIME_CHECK(x) \
	_t_end.HomeTime(); \
	_t_message.Set((TPtrC((const TText *)L##x))); \
	TGsmPosLogger::Log(TPtrC((const TText *)L"%S %Ld"), &_t_message, _t_end.MicroSecondsFrom(_t_start).Int64()); \
	_t_start.HomeTime(); 
#else
	#define LOG_TIME_INIT
	#define LOG_TIME_CHECK(x)    
#endif
    
#if defined(GSMPOS_LOG_ERROR) || defined(GSMPOS_LOG) || defined(GSMPOS_LOG_FUNCTION) || defined(GSMPOS_LOG_TIME_PROFILE)	
	#define LOG_INIT				TGsmPosLogger::Init(EFalse, __PRETTY_FUNCTION__)
	#define LOG_INIT_CLEAR			TGsmPosLogger::Init(ETrue, __PRETTY_FUNCTION__)
#else
	#define LOG_INIT
	#define LOG_INIT_CLEAR
#endif

#endif /*__GSMPOSLOGGER_H_*/

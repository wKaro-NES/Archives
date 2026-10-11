 
#include "GsmPosLogger.h"

#include <e32debug.h>
#include <flogger.h>
#include <BAUTILS.H>
#include <f32file.h>

_LIT(KLoggingPrefix,		"#GsmPos#");
_LIT(KLoggingPrefixError,	"#GsmPos# Error! ");
_LIT(KLoggingPrefixTruncate, "(...)");
_LIT(KLoggingPrefixFormat,   "%S [0x%x] ");

_LIT8(KLoggingPrefix8,		"#GsmPos#");
_LIT8(KLoggingPrefixError8,	"#GsmPos# Error! ");
_LIT8(KLoggingPrefixTruncate8, "(...)");
_LIT8(KLoggingPrefixFormat8, "%S [0x%x] ");

_LIT(KLoggingDir,			"GsmPos");
_LIT(KLoggingFile,			"gsmpos_log.txt");
_LIT(KLoggingPath,			"c:\\logs\\%S\\");

_LIT(KLogStartedMessage,	"Log started, process: %S --------------------");
_LIT8(KLogClearedMessage,	"(cleared in %S)");

_LIT(KNewLine,				"\n");
_LIT8(KNewLine8,			"\n");			

const TInt KMultilineMaxLineLength = 120;

//---------------------------------------------------------------------

TGsmPosLoggerOverflowHandler16::TGsmPosLoggerOverflowHandler16()
	: iOverflow(EFalse)
	{
	}

TGsmPosLoggerOverflowHandler16::~TGsmPosLoggerOverflowHandler16()
	{
	}

TBool TGsmPosLoggerOverflowHandler16::IsOverflow()
	{
	return iOverflow;
	}

void TGsmPosLoggerOverflowHandler16::Overflow(TDes16& /*aDes*/)
	{
	iOverflow = ETrue;
	}

//---------------------------------------------------------------------

TGsmPosLoggerOverflowHandler8::TGsmPosLoggerOverflowHandler8()
	: iOverflow(EFalse)
	{
	}

TGsmPosLoggerOverflowHandler8::~TGsmPosLoggerOverflowHandler8()
	{
	}

TBool TGsmPosLoggerOverflowHandler8::IsOverflow()
	{
	return iOverflow;
	}

void TGsmPosLoggerOverflowHandler8::Overflow(TDes8& /*aDes*/)
	{
	iOverflow = ETrue;
	}

//---------------------------------------------------------------------

 
void TGsmPosLogger::Init(TBool aClear, const char* aFunction)
	{
	RFs fs;
	TFileName path;
	path.Format(KLoggingPath, &KLoggingDir());
	TRAP_IGNORE(
				User::LeaveIfError(fs.Connect());
				CleanupClosePushL(fs);
				BaflUtils::EnsurePathExistsL(fs, path);
				if (aClear)
					{
					path.Append(KLoggingFile);
					User::LeaveIfError(BaflUtils::DeleteFile(fs, path));
					TPtrC8 func((const TUint8*)aFunction);
					Log(KLogClearedMessage,&func);
					}
				CleanupStack::PopAndDestroy(&fs);
	);
	path = RProcess().FileName();
	Log(KLogStartedMessage, &path);
	}

//---------------------------------------------------------------------

 
void TGsmPosLogger::Log(TRefByValue<const TDesC16> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	Log(aFmt, EFalse, list);
	}

//---------------------------------------------------------------------

 
void TGsmPosLogger::LogFault(TRefByValue<const TDesC16> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	Log(aFmt, ETrue, list);
	}

//---------------------------------------------------------------------

void TGsmPosLogger::Log(TRefByValue<const TDesC16> aFmt, TBool aError, VA_LIST& aList)
	{
	TGsmPosLoggerOverflowHandler16 overflow;
	TGsmPosLoggerOverflowHandler16 ignore;
	TBuf<KLogBufferSize> buf;							//patrz <flogger.h>, maks limit
	
	TPtrC prefix(aError ? KLoggingPrefixError() : KLoggingPrefix());
	buf.AppendFormat(KLoggingPrefixFormat , &ignore, &prefix, TUint(RThread().Id()));
	buf.AppendFormatList(aFmt, aList, &overflow);

	if (overflow.IsOverflow())
		{
		TInt free = KLogBufferSize - buf.Length();
		TInt len = KLoggingPrefixTruncate().Length();
		if (free < len)
			buf.Delete(buf.Length() - len - 1, len);
		buf.Insert(prefix.Length(), KLoggingPrefixTruncate);
		}
	//plik
	RFileLogger::Write(KLoggingDir,KLoggingFile, EFileLoggingModeAppend, buf);

	//konsola
	RDebug::RawPrint(buf);
	}

//---------------------------------------------------------------------

 
void TGsmPosLogger::Log(TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	Log(aFmt, EFalse, list);
	}

//---------------------------------------------------------------------

 
void TGsmPosLogger::LogFault(TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	Log(aFmt, ETrue, list);
	}

//---------------------------------------------------------------------

void TGsmPosLogger::Log(TRefByValue<const TDesC8> aFmt, TBool aError, VA_LIST& aList)
	{
	TGsmPosLoggerOverflowHandler8 overflow;
	TGsmPosLoggerOverflowHandler8 ignore;
	TBuf8<KLogBufferSize> buf;							//patrz <flogger.h>, maks limit
	
	TPtrC8 prefix(aError ? KLoggingPrefixError8() : KLoggingPrefix8());
	buf.AppendFormat(KLoggingPrefixFormat8 , &ignore, &prefix, TUint(RThread().Id()));
	buf.AppendFormatList(aFmt, aList, &overflow);

	if (overflow.IsOverflow())
		{
		TInt free = KLogBufferSize - buf.Length();
		TInt len = KLoggingPrefixTruncate8().Length();
		if (free < len)
			buf.Delete(buf.Length() - len - 1, len);
		buf.Insert(prefix.Length(), KLoggingPrefixTruncate8);
		}

	//plik
	RFileLogger::Write(KLoggingDir,KLoggingFile, EFileLoggingModeAppend, buf);

	//konsola
	RDebug::RawPrint(buf);
	}

//---------------------------------------------------------------------

 
void TGsmPosLogger::LogDump(const TDesC16& aText)
	{
	Log(KNullDesC, EFalse, EFalse);

	//plik
	RFileLogger::HexDump(KLoggingDir,KLoggingFile, EFileLoggingModeAppend, NULL, NULL, (const TUint8*) aText.Ptr(), 2 * aText.Length());

	//konsola
	RDebug::RawPrint(aText);
	RDebug::RawPrint(KNewLine);
	}

//---------------------------------------------------------------------

 
void TGsmPosLogger::LogDump(const TDesC8& aText)
	{
	Log(KNullDesC8, EFalse, EFalse);

	//plik
	RFileLogger::HexDump(KLoggingDir,KLoggingFile, EFileLoggingModeAppend, NULL, NULL, aText.Ptr(), aText.Length());

	//konsola
	RDebug::RawPrint(aText);
	RDebug::RawPrint(KNewLine8);
	}

//---------------------------------------------------------------------

void TGsmPosLogger::LogMultiline(const TDesC8& aText)
	{
	TInt idx = 0;
	do
		{
		TPtrC8 buf;
		if (idx + KMultilineMaxLineLength < aText.Length())
			buf.Set(aText.Mid(idx, KMultilineMaxLineLength));
		else 
			buf.Set(aText.Mid(idx));
		
		//plik
		RFileLogger::Write(KLoggingDir,KLoggingFile, EFileLoggingModeAppend, buf);

		//konsola
		RDebug::RawPrint(buf);
		
		idx+=KMultilineMaxLineLength;
		} while (idx < aText.Length());
	}

//---------------------------------------------------------------------

void TGsmPosLogger::LogMultiline(const TDesC16& aText)
	{
	TInt idx = 0;
	do
		{
		TPtrC buf;
		if (idx + KMultilineMaxLineLength < aText.Length())
			buf.Set(aText.Mid(idx, KMultilineMaxLineLength));
		else 
			buf.Set(aText.Mid(idx));
		
		//plik
		RFileLogger::Write(KLoggingDir,KLoggingFile, EFileLoggingModeAppend, buf);

		//konsola
		RDebug::RawPrint(buf);
		
		idx+=KMultilineMaxLineLength;
		} while (idx < aText.Length());
	}

//---------------------------------------------------------------------

GsmPosLoggerScope::GsmPosLoggerScope(const char* aFunction, TBool aLeaveCheck) 
: iLeaveOccured(EFalse), iLeaveCheck(aLeaveCheck), iFunction((const TUint8*) aFunction)
	{
	_LIT8(KEnterMsg8, "%S enter...");
	TGsmPosLogger::Log(KEnterMsg8, &iFunction);
	}

 
GsmPosLoggerScope::~GsmPosLoggerScope()
	{
	if (iLeaveOccured)
		{
		_LIT8(KLeaveMsg8, "%S ...LEAVE");
		TGsmPosLogger::LogFault(KLeaveMsg8, &iFunction);
		}
	else
		{
		_LIT8(KExitMsg8, "%S ...exit");
		TGsmPosLogger::Log(KExitMsg8, &iFunction);
		if (iLeaveCheck)
			{
			CleanupStack::Pop( this );
			}
		}
	}

//---------------------------------------------------------------------

 
void GsmPosLoggerScope::Close()
	{
	iLeaveOccured = ETrue;
	}

//---------------------------------------------------------------------

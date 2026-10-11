#include <BAUTILS.H>
#include <f32file.h>

#include "GsmPosLogger.h"
#include "GsmPosDatabase.h"
#include "GsmPosEngine.h"

//TODO - private path? ale na karcie pamieci
_LIT(KDatabaseFolder, "c:\\logs\\GsmPos\\");
_LIT(KDatabase,"GsmPos.db");

//TODO - normalizacja
//TODO - baza wielokatow
_LIT(KRawTable, "raw"); 
_LIT(KColTime,				"time");
_LIT(KColCountryCode,   	"country_code");
_LIT(KColNetworkId,     	"network_id");
_LIT(KColShortName,     	"short_name");
_LIT(KColLocationAreaCode,	"lac");
_LIT(KColCellId,			"cell_id");
_LIT(KColLatitude,			"latitude");
_LIT(KColLongitude,			"longitude");
_LIT(KColHorizontalAccuracy,"horizontal_accuracy");

_LIT(KSQLAllColumn,	"select time,country_code,network_id,short_name,lac,cell_id,latitude,longitude,horizontal_accuracy from raw");
_LIT(KSQLFilterCellId, " where (country_code like '%S') and (network_id like '%S') and (lac=%u) and (cell_id=%u) ");
//_LIT(KSQLAllColumnByContactId,	"select type,contactid,contacturi, time,direction,details from events where contactid=%d");
//_LIT(KSQLAllColumnByContactUri,	"select type,contactid,contacturi, time,direction,details from events where contacturi like '%S'");
//_LIT(KSQLDeleteAllEarlierThan,  "delete from events where time <= #%S#");

_LIT(KSQLFormatTime,"%F%D/%M/%Y");

CGsmPosDatabase* CGsmPosDatabase::NewL()
	{
	CGsmPosDatabase* self = new(ELeave) CGsmPosDatabase();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CGsmPosDatabase::ConstructL()
	{
	LOG_FUNCTION_LEAVE;

	OpenDatabaseL();
	}

CGsmPosDatabase::CGsmPosDatabase()
	{
	}

CGsmPosDatabase::~CGsmPosDatabase()
	{
	LOG_FUNCTION;
	
	iDatabase.Close();
	iDbSession.Close();
	}

void CGsmPosDatabase::HandleTransactionErrors(TInt aErrorCode)
	{
	LOG(_L("CGsmPosDatabase::HandleTransactionErrors(%d)"), aErrorCode);

	//revert changes, release lock
	if (iDatabase.InTransaction())
		iDatabase.Rollback();
	}

TInt CGsmPosDatabase::Add(const vector<CTelephony::TNetworkInfoV1>& aNetworkInfo, const TPositionInfo& aPositionInfo)
	{
	LOG_FUNCTION;

	TRAPD(err,DoAddL(aNetworkInfo, aPositionInfo));
	if (err)
		{
		HandleTransactionErrors(err);
		}
	
	//compact database
	TRAP(err,CompactL());
	if (err)
		{
		LOG_ERROR(_L("CGsmPosDatabase::Add(),db compact failed, error: %d"), err);
		}
	
	return err;
	}

//TODO view caly czas otwarte ?
void CGsmPosDatabase::DoAddL(const vector<CTelephony::TNetworkInfoV1>& aNetworkInfo, const TPositionInfo& aPositionInfo)
	{
	LOG_FUNCTION_LEAVE;
	
	User::LeaveIfError(iDatabase.Begin());
	
	RDbView view;
	User::LeaveIfError(view.Prepare(iDatabase,TDbQuery(KSQLAllColumn,EDbCompareNormal)));
	CleanupClosePushL(view);
	User::LeaveIfError(view.EvaluateAll());

	TPosition pos;
	aPositionInfo.GetPosition(pos);
	
	for (TInt i = 0; i < aNetworkInfo.size(); i++)
		{
		view.InsertL();
	
		view.SetColL(1,pos.Time());
		view.SetColL(2,aNetworkInfo[i].iCountryCode);
		view.SetColL(3,aNetworkInfo[i].iNetworkId);
		view.SetColL(4,aNetworkInfo[i].iShortName);
		view.SetColL(5,aNetworkInfo[i].iLocationAreaCode);
		view.SetColL(6,aNetworkInfo[i].iCellId);
		view.SetColL(7,pos.Latitude());
		view.SetColL(8,pos.Longitude());
		view.SetColL(9,pos.HorizontalAccuracy());
				
		view.PutL();
		}
	CleanupStack::PopAndDestroy(&view);
	
	User::LeaveIfError(iDatabase.Commit());
	}

//TODO takze wg czasu (aby pokazac postep rozbudowy mapy pokrycia)
TInt CGsmPosDatabase::GetAllCoordinates(const CTelephony::TNetworkInfoV1& aNetworkInfo, vector<TPosition>& aPositions)
	{
	//LOG_FUNCTION;
	
	TRAPD(err, DoGetAllCoordinatesL( aNetworkInfo, aPositions));

	if (err)
		{
		HandleTransactionErrors(err);			
		}
	
	return err;
	}


void CGsmPosDatabase::DoGetAllCoordinatesL(const CTelephony::TNetworkInfoV1& aNetworkInfo, vector<TPosition>& aPositions)
	{
	//LOG_FUNCTION_LEAVE;

	User::LeaveIfError(iDatabase.Begin());
		
	RBuf query;
	query.CreateL(KSQLAllColumn().Length() + KSQLFilterCellId().Length() + aNetworkInfo.iCountryCode.MaxLength() + aNetworkInfo.iNetworkId.MaxLength() + 32);
	CleanupClosePushL(query);
	query.Format(KSQLFilterCellId, &aNetworkInfo.iCountryCode, &aNetworkInfo.iNetworkId, aNetworkInfo.iLocationAreaCode, aNetworkInfo.iCellId);
	query.Insert(0, KSQLAllColumn);
	
	LOG_MULTILINE(query);
	
	RDbView view;
	User::LeaveIfError(view.Prepare(iDatabase,TDbQuery(query,EDbCompareNormal)));
	CleanupClosePushL(view);
	User::LeaveIfError(view.EvaluateAll());

	for (view.FirstL(); view.AtRow(); view.NextL())
		{
		view.GetL();
		
		TCoordinate coord(view.ColReal64(7),view.ColReal64(8));	//latitude, longitude
		TLocality loc(coord, view.ColReal32(9));				//horizontal_accuracy
		TPosition pos(loc, view.ColTime(1));					//time
		
		aPositions.push_back(pos);
		}
	
	CleanupStack::PopAndDestroy(&view);
	CleanupStack::PopAndDestroy(&query);
	
	User::LeaveIfError(iDatabase.Commit());
	}

TInt CGsmPosDatabase::GetAllCellNames(vector<CTelephony::TNetworkInfoV1>& aNetworkInfo, const TCoordinate& aPosition, TReal32 aRadius)
	{
	LOG_FUNCTION;

	TRAPD(err, DoGetAllCellNamesL( aNetworkInfo, aPosition, aRadius));

	if (err)
		{
		HandleTransactionErrors(err);			
		}
	
	return err;
	}

//TODO dodaj filtrowanie wg operatora (iCountryCode+iNetworkId)
void CGsmPosDatabase::DoGetAllCellNamesL(vector<CTelephony::TNetworkInfoV1>& aNetworkInfo, const TCoordinate& aPosition, TReal32 aRadius)
	{
	LOG_FUNCTION_LEAVE;
	
	User::LeaveIfError(iDatabase.Begin());
		
	RDbView view;
	User::LeaveIfError(view.Prepare(iDatabase,TDbQuery(KSQLAllColumn,EDbCompareNormal)));
	CleanupClosePushL(view);
	User::LeaveIfError(view.EvaluateAll());

	for (view.FirstL(); view.AtRow(); view.NextL())
		{
		view.GetL();
		
		TCoordinate coord(view.ColReal64(7),view.ColReal64(8));	//latitude, longitude
		
		TReal32 distance = 0.0f;
		User::LeaveIfError(coord.Distance(aPosition, distance));
		if ( !Math::IsZero(aRadius) && Abs(distance) > aRadius )
			{
			//pomin komorki w zbyt duzej odleglosci
			continue;
			}
		
		CTelephony::TNetworkInfoV1 info;
		info.iCountryCode = view.ColDes(2);
		info.iNetworkId   = view.ColDes(3);
		info.iShortName   = view.ColDes(4);
		info.iLocationAreaCode = view.ColUint(5);
		info.iCellId = view.ColUint(6);
		
		//nie wstawiaj duplikatow
		vector<CTelephony::TNetworkInfoV1>::const_iterator exist = find_if(aNetworkInfo.begin(), aNetworkInfo.end(), bind2nd(TNetworkInfoV1_equal(), info));
		if (exist == aNetworkInfo.end())
			{
			LOG(_L("\t%S %S %S %u %u\t\t%f"), &info.iCountryCode, &info.iNetworkId, &info.iShortName, info.iLocationAreaCode, info.iCellId, distance);
			
			try
				{
				aNetworkInfo.push_back(info);
				}
			catch (...)
				{
				//TODO - spojna dla projektu! obsluga wyjatkow STL
				User::Leave(KErrGeneral);
				}
			}
		}
	
	CleanupStack::PopAndDestroy(&view);
	
	User::LeaveIfError(iDatabase.Commit());
	}

//TODO - nie korzystaj z interfejsu client-server
void CGsmPosDatabase::OpenDatabaseL()
	{
	LOG_FUNCTION_LEAVE;

	//TODO - karta MMC ?
	
	//database location
	TParse fspec;
	User::LeaveIfError(fspec.Set(KDatabaseFolder, NULL, &KDatabase()));

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	BaflUtils::EnsurePathExistsL(fs, fspec.DriveAndPath());
	LOG(_L("db file: %S"), &fspec.FullName());

	//open database
	User::LeaveIfError(iDbSession.Connect());
	TInt err = iDatabase.Open(iDbSession, fspec.FullName());
	if (err != KErrNone)
		{
		if (err == KErrNotFound)
			{
			LOG(_L("db doesn't exist, create"));
			//create database
			User::LeaveIfError(iDatabase.Replace(fs, fspec.FullName()));
			
			//create tables & indexes
			CreateTableL();
			CreateIndexL();
			
			//close & reopen via C/S interface
			iDatabase.Close();
			
			LOG(_L("db created, reopen"));
			User::LeaveIfError(iDatabase.Open(iDbSession, fspec.FullName()));
			}
		else
			{
			User::Leave(err);
			}
		}
	CleanupStack::PopAndDestroy(&fs);
	}


void CGsmPosDatabase::CreateTableL()
	{
	LOG_FUNCTION_LEAVE;
	
	CDbColSet* columns = CDbColSet::NewLC();
	
	columns->AddL(TDbCol(KColTime, EDbColDateTime));
	columns->AddL(TDbCol(KColCountryCode, EDbColText16));
	columns->AddL(TDbCol(KColNetworkId, EDbColText16));
	columns->AddL(TDbCol(KColShortName, EDbColText16));
	columns->AddL(TDbCol(KColLocationAreaCode, EDbColUint32));
	columns->AddL(TDbCol(KColCellId, EDbColUint32));
	columns->AddL(TDbCol(KColLatitude, EDbColReal64));
	columns->AddL(TDbCol(KColLongitude, EDbColReal64));
	columns->AddL(TDbCol(KColHorizontalAccuracy, EDbColReal32));

	User::LeaveIfError (iDatabase.CreateTable(KRawTable, *columns));

	CleanupStack::PopAndDestroy(columns);
	}

//TODO testy wydajnosci i dobor klucza
void CGsmPosDatabase::CreateIndexL()
	{
	LOG_FUNCTION_LEAVE;

	CDbKey* key = CDbKey::NewLC();

	key->AddL(TDbKeyCol(KColLocationAreaCode));
	key->AddL(TDbKeyCol(KColCellId));
	//key->MakeUnique();						//klucz nie jest unikalny

	User::LeaveIfError(iDatabase.CreateIndex(KRawTable,KRawTable,*key));

	CleanupStack::PopAndDestroy();
	}

void CGsmPosDatabase::CompactL()
	{
	LOG_FUNCTION_LEAVE;
	
	User::LeaveIfError(iDatabase.Compact());
	}


#ifndef GSMPOSDATABASE_H_
#define GSMPOSDATABASE_H_

#include <etel3rdparty.h>
#include <lbs.h>
#include <D32DBMS.H>

#include <vector>
using namespace std;

class CGsmPosDatabase : 	public CBase
 	{
	public:
		static CGsmPosDatabase* NewL();
		~CGsmPosDatabase();
		
		TInt Add(const vector<CTelephony::TNetworkInfoV1>& aNetworkInfo, const TPositionInfo& aPositionInfo);
		TInt GetAllCoordinates(const CTelephony::TNetworkInfoV1& aNetworkInfo, vector<TPosition>& aPositions);
		TInt GetAllCellNames(vector<CTelephony::TNetworkInfoV1>& aNetworkInfo, const TCoordinate& aPosition, TReal32 aRadius = 0.0f);
		
	private:
		CGsmPosDatabase();
		void ConstructL();
		void OpenDatabaseL();
		void CreateTableL();
		void CreateIndexL();
		void CompactL();
		void HandleTransactionErrors(TInt aErrorCode);
		
		void DoAddL(const vector<CTelephony::TNetworkInfoV1>& aNetworkInfo, const TPositionInfo& aPositionInfo);
		void DoGetAllCoordinatesL(const CTelephony::TNetworkInfoV1& aNetworkInfo, vector<TPosition>& aPositions);
		void DoGetAllCellNamesL(vector<CTelephony::TNetworkInfoV1>& aNetworkInfo, const TCoordinate& aPosition, TReal32 aRadius);
		
	private:
		RDbs iDbSession;
		RDbNamedDatabase iDatabase;
	};

#endif

#ifndef GSMPOSMAPOBSERVER_H_
#define GSMPOSMAPOBSERVER_H_

class CFbsBitmap;
class TPosition;

class MGsmPosMapObserver
	{
	public:
		virtual void MapCreated(CFbsBitmap* aBitmap) = 0;	//transfer ownership
		virtual void MapPositionMarked(const TPosition& aGps, const TPosition& aCentroid) = 0;
		virtual void MapDownloadStarted() = 0;
		//void MapCreationError(TInt aError) = 0;
	};


#endif /* GSMPOSMAPOBSERVER_H_ */

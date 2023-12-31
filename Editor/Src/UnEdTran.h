/*=============================================================================
	UnEdTran.h: Unreal transaction tracking system
	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	UTransBuffer
-----------------------------------------------------------------------------*/

class EDITOR_API UTransBuffer : public UObject, public FTransactionTracker
{
	DECLARE_CLASS_WITHOUT_CONSTRUCT(UTransBuffer,UObject,CLASS_Transient)
	NO_DEFAULT_CONSTRUCTOR(UTransBuffer)

	// Variables.
	TArray<BYTE> Data;			// Transaction tracker data.
	INT_UNREAL_32S		Overflow;			// 1 if active transaction overflowed data buffer.
	INT_UNREAL_32S		TransCount;			// Ever-increasing counter of maximum transaction.
	INT_UNREAL_32S		UndoTransCount;		// Transaction count in undo/redo cycle.
	INT_UNREAL_32S		NumTrans;			// Number of transactions in *Transactions.
	INT_UNREAL_32S		MaxTrans;			// Maximum number of transactions in *Transactions.
	INT_UNREAL_32S		NumChanges;			// Number of entries in *ChangeLog.
	INT_UNREAL_32S		MaxChanges;			// Maximum entries in *ChangeLog.
	INT_UNREAL_32S		MaxDataOffset;		// Maximum bytes allocated for *Data.
	char	ResetAction[256];	// Reason transaction system was last reset.

	// Pointers, only valid when locked.
	class FTransaction*		Transactions;	// List of all transactions.
	class FTransChangeLog*	ChangeLog;		// List of all modifications made in a transaction.
	BYTE*					Buffer;			// Big raw data buffer.

	// Constructor.
	UTransBuffer( int MaxTrans, int MaxChanges, int MaxData );

	// UObject interface.
	UBOOL Lock( DWORD NewLockType );
	void Unlock( DWORD OldLockType );
	void Serialize( FArchive& Ar );

	// UTransBuffer interface.
	virtual void Reset(const char *Action);
	virtual void Begin(ULevel *Level, const char *SessionName);
	virtual void End();
	virtual void Rollback(ULevel *Level);
	virtual void ForceOverflow(const char *Reason);
	virtual int  CanUndo(char *Str);
	virtual int  CanRedo(char *Str);
	virtual int  Undo(ULevel *Level);
	virtual int  Redo(ULevel *Level);
	virtual void Continue();
	virtual void NoteObject(UObject *Res);
	virtual void NoteSingleChange(UObject *Res, int Index);

private:
	void DeleteFirstTransaction();
	void BeginChanges (class FTransaction *Trans);
	void EndChanges (class FTransaction *Trans);
	void RecursiveTransBspNode (UModel *Model,INT_UNREAL_32S iNode);
	void ApplyChange (class FTransChangeLog *Change, BYTE *SourcePtr, INT_UNREAL_32S DataSize);
};

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/

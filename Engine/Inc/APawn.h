/*=============================================================================
	APawn.h: Class functions residing in the APawn class.
	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
=============================================================================*/

	// Constructors.

	// Latent movement
	void setMoveTimer(FLOAT MoveSize);
	int moveToward(const FVector &Dest);
	int rotateToward(const FVector &FocalPoint);
	int PickWallAdjust();

	// Seeing and hearing checks
	void ShowSelf();
	int CanHear(FVector NoiseLoc, FLOAT Loudness); 
	DWORD LineOfSightTo(AActor *Other, int bShowSelf = 0);
	void CheckEnemyVisible();

	inline int walkToward(const FVector &Destination, FLOAT Movesize);

	// reach tests
	int pointReachable(FVector aPoint, int bKnowVisible = 0);
	int actorReachable(AActor *Other, int bKnowVisible = 0);
	int Reachable(FVector aPoint, FLOAT Threshold, AActor* GoalActor);
	int walkReachable(FVector Dest, FLOAT Threshold, int reachFlags, AActor* GoalActor);
	int jumpReachable(FVector Dest, FLOAT Threshold, int reachFlags, AActor* GoalActor);
	int flyReachable(FVector Dest, FLOAT Threshold, int reachFlags, AActor* GoalActor);
	int swimReachable(FVector Dest, FLOAT Threshold, int reachFlags, AActor* GoalActor);
	void jumpLanding(FVector testvel, FVector &Landing, int moveActor = 0);
	int walkMove(FVector Delta, FCheckResult& Hit, AActor* GoalActor = NULL, FLOAT threshold = 4.1, int bAdjust = 1);
	int flyMove(FVector Delta, AActor* GoalActor, FLOAT threshold = 4.1, int bAdjust = 1);
	int swimMove(FVector Delta, AActor* GoalActor, FLOAT threshold = 4.1, int bAdjust = 1);
	int FindBestJump(FVector Dest, FVector vel, FVector &Landing, int moveActor = 0); 
	int FindJumpUp(FVector Dest, FVector vel, FVector &Landing, int moveActor = 0); 
	void SuggestJumpVelocity(FVector Dest, FVector &Vel);

	// Path finding
	int findPathTo(FVector Dest, INT_UNREAL_32S bSinglePath, AActor *&bestPath, INT_UNREAL_32S bClearPaths);
	int findPathToward(AActor *goal, INT_UNREAL_32S bSinglePath, AActor *&bestPath, INT_UNREAL_32S bClearPaths);
	FLOAT findPathTowardBestInventory(AActor *&bestPath, INT_UNREAL_32S bClearPaths, FLOAT MinWeight, INT_UNREAL_32S bPredictRespawns);
	int findRandomDest(AActor *&bestPath);
	int TraverseFrom(AActor *startnode, int moveFlags);
	int breadthPathFrom(AActor *startnode, AActor *&bestPath, int bSinglePath, int moveFlags);
	FLOAT breadthPathToInventory(AActor *startnode, AActor *&bestPath, int moveFlags, FLOAT bestInventoryWeight, INT_UNREAL_32S bPredictRespawns);
	inline int calcMoveFlags();
	void clearPaths();
	void clearPath(ANavigationPoint *node);
	void HandleSpecial(AActor *&bestPath);
	int CanMoveTo(AActor *Anchor, AActor *Dest);

	// Pawn physics modes
	void physWalking(FLOAT deltaTime, INT_UNREAL_32S Iterations);
	void physFlying(FLOAT deltaTime, INT_UNREAL_32S Iterations);
	void physSwimming(FLOAT deltaTime, INT_UNREAL_32S Iterations);
	void physSpider(FLOAT deltaTime, INT_UNREAL_32S Iterations);
	void startSwimming(FVector OldVelocity, FLOAT timeTick, FLOAT remainingTime, INT_UNREAL_32S Iterations);
	void physicsRotation(FLOAT deltaTime, FVector OldVelocity);
	void performPhysics(FLOAT DeltaSeconds);

	// Intrinsics.
	INTRINSIC(execPollWaitForLanding)
	INTRINSIC(execPollMoveTo)
	INTRINSIC(execPollMoveToward)
	INTRINSIC(execPollStrafeTo)
	INTRINSIC(execPollStrafeFacing)
	INTRINSIC(execPollTurnToward)
	INTRINSIC(execPollTurnTo)

private:
	FLOAT Swim(FVector Delta, FCheckResult &Hit); 
	void findWaterLine(FVector Start, FVector &End);
	void stepUp(FVector GravDir, FVector DesiredDir, FVector Delta, FCheckResult &Hit);
	void calcVelocity(FVector AccelDir, FLOAT deltaTime, FLOAT maxSpeed, FLOAT friction, INT_UNREAL_32S bFluid, INT_UNREAL_32S bBrake, INT_UNREAL_32S bBuoyant);
	int findNewFloor(FVector OldLocation, FLOAT deltaTime, FLOAT remainingTime, INT_UNREAL_32S Iterations);
	inline int checkFloor(FVector Dir, FCheckResult &Hit);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

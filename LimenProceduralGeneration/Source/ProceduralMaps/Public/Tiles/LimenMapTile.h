// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayActor.h"
#include "LimenMapTile.generated.h"

class ALimenProceduralMapManager;
class ALimenMapTile;
struct FTileInfo;
class ANavMeshBoundsVolume;
class ALimenProceduralMapBuilder;
class UBoxComponent;

UENUM()
enum class ETileType : uint8
{
	Undefined			= 0,
	DeadEnd			    = 1,
	LinearCorridor	    = 2,
	CurvedCorridor	    = 3,
	TJunction		    = 4,
	CrossIntersection	= 5,
};

UCLASS()
class PROCEDURALMAPS_API UTileInfo : public UObject
{
	GENERATED_BODY()

public:
	UTileInfo() = default;
	
	UPROPERTY(BlueprintReadOnly)
	FVector Location = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly)
	FRotator Rotation = FRotator::ZeroRotator;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ALimenMapTile> TileActor = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<ALimenMapTile> TileClass = nullptr;
	UPROPERTY(BlueprintReadOnly)
	ETileType Type = ETileType::Undefined;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBoundingBoxBeginOverlap, AActor*, OtherActor);

UCLASS(Abstract, Blueprintable, BlueprintType)
class PROCEDURALMAPS_API ALimenMapTile : public ALimenGameplayActor
{
	GENERATED_BODY()

	friend class ALimenProceduralTileMapBuilder;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLevelChangeDelegate);

public:
	UPROPERTY(BlueprintAssignable)
	FBoundingBoxBeginOverlap OnBoundingBoxBeginOverlap;

	UPROPERTY(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	FLevelChangeDelegate OnLevelComplete;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles", meta=(WorldContext="WorldContext", DeterminesOutputType="TileClass"))
	static ALimenMapTile* GetTileOfClass(UObject* WorldContext, TSubclassOf<ALimenMapTile> TileClass);
	
	ALimenMapTile();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	/**
	 * @brief Called by the map manager. Do not call this directly!
	 * @param MapManager The 
	 */
	void SetMapManager(ALimenProceduralMapManager* MapManager);
	
	/**
	 * @brief Calculates an actor's location and rotation, relative to this tile. Useful for seamless teleports between tiles where you can teleport a player to the same location, but in another tile.
	 * @param InActor The actor to get the relative transform from.
	 * @param OutLocation
	 * @param OutRotation
	 * @param OutTileRotation
	 * @return The actor's transform relative to this tile. Identity transform if actor is invalid.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	bool GetRelativeActorLocationAndRotation(AActor* InActor, FVector& OutLocation, FRotator& OutRotation, FRotator& OutTileRotation) const;
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	bool GetRelativePlayerLocationAndRotation(AController* InController, FVector& OutLocation, FRotator& OutRotation, FRotator& OutTileRotation) const;

	/**
	 * @brief Teleports an actor, using the location and rotation parameters. Important: The new transform is in this tile's local space, not in world space.
	 * Useful for seamless teleports between tiles where you can teleport an actor to the same location, but in another tile.
	 * @param InActor The actor to teleport
	 * @param InRelativeLocation The new location to apply to the actor
	 * @param InRelativeRotation The new rotation to apply to the actor
	 * @param InOtherTileRotation
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	void TeleportActorToRelativeLocationAndRotation(AActor* InActor, const FVector& InRelativeLocation, const FRotator& InRelativeRotation, const FRotator& InOtherTileRotation);

	/**
	 * @brief Teleports a pawn, using the location and rotation parameters. Important: The new transform is in this tile's local space, not in world space.
	 * Useful for seamless teleports between tiles where you can teleport a pawn to the same location, but in another tile.
	 * @param InController The controller that owns the pawn.
	 * @param InRelativeLocation The new location to apply to the actor
	 * @param InRelativeRotation The new rotation to apply to the actor
	 * @param InOtherTileRotation
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	bool TeleportPawnToRelativeLocationAndRotation(AController* InController, const FVector& InRelativeLocation, const FRotator& InRelativeRotation, const FRotator& InOtherTileRotation);

	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	ETileType GetTileType() const;

	UFUNCTION(BlueprintCallable)
	UTileInfo* GetAssignedTileInfo() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Generation")
	ALimenProceduralMapManager* GetMapManager() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	float TileCullDistance;
	
	UFUNCTION()
	virtual void BoundingBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category="Limen|Tiles|Collision")
	TArray<USceneComponent*> GetScenarios() const;
	
private:	
	UPROPERTY()
	TObjectPtr<UTileInfo> AssignedTileInfo;
	
	UPROPERTY(EditDefaultsOnly)
	ETileType TileType;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> BoundingBox;

	UPROPERTY(EditDefaultsOnly, Category="Limen|Scenarios")
	TSubclassOf<USceneComponent> ScenarioComponentClass;
	UPROPERTY(EditDefaultsOnly, Category="Limen|Scenarios")
	FString ScenarioComponentKeyword;
	UPROPERTY(EditDefaultsOnly, Category="Limen|Scenarios")
	uint8 bForceNoScenarios : 1;

#if WITH_EDITORONLY_DATA
	
	UPROPERTY(EditDefaultsOnly, Transient, Category="Limen|Scenarios", meta=(ClampMin="-1"))
	int PreviewScenarioIndex;

#endif

	UPROPERTY()
	TArray<USceneComponent*> Scenarios;
	TSharedPtr<FRandomStream> RandomStream;

	TWeakObjectPtr<ALimenProceduralMapManager> BoundMapManager;

	static void RecursivelyDestroyChildComponents(const USceneComponent* ParentComponent);
	bool SelectRandomScenario();
};

// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayActor.h"
#include "Components/PrimitiveComponent.h"
#include "LimenMapTile.generated.h"

class ALimenMapTile;
struct FTileInfo;
class ANavMeshBoundsVolume;
class ALimenProceduralMapBuilder;
class UBoxComponent;
class ULimenDynamicLoadingComponent;

UENUM()
enum class ETileType : uint8
{
	Undefined,
	DeadEnd,
	LinearCorridor,
	CurvedCorridor,
	TJunction,
	CrossIntersection,
};

USTRUCT(BlueprintType)
struct FTileInfo
{
	GENERATED_BODY()

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

	UPROPERTY(BlueprintReadOnly)
	bool bIsRoom = false;
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
	 * @brief Used by the map builder to see whether or not this tile should be constructed at a specific location.
	 * You should define rules for whether or not the tile should be created, for example, with collision checks.
	 * @param TileInformation Information about the current tile.
	 * @param InMapBuilder The map builder reference.
	 * @return True if it should be built, false otherwise.
	 * @warning When not defined it will return true by default.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Limen|Procedural Maps|Tiles")
	bool CanCreateTile(const FTileInfo& TileInformation, const ALimenProceduralTileMapBuilder* InMapBuilder) const;
	bool CanCreateTile_Implementation(const FTileInfo& TileInformation, const ALimenProceduralTileMapBuilder* InMapBuilder) const { return true; }

	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	void CheckTileLoadState();

	ULimenDynamicLoadingComponent* GetDynamicLoadingComponent() const;

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

	ANavMeshBoundsVolume* GetNavigationMesh();

	UFUNCTION(BlueprintCallable)
	FTileInfo GetAssignedTileInfo() const;
	
protected:	
	UFUNCTION()
	virtual void BoundingBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * @brief Every tile has a bounding box that checks collisions in order to optimize performance. This can cause issues with interaction so if theres a need to change the collision profile (default "OverlapAll"), use this function.
	 * @param ProfileName The new profile name to override to.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Tiles|Collision")
	void OverrideBoundingBoxCollisionProfileName(const FName& ProfileName);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Tiles|Collision")
	TArray<USceneComponent*> GetScenarios() const;
	
private:
	UPROPERTY()
	FTileInfo AssignedTileInfo;
	
	UPROPERTY(EditDefaultsOnly)
	ETileType TileType;
	UPROPERTY()
	TWeakObjectPtr<APlayerController> TargetPlayerController;
	UPROPERTY()
	TObjectPtr<ULimenDynamicLoadingComponent> DynamicLoadingComponent;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> BoundingBox;
	UPROPERTY()
	TObjectPtr<ANavMeshBoundsVolume> NavigationMesh;

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
	TSharedPtr<FRandomStream, ESPMode::ThreadSafe> RandomStream;

	static void RecursivelyDestroyChildComponents(const USceneComponent* ParentComponent);
	bool SelectRandomScenario();
};

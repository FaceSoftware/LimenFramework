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

/**
 * @brief Enumeration representing different types of tiles in a procedural map.
 * Used to identify the structure and connectivity of map tiles.
 */
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

/**
 * @brief Class representing detailed information about a tile in a procedural map.
 * Provides attributes related to the tile's properties, such as type, position,
 * and any associated metadata required for map generation or gameplay.
 */
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

/**
 * @brief Abstract base class representing a procedural map tile.
 * Provides functionality for procedural map generation, tile interaction, and spatial transformations.
 */
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
	
	explicit ALimenMapTile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	/**
	 * @brief Sets the procedural map manager associated with the tile.
	 * Links the specified procedural map manager to this tile, enabling interaction
	 * between the tile and the overall map management system.
	 *
	 * @warning Called by ALimenProceduralTileMapBuilder class after spawning the tile.
	 * You don't have to call this manually.
	 * @param MapManager A pointer to the ALimenProceduralMapManager instance to associate with this tile.
	 */
	void SetMapManager(ALimenProceduralMapManager* MapManager);

	/**
	 * @brief Calculates the relative location and rotation of the specified actor with respect to this tile.
	 *
	 * This method determines how the given actor's location and rotation are positioned relative to this tile's
	 * origin and rotation, and can be used for spatial adjustments or transformations based on the tile.
	 *
	 * @param InActor The actor whose location and rotation are to be evaluated. Must be a valid actor.
	 * @param OutLocation [out] Holds the computed relative location of the actor with respect to this tile.
	 * @param OutRotation [out] Holds the computed relative rotation of the actor with respect to this tile.
	 * @param OutTileRotation [out] Holds this tile's current rotation.
	 * @return true if the actor is valid and the computation succeeded; false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	bool GetRelativeActorLocationAndRotation(AActor* InActor, FVector& OutLocation, FRotator& OutRotation, FRotator& OutTileRotation) const;
	/**
	 * @brief Calculates the relative location and rotation of the player character with respect to this tile.
	 *
	 * This method computes the player's position and orientation relative to the tile's origin
	 * and rotation. The resulting values are output through the provided parameters.
	 *
	 * @param InController The controller owning the player whose relative location and rotation are to be computed. Must be valid.
	 * @param OutLocation [out] The computed relative location of the player with respect to this tile.
	 * @param OutRotation [out] The computed relative rotation of the player with respect to this tile.
	 * @param OutTileRotation [out] The current rotation of this tile.
	 * @return true if the controller is valid and the computation was successful; false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	bool GetRelativePlayerLocationAndRotation(AController* InController, FVector& OutLocation, FRotator& OutRotation, FRotator& OutTileRotation) const;

	/**
	 * @brief Teleports an actor to a new location and rotation relative to this tile.
	 * Used for spatial transformations within a procedural map, especially when transitioning actors between tiles.
	 *
	 * @param InActor The actor to teleport. Must be a valid actor instance.
	 * @param InRelativeLocation The relative location to apply to the actor in this tile's local space.
	 * @param InRelativeRotation The relative rotation to apply to the actor in this tile's local space.
	 * @param InOtherTileRotation The rotation of another tile, used to adjust the actor's final transform.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	void TeleportActorToRelativeLocationAndRotation(AActor* InActor, const FVector& InRelativeLocation, const FRotator& InRelativeRotation, const FRotator& InOtherTileRotation);

	/**
	 * @brief Teleports the pawn controlled by the specified controller to a new location and rotation relative to this tile.
	 * This method adjusts the pawn's position and orientation based on the relative location, relative rotation,
	 * and the rotation of another tile, enabling seamless transitions between tiles in a procedural map.
	 *
	 * @param InController The controller of the pawn to teleport. Must be a valid controller with a valid pawn.
	 * @param InRelativeLocation The relative location to apply to the pawn in this tile's local space.
	 * @param InRelativeRotation The relative rotation to apply to the pawn in this tile's local space.
	 * @param InOtherTileRotation The rotation of another tile, used to realign the pawn's location and orientation.
	 * @return true if the teleportation succeeded; false if the controller or pawn is invalid.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	bool TeleportPawnToRelativeLocationAndRotation(AController* InController, const FVector& InRelativeLocation, const FRotator& InRelativeRotation, const FRotator& InOtherTileRotation);

	/**
	 * @brief Retrieves the type of the tile.
	 *
	 * This method returns the specific type of the tile based on the ETileType enumeration,
	 * which represents the structural and navigational characteristics of the tile within the procedural map.
	 *
	 * @return The type of the tile as an ETileType value.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	ETileType GetTileType() const;

	/**
	 * @brief Retrieves the tile information assigned to this map tile.
	 * Allows access to the detailed tile configuration associated with this specific map tile.
	 *
	 * @return A pointer to the tile information assigned to this map tile.
	 */
	UFUNCTION(BlueprintCallable)
	UTileInfo* GetAssignedTileInfo() const;

	/**
	 * @brief Retrieves the procedural map manager associated with this map tile.
	 * Provides access to the manager responsible for handling the procedural map functionalities.
	 *
	 * @return A pointer to the associated ALimenProceduralMapManager instance, or nullptr if no manager is bound.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Generation")
	ALimenProceduralMapManager* GetMapManager() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	float TileCullDistance;

	virtual void MapManagerSet(ALimenProceduralMapManager* MapManager);
	
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

	TArray<TWeakObjectPtr<USceneComponent>> Scenarios;
	TSharedPtr<FRandomStream, ESPMode::NotThreadSafe> RandomStream;

	TWeakObjectPtr<ALimenProceduralMapManager> BoundMapManager;

	static void RecursivelyDestroyChildComponents(const USceneComponent* ParentComponent);
	bool SelectRandomScenario();
};

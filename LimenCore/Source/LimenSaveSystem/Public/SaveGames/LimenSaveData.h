// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "LimenSaveData.generated.h"


USTRUCT()
struct LIMENSAVESYSTEM_API FObjectSaveData
{
	GENERATED_BODY()
	
public:
	FObjectSaveData() = default;
	~FObjectSaveData() = default;

	explicit FObjectSaveData(UObject* InObject)
	{
		SaveObjectData(InObject);
	}
	
	void SaveObjectData(UObject* InObject)
	{
		ILimenSaveObjectInterface* ObjectSaveInterface = Cast<ILimenSaveObjectInterface>(InObject);
		check(ObjectSaveInterface != nullptr);
		if (!ByteData.IsEmpty())
		{
			ByteData.Empty(10);
		}
		
		FMemoryWriter Writer(ByteData);
		FObjectAndNameAsStringProxyArchive Archive(Writer, true);
		Archive.ArIsSaveGame = true;
	
		InObject->Serialize(Archive);
		ObjectClass = FSoftClassPath(InObject->GetClass());
	}
	
	void LoadData(UObject* OutObject) const
	{
		ILimenSaveObjectInterface* ObjectSaveInterface = Cast<ILimenSaveObjectInterface>(OutObject);
		check(ObjectSaveInterface != nullptr);
		// Do not empty byte data! Big mistake...
		
		FMemoryReader Reader(ByteData);
		FObjectAndNameAsStringProxyArchive Archive(Reader, true);
		Archive.ArIsSaveGame = true;
		
		OutObject->Serialize(Archive);
		ObjectSaveInterface->DataLoaded();
	}

	const FSoftClassPath& GetObjectClass() const
	{
		return ObjectClass;
	}


private:
	UPROPERTY(SaveGame)
	TArray<uint8> ByteData;
	UPROPERTY(SaveGame)
	FSoftClassPath ObjectClass;
};

USTRUCT()
struct LIMENSAVESYSTEM_API FActorSaveData
{
	GENERATED_BODY()
	
public:
	FActorSaveData() = default;
	~FActorSaveData() = default;
	explicit FActorSaveData(AActor* SaveActor);
	FTransform GetActorTransform() const;
	FName GetActorName() const;
	UClass* GetActorClass() const;
	
	/**
	 * 
	 * @param Actor Actor to load the data to
	 */
	void LoadData(AActor* Actor) const;

	bool operator==(const FActorSaveData& Other) const;

protected:
	const TArray<uint8>& GetByteData() const;
	void SetByteData(const TArray<uint8>& NewData);

	void SetActorTransform(const FTransform& InActorTransform);

	void SetActorName(const FName& InActorName);

	void SaveActorData(AActor* InActor);
	void LoadActorData(AActor* InActor) const;

private:
	UPROPERTY(SaveGame)
	TArray<uint8> ByteData;
	UPROPERTY(SaveGame)
	FTransform ActorTransform;
	UPROPERTY(SaveGame)
	FName ActorName;
	UPROPERTY(SaveGame)
	TObjectPtr<UClass> ActorClass;
};


UCLASS()
class LIMENSAVESYSTEM_API ULimenSaveData : public USaveGame
{
	GENERATED_BODY()

public:
	bool GetActorSaveData(const FName& ActorName, FActorSaveData& OutSaveData) const;
	bool GetActorSaveData(const int Index, FActorSaveData& OutSaveData) const;
	uint32 GetActorSaveDataCount() const;
	
	bool GetObjectSaveData(const int32 Index, FObjectSaveData& OutSaveData) const;
	uint32 GetObjectSaveDataCount() const;

	int AddActorSaveData(const FActorSaveData& ActorSaveData);
	int AddActorSaveData(AActor* ActorSaveData);

	int AddObjectSaveData(const FObjectSaveData& ObjectSaveData);
	int AddObjectSaveData(UObject* ObjectSaveData);
	
	
protected:
	
private:
	UPROPERTY()
	TArray<FActorSaveData> ActorSaveDataList;
	UPROPERTY()
	TArray<FObjectSaveData> ObjectSaveDataList;
	
};

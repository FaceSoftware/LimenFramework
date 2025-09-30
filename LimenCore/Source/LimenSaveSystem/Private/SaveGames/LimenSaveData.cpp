// Copyright Face Software. All Rights Reserved.


#include "SaveGames/LimenSaveData.h"

#include "GameFramework/Actor.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"


FActorSaveData::FActorSaveData(AActor* SaveActor)
{	
	SaveActorData(SaveActor);
}

const TArray<uint8>& FActorSaveData::GetByteData() const
{
	return ByteData;
}

void FActorSaveData::SetByteData(const TArray<uint8>& NewData)
{
	ByteData = NewData;
}

FTransform FActorSaveData::GetActorTransform() const
{
	return ActorTransform;
}

void FActorSaveData::SetActorTransform(const FTransform& InActorTransform)
{
	ActorTransform = InActorTransform;
}

FName FActorSaveData::GetActorName() const
{
	return ActorName;
}

UClass* FActorSaveData::GetActorClass() const
{
	return ActorClass.Get();
}

void FActorSaveData::LoadData(AActor* Actor) const
{
	if (!IsValid(Actor))
	{
		return;
	}

	LoadActorData(Actor);
}

bool FActorSaveData::operator==(const FActorSaveData& Other) const
{
	return ActorClass == Other.ActorClass;
}

bool FActorSaveData::operator==(const AActor* Other) const
{
	if (!Other) return !ActorClass;

	return Other->GetClass() == ActorClass;
}

void FActorSaveData::SetActorName(const FName& InActorName)
{
	ActorName = InActorName;
}

void FActorSaveData::SaveActorData(AActor* InActor)
{
	ILimenSaveObjectInterface* SaveObjectInterface = Cast<ILimenSaveObjectInterface>(InActor);
	check(SaveObjectInterface != nullptr);

	TArray<uint8> NewByteData;
	FMemoryWriter Writer(NewByteData);

	FObjectAndNameAsStringProxyArchive Archive(Writer, true);
	Archive.ArIsSaveGame = true;
	
	SaveObjectInterface->PreDataSaved();

	InActor->Serialize(Archive);
	SetByteData(NewByteData);
	SetActorTransform(InActor->GetTransform());
	SetActorName(InActor->GetFName());
	ActorClass = InActor->GetClass();

	SaveObjectInterface->PostDataSaved();
}

void FActorSaveData::LoadActorData(AActor* InActor) const
{
	ILimenSaveObjectInterface* ObjectSaveInterface = Cast<ILimenSaveObjectInterface>(InActor);
	check(ObjectSaveInterface != nullptr);

	FMemoryReader Reader(GetByteData());
	FObjectAndNameAsStringProxyArchive Archive(Reader, true);
	Archive.ArIsSaveGame = true;

	ObjectSaveInterface->PreDataLoaded();
	InActor->Serialize(Archive);
	ObjectSaveInterface->PostDataLoaded();
}

int ULimenSaveData::AddActorSaveData(const FActorSaveData& ActorSaveData)
{
	const int Index = ActorSaveDataList.Find(ActorSaveData);
	if (Index == INDEX_NONE)
	{
		return ActorSaveDataList.Add(ActorSaveData);
	}

	ActorSaveDataList[Index] = ActorSaveData;
	return Index;
}

int ULimenSaveData::AddActorSaveData(AActor* ActorSaveData)
{
	return AddActorSaveData(FActorSaveData(ActorSaveData));
}

int ULimenSaveData::AddObjectSaveData(const FObjectSaveData& ObjectSaveData)
{
	return ObjectSaveDataList.Add(ObjectSaveData);
}

int ULimenSaveData::AddObjectSaveData(UObject* ObjectSaveData)
{
	return AddObjectSaveData(FObjectSaveData(ObjectSaveData));
}

bool ULimenSaveData::GetActorSaveData(const FName& ActorName, FActorSaveData& OutSaveData) const
{
	const FActorSaveData* SaveData = ActorSaveDataList.FindByPredicate([&ActorName] (const FActorSaveData& CurrentActorSaveData) -> bool
	{
		return CurrentActorSaveData.GetActorName() == ActorName;
	});

	if (SaveData == nullptr)
	{
		return false;
	}

	OutSaveData = *SaveData;
	return true;
}

bool ULimenSaveData::GetActorSaveData(const int Index, FActorSaveData& OutSaveData) const
{
	if (!ActorSaveDataList.IsValidIndex(Index))
	{
		return false;
	}
	
	OutSaveData = ActorSaveDataList[Index];
	return true;
}

uint32 ULimenSaveData::GetActorSaveDataCount() const
{
	return ActorSaveDataList.Num();
}

bool ULimenSaveData::GetObjectSaveData(const int32 Index, FObjectSaveData& OutSaveData) const
{
	if (!ObjectSaveDataList.IsValidIndex(Index))
	{
		return false;
	}
	
	OutSaveData = ObjectSaveDataList[Index];
	return true;
}

uint32 ULimenSaveData::GetObjectSaveDataCount() const
{
	return ObjectSaveDataList.Num();
}

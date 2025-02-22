// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include <typeinfo>

#include "Kismet/BlueprintFunctionLibrary.h"
#include "LimenPolymorphicData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct LIMENCORE_API FLimenPolymorphicData
{
	GENERATED_BODY()

public:
	FLimenPolymorphicData();
	~FLimenPolymorphicData();

	template<class T>
	FLimenPolymorphicData& operator=(const T& InData)
	{
		SetDataInternal(InData);
		return *this;
	}

	FLimenPolymorphicData& operator=(const FLimenPolymorphicData& InData);

	template<class T>
	bool ContainsDataType() const
	{
		return DataType == typeid(T);
	}
	
	template<class T>
	operator T() const
	{
		check(ContainsDataType<T>())
		T OutData = *reinterpret_cast<T>(Data.Get());
		return OutData;
	}

	void Serialize(FArchive& Ar);

private:
	FString DataType;
	int32 DataSize;
	TUniquePtr<std::byte[]> Data;

	template<class T>
	void SetDataInternal(const T& InData)
	{
		DataType = typeid(T).name();
		DataSize = sizeof(InData);
		
		Data = MakeUnique<std::byte[]>(DataSize);
		FMemory::Memcpy(Data.Get(), &InData, DataSize);
	}
};

UCLASS()
class ULimenPolymorphicDataUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
};

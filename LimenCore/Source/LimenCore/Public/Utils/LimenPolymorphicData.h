// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include <typeinfo>


/**
 * 
 */
struct LIMENCORE_API FLimenPolymorphicData
{
public:
	FLimenPolymorphicData();
	~FLimenPolymorphicData();
	FLimenPolymorphicData(FLimenPolymorphicData&& Other);
	FLimenPolymorphicData(const FLimenPolymorphicData& Other);

	template<class T>
	FLimenPolymorphicData& operator=(const T& InData)
	{
		if (*this != &InData)
		{
			SetDataInternal(InData);
		}
		return *this;
	}
	FLimenPolymorphicData& operator=(const FLimenPolymorphicData& InData);

	template<class T>
	bool ContainsDataType() const
	{
		return DataType == GetTypeHash(typeid(T).name());
	}
	
	template<class T>
	operator T() const
	{
		return As<T>();
	}

	template<class T>
	T* As()
	{
		static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
		check(ContainsDataType<T>());

		T OutData;
		FMemory::Memcpy(&OutData, Data.Get(), sizeof(T));
		return OutData;
	}

	void Serialize(FArchive& Ar);

private:
	uint32 DataType;
	int32 DataSize;
	TUniquePtr<std::byte[]> Data;

	template<class T>
	void SetDataInternal(const T& InData)
	{
		DataType = GetTypeHash(typeid(T).name());
		DataSize = sizeof(InData);
		
		Data = MakeUnique<std::byte[]>(DataSize);
		FMemory::Memcpy(Data.Get(), &InData, DataSize);
	}
};

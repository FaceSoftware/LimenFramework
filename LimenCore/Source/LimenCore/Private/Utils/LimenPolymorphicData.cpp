// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Utils/LimenPolymorphicData.h"


FLimenPolymorphicData::FLimenPolymorphicData(): DataType(-1), DataSize(-1)
{
}

FLimenPolymorphicData::~FLimenPolymorphicData()
{
}

FLimenPolymorphicData& FLimenPolymorphicData::operator=(const FLimenPolymorphicData& InData)
{
    if (this != &InData)
    {
        DataType = InData.DataType;
        DataSize = InData.DataSize;  // Fix incorrect size calculation

        Data = MakeUnique<std::byte[]>(DataSize);
        FMemory::Memcpy(Data.Get(), InData.Data.Get(), DataSize);
    }
    return *this;
}

void FLimenPolymorphicData::Serialize(FArchive& Ar)
{
	Ar << DataType;  // Serialize type info
	Ar << DataSize;  // Serialize data size

	if (Ar.IsLoading()) // Deserialization
	{
		Data = MakeUnique<std::byte[]>(DataSize);  // Allocate memory
	}

	Ar.Serialize(Data.Get(), DataSize);  // Serialize the actual data bytes
}

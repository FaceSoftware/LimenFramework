// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Utils/LimenOnlineUtils.h"

#include "Misc/AES.h"
#include "Misc/Base64.h"
#include "Misc/SecureHash.h"


FString FLimenOnlineUtils::Encrypt(const TArray<uint8>& Data, const FString& Key)
{	
    if (Key.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Encryption key is empty!"));
        return FString();
    }

    // Generate a 32-byte AES key using SHA1
    TArray<uint8> AESKey;
    AESKey.SetNum(32); // AES requires 32 bytes for the key
    FSHA1::HashBuffer((uint8*)TCHAR_TO_UTF8(*Key), Key.Len(), AESKey.GetData());

    // Pad input data to be a multiple of the AES block size
    TArray<uint8> PaddedData = Data;
    while (PaddedData.Num() % FAES::AESBlockSize != 0)
    {
        PaddedData.Add(0); // Add zero-padding
    }

    // Encrypt the data
    TArray<uint8> EncryptedBytes = PaddedData;
    FAES::EncryptData(EncryptedBytes.GetData(), EncryptedBytes.Num(), AESKey.GetData(), AESKey.Num());

    // Convert encrypted data to Base64
    return FBase64::Encode(EncryptedBytes);
}

TArray<uint8> FLimenOnlineUtils::Decrypt(const FString& Data, const FString& Key)
{
    if (Key.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Decryption key is empty!"));
        return TArray<uint8>();
    }

    // Decode Base64 into bytes
    TArray<uint8> EncryptedBytes;
    if (!FBase64::Decode(Data, EncryptedBytes))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to decode Base64 data!"));
        return TArray<uint8>();
    }

    // Generate a 32-byte AES key using SHA1
    TArray<uint8> AESKey;
    AESKey.SetNum(32); // AES requires 32 bytes for the key
    FSHA1::HashBuffer((uint8*)TCHAR_TO_UTF8(*Key), Key.Len(), AESKey.GetData());

    // Decrypt the data
    TArray<uint8> DecryptedBytes = EncryptedBytes;
    FAES::DecryptData(DecryptedBytes.GetData(), DecryptedBytes.Num(), AESKey.GetData(), AESKey.Num());

    // Remove zero-padding
    while (DecryptedBytes.Num() > 0 && DecryptedBytes.Last() == 0)
    {
        DecryptedBytes.Pop();
    }

    return DecryptedBytes;
}

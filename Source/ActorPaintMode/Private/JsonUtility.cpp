// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonUtility.h"

struct MyStruct
{
public:
	int a;
	int b;
	bool test;
	FString message;
};

FString JsonUtility::GetJson()
{
	MyStruct str;
	str.a = 1;
	str.b = 2;
	str.message = TEXT("Bonjour rr");
	//FJsonSerializer::Serialize(str, )
	return FString();
}

TSharedPtr<FJsonObject> JsonUtility::FromJson(FString str)
{
	//TSharedPtr<FJsonObject> ReturnObj;
	//FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(str), ReturnObj);
	return nullptr;
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ACTORPAINTMODE_API JsonUtility
{
public:
	static FString GetJson();
	static TSharedPtr<FJsonObject> FromJson(FString str);
};

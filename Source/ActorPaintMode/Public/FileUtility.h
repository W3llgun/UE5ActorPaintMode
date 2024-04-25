#pragma once
#include "CoreMinimal.h"
#include "Misc/FileHelper.h"

enum FilePathPreset
{
	SAVED_DIR,
	PROJECT_DIR,
	CONTENT_DIR,
	USER_DIR
};

class FileUtility
{
public:
	static bool CreateFile(const FilePathPreset PathPreset, const FString& Filename, const FString& Content);
	static bool LoadFile(const FString& Filename, FilePathPreset PathPreset, FString& Output);
	static bool IsFileExisting(const FString& Filename, FilePathPreset PathPreset);
	static FString GetPath(FilePathPreset PathPreset);
};

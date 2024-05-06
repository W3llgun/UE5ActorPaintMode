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

/*	Test To create file in Unreal engine to save and load some Editor Data.
 *	A good folder is ./Saved/PluginName/ (It is commonly added to UE gitignore)
 *  No used at the moment.
 */
class FileUtility
{
public:
	static bool CreateFile(const FilePathPreset PathPreset, const FString& Filename, const FString& Content);
	static bool LoadFile(const FString& Filename, FilePathPreset PathPreset, FString& Output);
	static bool IsFileExisting(const FString& Filename, FilePathPreset PathPreset);
	static FString GetPath(FilePathPreset PathPreset);
};

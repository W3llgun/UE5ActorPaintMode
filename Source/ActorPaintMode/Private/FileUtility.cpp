#include "FileUtility.h"
#include "Logging/StructuredLog.h"

//UE_LOGFMT(LogTemp, Warning, "FilePath {value}", str);

bool FileUtility::CreateFile(const FilePathPreset PathPreset, const FString& Filename, const FString& Content)
{
	const FString CompletePath = GetPath(PathPreset) + Filename;

	return FFileHelper::SaveStringToFile(Content, *CompletePath, FFileHelper::EEncodingOptions::ForceUTF8);
}

bool FileUtility::LoadFile(const FString& Filename, FilePathPreset PathPreset, FString& Output)
{
	const FString CompletePath = GetPath(PathPreset) + Filename;
	return FFileHelper::LoadFileToString(Output, *CompletePath);
}

bool FileUtility::IsFileExisting(const FString& Filename, FilePathPreset PathPreset)
{
	const FString CompletePath = GetPath(PathPreset) + Filename;
	return IFileManager::Get().FileExists(*CompletePath);
}

FString FileUtility::GetPath(const FilePathPreset PathPreset)
{
	switch (PathPreset)
	{
	case FilePathPreset::SAVED_DIR: return FPaths::ProjectSavedDir();
	case FilePathPreset::PROJECT_DIR: return FPaths::ProjectDir();
	case FilePathPreset::CONTENT_DIR: return FPaths::ProjectContentDir();
	case FilePathPreset::USER_DIR: return FPaths::ProjectUserDir();
	default: return FPaths::ProjectDir(); // default to project dir
	}
}

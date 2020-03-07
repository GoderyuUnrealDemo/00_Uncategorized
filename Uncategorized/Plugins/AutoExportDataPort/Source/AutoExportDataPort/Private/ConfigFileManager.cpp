
#include "ConfigFileManager.h"
#include "Misc/Paths.h"
UConfigFileManager::UConfigFileManager()
{
    // 这个动态获取不了了。硬编码方式，获取固定路径下的配置文件
    FString ConfigFilePath = FPaths::ProjectConfigDir() + FString(TEXT("AutoExportDataPort/Config.ini"));
    ConfigFile.Read(ConfigFilePath);
}

UConfigFileManager::~UConfigFileManager()
{
}

UConfigFileManager& UConfigFileManager::GetInstance()
{
    static UConfigFileManager Instance;
    return Instance;
}

FString UConfigFileManager::GetValue(const FName& Key, const FString& Section) const
{
    if (!Section.IsEmpty())
    {
        auto TargetSection = ConfigFile.Find(Section);
        if (TargetSection)
        {
            if (TargetSection->Contains(Key))
            {
                return TargetSection->Find(Key)->GetValue().Replace(TEXT("\\n"), TEXT("\n"));
            }
        }
        return "";
    }
    else
    {
        for (const auto& Pair : ConfigFile)
        {
            if (Pair.Value.Contains(Key))
            {
                return Pair.Value.Find(Key)->GetValue().Replace(TEXT("\\n"), TEXT("\n"));
            }
        }
        return "";
    }
    
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "DataPortManager.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
/// 以下两个头文件是用来获取项目目录以及使用文件接口写文件用的
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
/// 包含插件默认头文件，其中定义了自定义Log分类，打印日志用
#include "AutoExportDataPort.h"


// Sets default values
ADataPortManager::ADataPortManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
		PrimaryActorTick.bCanEverTick = true;
}

ADataPortManager* ADataPortManager::GetInstance(const UObject* WorldContextObject)
{
	static ADataPortManager* Instance = nullptr;
	if (!Instance->IsValidLowLevel())
	{
		if (auto It = TActorIterator<ADataPortManager>(WorldContextObject->GetWorld()))
		{
			Instance = *It;
		}
		else
		{
			Instance = WorldContextObject->GetWorld()->SpawnActor<ADataPortManager>();
		}
	}
	return Instance;
}

void ADataPortManager::AddDataPort(const FString& Name, const EDataPortType Type, const EDataPortMode Mode, const FString& Description)
{
	auto It = DataPortsInfo.Find(Name);
	if (!It)
	{
		DataPortsInfo.Emplace(Name, FDataPortInfo(Type, Mode, Description));
	}
}

void ADataPortManager::ExportDataPortsInfo(const FString& SavedFileName)
{
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectConfigDir()) + "AutoExportDataPort/" + SavedFileName;
	FString FileContent = ANSI_TO_TCHAR("ChannelName,Type,Mode\r\n");
	/// 按通道名字母正序排序
	DataPortsInfo.KeyStableSort([](const FString& A, const FString& B) {
		return A < B;
	});

	for (const auto& DataPortInfo : DataPortsInfo)
	{
		if (DataPortInfo.Key.IsEmpty())
		{
			++EmptyDataPortNameCount;
			continue;
		}
		auto Type = GetDataPortTypeString(DataPortInfo.Value.Type);
		auto Mode = GetDataPortModeString(DataPortInfo.Value.Mode);
		FileContent += (DataPortInfo.Key + "," + Type + "," + Mode);
		FileContent += "\r\n";
	}
	FFileHelper::SaveStringToFile(FileContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), EFileWrite::FILEWRITE_NoReplaceExisting);
	if (EmptyDataPortNameCount > 0)
	{
		UE_LOG(LogAutoExportDataPort, Warning, TEXT("输出通道权限表时检测到%d个空通道，已过滤"), EmptyDataPortNameCount);
	}
	// 通知对通道权限信息表已生成完毕这一消息感兴趣的模块，并提供给它们一个生成文件路径
	OnDataPortInfoFileGenerated.Broadcast(FilePath);
}

void ADataPortManager::IntegrateDataPorts_Implementation()
{
	// 整合额外填表的通道信息
	TArray<AActor*> FindedActors; 
	UGameplayStatics::GetAllActorsOfClass(this, AExtraDataPortInfo::StaticClass(), OUT FindedActors);
	for (const auto& Actor : FindedActors)
	{
		if (auto ExtraDataPortInfoActor = Cast<AExtraDataPortInfo>(Actor))
		{
			ExtraDataPortInfoActor->IntegrateDataPorts(OUT DataPortsInfo);
		}
		else
		{
			UE_LOG(LogAutoExportDataPort, Warning, TEXT("获取所有ExtraDataPortInfo的Actor时从AActor转型到AExtraDataPortInfo失败一个，该Actor所在关卡为%s"), *Actor->GetLevel()->GetName());
		}
	}

	// TODO 将本管理类添加的默认通道整合，根据类型区分（视景只读/可操作读写）
	// 整合分权限的通道
	FString FileString;
	FString FileName = FPaths::ConvertRelativePathToFull(FPaths::ProjectConfigDir()) + "AutoExportDataPort/zsj.csv"; 
	FFileHelper::LoadFileToString(OUT FileString, *FileName);
	TArray<FString> DataPortInfoStringArray;
	FileString.ParseIntoArray(OUT DataPortInfoStringArray, TEXT("\r\n"));
	// 从索引一开始，将CSV文件中的表头行跳过
	for (int32 i = 1; i < DataPortInfoStringArray.Num(); ++i)
	{
		TArray<FString> DataPortInfoItemStringArray;
		DataPortInfoStringArray[i].ParseIntoArray(OUT DataPortInfoItemStringArray, TEXT(","));
		if (DataPortInfoItemStringArray.Num() < 3)
		{
			UE_LOG(LogAutoExportDataPort, Warning, TEXT("分权限信息表中有数据未填，其将不被导出发送至服务器，请及时检查错误"));
			continue;
		}
		// 目前服务器只要前三列，这里直接取前三列，以后调整了再说
		auto Name = DataPortInfoItemStringArray[0];
		auto Type = GetDataPortTypeEnum(DataPortInfoItemStringArray[1]);
		auto Mode = GetDataPortModeEnum(DataPortInfoItemStringArray[2]);
		if (DataPortsInfo.Contains(Name))
		{
			UE_LOG(LogAutoExportDataPort, Warning, TEXT("编辑器内已经填写%s通道，已将分权限信息表中的通道覆盖编辑器填写的通道信息"), *Name);
		}
		DataPortsInfo.Emplace(Name, FDataPortInfo(Type, Mode));
	}

	// 整合固定通道
	for (const auto& FixedDataPortInfo : FixedDataPortsInfo)
	{
		if (DataPortsInfo.Contains(FixedDataPortInfo.Key))
		{
			UE_LOG(LogAutoExportDataPort, Warning, TEXT("添加默认固定通道时发现已有重名通道%s，但已将其覆盖"), *FixedDataPortInfo.Key);
		}
		DataPortsInfo.Emplace(FixedDataPortInfo.Key, FixedDataPortInfo.Value);
	}
}

// Called when the game starts or when spawned
void ADataPortManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADataPortManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const FString ADataPortManager::GetDataPortTypeString(const EDataPortType& Type) const
{
	switch (Type)
	{
	case EDataPortType::DPT_BOOL:
		return FString(TEXT("Bool"));
	case EDataPortType::DPT_INTEGER:
		return FString(TEXT("Integer"));
	case EDataPortType::DPT_REAL:
		return FString(TEXT("Real"));
	case EDataPortType::DPT_STRING:
		return FString(TEXT("String"));
	default:
		return FString(TEXT("Unknown"));
	}
}

const FString ADataPortManager::GetDataPortModeString(const EDataPortMode& Mode) const
{
	switch (Mode)
	{
	case EDataPortMode::DPM_READONLY:
		return FString(TEXT("Input"));
	case EDataPortMode::DPM_WRITEONLY:
		return FString(TEXT("Output"));
	case EDataPortMode::DPM_READWRITE:
		// TODO 现在读写和只读没有区分，只要是Output即为读写权限
		return FString(TEXT("Output"));
	default:
		return FString(TEXT("Unknown"));
	}
}

const EDataPortType ADataPortManager::GetDataPortTypeEnum(const FString& Type) const
{
	if (Type.Equals(TEXT("String"), ESearchCase::IgnoreCase))
		return EDataPortType::DPT_STRING;
	else if (Type.Equals(TEXT("Integer"), ESearchCase::IgnoreCase))
		return EDataPortType::DPT_INTEGER;
	else if (Type.Equals(TEXT("Real"), ESearchCase::IgnoreCase))
		return EDataPortType::DPT_REAL;
	else if (Type.Equals(TEXT("Bool"), ESearchCase::IgnoreCase))
		return EDataPortType::DPT_BOOL;
	else
	{
		UE_LOG(LogAutoExportDataPort, Warning, TEXT("分权限信息表中有通道类型不正确，值为%s，已默认为其分配字符串类型"), *Type);
		return EDataPortType::DPT_STRING;
	}
}

const EDataPortMode ADataPortManager::GetDataPortModeEnum(const FString& Mode) const
{
	if (Mode.Equals(TEXT("Input"), ESearchCase::IgnoreCase))
		return EDataPortMode::DPM_READONLY;
	else if (Mode.Equals(TEXT("Output"), ESearchCase::IgnoreCase))
		return EDataPortMode::DPM_READWRITE;
	else
	{
		UE_LOG(LogAutoExportDataPort, Warning, TEXT("分权限信息表中有通道权限不正确，值为%s，已默认为其分配只读权限"), *Mode);
		return EDataPortMode::DPM_READONLY;
	}
}
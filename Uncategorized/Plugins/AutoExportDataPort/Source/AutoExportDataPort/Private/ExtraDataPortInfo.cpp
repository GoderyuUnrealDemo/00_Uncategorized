// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtraDataPortInfo.h"
/// 包含插件默认头文件，其中定义了自定义Log分类，打印日志用
#include "AutoExportDataPort.h"

// Sets default values
AExtraDataPortInfo::AExtraDataPortInfo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AExtraDataPortInfo::IntegrateDataPorts(OUT TMap<FString, FDataPortInfo>& DataPortsInfo)
{
	AppendDataPort(DataPortsInfo, StringReadOnlyDataPorts, EDataPortType::DPT_STRING, EDataPortMode::DPM_READONLY);
	AppendDataPort(DataPortsInfo, IntegerReadOnlyDataPorts, EDataPortType::DPT_INTEGER, EDataPortMode::DPM_READONLY);
	AppendDataPort(DataPortsInfo, RealReadOnlyDataPorts, EDataPortType::DPT_REAL, EDataPortMode::DPM_READONLY);
	AppendDataPort(DataPortsInfo, BoolReadOnlyDataPorts, EDataPortType::DPT_BOOL, EDataPortMode::DPM_READONLY);

	AppendDataPort(DataPortsInfo, StringReadWriteDataPorts, EDataPortType::DPT_STRING, EDataPortMode::DPM_READWRITE);
	AppendDataPort(DataPortsInfo, IntegerReadWriteDataPorts, EDataPortType::DPT_INTEGER, EDataPortMode::DPM_READWRITE);
	AppendDataPort(DataPortsInfo, RealReadWriteDataPorts, EDataPortType::DPT_REAL, EDataPortMode::DPM_READWRITE);
	AppendDataPort(DataPortsInfo, BoolReadWriteDataPorts, EDataPortType::DPT_BOOL, EDataPortMode::DPM_READWRITE);
}

void AExtraDataPortInfo::AppendDataPort(
	OUT TMap<FString, FDataPortInfo>& DataPortsInfo,
	const TMap<FString, FString>& CategorizedDataPorts,
	const EDataPortType& Type,
	const EDataPortMode& Mode)
{
	for (auto& NameAndDescription : CategorizedDataPorts)
	{
		if (NameAndDescription.Key.IsEmpty())
		{
			// TODO 想在有空通道时提醒用户哪一个表项出现的空通道
			// for (TFieldIterator<UProperty> ProIt(AExtraDataPortInfo::StaticClass()); ProIt; ++ProIt)
			// {
			// 	auto Pro = *ProIt;
			// 	FString ValName = FString(VALNAME(CategorizedDataPorts));
			// 	if (Pro->GetNameCPP().Equals(ValName))
			// 	{
			// 		if (auto StrPro = Cast<UStrProperty>(Pro))
			// 		{
			// 			StrPro->GetDisplayNameText();
			// 		}
			// 	}
			// }
			UE_LOG(LogAutoExportDataPort, Warning, TEXT("关卡%s的额外通道权限配置填表存在空通道"), *(this->GetLevel()->GetOuter()->GetName()));
			continue;
		}
		auto PtrFindedItem = DataPortsInfo.Find(NameAndDescription.Key);
		if (PtrFindedItem)
		{
			UE_LOG(LogAutoExportDataPort, Warning, TEXT("关卡%s的额外通道权限配置填表存在重名通道%s"), *(this->GetLevel()->GetOuter()->GetName()),*(NameAndDescription.Key));
		}
		else
		{
			DataPortsInfo.Emplace(NameAndDescription.Key, FDataPortInfo(Type, Mode, NameAndDescription.Value));
		}
	}
}

// Called when the game starts or when spawned
void AExtraDataPortInfo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExtraDataPortInfo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


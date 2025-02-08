#include "MayRecoilDataCustomization.h"
#include "Core/Data/MayRecoilData.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Engine/Texture2D.h"
#include "PropertyHandle.h"
#include "Widgets/SOverlay.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include <xmmintrin.h>
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"


TSharedRef<IDetailCustomization> FMayRecoilDataCustomization::MakeInstance()
{
	return MakeShareable(new FMayRecoilDataCustomization);
}

void FMayRecoilDataCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);

	if (Objects.Num() == 1 && Objects[0]->IsA<UMayRecoilData>())
	{
		DataAssetPtr = Cast<UMayRecoilData>(Objects[0].Get());
	}

	DetailBuilderInit = &DetailBuilder;

	static const TArray<FName> RecoilProperties = {
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, MinRecoilVerticalStrength),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, MaxRecoilVerticalStrength),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, MinRecoilHorizontalStrength),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, MaxRecoilHorizontalStrength),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, ForceMinMaxVerticalStrength),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, ForceMinMaxHorizontalStrength),
		// GET_MEMBER_NAME_CHECKED(UMayRecoilData, UseStaticPattern),
		// GET_MEMBER_NAME_CHECKED(UMayRecoilData, StaticPattern),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, OverrideScaleSettings),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, RecoilScale),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, RecoilScaleSprint),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, RecoilScaleCrouch),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, RecoilScaleJump),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, RecoilScaleADS),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, NumShots),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, Iterations),
		GET_MEMBER_NAME_CHECKED(UMayRecoilData, BulletRadius),
	};

	for (const FName& PropertyName : RecoilProperties)
	{
		TSharedRef<IPropertyHandle> PropertyHandle = DetailBuilder.GetProperty(PropertyName, UMayRecoilData::StaticClass());

		if (PropertyHandle->IsValidHandle())
		{
			PropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FMayRecoilDataCustomization::OnAnyPropertyChanged));
		}
	}
	
	RebuildRecoilTextureFromData();
	
	if (!RenderedImageBrush.IsValid())
	{
		static int32 UniqueBrushId = 0;
		FName BrushName = FName(*FString::Printf(TEXT("MayRecoilDataPreview_%d"), UniqueBrushId++));

		RenderedImageBrush = MakeShareable(
			new FSlateDynamicImageBrush((UTexture2D*)nullptr, FVector2D(512, 512), BrushName)
		);

		if (GeneratedTexture)
		{
			if (GeneratedTexture) GeneratedTexture->AddToRoot(); // Prevent garbage collection
			RenderedImageBrush->SetResourceObject(GeneratedTexture);
		}
	}
	
	IDetailCategoryBuilder& PreviewCategory = DetailBuilder.EditCategory(
		TEXT("Preview"),
		FText::FromString("Preview"),
		ECategoryPriority::Default
	);

	PreviewCategory.AddCustomRow(FText::FromString("Preview"))
	[
		SNew(SBox)
		.WidthOverride(512)
		.HeightOverride(512)
		.MaxAspectRatio(1.0f)
		[
			SNew(SImage)
			.Image(RenderedImageBrush.Get())
		]
	];
}

void FMayRecoilDataCustomization::OnAnyPropertyChanged()
{
	RebuildRecoilTextureFromData();
}

void FMayRecoilDataCustomization::GenerateRecoilPattern_SIMD(const int32 TexSize, TArray<FColor>& PixelData)
{
    const int32 NumShots = DataAssetPtr->NumShots;
    const int32 Iterations = DataAssetPtr->Iterations;

    const float CenterX = TexSize * 0.5f;
    const float CenterY = TexSize * 0.5f;

    const float MinVertical = DataAssetPtr->MinRecoilVerticalStrength;
    const float MaxVertical = DataAssetPtr->MaxRecoilVerticalStrength;
    const float MinHorizontal = DataAssetPtr->MinRecoilHorizontalStrength;
    const float MaxHorizontal = DataAssetPtr->MaxRecoilHorizontalStrength;

    const bool bForceMinMaxVertical = DataAssetPtr->ForceMinMaxVerticalStrength;
    const bool bForceMinMaxHorizontal = DataAssetPtr->ForceMinMaxHorizontalStrength;

    const __m128 factor = _mm_set1_ps(5.0f);
    const __m128 zero   = _mm_set1_ps(0.0f);
    const __m128 maxCoord = _mm_set1_ps(static_cast<float>(TexSize - 1));

    for (int32 Iter = 0; Iter < Iterations; ++Iter)
    {
        __m128 currentX = _mm_set1_ps(CenterX);
        __m128 currentY = _mm_set1_ps(CenterY);
    	
        for (int32 Shot = 0; Shot < NumShots; Shot += 4)
        {
            float randVertArray[4];
            float randHorzArray[4];
            for (int i = 0; i < 4; ++i)
            {
                if (bForceMinMaxVertical)
                {
                    randVertArray[i] = FMath::RandBool() ? MaxVertical : MinVertical;
                }
                else
                {
                    randVertArray[i] = FMath::FRandRange(MinVertical, MaxVertical);
                }
                if (bForceMinMaxHorizontal)
                {
                    randHorzArray[i] = FMath::RandBool() ? MaxHorizontal : MinHorizontal;
                }
                else
                {
                    randHorzArray[i] = FMath::FRandRange(MinHorizontal, MaxHorizontal);
                }
            }
            __m128 randomVert = _mm_loadu_ps(randVertArray);
            __m128 randomHorz = _mm_loadu_ps(randHorzArray);
        	
            currentX = _mm_add_ps(currentX, _mm_mul_ps(randomHorz, factor));
            currentY = _mm_sub_ps(currentY, _mm_mul_ps(randomVert, factor));

            currentX = _mm_min_ps(_mm_max_ps(currentX, zero), maxCoord);
            currentY = _mm_min_ps(_mm_max_ps(currentY, zero), maxCoord);
        	
            alignas(16) float xArray[4];
            alignas(16) float yArray[4];
            _mm_store_ps(xArray, currentX);
            _mm_store_ps(yArray, currentY);

            for (int i = 0; i < 4; ++i)
            {
                FColor ShotColor = GetGradientColor(Shot + i, NumShots);
                int intX = FMath::RoundToInt(xArray[i]);
                int intY = FMath::RoundToInt(yArray[i]);
                DrawCircleOnTexture(PixelData, TexSize, intX, intY, DataAssetPtr->BulletRadius, ShotColor);
            }
        }
    }
}

void FMayRecoilDataCustomization::GenerateRecoilPattern(const int32 TexSize, TArray<FColor>& PixelData)
{
	const int32 NumShots = DataAssetPtr->NumShots;
	const int32 Iterations = DataAssetPtr->Iterations;

	const float CenterX = TexSize * 0.5f;
	const float CenterY = TexSize * 0.5f;

	const float MinVertical = DataAssetPtr->MinRecoilVerticalStrength;
	const float MaxVertical = DataAssetPtr->MaxRecoilVerticalStrength;

	const float MinHorizontal = DataAssetPtr->MinRecoilHorizontalStrength;
	const float MaxHorizontal = DataAssetPtr->MaxRecoilHorizontalStrength;

	const bool bForceMinMaxVertical = DataAssetPtr->ForceMinMaxVerticalStrength;
	const bool bForceMinMaxHorizontal = DataAssetPtr->ForceMinMaxHorizontalStrength;
	
	for (int32 Iter = 0; Iter < Iterations; ++Iter)
	{
		float CurrentX = CenterX;
		float CurrentY = CenterY;

		for (int32 Shot = 0; Shot < NumShots; ++Shot)
		{
			FColor ShotColor = GetGradientColor(Shot, NumShots);
			
			DrawCircleOnTexture(PixelData, TexSize, FMath::RoundToInt(CurrentX), FMath::RoundToInt(CurrentY), DataAssetPtr->BulletRadius, ShotColor);
			
			float RandomVert = bForceMinMaxVertical ? (FMath::RandBool() ? MaxVertical : MinVertical) : FMath::FRandRange(MinVertical, MaxVertical);
			float RandomHorz = bForceMinMaxHorizontal ? (FMath::RandBool() ? MaxHorizontal : MinHorizontal) : FMath::FRandRange(MinHorizontal, MaxHorizontal);
			
			CurrentX += RandomHorz * 5.0f;
			CurrentY -= RandomVert * 5.0f;
			
			CurrentX = FMath::Clamp(CurrentX, 0.0f, static_cast<float>(TexSize - 1));
			CurrentY = FMath::Clamp(CurrentY, 0.0f, static_cast<float>(TexSize - 1));
		}
	}
}

void FMayRecoilDataCustomization::RebuildRecoilTextureFromData()
{
    if (!DataAssetPtr) return;
	
    constexpr int32 TexSize = 512;
    TArray<FColor> PixelData;
    PixelData.SetNumUninitialized(TexSize * TexSize);
	
    for (FColor& Color : PixelData)
    {
        Color = FColor::Black;
    }
	
    FString ImagePath = IPluginManager::Get().FindPlugin(TEXT("MaySimpleRecoil"))->GetContentDir() + TEXT("/Textures/target2.png");
    TArray<uint8> FileData;

    if (FFileHelper::LoadFileToArray(FileData, *ImagePath))
    {
        IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
        EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(FileData.GetData(), FileData.Num());

        if (ImageFormat != EImageFormat::Invalid)
        {
            TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

            if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(FileData.GetData(), FileData.Num()))
            {
                TArray<uint8> RawData;
                if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
                {
                    if (RawData.Num() == TexSize * TexSize * 4)
                    {
                        FMemory::Memcpy(PixelData.GetData(), RawData.GetData(), RawData.Num());
                    }
                }
            }
        }
    }
	
 //    if (DataAssetPtr->UseStaticPattern)
	// {
 //    	for (int32 Shot = 0; Shot < DataAssetPtr->StaticPattern.Num(); ++Shot)
	// 	{
	// 		FColor ShotColor = GetGradientColor(Shot, DataAssetPtr->StaticPattern.Num());
	// 		DrawHollowEllipseOnTexture(PixelData, TexSize,
	// 			DataAssetPtr->StaticPattern[Shot].Point.X, DataAssetPtr->StaticPattern[Shot].Point.Y,
	// 			10, 10, ShotColor);
	// 	}
	// }
	// else
	// {
		GenerateRecoilPattern(TexSize, PixelData);
	// }
	
    if (!GeneratedTexture)
    {
        GeneratedTexture = UTexture2D::CreateTransient(TexSize, TexSize, PF_B8G8R8A8);
    	if (GeneratedTexture) GeneratedTexture->AddToRoot(); // This is important so the texture is not garbage collected
    }

	if (GeneratedTexture)
	{
		FScopeLock Lock(&BulkDataCriticalSection);
		void* TextureData = GeneratedTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, PixelData.GetData(), PixelData.Num() * sizeof(FColor));
		GeneratedTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
		GeneratedTexture->UpdateResource();
		if (RenderedImageBrush) RenderedImageBrush->SetResourceObject(GeneratedTexture);
	}
}

FColor FMayRecoilDataCustomization::GetGradientColor(int32 ShotIndex, int32 TotalShots)
{
	float Progress = static_cast<float>(ShotIndex) / static_cast<float>(TotalShots - 1);
	uint8 Red = 255, Green = 255;
	if (Progress < 0.5f)
	{
		Red = 255 * (Progress / 0.5f);
	}
	else
	{
		Green = 255 - (255 * ((Progress - 0.5f) / 0.5f));
	}
	return FColor(Red, Green, 0);
}

void FMayRecoilDataCustomization::DrawCircleOnTexture(
    TArray<FColor>& PixelData,
    int32 TexSize,
    int32 CenterX, 
    int32 CenterY, 
    int32 Radius, 
    FColor Color)
{
    for (int32 y = -Radius; y <= Radius; ++y)
    {
        for (int32 x = -Radius; x <= Radius; ++x)
        {
            const int32 CheckX = CenterX + x;
            const int32 CheckY = CenterY + y;

            if (CheckX < 0 || CheckX >= TexSize || CheckY < 0 || CheckY >= TexSize)
            {
                continue;
            }

            if ((x * x + y * y) <= (Radius * Radius))
            {
                PixelData[CheckY * TexSize + CheckX] = Color;
            }
        }
    }
}

void FMayRecoilDataCustomization::DrawHollowCircleOnTexture(TArray<FColor>& PixelData, int32 TexSize, int32 CenterX,
	int32 CenterY, int32 Radius, FColor Color)
{
	for (int32 y = -Radius; y <= Radius; ++y)
	{
		for (int32 x = -Radius; x <= Radius; ++x)
		{
			const int32 CheckX = CenterX + x;
			const int32 CheckY = CenterY + y;

			if (CheckX < 0 || CheckX >= TexSize || CheckY < 0 || CheckY >= TexSize)
			{
				continue;
			}

			if ((x * x + y * y) <= (Radius * Radius) && (x * x + y * y) >= ((Radius - 1) * (Radius - 1)))
			{
				PixelData[CheckY * TexSize + CheckX] = Color;
			}
		}
	}
}

void FMayRecoilDataCustomization::DrawHollowEllipseOnTexture(TArray<FColor>& PixelData, int32 TexSize, int32 CenterX,
	int32 CenterY, int32 RadiusX, int32 RadiusY, FColor Color)
{
	for (int32 y = -RadiusY; y <= RadiusY; ++y)
	{
		for (int32 x = -RadiusX; x <= RadiusX; ++x)
		{
			const int32 CheckX = CenterX + x;
			const int32 CheckY = CenterY + y;

			if (CheckX < 0 || CheckX >= TexSize || CheckY < 0 || CheckY >= TexSize)
			{
				continue;
			}

			if ((x * x) / (RadiusX * RadiusX) + (y * y) / (RadiusY * RadiusY) <= 1.0f &&
				(x * x) / ((RadiusX - 1) * (RadiusX - 1)) + (y * y) / ((RadiusY - 1) * (RadiusY - 1)) > 1.0f)
			{
				PixelData[CheckY * TexSize + CheckX] = Color;
			}
		}
	}
}

void FMayRecoilDataCustomization::DrawHollowEllipseOnTextureWithMinMaxRadiusOnEachAxis(TArray<FColor>& PixelData,
	int32 TexSize, int32 CenterX, int32 CenterY, int32 MinRadiusX, int32 MaxRadiusX, int32 MinRadiusY, int32 MaxRadiusY,
	FColor Color)
{
	for (int32 y = -MaxRadiusY; y <= MaxRadiusY; ++y)
	{
		for (int32 x = -MaxRadiusX; x <= MaxRadiusX; ++x)
		{
			const int32 CheckX = CenterX + x;
			const int32 CheckY = CenterY + y;

			if (CheckX < 0 || CheckX >= TexSize || CheckY < 0 || CheckY >= TexSize)
			{
				continue;
			}

			if ((x * x) / (MaxRadiusX * MaxRadiusX) + (y * y) / (MaxRadiusY * MaxRadiusY) <= 1.0f &&
				(x * x) / (MinRadiusX * MinRadiusX) + (y * y) / (MinRadiusY * MinRadiusY) > 1.0f)
			{
				PixelData[CheckY * TexSize + CheckX] = Color;
			}
		}
	}
}
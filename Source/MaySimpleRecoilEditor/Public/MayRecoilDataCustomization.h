#pragma once

#include "IDetailCustomization.h"
#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SlateFwd.h"
#include "HAL/CriticalSection.h"

class UMayRecoilData;
struct FSlateDynamicImageBrush;

/**
 * Custom-Detail-Klasse für UMayRecoilData
 */
class FMayRecoilDataCustomization : public IDetailCustomization
{
public:
	/** Statische Fabrikmethode für die Unreal Engine */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization Interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	/** Wird aufgerufen, wenn eine Property geändert wurde */
	void OnAnyPropertyChanged();

	void GenerateRecoilPattern_SIMD(const int32 TexSize, TArray<FColor>& PixelData);
	void GenerateRecoilPattern(const int32 TexSize, TArray<FColor>& PixelData);

	void RebuildRecoilTextureFromData();

	FColor GetGradientColor(int32 ShotIndex, int32 TotalShots);
	
	void DrawCircleOnTexture(TArray<FColor>& PixelData, int32 TexSize, int32 CenterX, int32 CenterY, int32 Radius, FColor Color);
	void DrawHollowCircleOnTexture(TArray<FColor>& PixelData, int32 TexSize, int32 CenterX, int32 CenterY, int32 Radius, FColor Color);
	void DrawHollowEllipseOnTexture(TArray<FColor>& PixelData, int32 TexSize, int32 CenterX, int32 CenterY, int32 RadiusX, int32 RadiusY, FColor Color);
	void DrawHollowEllipseOnTextureWithMinMaxRadiusOnEachAxis(TArray<FColor>& PixelData, int32 TexSize, int32 CenterX, int32 CenterY, int32 MinRadiusX, int32 MaxRadiusX, int32 MinRadiusY, int32 MaxRadiusY, FColor Color);
	/** Zeiger auf das aktuell bearbeitete DataAsset */
	UMayRecoilData* DataAssetPtr = nullptr;

	IDetailLayoutBuilder* DetailBuilderInit = nullptr;

	/** Die transiente Texture, die als Vorschau gerendert wird */
	UTexture2D* GeneratedTexture = nullptr;

	/** Slate-Brush, der das Bild anzeigt */
	TSharedPtr<FSlateDynamicImageBrush> RenderedImageBrush;

	/** Kritischer Abschnitt für den synchronisierten BulkData-Zugriff */
	FCriticalSection BulkDataCriticalSection;
};
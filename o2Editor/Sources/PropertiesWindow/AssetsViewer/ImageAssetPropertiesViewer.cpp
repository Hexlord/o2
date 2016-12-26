#include "ImageAssetPropertiesViewer.h"

#include "Assets/ImageAsset.h"
#include "UI/VerticalLayout.h"
#include "UI/Widget.h"
#include "Utils/Reflection/Reflection.h"

namespace Editor
{
	ImageAssetPropertiesViewer::ImageAssetPropertiesViewer()
	{
		mContent = mnew UIVerticalLayout();
	}

	ImageAssetPropertiesViewer::~ImageAssetPropertiesViewer()
	{
		delete mContent;
	}

	void ImageAssetPropertiesViewer::SetTargetAssets(const Vector<Asset*>& assets)
	{

	}

	const Type* ImageAssetPropertiesViewer::GetAssetType() const
	{
		return &TypeOf(ImageAsset);
	}

	UIWidget* ImageAssetPropertiesViewer::GetWidget() const
	{
		return mContent;
	}
}
 
CLASS_META(Editor::ImageAssetPropertiesViewer)
{
	BASE_CLASS(Editor::IAssetPropertiesViewer);

	PROTECTED_FIELD(mContent);

	PUBLIC_FUNCTION(void, SetTargetAssets, const Vector<Asset*>&);
	PUBLIC_FUNCTION(const Type*, GetAssetType);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
}
END_META;
 
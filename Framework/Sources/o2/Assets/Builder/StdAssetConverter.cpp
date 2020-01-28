#include "o2/stdafx.h"
#include "StdAssetConverter.h"

#include "o2/Assets/Assets.h"
#include "o2/Assets/AssetTypes/BinaryAsset.h"
#include "o2/Assets/Builder/AssetsBuilder.h"
#include "o2/Assets/AssetTypes/ImageAsset.h"
#include "o2/Utils/FileSystem/FileSystem.h"

namespace o2
{
	Vector<const Type*> StdAssetConverter::GetProcessingAssetsTypes() const
	{
		Vector<const Type*> res;
		res.Add(&TypeOf(BinaryAsset));
		return res;
	}

	void StdAssetConverter::ConvertAsset(const AssetInfo& node)
	{
		String sourceAssetPath = mAssetsBuilder->GetSourceAssetsPath() + node.path;
		String buildedAssetPath = mAssetsBuilder->GetBuiltAssetsPath() + node.path;

		o2FileSystem.FileCopy(sourceAssetPath, buildedAssetPath);
		o2FileSystem.SetFileEditDate(buildedAssetPath, node.editTime);
	}

	void StdAssetConverter::RemoveAsset(const AssetInfo& node)
	{
		String buildedAssetPath = mAssetsBuilder->GetBuiltAssetsPath() + node.path;

		o2FileSystem.FileDelete(buildedAssetPath);
	}

	void StdAssetConverter::MoveAsset(const AssetInfo& nodeFrom, const AssetInfo& nodeTo)
	{
		String fullPathFrom = mAssetsBuilder->GetBuiltAssetsPath() + nodeFrom.path;
		String fullPathTo = mAssetsBuilder->GetBuiltAssetsPath() + nodeTo.path;

		o2FileSystem.FileMove(fullPathFrom, fullPathTo);
	}
}

DECLARE_CLASS(o2::StdAssetConverter);

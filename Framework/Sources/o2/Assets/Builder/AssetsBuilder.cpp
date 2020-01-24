#include "o2/stdafx.h"
#include "AssetsBuilder.h"

#include "o2/Assets/Assets.h"
#include "o2/Assets/AtlasAsset.h"
#include "o2/Assets/Builder/AtlasAssetConverter.h"
#include "o2/Assets/Builder/FolderAssetConverter.h"
#include "o2/Assets/Builder/ImageAssetConverter.h"
#include "o2/Assets/FolderAsset.h"
#include "o2/Utils/Debug/Debug.h"
#include "o2/Utils/Debug/Log/LogStream.h"
#include "o2/Utils/FileSystem/FileSystem.h"
#include "o2/Utils/System/Time/Timer.h"

namespace o2
{
	AssetsBuilder::AssetsBuilder()
	{
		mLog = mnew LogStream("Assets builder");
		o2Debug.GetLog()->BindStream(mLog);

		InitializeConverters();
	}

	AssetsBuilder::~AssetsBuilder()
	{
		Reset();
	}

	Vector<UID> AssetsBuilder::BuildAssets(const String& assetsPath, const String& dataAssetsPath, const String& dataAssetsTreePath,
										   bool forcible /*= false*/)
	{
		Reset();

		mSourceAssetsPath = assetsPath;
		mBuiltAssetsPath = dataAssetsPath;
		mBuiltAssetsTreePath = dataAssetsTreePath;

		mLog->Out("Started assets building from: " + mSourceAssetsPath + " to: " + mBuiltAssetsPath);

		Timer timer;

		if (forcible)
			RemoveBuiltAssets();

		CheckBasicAtlas();

		FolderInfo folderInfo = o2FileSystem.GetFolderInfo(mSourceAssetsPath);
		folderInfo.ClampPathNames();

		ProcessMissingMetasCreation(folderInfo);

		mSourceAssetsTree.Build(folderInfo);
		mBuiltAssetsTree.DeserializeFromString(o2FileSystem.ReadFile(mBuiltAssetsTreePath));

		Vector<UID> modifiedAssets;
		modifiedAssets.Add(ProcessModifiedAssets());
		modifiedAssets.Add(ProcessRemovedAssets());
		modifiedAssets.Add(ProcessNewAssets());
		modifiedAssets.Add(ConvertersPostProcess());

		o2FileSystem.WriteFile(mBuiltAssetsTreePath, mBuiltAssetsTree.SerializeToString());

		mLog->Out("Completed for " + (String)timer.GetDeltaTime() + " seconds");

		return modifiedAssets;
	}

	const String& AssetsBuilder::GetSourceAssetsPath() const
	{
		return mSourceAssetsPath;
	}

	const String& AssetsBuilder::GetBuiltAssetsPath() const
	{
		return mBuiltAssetsPath;
	}

	void AssetsBuilder::InitializeConverters()
	{
		auto converterTypes = TypeOf(IAssetConverter).GetDerivedTypes();
		for (auto converterType : converterTypes)
		{
			IAssetConverter* converter = (IAssetConverter*)converterType->CreateSample();
			converter->SetAssetsBuilder(this);
			auto availableAssetTypes = converter->GetProcessingAssetsTypes();
			for (auto tp : availableAssetTypes)
				mAssetConverters.Add(tp, converter);
		}

		mStdAssetConverter.SetAssetsBuilder(this);
	}

	void AssetsBuilder::RemoveBuiltAssets()
	{
		o2FileSystem.FolderRemove(mBuiltAssetsPath);
		o2FileSystem.FolderCreate(mBuiltAssetsPath);
	}

	void AssetsBuilder::CheckBasicAtlas()
	{
		String basicAtlasFullPath = mSourceAssetsPath + GetBasicAtlasPath();

		if (!o2FileSystem.IsFileExist(basicAtlasFullPath))
		{
			AtlasAssetRef basicAtlas = AtlasAssetRef::CreateAsset();
			basicAtlas->Save(GetBasicAtlasPath(), false);
		}
	}

	void AssetsBuilder::ProcessMissingMetasCreation(FolderInfo& folder)
	{
		for (auto fileInfo : folder.files)
		{
			String extension = o2FileSystem.GetFileExtension(fileInfo.path);

			if (extension == "meta")
			{
				String metaFullPath = mSourceAssetsPath + fileInfo.path;
				String assetForMeta = o2FileSystem.GetFileNameWithoutExtension(metaFullPath);
				bool isExistAssetForMeta = o2FileSystem.IsFileExist(assetForMeta) || o2FileSystem.IsFolderExist(assetForMeta);
				if (!isExistAssetForMeta)
				{
					mLog->Warning("Missing asset for meta: " + fileInfo.path + " - removing meta");
					o2FileSystem.FileDelete(metaFullPath);
				}
			}
			else
			{
				String assetFullPath = mSourceAssetsPath + fileInfo.path;
				String metaFullPath = assetFullPath + ".meta";
				bool isExistMetaForAsset = o2FileSystem.IsFileExist(metaFullPath);
				if (!isExistMetaForAsset)
				{
					auto assetType = o2Assets.GetAssetTypeByExtension(extension);
					GenerateMeta(*assetType, metaFullPath);
				}
			}
		}

		for (auto subFolder : folder.folders)
		{
			String folderFullPath = mSourceAssetsPath + subFolder.path;
			String metaFullPath = folderFullPath + ".meta";
			bool isExistMetaForFolder = o2FileSystem.IsFileExist(metaFullPath);
			if (!isExistMetaForFolder)
			{
				auto& assetType = TypeOf(FolderAsset);
				GenerateMeta(assetType, metaFullPath);
			}

			ProcessMissingMetasCreation(subFolder);
		}
	}

	Vector<UID> AssetsBuilder::ProcessRemovedAssets()
	{
		Vector<UID> modifiedAssets;
		const Type* folderTypeId = &TypeOf(FolderAsset);

		mBuiltAssetsTree.SortAssetsInverse();

		// in first pass skipping folders (only files), in second - files
		for (int pass = 0; pass < 2; pass++)
		{
			for (auto builtAssetInfoIt = mBuiltAssetsTree.allAssets.Begin(); builtAssetInfoIt != mBuiltAssetsTree.allAssets.End(); )
			{
				bool isFolder = (*builtAssetInfoIt)->assetType == folderTypeId;
				bool skip = pass == 0 ? isFolder : !isFolder;
				if (skip)
				{
					++builtAssetInfoIt;
					continue;
				}

				bool needRemove = true;
				for (auto sourceAssetInfo : mSourceAssetsTree.allAssets)
				{
					if ((*builtAssetInfoIt)->path == sourceAssetInfo->path &&
						(*builtAssetInfoIt)->meta->ID() == sourceAssetInfo->meta->ID())
					{
						needRemove = false;
						break;
					}
				}

				if (!needRemove)
				{
					++builtAssetInfoIt;
					continue;
				}

				for (auto sourceAssetInfo : mSourceAssetsTree.allAssets)
				{
					if ((*builtAssetInfoIt)->path == sourceAssetInfo->path &&
						(*builtAssetInfoIt)->meta->ID() == sourceAssetInfo->meta->ID())
					{
						needRemove = false;
						break;
					}
				}

				GetAssetConverter((*builtAssetInfoIt)->assetType)->RemoveAsset(**builtAssetInfoIt);

				modifiedAssets.Add((*builtAssetInfoIt)->id);

				mLog->OutStr("Removed asset: " + (*builtAssetInfoIt)->path);

				auto builtAssetInfo = *builtAssetInfoIt;

				builtAssetInfoIt = mBuiltAssetsTree.allAssets.Remove(builtAssetInfoIt);
				mBuiltAssetsTree.rootAssets.Remove(builtAssetInfo);

				if (builtAssetInfo->parent)
					builtAssetInfo->parent->RemoveChild(builtAssetInfo);
			}
		}

		return modifiedAssets;
	}

	Vector<UID> AssetsBuilder::ProcessModifiedAssets()
	{
		Vector<UID> modifiedAssets;
		const Type* folderType = &TypeOf(FolderAsset);

		mSourceAssetsTree.SortAssets();

		// in first pass processing folders, in second - files
		for (int pass = 0; pass < 2; pass++)
		{
			for (auto sourceAssetInfo : mSourceAssetsTree.allAssets)
			{
				bool isFolder = sourceAssetInfo->assetType == folderType;
				bool skip = pass == 0 ? !isFolder : isFolder;
				if (skip)
					continue;

				mBuiltAssetsTree.Find(

				for (auto builtAssetInfo : mBuiltAssetsTree.allAssets)
				{
					if (sourceAssetInfo->meta->ID() != builtAssetInfo->meta->ID())
						continue;

					if (sourceAssetInfo->path == builtAssetInfo->path)
					{
						if (sourceAssetInfo->editTime != builtAssetInfo->editTime ||
							!sourceAssetInfo->meta->IsEqual(builtAssetInfo->meta))
						{
							GetAssetConverter(sourceAssetInfo->assetType)->ConvertAsset(*sourceAssetInfo);
							modifiedAssets.Add(sourceAssetInfo->id);
							builtAssetInfo->editTime = sourceAssetInfo->editTime;
							delete builtAssetInfo->meta;
							builtAssetInfo->meta = sourceAssetInfo->meta->CloneAs<AssetMeta>();

							mModifiedAssets.Add(builtAssetInfo);

							mLog->Out("Modified asset: " + sourceAssetInfo->path);
						}
					}
					else
					{
						if (sourceAssetInfo->editTime != builtAssetInfo->editTime ||
							!sourceAssetInfo->meta->IsEqual(builtAssetInfo->meta))
						{
							GetAssetConverter(builtAssetInfo->assetType)->RemoveAsset(*builtAssetInfo);

							mBuiltAssetsTree.RemoveAsset(builtAssetInfo, false);

							builtAssetInfo->path = sourceAssetInfo->path;
							builtAssetInfo->editTime = sourceAssetInfo->editTime;
							delete builtAssetInfo->meta;
							builtAssetInfo->meta = sourceAssetInfo->meta->CloneAs<AssetMeta>();
							builtAssetInfo->id = builtAssetInfo->meta->ID();

							GetAssetConverter(sourceAssetInfo->assetType)->ConvertAsset(*sourceAssetInfo);
							mLog->Out("Modified and moved to " + sourceAssetInfo->path + " asset: " + builtAssetInfo->path);

							modifiedAssets.Add(sourceAssetInfo->id);

							mModifiedAssets.Add(builtAssetInfo);

							mBuiltAssetsTree.AddAsset(builtAssetInfo);
						}
						else
						{
							GetAssetConverter(sourceAssetInfo->assetType)->MoveAsset(*builtAssetInfo, *sourceAssetInfo);
							modifiedAssets.Add(sourceAssetInfo->id);
							mLog->Out("Moved asset: " + builtAssetInfo->path + " to " + sourceAssetInfo->path);

							mBuiltAssetsTree.RemoveAsset(builtAssetInfo, false);

							builtAssetInfo->path = sourceAssetInfo->path;
							builtAssetInfo->editTime = sourceAssetInfo->editTime;
							delete builtAssetInfo->meta;
							builtAssetInfo->meta = sourceAssetInfo->meta->CloneAs<AssetMeta>();

							mBuiltAssetsTree.AddAsset(builtAssetInfo);
						}
					}

					break;
				}
			}
		}

		return modifiedAssets;
	}

	Vector<UID> AssetsBuilder::ProcessNewAssets()
	{
		Vector<UID> modifiedAssets;
		const Type* folderType = &TypeOf(FolderAsset);

		mSourceAssetsTree.SortAssets();

		// in first pass skipping files (only folders), in second - folders
		for (int pass = 0; pass < 2; pass++)
		{
			for (auto sourceAssetInfoIt = mSourceAssetsTree.allAssets.Begin(); sourceAssetInfoIt != mSourceAssetsTree.allAssets.End(); ++sourceAssetInfoIt)
			{
				bool isFolder = (*sourceAssetInfoIt)->assetType == folderType;
				bool skip = pass == 0 ? !isFolder : isFolder;

				if (skip)
					continue;

				bool isNew = true;
				for (auto builtAssetInfo : mBuiltAssetsTree.allAssets)
				{
					if ((*sourceAssetInfoIt)->path == builtAssetInfo->path &&
						(*sourceAssetInfoIt)->meta->ID() == builtAssetInfo->meta->ID())
					{
						isNew = false;
						break;
					}
				}

				if (!isNew)
					continue;

				GetAssetConverter((*sourceAssetInfoIt)->assetType)->ConvertAsset(**sourceAssetInfoIt);

				modifiedAssets.Add((*sourceAssetInfoIt)->id);

				mLog->Out("New asset: " + (*sourceAssetInfoIt)->path);

				AssetInfo* newBuiltAsset = mnew AssetInfo();
				newBuiltAsset->path = (*sourceAssetInfoIt)->path;
				newBuiltAsset->assetType = (*sourceAssetInfoIt)->assetType;
				newBuiltAsset->editTime = (*sourceAssetInfoIt)->editTime;
				newBuiltAsset->meta = (*sourceAssetInfoIt)->meta->CloneAs<AssetMeta>();
				newBuiltAsset->id = newBuiltAsset->meta->ID();

				mBuiltAssetsTree.AddAsset(newBuiltAsset);
			}
		}

		return modifiedAssets;
	}

	Vector<UID> AssetsBuilder::ConvertersPostProcess()
	{
		Vector<UID> res;

		for (auto it = mAssetConverters.Begin(); it != mAssetConverters.End(); ++it)
			res.Add(it->second->AssetsPostProcess());

		res.Add(mStdAssetConverter.AssetsPostProcess());

		return res;
	}

	void AssetsBuilder::GenerateMeta(const Type& assetType, const String& metaFullPath)
	{
		auto assetTypeSample = (Asset*)assetType.CreateSample();
		auto assetTypeSampleMeta = assetTypeSample->GetMeta();

		DataNode metaData;
		metaData = assetTypeSampleMeta;
		metaData["Value/mId"] = UID();

		metaData.SaveToFile(metaFullPath);

		delete assetTypeSample;
	}

	IAssetConverter* AssetsBuilder::GetAssetConverter(const Type* assetType)
	{
		if (mAssetConverters.ContainsKey(assetType))
			return mAssetConverters[assetType];

		return &mStdAssetConverter;
	}

	void AssetsBuilder::Reset()
	{
		mModifiedAssets.Clear();
		mSourceAssetsTree.Clear();
		mBuiltAssetsTree.Clear();

		for (auto it = mAssetConverters.Begin(); it != mAssetConverters.End(); ++it)
			it->second->Reset();

		mStdAssetConverter.Reset();
	}
}

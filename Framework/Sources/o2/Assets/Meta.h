#pragma once

#include "o2/Utils/Types/UID.h"
#include "o2/Utils/Serialization/Serializable.h"

namespace o2
{
	// --------------------------------------
	// Basic asset meta information interface
	// --------------------------------------
	class AssetMeta : public ISerializable
	{
	public:
		// Default constructor
		AssetMeta();

		// Virtual destructor
		virtual ~AssetMeta();

		// Returns asset id
		const UID& ID() const;

		// Returns asset type id
		virtual const Type* GetAssetType() const;

		// Returns true if other meta is equal to this
		virtual bool IsEqual(AssetMeta* other) const;

		SERIALIZABLE(AssetMeta);

	private:
		UID mId; // Id of asset @SERIALIZABLE

		friend class Asset;
	};
}

CLASS_BASES_META(o2::AssetMeta)
{
	BASE_CLASS(o2::ISerializable);
}
END_META;
CLASS_FIELDS_META(o2::AssetMeta)
{
	PRIVATE_FIELD(mId).SERIALIZABLE_ATTRIBUTE();
}
END_META;
CLASS_METHODS_META(o2::AssetMeta)
{

	PUBLIC_FUNCTION(const UID&, ID);
	PUBLIC_FUNCTION(const Type*, GetAssetType);
	PUBLIC_FUNCTION(bool, IsEqual, AssetMeta*);
}
END_META;

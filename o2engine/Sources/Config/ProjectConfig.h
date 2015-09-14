#pragma once

#include "Utils/String.h"
#include "Utils/Serialization.h"
#include "Utils/Singleton.h"
#include "Utils/Memory/Ptr.h"
#include "Utils/Property.h"

// Project configuration access macros
#define o2ProjectConfig o2::ProjectConfig::Instance()

namespace o2
{
	class ProjectBuildConfig;

	// ---------------------
	// Project configuration
	// ---------------------
	class ProjectConfig: public ISerializable, public Singleton<ProjectConfig>
	{
		friend class AssetBuildSystem;
		friend class IApplication;

	public:
		enum class Platform { Windows, MacOSX, iOS, Android };

	protected:
		String   mProjectName; // Current project name
		Platform mPlatform;    // Current project target platform

	public:
		Property<String>   ProjectName;     // Project name property
		Property<Platform> CurrentPlatform; // Project platform property

		// Default constructor
		ProjectConfig();

		// Destructor
		~ProjectConfig();

		// Returns project name 
		String GetProjectName() const;

		// Sets project name
		void SetProjectName(const String& name);

		// Returns platform
		Platform GetPlatform() const;

		// Sets platform
		void SetPlatform(Platform platform);

		SERIALIZABLE_IMPL(ProjectConfig);

		IOBJECT(ProjectConfig)
		{
			SRLZ_FIELD(mProjectName);
			SRLZ_FIELD(mPlatform);
		}

	protected:
		// Initializes config by default
		void InitializeDefault(const String& configFilePath);

		// Initializes properties
		void InitializeProperties();
	};
}
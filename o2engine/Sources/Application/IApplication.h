#pragma once

#include "Utils/Delegates.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Memory/Ptr.h"
#include "Utils/Property.h"
#include "Utils/Singleton.h"
#include "Utils/String.h"

namespace o2
{
	class Assets;
	class EventSystem;
	class FileSystem;
	class Input;
	class LogStream;
	class ProjectConfig;
	class Render;
	class Time;
	class Timer;
	class UIManager;

	// ---------------------------------
	// Basic application class interface
	// ---------------------------------
	class IApplication: public Singleton<IApplication>
	{
	public:
		Function<void()> onActivatedEvent;   // On Activated event callbacks
		Function<void()> onDeactivatedEvent; // On deactivated event callbacks
		Function<void()> onStartedEvent;     // On started event callbacks
		Function<void()> onClosingEvent;     // On closing event callbacks
		Function<void()> onResizingEvent;    // On resized app window callbacks. Ignoring on mobiles/tables
		Function<void()> onMovingEvent;      // On moving app window callbacks. Ignoring on mobiles/tables

		Property<bool>   fullscreen;         // Full screen/window changing property
		Property<bool>   resizible;          // Resizible window property
		Property<Vec2I>  windowSize;         // Window frame size property
		Property<Vec2I>  windowContentSize;  // Window content frame size property
		Property<Vec2I>  windowPosition;     // Window position on screen property
		Property<String> windowCaption;      // Window caption property

		// Default constructor
		IApplication();

		// Destructor 
		virtual ~IApplication();

		// Returns pointer to log object
		virtual Ptr<LogStream> GetLog() const;

		// Returns pointer to input message object
		virtual Ptr<Input> GetInput() const;

		// Returns pointer to project config
		virtual Ptr<ProjectConfig> GetProjectConfig() const;

		// Returns pointer to time utils object
		virtual Ptr<Time> GetTime() const;

		// Launching application cycle
		virtual void Launch();

		// Makes application fullscreen. On mobiles/tablets has no effect, just ignoring
		virtual void SetFullscreen(bool fullscreen = true);

		// Return true, if application is fullscreen On mobiles/tables always true
		virtual bool IsFullScreen() const;

		// Sets application window as resizible. On mobiles/tablets has no effect, just ignoring
		virtual void SetResizible(bool resizible);

		// Returns true, if application is resizible. On mobiles/tablets always returns false
		virtual bool IsResizible() const;

		// Sets application window size. On mobiles/tablets has no effect, just ignoring
		virtual void SetWindowSize(const Vec2I& size);

		// Returns application window size. On mobiles/tablets returns content size
		virtual Vec2I GetWindowSize() const;

		// Sets application window position. On mobiles/tablets has no effect, just ignoring
		virtual void SetWindowPosition(const Vec2I& position);

		// Returns application window position. On mobiles/tablets return zero vector
		virtual Vec2I GetWindowPosition() const;

		// Sets application window caption. On mobiles/tablets has no effect, just ignoring
		virtual void SetWindowCaption(const String& caption);

		// Returns application window caption. On mobiles/tablets returns empty string
		virtual String GetWindowCaption() const;

		// Sets inside content size
		virtual void SetContentSize(const Vec2I& size);

		// Returns inside content size
		virtual Vec2I GetContentSize() const;

		// Returns device screen resolution
		virtual Vec2I GetScreenResolution() const;

	protected:
		Ptr<Assets>        mAssets;        // Assets
		Ptr<EventSystem>   mEventSystem;   // Events processing system
		Ptr<FileSystem>    mFileSystem;    // File system
		Ptr<Input>         mInput;         // While application user input message
		Ptr<LogStream>     mLog;           // Log stream with id "app", using only for application messages
		Ptr<ProjectConfig> mProjectConfig; // Project config
		Ptr<Render>        mRender;        // Graphics render
		Ptr<Time>          mTime;          // Time utilities
		Ptr<Timer>         mTimer;         // Timer for detecting delta time for update
		Ptr<UIManager>     mUIManager;     // UI manager

	protected:
		// Called on updating
		virtual void OnUpdate(float dt);

		// Called on drawing
		virtual void OnDraw();

		// Calls when application activated
		virtual void OnActivated();

		// Calls when application deactivated
		virtual void OnDeactivated();

		// Calls when application is starting
		virtual void OnStarted();

		// Calls when application is closing
		virtual void OnClosing();

		// Calls when application window resized. Ignoring on mobiles/tablets
		virtual void OnResizing();

		// Calls when application window moved. Ignoring on mobiles/tablets
		virtual void OnMoved();

		// Initializing all systems and log. Call it when creating applications
		void InitalizeSystems();

		// Deinitializing systems
		void DeinitializeSystems();

		// Processing frame update, drawing and input messages
		void ProcessFrame();

		// Initializes properties
		void InitializeProperties();
	};
}
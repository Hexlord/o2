#pragma once

#include "o2/Assets/ActorAsset.h"
#include "o2/Scene/ActorCreationMode.h"
#include "o2/Scene/ActorRef.h"
#include "o2/Scene/ActorTransform.h"
#include "o2/Scene/Tags.h"
#include "o2/Utils/Editor/Attributes/AnimatableAttribute.h"
#include "o2/Utils/Editor/Attributes/EditorPropertyAttribute.h"
#include "o2/Utils/Editor/SceneEditableObject.h"
#include "o2/Utils/Singleton.h"
#include "o2/Utils/Types/UID.h"

namespace o2
{
	class Component;
	class Scene;
	class SceneLayer;

#if IS_EDITOR
	typedef SceneEditableObject ActorBase;
#else
	typedef ISerializable ActorBase;
#endif

	// ---------------------------------------------------------------------------------------------
	// Scene actor. This is a basic scene object. All other scene object types is derived from this.
	// It has an unique id, name, tags, scene layer, transformation, components and children actors.
	// Can be created from prototype of other actor. 
	// When editor pragma is enabled, it is derived from SceneEditableObject and support all editor 
	// features for editing actor
	// ---------------------------------------------------------------------------------------------
	class Actor : virtual public ActorBase
	{

	public:
		PROPERTIES(Actor);
		PROPERTY(ActorAssetRef, prototype, SetPrototype, GetPrototype); // Prototype asset reference property @EDITOR_IGNORE

		GETTER(SceneUID, id, GetID);              // Actor unique id
		PROPERTY(String, name, SetName, GetName); // Actor name property @EDITOR_IGNORE @ANIMATABLE

		PROPERTY(SceneLayer*, layer, SetLayer, GetLayer);    // Layer property @EDITOR_IGNORE
		PROPERTY(String, layerName, SetLayer, GetLayerName); // Layer name property @EDITOR_IGNORE @ANIMATABLE

		PROPERTY(bool, enabled, SetEnabled, IsEnabled);         // Is actor enabled property @EDITOR_IGNORE @ANIMATABLE
		GETTER(bool, enabledInHierarchy, IsEnabledInHierarchy); // Is actor enabled in hierarchy getter

		GETTER(Vector<Actor*>, children, GetChildren);         // Children array getter
		GETTER(Vector<Component*>, components, GetComponents); // Components array getter

		ACCESSOR(Actor*, child, String, GetChild, GetAllChilds);                 // Children accessor
		ACCESSOR(Component*, component, String, GetComponent, GetAllComponents); // Component accessor by type name

	public:
		TagGroup              tags;      // Tags group @EDITOR_IGNORE
		ActorTransform* const transform; // Transformation of actor @EDITOR_IGNORE @ANIMATABLE

	public:
		Function<void(bool)>  onEnableChanged; // Enable changing event

#if IS_EDITOR
		PROPERTY(bool, locked, SetLocked, IsLocked);          // Is actor locked property @EDITOR_IGNORE
		GETTER(bool, lockedInHierarchy, IsLockedInHierarchy); // Is actor locked in hierarchy getter

		Function<void()>       onChanged;               // Something in actor change event
		Function<void(Actor*)> onParentChanged;         // Actor change parent event
		Function<void()>       onChildHierarchyChanged; // Actor childs hierarchy change event
		Function<void(bool)>   onLockChanged;           // Locking changing event
		Function<void()>       onNameChanged;           // Name changing event
#endif // IS_EDITOR

	public:
		// Default constructor
		Actor(ActorCreateMode mode = ActorCreateMode::Default);

		// Actor constructor from prototype
		Actor(const ActorAssetRef& prototype, ActorCreateMode mode = ActorCreateMode::Default);

		// Constructor with components
		Actor(Vector<Component*> components, ActorCreateMode mode = ActorCreateMode::Default);

		// Copy-constructor
		Actor(const Actor& other);

		// Destructor
		~Actor();

		// Assign operator
		Actor& operator=(const Actor& other);

		// Updates actor and components
		virtual void Update(float dt);

		// Updates childs
		virtual void UpdateChildren(float dt);

		// Updtates self transform, dependent parents and children transforms
		virtual void UpdateTransform();

		// Updates transform
		virtual void UpdateSelfTransform();

		// Updates children transforms
		virtual void UpdateChildrenTransforms();

		// Sets actor's name
		void SetName(const String& name);

		// Returns name
		String GetName() const;

		// Returns actor's unique id
		SceneUID GetID() const;

		// Sets id. Be carefully! Ids must be unique! Don't recommending to change this
		void SetID(SceneUID id);

		// Generates new random id 
		void GenerateNewID(bool childs = true);

		// Returns asset id
		UID GetAssetID() const;

		// Is this from asset
		bool IsAsset() const;

		// Excludes from scene and will not be update and draw automatically from scene
		void ExcludeFromScene(bool keepEditorObjects = false);

		// Includes to scene and now will be update and draw automatically from scene
		void IncludeInScene();

		// Is actor on scene
		bool IsOnScene() const;

		// Is actor hierarchy on scene. Returns true when this on scene, or some parent actor on scene
		bool IsHieararchyOnScene() const;

		// Sets actor enabling
		virtual void SetEnabled(bool enabled);

		// Enables actor
		void Enable();

		// Disables actor
		void Disable();

		// Returns is actor enabled
		bool IsEnabled() const;

		// Returns is really enabled
		bool IsResEnabled() const;

		// Returns is actor enabled in hierarchy
		bool IsEnabledInHierarchy() const;

		// Sets parent
		void SetParent(Actor* actor, bool worldPositionStays = true);

		// Returns parent
		Actor* GetParent() const;

		// Sets index position in parent or scene
		virtual void SetIndexInSiblings(int index);

		// Add child actor
		Actor* AddChild(Actor* actor);

		// Add children actors
		Vector<Actor*> AddChildren(Vector<Actor*> actors);

		// Add child actor
		Actor* AddChild(Actor* actor, int index);

		// Returns child actor by path (ex "root/some node/other node/target node")
		Actor* GetChild(const String& path) const;

		// Returns child actor by name
		Actor* FindChild(const String& name) const;

		// Returns child actor by path (ex "root/some node/other node/target node")
		template<typename _type>
		_type* GetChildByType(const String& path) const;

		// Returns child actor by name
		template<typename _type>
		_type* FindChildByTypeAndName(const String& name) const;

		// Searches child with specified type
		template<typename _type>
		_type* FindChildByType(bool searchInChildren = true);

		// Returns children array
		Vector<Actor*> GetChildren() const;

		// Removes child and destroys him if needed
		void RemoveChild(Actor* actor, bool release = true);

		// Removes and destroys all childs
		void RemoveAllChildren(bool release = true);

		// And new component
		template<typename _type>
		_type* AddComponent();

		// Adds new component
		Component* AddComponent(Component* component);

		// Removes component
		void RemoveComponent(Component* component, bool release = true);

		// Removes all components
		void RemoveAllComponents();

		// Returns component with type name
		Component* GetComponent(const String& typeName);

		// Returns component with type
		Component* GetComponent(const Type* type);

		// Returns component by id
		Component* GetComponent(SceneUID id);

		// Returns component with type
		template<typename _type>
		_type* GetComponent() const;

		// Returns component with type in this and children
		template<typename _type>
		_type* GetComponentInChildren() const;

		// Return all components by type
		template<typename _type>
		Vector<_type*> GetComponents() const;

		// Returns all components by type in this and children
		template<typename _type>
		Vector<_type*> GetComponentsInChildren() const;

		// Returns all components
		Vector<Component*> GetComponents() const;

		// Sets layer
		void SetLayer(SceneLayer* layer);

		// Sets layer by name
		void SetLayer(const String& layerName);

		// Returns layer
		SceneLayer* GetLayer() const;

		// Returns layer name
		String GetLayerName() const;

		// Sets default actors creation mode
		static void SetDefaultCreationMode(ActorCreateMode mode);

		// Return default actors creation mode
		static ActorCreateMode GetDefaultCreationMode();

		// Returns is current mode means mode on scene. For InScene always returns true. For returns true only when default creation mode is onscene.
		static bool IsModeOnScene(ActorCreateMode mode);

#if IS_EDITOR
		// Returns the path of create menu category
		virtual const String& GetCreateMenuCategory() const;

		// Sets locking. Locked actor can't be selected in editor scene view. But is can be selected in scene tree
		void SetLocked(bool locked);

		// Locks actor. Locked actor can't be selected in editor scene view. But is can be selected in scene tree
		void Lock();

		// Unlocks actor. Locked actor can't be selected in editor scene view. But is can be selected in scene tree
		void Unlock();

		// Returns is actor locked. Locked actor can't be selected in editor scene view. But is can be selected in scene tree
		bool IsLocked() const;

		// Returns is actor locked in hierarchy. Locked actor can't be selected in editor scene view. But is can be selected in scene tree
		bool IsLockedInHierarchy() const;

		// Returns prototype from this or this parent
		ActorAssetRef GetPrototype() const;

		// Returns prototype directly from only this
		ActorAssetRef GetPrototypeDirectly() const;

		// Breaks link to prototype
		void BreakPrototypeLink();

		// Applies all changes to prototype and saves it
		void ApplyChangesToPrototype();

		// Reverts all properties to prototype
		void RevertToPrototype();

		// Makes prototype asset from this actor and links this to new asset
		ActorAssetRef MakePrototype();

		// Returns prototype link pointer
		ActorRef GetPrototypeLink() const;

		// Returns is this linked to specified actor with depth links search
		bool IsLinkedToActor(Actor* actor) const;

		// Returns is this linked to specified actor id with depth links search
		bool IsLinkedToActor(SceneUID id) const;

		// Searches actor in this, what linked to linkActor
		Actor* FindLinkedActor(Actor* linkActor);

		// Searches actor in this, what linked to actor with id
		Actor* FindLinkedActor(SceneUID id);

		// Searches actor with id in this and this children
		Actor* FindActorById(SceneUID id);

		// Returns list of object's children
		Vector<SceneEditableObject*> GetEditablesChildren() const override;

		// Returns object's parent object. Return nullptr when it is a root scene object
		SceneEditableObject* GetEditableParent() const override;

		// Sets parent object. nullptr means make this object as root. idx is place in parent children. idx == -1 means last
		void SetEditableParent(SceneEditableObject* object) override;

		// Adds child. idx is place in parent children. idx == -1 means last
		void AddEditableChild(SceneEditableObject* object, int idx = -1) override;

		// Returns is that type of object can be enabled and disabled
		bool IsSupportsDisabling() const override;

		// Returns is that type of object can be locked
		bool IsSupportsLocking() const override;

		// Returns is that type of object can be transformed
		bool IsSupportsTransforming() const override;

		// Returns transform, override when it's supports
		Basis GetTransform() const override;

		// Sets transform of object, override when it's supports
		void SetTransform(const Basis& transform) override;

		// Returns is object supports pivot 
		bool IsSupportsPivot() const override;

		// Sets transform pivot point
		void SetPivot(const Vec2F& pivot) override;

		// Returns transform pivot
		Vec2F GetPivot() const override;

		// It is called when some changed in actor
		void OnChanged() override;

		// It is called when actor's locking was changed
		void OnLockChanged() override;

		// It is called when actor's name was changed
		void OnNameChanged() override;

		// It is called when child changed
		void OnChildrenChanged() override;

#endif //IS_EDITOR

		SERIALIZABLE(Actor);

	protected:
		static ActorCreateMode mDefaultCreationMode;   // Default mode creation

		SceneUID mId;   // Unique actor id
		String   mName; // Name of actor

		SceneLayer* mLayer = nullptr; // Scene layer @EXCLUDE_POINTER_SEARCH

		Actor*         mParent = nullptr; // Parent actor @EXCLUDE_POINTER_SEARCH
		Vector<Actor*> mChildren;         // Children actors 

		Vector<Component*> mComponents; // Components vector 

		bool mEnabled = true;               // Is actor enabled
		bool mResEnabled = true;            // Is actor really enabled. 
		bool mResEnabledInHierarchy = true; // Is actor enabled in hierarchy

		bool mIsOnScene = true; // Is actor on scene

		bool mIsAsset = false; // Is this actor cached asset
		UID  mAssetId;         // Source asset id

		Vector<ActorRef*> mReferences; // References to this actor

#if IS_EDITOR
		ActorAssetRef mPrototype;               // Prototype asset
		ActorRef      mPrototypeLink = nullptr; // Prototype link actor. Links to source actor from prototype

		bool mLocked = false;    // Is actor locked
		bool mResLocked = false; // Is actor locked in hierarchy
#endif

	protected:
		// Base actor constructor with transform
		Actor(ActorTransform* transform, bool isOnScene = true, const String& name = "unnamed", bool enabled = true,
			  bool resEnabled = true, bool locked = false, bool resLocked = false,
			  SceneLayer* layer = nullptr, SceneUID id = Math::Random(), UID assetId = 0);

		// Default constructor with transform
		Actor(ActorTransform* transform, ActorCreateMode mode = ActorCreateMode::Default);

		// Actor constructor from prototype with transform
		Actor(ActorTransform* transform, const ActorAssetRef& prototype, ActorCreateMode mode = ActorCreateMode::Default);

		// Constructor with components with transform
		Actor(ActorTransform* transform, Vector<Component*> components, ActorCreateMode mode = ActorCreateMode::Default);

		// Copy-constructor with transform
		Actor(ActorTransform* transform, const Actor& other);

		// Copies data of actor from other to this
		virtual void CopyData(const Actor& otherActor);

		// Processes copying actor
		void ProcessCopying(Actor* dest, const Actor* source,
							Vector<Actor**>& actorsPointers, Vector<Component**>& componentsPointers,
							Map<const Actor*, Actor*>& actorsMap,
							Map<const Component*, Component*>& componentsMap,
							bool isSourcePrototype);

		// Copies fields from source to dest
		void CopyFields(Vector<FieldInfo*>& fields, IObject* source, IObject* dest,
						Vector<Actor**>& actorsPointers,
						Vector<Component**>& componentsPointers,
						Vector<ISerializable*>& serializableObjects);

		// Collects fixing actors and components pointers in new component
		void CollectFixingFields(Component* newComponent, Vector<Component**>& componentsPointers,
								 Vector<Actor**>& actorsPointers);

		// Collects component field, except Component class fields
		void GetComponentFields(Component* component, Vector<FieldInfo*>& fields);

		// Fixes actors and components pointers by actors map
		void FixComponentFieldsPointers(const Vector<Actor**>& actorsPointers,
										const Vector<Component**>& componentsPointers,
										const Map<const Actor*, Actor*>& actorsMap,
										const Map<const Component*, Component*>& componentsMap);

		// Updates result read enable flag
		virtual void UpdateResEnabled();

		// Updates enabling
		virtual void UpdateResEnabledInHierarchy();

		// Beginning serialization callback
		void OnSerialize(DataNode& node) const override;

		// Completion deserialization callback
		void OnDeserialized(const DataNode& node) override;

		// Regular serializing without prototype
		void SerializeRaw(DataNode& node) const;

		// Regular deserializing without prototype
		void DeserializeRaw(const DataNode& node);

		// Returns dictionary of all children by names
		Map<String, Actor*> GetAllChilds();

		// Returns dictionary of all components by type names
		Map<String, Component*> GetAllComponents();

		// Returns all children actors with their children
		void GetAllChildrenActors(Vector<Actor*>& actors);

		// It is called when actor excluding from scene
		virtual void OnExcludeFromScene();

		// It is called when actor including from scene
		virtual void OnIncludeToScene();

		// Applies excluding from scene for all components in hierarchy
		void ExcludeComponentsFromScene();

		// Applies including to scene for all components in hierarchy
		void IncludeComponentsToScene();

		// Sets parent
		void SetParentProp(Actor* actor);

		// It is called when result enable was changed
		virtual void OnResEnableInHierarchyChanged();

		// It is called when transformation was changed and updated
		virtual void OnTransformUpdated();

		// It is called when parent changed
		virtual void OnParentChanged(Actor* oldParent);

		// It is called when child actor was added
		virtual void OnChildAdded(Actor* child);

		// It is called when child actor was removed
		virtual void OnChildRemoved(Actor* child);

		// It is called when layer was changed
		virtual void OnLayerChanged(SceneLayer* oldLayer);

#if IS_EDITOR

		// -----------------------------
		// Actor prototype applying info
		// -----------------------------
		struct ApplyActorInfo
		{
			Actor*    actor;
			Actor*    matchingChild;
			Vector<Actor*> allChildren;

			Vector<Actor**>     actorPointersFields;
			Vector<Component**> componentPointersFields;

			Map<const Actor*, Actor*>         actorsMap;
			Map<const Component*, Component*> componentsMap;

			bool operator==(const ApplyActorInfo& other) const { return actor == other.actor; }
		};

		// Regular serializing with prototype
		void SerializeWithProto(DataNode& node) const;

		// Regular deserializing with prototype
		void DeserializeWithProto(const DataNode& node);

		// Processes making prototype
		void ProcessPrototypeMaking(Actor* dest, Actor* source,
									Vector<Actor**>& actorsPointers, Vector<Component**>& componentsPointers,
									Map<const Actor*, Actor*>& actorsMap,
									Map<const Component*, Component*>& componentsMap,
									bool isInsidePrototype);

		// Copies changed field from source to dest
		void CopyChangedFields(Vector<FieldInfo*>& fields,
							   IObject* source, IObject* changed, IObject* dest,
							   Vector<Actor**>& actorsPointers,
							   Vector<Component**>& componentsPointers,
							   Vector<ISerializable*>& serializableObjects);

		// Applies basic actor fields and transform from source to dest  
		void CopyActorChangedFields(Actor* source, Actor* changed, Actor* dest, Vector<Actor*>& allDestChilds, bool withTransform);

		// Separates children actors to linear array, removes child and parent links
		void SeparateActors(Vector<Actor*>& separatedActors);

		// Processes reverting actor
		void ProcessReverting(Actor* dest, const Actor* source, const Vector<Actor*>& separatedActors,
							  Vector<Actor**>& actorsPointers, Vector<Component**>& componentsPointers,
							  Map<const Actor*, Actor*>& actorsMap,
							  Map<const Component*, Component*>& componentsMap,
							  Vector<ISerializable*>& serializableObjects);

		// Not using prototype setter
		void SetProtytypeDummy(ActorAssetRef asset);

		// Sets prototype and links actor to them
		void SetPrototype(ActorAssetRef asset);

		// Updates locking
		void UpdateLocking();

#endif // IS_EDITOR

		friend class ActorAsset;
		friend class ActorDataNodeConverter;
		friend class ActorRef;
		friend class ActorTransform;
		friend class Component;
		friend class DrawableComponent;
		friend class SceneDrawable;
		friend class Scene;
		friend class SceneLayer;
		friend class Tag;
		friend class Widget;
	};
}

#include "o2/Scene/Component.h"

namespace o2
{
	template<typename _type>
	_type* Actor::FindChildByType(bool searchInChildren /*= true*/)
	{
		for (auto child : mChildren)
			if (child->GetType() == TypeOf(_type))
				return (_type*)child;

		if (searchInChildren)
		{
			for (auto child : mChildren)
				if (auto res = child->FindChildByType<_type>())
					return res;
		}

		return nullptr;
	}

	template<typename _type>
	_type* Actor::GetChildByType(const String& path) const
	{
		return dynamic_cast<_type*>(GetChild(path));
	}

	template<typename _type>
	_type* Actor::FindChildByTypeAndName(const String& name) const
	{
		return dynamic_cast<_type*>(FindChild(name));
	}

	template<typename _type>
	Vector<_type*> Actor::GetComponentsInChildren() const
	{
		Vector<_type*> res = GetComponents<_type>();

		for (auto child : mChildren)
			res.Add(child->GetComponentsInChildren<_type>());

		return res;
	}

	template<typename _type>
	_type* Actor::GetComponentInChildren() const
	{
		_type* res = GetComponent<_type*>();

		if (res)
			return res;

		for (auto child : mChildren)
		{
			res = child->GetComponentInChildren<_type>();
			if (res)
				return res;
		}

		return res;
	}

	template<typename _type>
	_type* Actor::GetComponent() const
	{
		for (auto comp : mComponents)
		{
			if (comp->GetType().IsBasedOn(TypeOf(_type)))
				return dynamic_cast<_type*>(comp);
		}

		return nullptr;
	}

	template<typename _type>
	Vector<_type*> Actor::GetComponents() const
	{
		Vector<_type*> res;
		for (auto comp : mComponents)
		{
			if (comp->GetType().IsBasedOn(TypeOf(_type)))
				res.Add(comp);
		}

		return res;
	}

	template<typename _type>
	_type* Actor::AddComponent()
	{
		if (GetComponent<_type>() != nullptr)
			return nullptr;

		_type* newComponent = mnew _type();
		AddComponent(newComponent);
		return newComponent;
	}

}

CLASS_BASES_META(o2::Actor)
{
	BASE_CLASS(o2::SceneEditableObject);
}
END_META;
CLASS_FIELDS_META(o2::Actor)
{
	PUBLIC_FIELD(prototype).EDITOR_IGNORE_ATTRIBUTE();
	PUBLIC_FIELD(id);
	PUBLIC_FIELD(name).ANIMATABLE_ATTRIBUTE().EDITOR_IGNORE_ATTRIBUTE();
	PUBLIC_FIELD(layer).EDITOR_IGNORE_ATTRIBUTE();
	PUBLIC_FIELD(layerName).ANIMATABLE_ATTRIBUTE().EDITOR_IGNORE_ATTRIBUTE();
	PUBLIC_FIELD(enabled).ANIMATABLE_ATTRIBUTE().EDITOR_IGNORE_ATTRIBUTE();
	PUBLIC_FIELD(enabledInHierarchy);
	PUBLIC_FIELD(children);
	PUBLIC_FIELD(components);
	PUBLIC_FIELD(child);
	PUBLIC_FIELD(component);
	PUBLIC_FIELD(tags).EDITOR_IGNORE_ATTRIBUTE();
	PUBLIC_FIELD(transform).ANIMATABLE_ATTRIBUTE().EDITOR_IGNORE_ATTRIBUTE();
	PUBLIC_FIELD(onEnableChanged);
	PUBLIC_FIELD(locked).EDITOR_IGNORE_ATTRIBUTE();
	PUBLIC_FIELD(lockedInHierarchy);
	PUBLIC_FIELD(onChanged);
	PUBLIC_FIELD(onParentChanged);
	PUBLIC_FIELD(onChildHierarchyChanged);
	PUBLIC_FIELD(onLockChanged);
	PUBLIC_FIELD(onNameChanged);
	PROTECTED_FIELD(mId);
	PROTECTED_FIELD(mName);
	PROTECTED_FIELD(mLayer);
	PROTECTED_FIELD(mParent);
	PROTECTED_FIELD(mChildren);
	PROTECTED_FIELD(mComponents);
	PROTECTED_FIELD(mEnabled);
	PROTECTED_FIELD(mResEnabled);
	PROTECTED_FIELD(mResEnabledInHierarchy);
	PROTECTED_FIELD(mIsOnScene);
	PROTECTED_FIELD(mIsAsset);
	PROTECTED_FIELD(mAssetId);
	PROTECTED_FIELD(mReferences);
	PROTECTED_FIELD(mPrototype);
	PROTECTED_FIELD(mPrototypeLink);
	PROTECTED_FIELD(mLocked);
	PROTECTED_FIELD(mResLocked);
}
END_META;
CLASS_METHODS_META(o2::Actor)
{

	typedef Map<const Actor*, Actor*>& _tmp1;
	typedef Map<const Component*, Component*>& _tmp2;
	typedef const Map<const Actor*, Actor*>& _tmp3;
	typedef const Map<const Component*, Component*>& _tmp4;
	typedef Map<String, Actor*> _tmp5;
	typedef Map<String, Component*> _tmp6;
	typedef Map<const Actor*, Actor*>& _tmp7;
	typedef Map<const Component*, Component*>& _tmp8;
	typedef Map<const Actor*, Actor*>& _tmp9;
	typedef Map<const Component*, Component*>& _tmp10;

	PUBLIC_FUNCTION(void, Update, float);
	PUBLIC_FUNCTION(void, UpdateChildren, float);
	PUBLIC_FUNCTION(void, UpdateTransform);
	PUBLIC_FUNCTION(void, UpdateSelfTransform);
	PUBLIC_FUNCTION(void, UpdateChildrenTransforms);
	PUBLIC_FUNCTION(void, SetName, const String&);
	PUBLIC_FUNCTION(String, GetName);
	PUBLIC_FUNCTION(SceneUID, GetID);
	PUBLIC_FUNCTION(void, SetID, SceneUID);
	PUBLIC_FUNCTION(void, GenerateNewID, bool);
	PUBLIC_FUNCTION(UID, GetAssetID);
	PUBLIC_FUNCTION(bool, IsAsset);
	PUBLIC_FUNCTION(void, ExcludeFromScene, bool);
	PUBLIC_FUNCTION(void, IncludeInScene);
	PUBLIC_FUNCTION(bool, IsOnScene);
	PUBLIC_FUNCTION(bool, IsHieararchyOnScene);
	PUBLIC_FUNCTION(void, SetEnabled, bool);
	PUBLIC_FUNCTION(void, Enable);
	PUBLIC_FUNCTION(void, Disable);
	PUBLIC_FUNCTION(bool, IsEnabled);
	PUBLIC_FUNCTION(bool, IsResEnabled);
	PUBLIC_FUNCTION(bool, IsEnabledInHierarchy);
	PUBLIC_FUNCTION(void, SetParent, Actor*, bool);
	PUBLIC_FUNCTION(Actor*, GetParent);
	PUBLIC_FUNCTION(void, SetIndexInSiblings, int);
	PUBLIC_FUNCTION(Actor*, AddChild, Actor*);
	PUBLIC_FUNCTION(Vector<Actor*>, AddChildren, Vector<Actor*>);
	PUBLIC_FUNCTION(Actor*, AddChild, Actor*, int);
	PUBLIC_FUNCTION(Actor*, GetChild, const String&);
	PUBLIC_FUNCTION(Actor*, FindChild, const String&);
	PUBLIC_FUNCTION(Vector<Actor*>, GetChildren);
	PUBLIC_FUNCTION(void, RemoveChild, Actor*, bool);
	PUBLIC_FUNCTION(void, RemoveAllChildren, bool);
	PUBLIC_FUNCTION(Component*, AddComponent, Component*);
	PUBLIC_FUNCTION(void, RemoveComponent, Component*, bool);
	PUBLIC_FUNCTION(void, RemoveAllComponents);
	PUBLIC_FUNCTION(Component*, GetComponent, const String&);
	PUBLIC_FUNCTION(Component*, GetComponent, const Type*);
	PUBLIC_FUNCTION(Component*, GetComponent, SceneUID);
	PUBLIC_FUNCTION(Vector<Component*>, GetComponents);
	PUBLIC_FUNCTION(void, SetLayer, SceneLayer*);
	PUBLIC_FUNCTION(void, SetLayer, const String&);
	PUBLIC_FUNCTION(SceneLayer*, GetLayer);
	PUBLIC_FUNCTION(String, GetLayerName);
	PUBLIC_STATIC_FUNCTION(void, SetDefaultCreationMode, ActorCreateMode);
	PUBLIC_STATIC_FUNCTION(ActorCreateMode, GetDefaultCreationMode);
	PUBLIC_STATIC_FUNCTION(bool, IsModeOnScene, ActorCreateMode);
	PUBLIC_FUNCTION(const String&, GetCreateMenuCategory);
	PUBLIC_FUNCTION(void, SetLocked, bool);
	PUBLIC_FUNCTION(void, Lock);
	PUBLIC_FUNCTION(void, Unlock);
	PUBLIC_FUNCTION(bool, IsLocked);
	PUBLIC_FUNCTION(bool, IsLockedInHierarchy);
	PUBLIC_FUNCTION(ActorAssetRef, GetPrototype);
	PUBLIC_FUNCTION(ActorAssetRef, GetPrototypeDirectly);
	PUBLIC_FUNCTION(void, BreakPrototypeLink);
	PUBLIC_FUNCTION(void, ApplyChangesToPrototype);
	PUBLIC_FUNCTION(void, RevertToPrototype);
	PUBLIC_FUNCTION(ActorAssetRef, MakePrototype);
	PUBLIC_FUNCTION(ActorRef, GetPrototypeLink);
	PUBLIC_FUNCTION(bool, IsLinkedToActor, Actor*);
	PUBLIC_FUNCTION(bool, IsLinkedToActor, SceneUID);
	PUBLIC_FUNCTION(Actor*, FindLinkedActor, Actor*);
	PUBLIC_FUNCTION(Actor*, FindLinkedActor, SceneUID);
	PUBLIC_FUNCTION(Actor*, FindActorById, SceneUID);
	PUBLIC_FUNCTION(Vector<SceneEditableObject*>, GetEditablesChildren);
	PUBLIC_FUNCTION(SceneEditableObject*, GetEditableParent);
	PUBLIC_FUNCTION(void, SetEditableParent, SceneEditableObject*);
	PUBLIC_FUNCTION(void, AddEditableChild, SceneEditableObject*, int);
	PUBLIC_FUNCTION(bool, IsSupportsDisabling);
	PUBLIC_FUNCTION(bool, IsSupportsLocking);
	PUBLIC_FUNCTION(bool, IsSupportsTransforming);
	PUBLIC_FUNCTION(Basis, GetTransform);
	PUBLIC_FUNCTION(void, SetTransform, const Basis&);
	PUBLIC_FUNCTION(bool, IsSupportsPivot);
	PUBLIC_FUNCTION(void, SetPivot, const Vec2F&);
	PUBLIC_FUNCTION(Vec2F, GetPivot);
	PUBLIC_FUNCTION(void, OnChanged);
	PUBLIC_FUNCTION(void, OnLockChanged);
	PUBLIC_FUNCTION(void, OnNameChanged);
	PUBLIC_FUNCTION(void, OnChildrenChanged);
	PROTECTED_FUNCTION(void, CopyData, const Actor&);
	PROTECTED_FUNCTION(void, ProcessCopying, Actor*, const Actor*, Vector<Actor**>&, Vector<Component**>&, _tmp1, _tmp2, bool);
	PROTECTED_FUNCTION(void, CopyFields, Vector<FieldInfo*>&, IObject*, IObject*, Vector<Actor**>&, Vector<Component**>&, Vector<ISerializable*>&);
	PROTECTED_FUNCTION(void, CollectFixingFields, Component*, Vector<Component**>&, Vector<Actor**>&);
	PROTECTED_FUNCTION(void, GetComponentFields, Component*, Vector<FieldInfo*>&);
	PROTECTED_FUNCTION(void, FixComponentFieldsPointers, const Vector<Actor**>&, const Vector<Component**>&, _tmp3, _tmp4);
	PROTECTED_FUNCTION(void, UpdateResEnabled);
	PROTECTED_FUNCTION(void, UpdateResEnabledInHierarchy);
	PROTECTED_FUNCTION(void, OnSerialize, DataNode&);
	PROTECTED_FUNCTION(void, OnDeserialized, const DataNode&);
	PROTECTED_FUNCTION(void, SerializeRaw, DataNode&);
	PROTECTED_FUNCTION(void, DeserializeRaw, const DataNode&);
	PROTECTED_FUNCTION(_tmp5, GetAllChilds);
	PROTECTED_FUNCTION(_tmp6, GetAllComponents);
	PROTECTED_FUNCTION(void, GetAllChildrenActors, Vector<Actor*>&);
	PROTECTED_FUNCTION(void, OnExcludeFromScene);
	PROTECTED_FUNCTION(void, OnIncludeToScene);
	PROTECTED_FUNCTION(void, ExcludeComponentsFromScene);
	PROTECTED_FUNCTION(void, IncludeComponentsToScene);
	PROTECTED_FUNCTION(void, SetParentProp, Actor*);
	PROTECTED_FUNCTION(void, OnResEnableInHierarchyChanged);
	PROTECTED_FUNCTION(void, OnTransformUpdated);
	PROTECTED_FUNCTION(void, OnParentChanged, Actor*);
	PROTECTED_FUNCTION(void, OnChildAdded, Actor*);
	PROTECTED_FUNCTION(void, OnChildRemoved, Actor*);
	PROTECTED_FUNCTION(void, OnLayerChanged, SceneLayer*);
	PROTECTED_FUNCTION(void, SerializeWithProto, DataNode&);
	PROTECTED_FUNCTION(void, DeserializeWithProto, const DataNode&);
	PROTECTED_FUNCTION(void, ProcessPrototypeMaking, Actor*, Actor*, Vector<Actor**>&, Vector<Component**>&, _tmp7, _tmp8, bool);
	PROTECTED_FUNCTION(void, CopyChangedFields, Vector<FieldInfo*>&, IObject*, IObject*, IObject*, Vector<Actor**>&, Vector<Component**>&, Vector<ISerializable*>&);
	PROTECTED_FUNCTION(void, CopyActorChangedFields, Actor*, Actor*, Actor*, Vector<Actor*>&, bool);
	PROTECTED_FUNCTION(void, SeparateActors, Vector<Actor*>&);
	PROTECTED_FUNCTION(void, ProcessReverting, Actor*, const Actor*, const Vector<Actor*>&, Vector<Actor**>&, Vector<Component**>&, _tmp9, _tmp10, Vector<ISerializable*>&);
	PROTECTED_FUNCTION(void, SetProtytypeDummy, ActorAssetRef);
	PROTECTED_FUNCTION(void, SetPrototype, ActorAssetRef);
	PROTECTED_FUNCTION(void, UpdateLocking);
}
END_META;
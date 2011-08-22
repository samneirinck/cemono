//////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2009.
// -------------------------------------------------------------------------
//  File name:   IEnginePropertyBlending.h
//  Version:     v1.00
//  Created:     03/09/2009 by CarstenW
//  Description: Engine property blending extension.
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////
#include DEVIRTUALIZE_HEADER_FIX(IEnginePropertyBlending.h)

#ifndef _I_ENGINE_PROPERTY_BLENDING_H_
#define _I_ENGINE_PROPERTY_BLENDING_H_

#pragma once

#include <CryExtension/ICryUnknown.h>
#include <Cry_Vector2.h>
#include <Cry_Vector3.h>
#include <Cry_Color.h>


class XmlNodeRef;

struct IEnginePropertySetDesc;
struct IEnginePropertyUpdateCallback;


// Description:
//   A data structure used to assign a multi-dimensional value to an engine property.
//   It defines various constructors and operators to conveniently work with common 
//   data types used throughout CryEngine.
// Summary:
//   A data structure used to assign a multi-dimensional value to an engine property.
struct EnginePropertyValue
{
	EnginePropertyValue() : n(0) {}
	EnginePropertyValue(float val) : n(1) {data[0] = val;}
	EnginePropertyValue(const Vec2& val) : n(2) {data[0] = val.x; data[1] = val.y;}
	EnginePropertyValue(const Vec3& val) : n(3) {data[0] = val.x; data[1] = val.y; data[2] = val.z;}
	EnginePropertyValue(const Vec4& val) : n(4) {data[0] = val.x; data[1] = val.y; data[2] = val.z; data[3] = val.w;}
	EnginePropertyValue(const ColorF& val) : n(4) {data[0] = val.r; data[1] = val.g; data[2] = val.b; data[3] = val.a;}
	EnginePropertyValue(const EnginePropertyValue& val) : n(val.n)
	{
		for (uint32 i=0; i<n; ++i)
			data[i] = val.data[i];
	}

	EnginePropertyValue& operator =(float val) {new(this)EnginePropertyValue(val); return *this;}
	EnginePropertyValue& operator =(const Vec2& val) {new(this)EnginePropertyValue(val); return *this;}
	EnginePropertyValue& operator =(const Vec3& val) {new(this)EnginePropertyValue(val); return *this;}
	EnginePropertyValue& operator =(const Vec4& val) {new(this)EnginePropertyValue(val); return *this;}
	EnginePropertyValue& operator =(const ColorF& val) {new(this)EnginePropertyValue(val); return *this;}
	EnginePropertyValue& operator =(const EnginePropertyValue& rhs)
	{
		n = rhs.n;
		for (uint32 i=0; i<n; ++i)
			data[i] = rhs.data[i];
		return *this;
	}

	operator float() const {assert(n == 1); return data[0];}
	operator Vec2() const {assert(n == 2); return Vec2(data[0], data[1]);}
	operator Vec3() const {assert(n == 3); return Vec3(data[0], data[1], data[2]);}
	operator Vec4() const {assert(n == 4); return Vec4(data[0], data[1], data[2], data[3]);}
	operator ColorF() const {assert(n == 4); return ColorF(data[0], data[1], data[2], data[3]);}

	// Summary:
	//   Maximum dimension of an engine property value.
	enum
	{
		MaxDimension = 4
	};
	// Summary:
	//   Current dimension of an engine property value.
	uint32 n;
	// Summary:
	//   Array of floats holding the actual values.
	float data[MaxDimension];
};


// Summary:
//   An engine property created via IEnginePropertyBlender.
UNIQUE_IFACE struct IEngineProperty
{
	// Summary:
	//   Returns name of engine property. Guaranteed to be not NULL!
	virtual const char* GetName() const = 0;
	// Summary:
	//   Returns engine property's current value.
	virtual const EnginePropertyValue& GetValue() const = 0;
	// Summary:
	//   Returns engine property's default value.
	virtual const EnginePropertyValue& GetDefValue() const = 0;

protected:
	virtual ~IEngineProperty() {}
};


// Summary:
//   An engine property set defines a collection of engine properties with associated values.
//   Multiple sets can be pushed into the engine property blender to compute a blended/weighted
//   result for each property defined in any set that has been pushed.
UNIQUE_IFACE struct IEnginePropertySet
{
	// Summary:
	//   To allow modification of engine properties in a controlled fashion a set of layers is defined.
	//   Whenever you push a set into the blender you need to specify for which layer you want to push it.
	//   Layers defined further down below have the chance to overwrite values set/blended in previous layers
	//   (e.g. values in a Game set can overwrite values specified in a FlowGraph set which in turn can overwrite
	//   values in an Environment set).
	enum ELayerTypes
	{
		Environment,
		FlowGraph,
		Game,

		NumLayers
	};

	// Summary:
	//   Releases the engine property set.
	virtual void Release() = 0;

	// Summary:
	//   Copies the set specified in pSrc into the engine property set the method is invoked on.
	virtual void Copy(const IEnginePropertySet* pSrc) = 0;
	// Summary:
	//   Clones the engine property set the method is invoked on and return the result.
	virtual IEnginePropertySet* Clone() const = 0;

	// Summary:
	//   Returns name of engine property set. Guaranteed to be not NULL!
	virtual const char* GetName() const = 0;
	// Summary:
	//   Sets name of engine property set.
	// Arguments:
	//   pName - new name of engine property set. Cannot be NULL!
	// Returns:
	//   True if setting name was successful, otherwise false.
	virtual bool SetName(const char* pName) = 0;

	// Summary:
	//   Returns number of properties included in this set.
	virtual size_t Size() const = 0;
	// Summary:
	//   Sets value of a given property in set.
	// Arguments:
	//   idx - index of property for which a new value should be set. Range of idx is [0 .. size) where size = Size().
	//         Properties are stored in the order they were originally added (with potential duplicates and non existing properties removed, see IEnginePropertySetDesc::Add()).
	//   val - new value to be set. Has to have the same dimension as the property's default value!
	// Returns:
	//   True if setting the new value was successful, otherwise false.
	virtual bool Set(size_t idx, const EnginePropertyValue& val) = 0;
	// Summary:
	//   Gets value of a given property in set.
	// Arguments:
	//   idx - index of property for which its current value in set should be retrieved. Range of idx is [0 .. size) where size = Size().
	//         Properties are stored in the order they were originally added (with potential duplicates and non existing property's removed, see IEnginePropertySetDesc::Add()).
	//   val - current value of property in set
	//   pPropertyName - name of the property which its current value in set should be retrieved. Can be set to NULL if of no interest to caller.
	// Returns:
	//   True if getting the current value was successful, otherwise false.
	virtual bool Get(size_t idx, EnginePropertyValue& val, const char** pPropertyName = 0) const = 0;

	// Summary:
	//   Saves engine property set to XML.
	// Arguments:
	//   pNode - reference to XML node an engine property set should be written to. Cannot be NULL!
	// Returns:
	//   True if saving was successful, otherwise false.
	virtual bool SaveToXML(XmlNodeRef pNode) const = 0;
	// Summary:
	//   Saves engine property set to XML.
	// Arguments:
	//   pFilePath - name of file an engine property set should be written to in XML notation. Cannot be NULL!
	// Returns:
	//   True if saving was successful, otherwise false.
	virtual bool SaveToXML(const char* pFilePath) const = 0;

protected:
	virtual ~IEnginePropertySet() {}
};


// Summary:
//   The engine property blender is a CryEngine extension that holds a number of properties that can be defined throughout the engine.
//   It allows property sets to be created. These store values for an arbitrary set of properties and can later be blended to yield new values for
//   any defined property stored in a set which has been pushed into the blender.
UNIQUE_IFACE struct IEnginePropertyBlender : public ICryUnknown
{
	CRYINTERFACE_DECLARE(IEnginePropertyBlender, 0xd0678207437445db, 0xb58aceaa55f000d5)

	// Summary:
	//   Creates an engine property.
	// Arguments:
	//   pName - name of engine property. Cannot be NULL!
	//   defval - default value of engine property. The property's initial value will be set to defval.
	//   copyName - set to true if pName points to a character array on the stack and hence needs to be copied during creation.
	//   pCallback - callback to notify client whenever the property has been updated. Make sure the callback is callable at all times!
	// Returns:
	//   Pointer to created engine property if successful, otherwise NULL.
	virtual IEngineProperty* CreateProperty(const char* pName, const EnginePropertyValue& defval, bool copyName = false, IEnginePropertyUpdateCallback* pCallback = 0) = 0;

	// Summary:
	//   Creates an engine property set description.
	// Returns:
	//   Pointer to created engine property set description if successful, otherwise NULL.
	virtual IEnginePropertySetDesc* CreatePropertySetDesc() const = 0;

	// Summary:
	//   Creates an engine property set.
	// Arguments:
	//   pName - name of engine property set. Cannot be NULL!
	//   pDesc - pointer to engine property set description. Cannot be NULL!
	// Returns:
	//   Pointer to created engine property set if successful, otherwise NULL.
	virtual IEnginePropertySet* CreatePropertySet(const char* pName, const IEnginePropertySetDesc* pDesc) const = 0;
	// Summary:
	//   Creates an engine property set from XML.
	// Arguments:
	//   pNode - reference to XML node tagged "EnginePropertySet". Cannot be NULL!
	// Returns:
	//   Pointer to created engine property set if successful, otherwise NULL.
	virtual IEnginePropertySet* CreatePropertySetFrom(const XmlNodeRef pNode) const = 0;
	// Summary:
	//   Creates an engine property set from XML.
	// Arguments:
	//   pXmlFilePath - name of XML file with the root node tagged "EnginePropertySet" for which a engine property should be created. Cannot be NULL!
	// Returns:
	//   Pointer to created engine property set if successful, otherwise NULL.
	virtual IEnginePropertySet* CreatePropertySetFrom(const char* pXmlFilePath) const = 0;

	// Summary:
	//   Returns number of properties.
	virtual size_t GetNumProperties() const = 0;
	// Summary:
	//   Gets engine property.
	// Arguments:
	//   idx - index of property that should be retrieved. Range of idx is [0 .. size) where size = GetNumProperties().
	//         The order in which engine properties are stored is NOT maintained so you cannot assume to retrieve the 
	//         exact same property for a given index at different points in times!
	// Returns:
	//   Pointer to engine property if successful, otherwise NULL.
	virtual IEngineProperty* GetProperty(size_t idx) const = 0;
	// Summary:
	//   Gets engine property.
	// Arguments:
	//   pName - name of property that should be retrieved
	// Returns:
	//   Pointer to engine property if successful, otherwise NULL.
	virtual IEngineProperty* GetProperty(const char* pName) const = 0;

	// Summary:
	//   Called by the client to initiate engine property update.
	virtual void BeginPropertyUpdate() const = 0;
	// Summary:
	//   Pushes an engine property set.
	// Arguments:
	//   pSet - engine property set to be pushed. Cannot be NULL!
	//   layer - the layer to which this engine property set should be pushed
	//   weight - weight to be used for this engine property during FinalizePropertyUpdate(). For details refer to FinalizePropertyUpdate().
	//   pDebugComment - optional comment passed to the debugger for run time inspection of blending results. Can be NULL.
	// Returns:
	//   True if the push operation was successful, otherwise false.
	virtual bool PushPropertySet(const IEnginePropertySet* pSet, IEnginePropertySet::ELayerTypes layer, float weight, const char* pDebugComment = 0) const = 0;
	// Summary:
	//   Called by the client to finalize engine property update. 
	//   Invokes IEnginePropertyUpdateCallback::OnPropertyUpdated() for any property with a callback specified.
	//   For each property the following steps will be done to compute its new value: For each layer it will accumulate val * w, 
	//   where val is the value of the property contained in each of the sets that's been pushed for a given layer and w is the 
	//   sets associated weight. Also all weights will be accumulated to compute a normalized value for each layer. In a final 
	//   resolve step the accumulated weights (clamped to [0, 1]) will be used to lerp between layers as documented under
	//   IEnginePropertySet::ELayerTypes.
	virtual void FinalizePropertyUpdate() const = 0;
};

DECLARE_BOOST_POINTERS(IEnginePropertyBlender);


// Summary:
//   Description of an engine property set. Used to fill in properties and associated values in a convenient and secure 
//   (that is, checked) manner. This description is then used to create the actual engine property set.
UNIQUE_IFACE struct IEnginePropertySetDesc
{
	// Summary:
	//   Releases the engine property set description.
	virtual void Release() = 0;

	// Summary:
	//   Reserves memory for the specified number of elements. Like for STL vectors use this if you know in advance how many properties you want to add.
	// Arguments:
	//   numElements - number of elements the clients expects to add
	virtual void Reserve(size_t numElements) = 0;
	// Summary:
	//   Removes all previously added properties.
	virtual void Clear() = 0;
	// Summary:
	//   Adds a property and its associated value to the description. The order in which properties are being added is preserved assuming the 
	//   operation is successful (see comments below). You can rely on this later on when creating a property set and trying to retrieve or 
	//   modify property values via IEnginePropertySet::Get() or IEnginePropertySet::Set().
	// Arguments:
	//   pProperty - engine property to be added to the set description. Cannot be NULL!
	//   val - value to be associated
	// Returns:
	//   True if the property was added successfully. False if property has previously been added or the dimension of val is different from 
	//   the dimension of the property's default value.
	virtual bool Add(IEngineProperty* pProperty, const EnginePropertyValue& val) = 0;

protected:
	virtual ~IEnginePropertySetDesc() {}
};


// Summary:
//   Callback interface to be implemented by a client who creates properties if they care about being informed when a properties value changes.
struct IEnginePropertyUpdateCallback
{
	// Summary:
	//   Called when a property's value changed.
	// Arguments:
	//   pProperty - property that's been changed
	//   oldValue - old value of property
	//   newVlaue - new value of property (already set)
	virtual void OnPropertyUpdated(const IEngineProperty* pProperty, const EnginePropertyValue& oldValue, const EnginePropertyValue& newValue) = 0;

protected:
	virtual ~IEnginePropertyUpdateCallback() {}
};


#endif // #ifndef _I_ENGINE_PROPERTY_BLENDING_H_

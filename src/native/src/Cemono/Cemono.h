#pragma once

#include "Headers/ICemono.h"
#include "MonoPathUtils.h"
#include "CemonoAssembly.h"
#include "CemonoArray.h"
#include "CemonoString.h"
#include "CemonoObject.h"

#include <CryExtension/Impl/ClassWeaver.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/environment.h>

class CCemono : public ICemono
{
	CRYINTERFACE_BEGIN()
	CRYINTERFACE_ADD(ICemono)
	CRYINTERFACE_END()
	
	CRYGENERATE_SINGLETONCLASS(CCemono, "Cemono", 0xc37b8ad5d62f47de, 0xa8debe525ff0fc8a)

public:
	// ICemono interface
	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual void AddClassBinding(std::shared_ptr<ICemonoClassBinding> pBinding) override;
	// -ICemono


	virtual void GetMemoryStatistics(ICrySizer * s) const;
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const;

	// Statics

private:
	bool InitializeDomain();
	void RegisterDefaultBindings();
	bool InitializeBaseClassLibraries();
	bool InitializeManager();

	MonoDomain* m_pMonoDomain;
	CCemonoAssembly* m_pBclAssembly;
	CCemonoAssembly* m_pManagerAssembly;
	MonoObject* m_pManagerObject;

	bool m_bDebugging;
	std::vector<std::shared_ptr<ICemonoClassBinding>>  m_classBindings;
};
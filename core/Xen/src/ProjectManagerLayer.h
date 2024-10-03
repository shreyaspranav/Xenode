#pragma once
#include <Xenode.h>

class ProjectManagerLayer : Xen::Layer
{
public:
	ProjectManagerLayer();
	~ProjectManagerLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float timestep) override;
	virtual void OnFixedUpdate() override;
	virtual void OnRender() override;
};
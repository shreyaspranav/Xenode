#include "pch"
#include "LayerStack.h"

#include "Log.h"

namespace Xen {

	LayerStack::LayerStack(uint32_t maxSize) : m_MaxSize(maxSize)
	{
		m_CurrentElementCount = 0;
		m_Layers = new Ref<Layer>[m_MaxSize];

		for (int i = 0; i < m_MaxSize; i++)
			m_Layers[i] = nullptr;
	}

	LayerStack::~LayerStack()
	{
		delete[] m_Layers;
	}

	void LayerStack::PushLayer(const Ref<Layer>& layer)
	{
		if (m_CurrentElementCount >= m_MaxSize) { XEN_ENGINE_LOG_ERROR("LayerStack Overflow!!"); TRIGGER_BREAKPOINT; }

		m_CurrentElementCount++;
		m_Layers[m_CurrentElementCount - 1] = layer;

		layer->OnAttach();
	}

	void LayerStack::PushLayer(const Ref<Layer>& layer, uint8_t loc)
	{
		if (m_CurrentElementCount >= m_MaxSize) { XEN_ENGINE_LOG_ERROR("LayerStack Overflow!!"); TRIGGER_BREAKPOINT; }
		else if (loc > m_MaxSize + 1) { XEN_ENGINE_LOG_ERROR("Layers are always filled in order!"); TRIGGER_BREAKPOINT; }
		else if (loc > m_CurrentElementCount + 1) { XEN_ENGINE_LOG_ERROR("Layers are always filled in order!"); TRIGGER_BREAKPOINT; }

		for (int i = m_CurrentElementCount - 1; i >= loc - 1; i--)
			m_Layers[i + 1] = m_Layers[i];

		m_Layers[loc - 1] = layer;
		m_CurrentElementCount++;

		layer->OnAttach();
	}
	void LayerStack::PopLayer()
	{
		if (m_CurrentElementCount == 0) { XEN_ENGINE_LOG_ERROR("LayerStack Underflow!!"); TRIGGER_BREAKPOINT; }

		m_Layers[m_CurrentElementCount - 1]->OnDetach();
		m_Layers[m_CurrentElementCount - 1] = nullptr;
		m_CurrentElementCount--;
	}
	void LayerStack::PopLayer(uint32_t loc)
	{
		if (loc < 1 || loc > m_CurrentElementCount) { XEN_ENGINE_LOG_ERROR("loc is out of bounds"); TRIGGER_BREAKPOINT; }
		
		m_Layers[loc - 1]->OnDetach();

		for (int i = loc - 1; i < m_CurrentElementCount - 1; i++)
			m_Layers[i] = m_Layers[i + 1]; 

		m_CurrentElementCount--;
		m_Layers[m_CurrentElementCount] = nullptr;
	}

	const Ref<Layer>& LayerStack::GetLayer(uint32_t loc)
	{
		if (loc < 1 || loc > m_CurrentElementCount) { XEN_ENGINE_LOG_ERROR("loc is out of bounds"); TRIGGER_BREAKPOINT; }
		return m_Layers[loc - 1];
	}
}
#include "pch"
#include "LayerStack.h"

#include "Log.h"

namespace Xen {

	LayerStack::LayerStack(uint32_t maxSize) : m_MaxSize(maxSize)
	{
		m_CurrentElementCount = 0;
		m_Layers = new int[m_MaxSize];

		for (int i = 0; i < m_MaxSize; i++)
			m_Layers[i] = 0;
	}

	LayerStack::~LayerStack()
	{
		delete[] m_Layers;
	}

	void LayerStack::PushLayer(int layer)
	{
		if (m_CurrentElementCount >= m_MaxSize) { XEN_ENGINE_LOG_ERROR("LayerStack Overflow!!"); }

		m_CurrentElementCount++;
		m_Layers[m_CurrentElementCount - 1] = layer;
	}

	void LayerStack::PushLayer(int layer, uint8_t loc)
	{
		if (m_CurrentElementCount >= m_MaxSize) { XEN_ENGINE_LOG_ERROR("LayerStack Overflow!!"); return; }
		else if (loc > m_MaxSize + 1) { XEN_ENGINE_LOG_ERROR("Layers are always filled in order!"); return; }
		else if (loc > m_CurrentElementCount + 1) { XEN_ENGINE_LOG_ERROR("Layers are always filled in order!"); return; }

		for (int i = m_CurrentElementCount - 1; i >= loc - 1; i--)
			m_Layers[i + 1] = m_Layers[i];

		m_Layers[loc - 1] = layer;
		m_CurrentElementCount++;
	}
	void LayerStack::PopLayer()
	{
		if (m_CurrentElementCount == 0) { XEN_ENGINE_LOG_ERROR("LayerStack Underflow!!"); }
		m_Layers[m_CurrentElementCount - 1] = 0; // = nullptr
		m_CurrentElementCount--;
	}
	void LayerStack::PopLayer(uint32_t loc)
	{
		if (loc < 1 || loc > m_MaxSize) { XEN_ENGINE_LOG_ERROR("loc is out of bounds"); }
		 
		for (int i = loc - 1; i < m_CurrentElementCount - 1; i++)
			m_Layers[i] = m_Layers[i + 1]; 

		m_CurrentElementCount--;
		m_Layers[m_CurrentElementCount] = 0;
	}

	void LayerStack::Te()
	{
		for (int i = 0; i < m_CurrentElementCount; i++) {
			XEN_ENGINE_LOG_INFO("{0}", m_Layers[i]);
		}
	}
}
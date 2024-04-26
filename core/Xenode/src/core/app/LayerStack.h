#pragma once

#include "Layer.h"
#include "Core.h"

namespace Xen {

	class XEN_API LayerStack
	{
	private:
		Ref<Layer>* m_Layers;
		uint32_t m_CurrentElementCount, m_MaxSize;

	public:
		LayerStack(uint32_t size = 10);
		~LayerStack();

		void PushLayer(const Ref<Layer>& layer);
		void PushLayer(const Ref<Layer>& layer, uint8_t loc);

		const Ref<Layer>& GetLayer(uint32_t loc);

		void PopLayer();
		void PopLayer(uint32_t loc);

		inline uint8_t GetCount() { return m_CurrentElementCount; }
		inline void FreeMem() const;

		inline const Ref<Layer>* GetLayerStack() const { return m_Layers; }
	};
}


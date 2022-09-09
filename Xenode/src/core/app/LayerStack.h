#pragma once

#include "Layer.h"

namespace Xen {

	using LayerVector = std::vector<Ref<Layer>>;

	class LayerStack
	{
	private:
		LayerVector stack;

	public:
		void PushLayer(const Ref<Layer>& layer);
		void PopLayer();

		inline const LayerVector& GetLayerStack() const { return stack; }
	};
}


#include "pch"
#include "LayerStack.h"

namespace Xen {
	void LayerStack::PushLayer(const Ref<Layer>& layer)
	{
		stack.push_back(layer);
		layer->OnAttach();
	}
	void LayerStack::PopLayer()
	{
		stack.pop_back();
		stack[stack.size() - 1]->OnDetach();
	}
}
#pragma once

#include <Core.h>
#include "Scene.h"

namespace Xen
{
	class XEN_API UndoRedoHistory
	{
	public:
		UndoRedoHistory(uint32_t maxSize = 50);

		void PushState(const Ref<Scene>& scene);
		bool Undo(Ref<Scene>& scene);
		bool Redo(Ref<Scene>& scene);
		void Clear();

		bool CanUndo() const;
		bool CanRedo() const;

	private:
		std::vector<Ref<Scene>> m_History;
		int32_t m_CurrentIndex = -1;
		uint32_t m_MaxSize;
	};
}

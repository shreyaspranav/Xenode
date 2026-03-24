#include "pch"
#include "UndoRedoHistory.h"
#include "SceneUtils.h"

namespace Xen
{
	UndoRedoHistory::UndoRedoHistory(uint32_t maxSize)
		: m_MaxSize(maxSize)
	{
	}

	void UndoRedoHistory::PushState(const Ref<Scene>& scene)
	{
		// Truncate any redo history beyond current index
		if (m_CurrentIndex + 1 < (int32_t)m_History.size())
			m_History.erase(m_History.begin() + m_CurrentIndex + 1, m_History.end());

		// Create a snapshot copy of the scene
		Ref<Scene> snapshot = std::make_shared<Scene>(scene->GetSceneType());
		SceneUtils::CopyScene(snapshot, scene);

		m_History.push_back(snapshot);

		// Enforce max size by removing oldest entries
		while (m_History.size() > m_MaxSize)
			m_History.erase(m_History.begin());

		m_CurrentIndex = (int32_t)m_History.size() - 1;
	}

	bool UndoRedoHistory::Undo(Ref<Scene>& scene)
	{
		if (!CanUndo())
			return false;

		m_CurrentIndex--;

		// Restore the snapshot at current index into the active scene
		scene->DestroyAllEntities();
		SceneUtils::CopyScene(scene, m_History[m_CurrentIndex]);

		return true;
	}

	bool UndoRedoHistory::Redo(Ref<Scene>& scene)
	{
		if (!CanRedo())
			return false;

		m_CurrentIndex++;

		scene->DestroyAllEntities();
		SceneUtils::CopyScene(scene, m_History[m_CurrentIndex]);

		return true;
	}

	void UndoRedoHistory::Clear()
	{
		m_History.clear();
		m_CurrentIndex = -1;
	}

	bool UndoRedoHistory::CanUndo() const
	{
		return m_CurrentIndex > 0;
	}

	bool UndoRedoHistory::CanRedo() const
	{
		return m_CurrentIndex + 1 < (int32_t)m_History.size();
	}
}

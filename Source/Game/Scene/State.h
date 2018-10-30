#ifndef __STATE_H__
#define __STATE_H__

#include <set>

namespace Engine
{
	class State
	{
	private:
		int mSelectionSize;
		std::set<int> mSelection;

	public:
		State()
		{
			mSelectionSize = 0;
		}

		~State()
		{
			mSelection.clear();
		}

		std::set<int>& GetSelection()
		{
			return mSelection;
		}

		void Clear()
		{
			mSelection.clear();
		}
	};
}

#endif
#ifndef __UNDO_REDO__H__
#define __UNDO_REDO__H__

#include "Command.h"

namespace Engine
{
	class UndoRedo
	{
	private:
		std::deque<Command*> mUndo;
		std::deque<Command*> mRedo;
		int mMaxSize;

	public:
		UndoRedo(int maxStackSize)
		{
			mMaxSize = maxStackSize;
		}

		~UndoRedo()
		{
			for (auto it = mUndo.begin(); it != mUndo.end(); it++)
			{
				Command* tmp = *it;
				delete tmp;
			}

			for (auto it = mRedo.begin(); it != mRedo.end(); it++)
			{
				Command* tmp = *it;
				delete tmp;
			}
		}

		void Apply(Command* c)
		{
			if (mUndo.size() == mMaxSize)
			{
				Command* tmp = mUndo.back();
				mUndo.pop_back();
				delete tmp;
			}
			mUndo.push_front(c);
			for (auto it = mRedo.begin(); it != mRedo.end(); it++)
			{
				Command* tmp = *it;
				delete tmp;
			}
			mRedo.clear();
		}

		Command* Undo()
		{
			if (mUndo.size() == 0)
			{
				return NULL;
			}

			Command* c = mUndo.front();
			mUndo.pop_front();
			c->Invert();
			if (mRedo.size() == mMaxSize)
			{
				Command* tmp = mRedo.back();
				mRedo.pop_back();
				delete tmp;
			}
			mRedo.push_front(c);
			return c;
		}

		Command* Redo()
		{
			if (mRedo.size() == 0)
			{
				return NULL;
			}

			Command* c = mRedo.front();
			mRedo.pop_front();
			c->Invert();
			if (mUndo.size() == mMaxSize)
			{
				Command* tmp = mUndo.back();
				mUndo.pop_back();
				delete tmp;
			}
			mUndo.push_front(c);
			return c;
		}

		bool CanUndo()
		{
			return (mUndo.size() > 0);
		}

		bool CanRedo()
		{
			return (mRedo.size() > 0);
		}

		ALIGNED_NEW_DELETE("UndoRedo")
	};
}

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// FileDialog.h
//
// Defines class for file dialog and its operations
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FILE_DIALOG__H__
#define __FILE_DIALOG__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include <Windows.h>
#include "Core/Log/Log.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	class FileDialog
	{
	public:
		/// <summary>Specifies type of file dialog</summary>
		enum class Type
		{
			OPEN_FILE_DIALOG,
			SAVE_FILE_DIALOG
		};

		/// <summary>Open and show file dialog</summary>
		/// <param name="log">Log for storing errors</param>
		/// <param name="title">Title for dialog</param>
		/// <param name="filter">Filter for specific file formats (Windows format - e.g. Image\0*.bmp\0All Files\0*.*\0</param>
		/// <param name="type">Dialog type (Open, Save, ...)</param>
		/// <param name="filename">Resulting filename</param>
		/// <return>True if file selected, false otherwise</return>
		static bool Show(Log* log, const std::string& title, const std::string& filter, Type type, std::string& filename);
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif

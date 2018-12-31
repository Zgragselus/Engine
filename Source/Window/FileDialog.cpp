///////////////////////////////////////////////////////////////////////////////////////////////////
//
// FileDialog.cpp
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

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "FileDialog.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration section

using namespace Engine;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

bool FileDialog::Show(Log* log, const std::string& title, const char* filter, Type type, std::string& filename)
{
	char result[MAX_PATH] = { 0 };
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = result;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = title.c_str();
	ofn.Flags = OFN_DONTADDTORECENT | (type == FileDialog::Type::OPEN_FILE_DIALOG ? OFN_FILEMUSTEXIST : 0);
	if (type == FileDialog::Type::SAVE_FILE_DIALOG ? GetSaveFileName(&ofn) : GetOpenFileName(&ofn))
	{
		filename = result;
		return true;
	}
	else
	{
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE:	log->Print("Engine::FileDialog", "The dialog box could not be created."); break;
		case CDERR_FINDRESFAILURE:	log->Print("Engine::FileDialog", "The common dialog box function failed to find a specified resource."); break;
		case CDERR_INITIALIZATION:	log->Print("Engine::FileDialog", "The common dialog box function failed during initialization. This error often occurs when sufficient memory is not available."); break;
		case CDERR_LOADRESFAILURE:	log->Print("Engine::FileDialog", "The common dialog box function failed to load a specified resource."); break;
		case CDERR_LOADSTRFAILURE:	log->Print("Engine::FileDialog", "The common dialog box function failed to load a specified string."); break;
		case CDERR_LOCKRESFAILURE:	log->Print("Engine::FileDialog", "The common dialog box function failed to lock a specified resource."); break;
		case CDERR_MEMALLOCFAILURE:	log->Print("Engine::FileDialog", "The common dialog box function was unable to allocate memory for internal structures."); break;
		case CDERR_MEMLOCKFAILURE:	log->Print("Engine::FileDialog", "The common dialog box function was unable to lock the memory associated with a handle."); break;
		case CDERR_NOHINSTANCE:		log->Print("Engine::FileDialog", "The ENABLETEMPLATE flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a corresponding instance handle."); break;
		case CDERR_NOHOOK:			log->Print("Engine::FileDialog", "The ENABLEHOOK flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a pointer to a corresponding hook procedure."); break;
		case CDERR_NOTEMPLATE:		log->Print("Engine::FileDialog", "The ENABLETEMPLATE flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a corresponding template."); break;
		case CDERR_STRUCTSIZE:		log->Print("Engine::FileDialog", "The RegisterWindowMessage function returned an error code when it was called by the common dialog box function."); break;
		case FNERR_BUFFERTOOSMALL:	log->Print("Engine::FileDialog", "The lStructSize member of the initialization structure for the corresponding common dialog box is invalid."); break;
		case FNERR_INVALIDFILENAME:	log->Print("Engine::FileDialog", "A file name is invalid."); break;
		case FNERR_SUBCLASSFAILURE:	log->Print("Engine::FileDialog", "An attempt to subclass a list box failed because sufficient memory was not available."); break;
		default:					break;
		}

		return false;
	}
}
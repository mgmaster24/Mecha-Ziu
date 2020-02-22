#ifndef _C_FILE_TRANSLATE_H
#define _C_FILE_TRANSLATE_H

#include "ModelExporter.h"
#include <maya/MPxFileTranslator.h>

class CFileTranslator : public MPxFileTranslator
{
	CModelExporter m_ModelExporter;
public:
	CFileTranslator(void);
	~CFileTranslator(void);
	////////////////////////////////////////////////////////////////////////////
	// Functions that must be written to override maya parent's virutal versions
	////////////////////////////////////////////////////////////////////////////

	// used to initialize plug in
	static void* creator(void);
	// gets called when we export
	MStatus writer(const MFileObject& file, const MString& optionString,
		FileAccessMode mode);
	// did we overwrite the writer method - yes, return true always;
	bool haveWriteMethod(void)const { return true; }
	// default extension for exported files.
	MString defaultExtension(void)const { return "Model"; }

	// filter for save dialog
	MString filter(void)const { return "*.Model"; }

};
#endif
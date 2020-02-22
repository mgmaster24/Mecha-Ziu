#include "FileTranslator.h"

CFileTranslator::CFileTranslator(void){}
CFileTranslator::~CFileTranslator(void){}

// return a copy of ouselves to maya to use as the plug in.
void* CFileTranslator::creator(void)
{
	return new CFileTranslator;
}

//this is the function we will need to complete to actually export our model
MStatus CFileTranslator::writer(const MFileObject& file, const MString& optionString, FileAccessMode mode)
{
	//this function is the one that gets called when you actually export
	//make sure to call all relevent functions from the CModelExporter class
	m_ModelExporter.ClearAndReset();
	m_ModelExporter.ExportMeshData();
	m_ModelExporter.WriteModelData_Binary(file.fullName().asChar());
	m_ModelExporter.WriteModelData_XML(MString(file.name() + ".xml").asChar());
	//return success
	return MStatus::kSuccess;
}
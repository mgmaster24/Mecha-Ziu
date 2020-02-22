#include <maya/MSimple.h>
#include <maya/MGlobal.h>
#include "FileTranslator.h"

MStatus initializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj,"Seven Head");

	status = plugin.registerFileTranslator("SH_Model_Exporter",NULL, CFileTranslator::creator);
	if(status == MS::kSuccess)
		MGlobal::displayInfo(MString("Plugin Initialized"));
	else
		MGlobal::displayInfo(MString("Plugin failed to Initialize"));

	return status;
}
MStatus uninitializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj);

	status = plugin.deregisterFileTranslator("SH_Model_Exporter");
	if(status == MS::kSuccess)
		MGlobal::displayInfo(MString("Plugin Uninitialized"));
	else 
		MGlobal::displayInfo(MString("Plugin failed to Uninitialize"));
	
	return status;
}
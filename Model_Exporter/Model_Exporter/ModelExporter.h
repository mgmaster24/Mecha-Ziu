#ifndef _C_MODELEXPORT_H
#define _C_MODELEXPORT_H

#include <maya/MFloatPoint.h>
#include <maya/MFloatVector.h>
#include <maya/MFnTransform.h>
#include <maya/MMatrix.h>
#include <maya/MFnMesh.h>
#include <maya/MTime.h>
#include <string>
#include <vector>
#include <fstream>

using std::vector;
using std::string;

class CModelExporter
{
	typedef struct _tKeyFrame
	{
		MTime keyTime;
		MMatrix	maTransform;
	}KeyFrame;

	// definition of a vertice in model
	typedef struct _tVert
	{
		float			fU;
		float			fV;
		MFloatPoint			MFP_pos;
		MFloatVector		MFV_norm;
		bool operator == (const _tVert& ref)
		{
			if(this->MFP_pos == ref.MFP_pos && this->MFV_norm == ref.MFV_norm
				&& this->fU == ref.fU && this->fV == ref.fV)
				return true;
			return false;
		}
	}UniqueVert;
	
	// used for indexed geometry
	typedef struct _tTri
	{
		unsigned int uiIndicies[3];
	}Tri;

	
	// the collection of vertices and indicies needed in mesh
	typedef struct _tMesh
	{
		unsigned int			uiStartIndex;
		unsigned int			uiEndIndex;
		MString					szTextureFileName;
		MString					szName;
		MString					szTransformName;
		MMatrix					maMeshTransform;
		//MMatrix					maLocalTransform;
		
		MString					szParentName;
		//MMatrix					maParenLocal;
		//MMatrix					maParentWorld;
		
		vector<UniqueVert>		UniqueVerts;
		vector<Tri>				Tris;	
		vector<KeyFrame>		KeyFrames;
	}Mesh;
	
	vector<Mesh> Meshes;
	double dAnimDuration;
	vector<UniqueVert> masterVertList;
	vector<Tri> masterIndexList;
	// gets the specific mesh info
	MStatus GetMeshSpecifics(MFnMesh& currMesh, Mesh* thisMesh);

public:
	////////////////////////////////////////////////////////////
	// Ctor and Dtor
	CModelExporter(void);
	~CModelExporter(void);
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	// necessary functions for exporting model information
	void GetTextureName(MFnMesh& theMesh, MString& texFile); 
	MStatus ClearAndReset(void);
	MStatus ExportMeshData(void);
	MStatus	ExportAnimData(Mesh& thisMesh, MDagPath& thePath, double& dAnimDur);
	MStatus WriteModelData_Binary(const char* szFileName);
	MStatus WriteModelData_XML(const char* szFileName);
	void	KeyFrameSmartInsert(Mesh& theMesh, KeyFrame& keyFrame);
	////////////////////////////////////////////////////////////
};
#endif

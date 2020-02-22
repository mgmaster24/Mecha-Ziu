#include "ModelExporter.h"

// maya objects needed to export ////////////////////////////////
#include <maya/MObjectArray.h>
#include <maya/MFnSet.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItDag.h>
#include <maya/MFloatPointArray.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MItGeometry.h>
#include <maya/MAnimUtil.h>
#include <maya/MAnimControl.h>
#include <maya/MPlugArray.h>
#include <maya/MFnAnimCurve.h>
/////////////////////////////////////////////////////////////////
CModelExporter::CModelExporter(void){}
CModelExporter::~CModelExporter(void){}

void CModelExporter::GetTextureName(MFnMesh& theMesh, MString& texFile)
{
	MStatus			status;
	MObjectArray	meshSets;
	MObjectArray	meshComps;
	unsigned int	meshSetCnt;
	texFile.clear();
	
	theMesh.getConnectedSetsAndMembers(0, meshSets, meshComps, true);
	meshSetCnt = meshSets.length();
	
	if(meshSetCnt > 1)
		--meshSetCnt;
	
	for(unsigned int i = 0; i < meshSetCnt; ++i)
	{
		MObject currSet = meshSets[i];
		MObject currComp = meshComps[i];

		MFnSet	fnSet(currSet);

		MItMeshPolygon meshSetIter(theMesh.dagPath(), currComp, &status);
		if(!status)
			continue;

		MFnDependencyNode	fnNode(currSet);
		MPlugArray			thePlugs;
		
		// used to catch all plugins
		MPlug shaderPlug;
		shaderPlug = fnNode.findPlug("surfaceShader",&status);
		if(!status || shaderPlug.isNull())
			continue;
		shaderPlug.connectedTo(thePlugs, true, false, &status);
		if(!status || thePlugs.length() != 1)
			continue;
		
		MPlug colorPlug = MFnDependencyNode(thePlugs[0].node()).findPlug("color", &status);
		if(!status)
			continue;

		MItDependencyGraph iterDepGraph( colorPlug, MFn::kFileTexture,
			MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, 
			MItDependencyGraph::kNodeLevel, &status );
		if(!status)
			continue;
		iterDepGraph.disablePruningOnFilter();
		if(iterDepGraph.isDone())
			continue;

		MObject textNode = iterDepGraph.thisNode();
		MPlug filePlug = MFnDependencyNode(textNode).findPlug("fileTextureName");
		
		MString texName("");
		if(filePlug.getValue(texName) && texName != "")
			texFile = texName;
	}
}
MStatus CModelExporter::GetMeshSpecifics(MFnMesh& currMesh, Mesh* thisMesh)
{
	MString tex;
	GetTextureName(currMesh, tex);
	thisMesh->szTextureFileName = tex.asChar();
	thisMesh->szName = currMesh.name().asChar();

	return MStatus::kSuccess;
}
MStatus CModelExporter::ClearAndReset(void)
{
	Meshes.clear();
	masterVertList.clear();
	masterIndexList.clear();
	dAnimDuration = 0.0;
	return MStatus::kSuccess;
}
MStatus CModelExporter::ExportMeshData(void)
{
	MDagPath			currPath;	// current path in scene
	MFloatPointArray	posArray;	// array for maya's master set of positions
	MFloatVectorArray	normArray;	// array for maya's master set of normals
	MFloatArray			uArray;		// array for maya's master set of u channels
	MFloatArray			vArray;		// array for maya's master set of v channels
	MPlug				mPlug;
	
	bool				isDup = false, isStart = true;
	unsigned int		lastStartIndex = 0, lastEndIndex = 0;

	// create DAG iterator that traverses meshes only, depth first
	MItDag dagIter(MItDag::kDepthFirst,MFn::kMesh);

	while(!dagIter.isDone())
	{
		// get the current path
		dagIter.getPath(currPath);
		// create a mesh function set
		MFnMesh	currMesh(currPath);
		

		// if not valid continue to next iteration
		if(currMesh.isIntermediateObject())
		{
			dagIter.next();
			continue;
		}
		
		Mesh tempMesh;
		ZeroMemory((void*)&tempMesh, sizeof(Mesh));
		
		//ExportTransformData(Transforms, RootNode);
		// get the specifics for this mesh
		GetMeshSpecifics(currMesh, &tempMesh);
		// get the world transform associated with this mesh
		tempMesh.maMeshTransform = currPath.inclusiveMatrix();

		MFnDagNode currNode(currPath);

		bool done = false;
		
		for(int i = 0; i < (signed)currNode.parentCount() && !done; ++i)
		{
			if(currNode.parent(i).apiType() == MFn::kTransform)
			{
				MFnDagNode transformNode(currNode.parent(i));
				MDagPath transformPath;
				transformNode.getPath(transformPath);
				MFnTransform currTransform(transformPath);
				//tempMesh.maLocalTransform = currTransform.transformationMatrix();
				tempMesh.szTransformName = currTransform.partialPathName();
				for(int j = 0; j < (signed)currTransform.parentCount() && !done; ++j)
				{
					if(currTransform.parent(i).apiType() == MFn::kTransform)
					{
						MFnDagNode parentTransformNode(currTransform.parent(j));
						MDagPath parentPath;
						parentTransformNode.getPath(parentPath);
						//tempMesh.maParentWorld = parentPath.inclusiveMatrix();
						MFnTransform parentTransform(parentPath);
						//tempMesh.maParenLocal = parentTransform.transformationMatrix();
						tempMesh.szParentName = parentTransform.partialPathName();
						done = true;
					}
				}
			}
		}

		// continue getting mesh data
		currMesh.getPoints(posArray);
		currMesh.getNormals(normArray);
		currMesh.getUVs(uArray, vArray);
		
		// iterate through the triangles
		MItMeshPolygon polyIter(currPath);

		if(isStart)
		{
			tempMesh.uiStartIndex = 0;
			isStart = false;
		}
		else
			tempMesh.uiStartIndex = (unsigned)masterIndexList.size();

		while(!polyIter.isDone())
		{
			UniqueVert		vert;
			Tri				tempTri;
			unsigned int	index = 0;

			for(int i = 0; i < 3; ++i)
			{
				int uvIndex = 0;
				vert.MFP_pos = posArray[polyIter.vertexIndex(i)];
				vert.MFV_norm = normArray[polyIter.normalIndex(i)];
				polyIter.getUVIndex(i, uvIndex);
				vert.fU = uArray[uvIndex];
				vert.fV = vArray[uvIndex];
				for(size_t j = 0; j < tempMesh.UniqueVerts.size(); ++j)
				{
					if(vert == tempMesh.UniqueVerts[j])
					{
						isDup = true;
						index = (unsigned)j;		
					}
				}
				if(isDup)
					tempTri.uiIndicies[i] = index;
				else
				{
					tempTri.uiIndicies[i] = (unsigned)tempMesh.UniqueVerts.size();
					tempMesh.UniqueVerts.push_back(vert);
				}
				isDup = false;
			}
				// done with triangle, add triangle to list
			tempMesh.Tris.push_back(tempTri);	
		
			polyIter.next();
		}	
		for(size_t x = 0; x < tempMesh.Tris.size(); ++x)
		{
			tempMesh.Tris[x].uiIndicies[0] += (unsigned)masterVertList.size();
			tempMesh.Tris[x].uiIndicies[1] += (unsigned)masterVertList.size();
			tempMesh.Tris[x].uiIndicies[2] += (unsigned)masterVertList.size();
			
			masterIndexList.push_back(tempMesh.Tris[x]);
		}
		// add verts and indices for mesh to master list mesh by mesh
		for(size_t i = 0; i < tempMesh.UniqueVerts.size(); ++i)
			masterVertList.push_back(tempMesh.UniqueVerts[i]);
		
		// update last index for this mesh
		tempMesh.uiEndIndex = (unsigned)masterIndexList.size() - 1;	
		// get animation data
		for(int n = 0; n < (signed)currNode.parentCount(); ++n)
		{
			if(currNode.parent(n).apiType() == MFn::kTransform)
			{
				MDagPath parentPath;
				MFnDagNode parentNode(currNode.parent(n));
				parentNode.getPath(parentPath);
				if(MAnimUtil::isAnimated(parentPath))
				{
					ExportAnimData(tempMesh, parentPath, dAnimDuration);
					break;	
				}
			}
		}
		
		// add mesh to model list of meshes
		Meshes.push_back(tempMesh);
		
		// set isStart back to true to get start index of next mesh
		dagIter.next();
	}
	
	return MStatus::kSuccess;	
}

void CModelExporter::KeyFrameSmartInsert(Mesh& theMesh, KeyFrame& keyFrame)
{
	if(!theMesh.KeyFrames.size())
	{
		theMesh.KeyFrames.push_back(keyFrame);
		return;
	}

	for(unsigned i = 0; i < theMesh.KeyFrames.size(); ++i)
		if(theMesh.KeyFrames[i].keyTime == keyFrame.keyTime)
			return;

	std::vector<KeyFrame>::iterator iter; 
	iter = theMesh.KeyFrames.begin(); 
	bool Inserted = false;
	for(unsigned j = 0; j < theMesh.KeyFrames.size(); ++j)
	{
		if(keyFrame.keyTime < (*iter).keyTime)
		{
			theMesh.KeyFrames.insert(iter, keyFrame);
			Inserted = true;
			break;
		}

		if(iter != theMesh.KeyFrames.end())
			iter++; 
	}

	if(!Inserted)
		theMesh.KeyFrames.push_back(keyFrame);
}


MStatus	CModelExporter::ExportAnimData(Mesh& thisMesh, MDagPath& thePath, double& dAnimDur)
{
	dAnimDur = 0.0;
	
	double endTime = MAnimControl::animationEndTime().as(MTime::kSeconds);
	double startTime = MAnimControl::animationStartTime().as(MTime::kSeconds);
	dAnimDur = endTime - startTime;
	MAnimControl::setCurrentTime(startTime);

	MPlugArray animPlugArray;
	MObjectArray animObjArray;
	
	MAnimUtil::findAnimatedPlugs(thePath, animPlugArray);

	for(unsigned l = 0; l < animPlugArray.length(); ++l)
	{
		MAnimUtil::findAnimation(animPlugArray[l], animObjArray);	
		for(unsigned n = 0; n < animObjArray.length(); ++n)
		{			
			MFnAnimCurve animCurve(animObjArray[n]);
			for(unsigned k = 0; k < animCurve.numKeys(); ++k)
			{
				// Itterate through all of the curves keys and get the time
				KeyFrame tempKey;
				tempKey.keyTime = animCurve.time(k);
				if(tempKey.keyTime.as(MTime::kSeconds) < 0 || tempKey.keyTime.as(MTime::kSeconds) > (dAnimDur + 0.01))
					continue;
				//	Set the new time
				MAnimControl::setCurrentTime(tempKey.keyTime);

				// world transform for this frame
				// get exclusive matrix will return the local matrix on this path
				tempKey.maTransform = thePath.inclusiveMatrix(); 
				KeyFrameSmartInsert(thisMesh, tempKey); 
			}
		}
	}

	return MStatus::kSuccess;	
}
MStatus CModelExporter::WriteModelData_Binary(const char* szFileName)
{
	std::ofstream fOut(szFileName, std::ios_base::out | std::ios_base::binary);
	char buffer[128];
	ZeroMemory(buffer, 128);
	// write texture file name
	string szTemp = Meshes[0].szTextureFileName.asChar();
	size_t nOffset = szTemp.find_last_of("/");
	string Name = szTemp.substr(nOffset + 1);
	fOut.write(Name.c_str(), (std::streamsize)Name.size()+1);
	
	unsigned int n = 0;
	// write number of meshes
	n = (unsigned)Meshes.size();
	fOut.write((char*)&n, sizeof(unsigned int));
	// write number of verts
	n = (unsigned)masterVertList.size();
	fOut.write((char*)&n, sizeof(unsigned int));
	// write number of indices
	n = (unsigned)masterIndexList.size();
	fOut.write((char*)&n, sizeof(unsigned int));
	// write all verts 	
	for(size_t i = 0; i < masterVertList.size(); ++i)
	{
		// 1. position
		float f;
		f = masterVertList[i].MFP_pos.x;
		fOut.write((char*)&f, sizeof(float));
		f = masterVertList[i].MFP_pos.y;
		fOut.write((char*)&f, sizeof(float));
		f = masterVertList[i].MFP_pos.z;
		fOut.write((char*)&f, sizeof(float));
		// 2. normals
		f = masterVertList[i].MFV_norm.x;
		fOut.write((char*)&f, sizeof(float));
		f = masterVertList[i].MFV_norm.y;
		fOut.write((char*)&f, sizeof(float));
		f = masterVertList[i].MFV_norm.z;
		fOut.write((char*)&f, sizeof(float));
		// 3. texture coordinates
		f = masterVertList[i].fU;
		fOut.write((char*)&f, sizeof(float));
		f = masterVertList[i].fV;
		fOut.write((char*)&f, sizeof(float));
	}
	// write all indices 
	for(size_t j = 0; j < masterIndexList.size(); ++j)
		fOut.write((char*)&masterIndexList[j], sizeof(unsigned int) * 3);

	// write all meshes
	for(size_t k = 0; k < Meshes.size(); ++k)
	{
		string temp; 
		// write mesh name
		temp = Meshes[k].szName.asChar();
		fOut.write(temp.c_str(), (std::streamsize)(temp.size() + 1));

		temp = Meshes[k].szTransformName.asChar();
		fOut.write(temp.c_str(), (std::streamsize)(temp.size() + 1));
		// write parent name
		temp = Meshes[k].szParentName.asChar();
		fOut.write(temp.c_str(), (std::streamsize)(temp.size() + 1));
		// write vert count
		unsigned int x = 0;
		x = (unsigned)Meshes[k].UniqueVerts.size();
		fOut.write((char*)&x, sizeof(unsigned int));
		// write index count
		x = (unsigned)Meshes[k].Tris.size();
		fOut.write((char*)&x, sizeof(unsigned int));
		// write start index into master index list
		fOut.write((char*)&Meshes[k].uiStartIndex, sizeof(unsigned int));
		// write end index into master index list
		fOut.write((char*)&Meshes[k].uiEndIndex, sizeof(unsigned int));
		// write world transform
		MMatrix rowMaj = Meshes[k].maMeshTransform;
		float mat[4][4];
		for(int y = 0; y < 4; y++)
			for(int z = 0; z < 4; z++)
				mat[y][z] = (float)rowMaj.matrix[y][z];
	
		fOut.write((char*)&mat, sizeof(float) * 16);
	}
	// write animation data
	n = (unsigned)Meshes[0].KeyFrames.size();
	// write duration
	fOut.write((char*)&dAnimDuration, sizeof(double));
	// write number of key frames
	fOut.write((char*)&n, sizeof(unsigned int));
	bool wroteKeyTime = false;
	for(unsigned l = 0; l < Meshes[0].KeyFrames.size(); ++l)
	{
		for(unsigned m = 0; m < Meshes.size(); ++m)
		{		
			// write key time for mesh
			if(!wroteKeyTime)
			{
				float keytime = (float)Meshes[m].KeyFrames[l].keyTime.as(MTime::kSeconds);
				fOut.write((char*)&keytime, sizeof(float));
				wroteKeyTime = true;
			}
			// write world transforms for key frame
			MMatrix rowMaj = Meshes[m].KeyFrames[l].maTransform.transpose();
			float mat[4][4];
			for(int y = 0; y < 4; y++)
				for(int z = 0; z < 4; z++)
					mat[y][z] = (float)rowMaj.matrix[y][z];
			fOut.write((char*)&mat, sizeof(float) * 16);
		}
		wroteKeyTime = false;
	}
	return MStatus::kSuccess;
}

MStatus CModelExporter::WriteModelData_XML(const char* szFileName)
{
	std::ofstream fOut(szFileName);
	//must be the first thing written out to xml file, makes xml file legal
	char XMLProlog[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	fOut << XMLProlog << '\n';

	fOut << "<Model>\n";
	
	string szTemp = Meshes[0].szTextureFileName.asChar();
	size_t nOffset = szTemp.find_last_of("/");
	string Name = szTemp.substr(nOffset + 1);
	fOut << "<Texture>" << Name << "</Texture>\n"; 
	
	int temp = (int)Meshes.size();
	fOut << "<MeshCount>" << temp << "</MeshCount>\n";
	fOut << "<TransformCount>" << temp << "</TransformCount>\n";
	
	temp = (int)masterVertList.size();
	fOut << "<MasterVertCount>" << temp << "</MasterVertCount>\n";
	
	temp = (int) masterIndexList.size();
	fOut << "<MasterIndexCount>" << temp << "</MasterIndexCount>\n";
	
	char buffer[128];
	double mat[4][4];
	MMatrix rowMaj;
	fOut << "<Vertices>\n";
	for(size_t i = 0; i < masterVertList.size(); ++i)
	{
		fOut << "<Vertex index = \"" << i << "\">\n";
		fOut << "<Position>\n";
		ZeroMemory((void*)buffer,128);
		sprintf_s(buffer,128,"<x>%f</x>\n", masterVertList[i].MFP_pos.x);
		fOut << buffer; 
		ZeroMemory((void*)buffer,128);
		sprintf_s(buffer,128,"<y>%f</y>\n", masterVertList[i].MFP_pos.y);
		fOut << buffer;
		ZeroMemory((void*)buffer,128);
		sprintf_s(buffer,128,"<z>%f</z>\n", masterVertList[i].MFP_pos.z);
		fOut << buffer;
		fOut << "</Position>\n";
		fOut << "<Normals>\n";
		ZeroMemory((void*)buffer,128);
		sprintf_s(buffer,128,"<Nx>%f</Nx>\n", masterVertList[i].MFV_norm.x);
		fOut << buffer;
		ZeroMemory((void*)buffer,128);
		sprintf_s(buffer,128,"<Ny>%f</Ny>\n", masterVertList[i].MFV_norm.y);
		fOut << buffer;
		ZeroMemory((void*)buffer,128);
		sprintf_s(buffer,128,"<Nz>%f</Nz>\n", masterVertList[i].MFV_norm.z);
		fOut << buffer;
		fOut << "</Normals>\n";
		fOut << "<TextureCoordinates>\n";
		ZeroMemory((void*)buffer,128);
		sprintf_s(buffer,128,"<Tu>%f</Tu>\n", masterVertList[i].fU);
		fOut << buffer;
		ZeroMemory((void*)buffer,128);
		sprintf_s(buffer,128,"<Tv>%f</Tv>\n", masterVertList[i].fV);
		fOut << buffer;
		fOut << "</TextureCoordinates>\n";
		fOut << "</Vertex>\n";
	}
	fOut << "</Vertices>\n";
	
	fOut << "<Triangles>\n";
	for(size_t i = 0; i < masterIndexList.size(); ++i)
	{
		fOut << "<Triangle>\n";
		ZeroMemory((void*)buffer,128);
		sprintf_s(buffer,128,"<i1>%i</i1>\n",masterIndexList[i].uiIndicies[0]);
		fOut << buffer;
		ZeroMemory((void*)buffer,128);
		sprintf_s(buffer,128,"<i2>%i</i2>\n",masterIndexList[i].uiIndicies[1]);
		fOut << buffer;
		ZeroMemory((void*)buffer,128);
		sprintf_s(buffer,128,"<i3>%i</i3>\n",masterIndexList[i].uiIndicies[2]);
		fOut << buffer;
		fOut << "</Triangle>\n";	
	}
	fOut << "</Triangles>\n";

	fOut << "<Meshes>\n";
	for(size_t i = 0; i < Meshes.size(); ++i)
	{
		fOut << "<Mesh name = \"" << Meshes[i].szName << "\">\n";
		fOut << "<Transform>" << Meshes[i].szTransformName << "</Transform>\n";
		fOut << "<TransformParent name = \"" << Meshes[i].szParentName << "\">"
			<< "</TransformParent>\n";
		fOut << "<VertCount>" << Meshes[i].UniqueVerts.size() << "</VertCount>\n";
		fOut << "<IndexCount>" << Meshes[i].Tris.size() << "</IndexCount>\n";
		fOut << "<MasterIndexList_Start>" << Meshes[i].uiStartIndex << "</MasterIndexList_Start>\n";
		fOut << "<MasterIndexList_End>" << Meshes[i].uiEndIndex << "</MasterIndexList_End>\n"; 
		fOut << "<WorldTransform>\n";
		mat[4][4];
		rowMaj = Meshes[i].maMeshTransform;
		memcpy(&mat[0], &rowMaj.matrix[0], 128);
		for(int r = 0; r < 4; r++)
			for(int c = 0; c < 4; c++)
			{
				if(c == 0)
				{
					fOut << "<r" << r << ">";
					ZeroMemory((void*)buffer, 128);
					sprintf_s(buffer,128,"%f",mat[r][c]);
					fOut <<"<x>" << buffer <<"</x>";
				}
				else if(c == 1)
				{
					ZeroMemory((void*)buffer, 128);
					sprintf_s(buffer,128,"%f",mat[r][c]);
					fOut <<"<y>" << buffer <<"</y>";
				}
				else if(c == 2)
				{
					ZeroMemory((void*)buffer, 128);
					sprintf_s(buffer,128,"%f",mat[r][c]);
					fOut <<"<z>" << buffer <<"</z>";
				}
				else
				{
					ZeroMemory((void*)buffer, 128);
					sprintf_s(buffer,128,"%f",mat[r][c]);
					fOut <<"<w>" << buffer <<"</w>";
					fOut << "</r" << r << ">\n";
				}		
			}
		fOut << "</WorldTransform>\n";
		fOut << "</Mesh>\n";
	}
	fOut << "</Meshes>\n";

	fOut << "<AnimationData>\n";
	ZeroMemory((void*)buffer, 128);
	sprintf_s(buffer, 128, "%f", dAnimDuration);
	fOut << "<Duration time = \"" << buffer << "\"></Duration>\n";
	fOut << "<NumKeyFrames num = \"" << Meshes[0].KeyFrames.size() << "\"></NumKeyFrames>\n"; 
	for(unsigned j = 0; j < Meshes[0].KeyFrames.size(); ++j)
	{
		for(unsigned k = 0; k < Meshes.size(); ++k)
		{
			ZeroMemory((void*)buffer, 128);
			sprintf_s(buffer, 128, "%f", Meshes[k].KeyFrames[j].keyTime.as(MTime::kSeconds));
			fOut << "<KeyFrame time = \"" << buffer 
				<< "\">\n";
			fOut << "<KeyFrameIndex>" << j << "</KeyFrameIndex>\n";
			rowMaj = Meshes[k].KeyFrames[j].maTransform.transpose();

			ZeroMemory((void*)mat, sizeof(double) * 16);
			fOut << "<KeyWorldMatrix>\n";
			memcpy(&mat[0], &rowMaj.matrix[0], 128);
			for(int r = 0; r < 4; r++)
				for(int c = 0; c < 4; c++)
				{
					if(c == 0)
					{
						fOut << "<r" << r << ">";
						ZeroMemory((void*)buffer, 128);
						sprintf_s(buffer,128,"%f",mat[r][c]);
						fOut <<"<x>" << buffer <<"</x>";
					}
					else if(c == 1)
					{
						ZeroMemory((void*)buffer, 128);
						sprintf_s(buffer,128,"%f",mat[r][c]);
						fOut <<"<y>" << buffer <<"</y>";
					}
					else if(c == 2)
					{
						ZeroMemory((void*)buffer, 128);
						sprintf_s(buffer,128,"%f",mat[r][c]);
						fOut <<"<z>" << buffer <<"</z>";
					}
					else
					{
						ZeroMemory((void*)buffer, 128);
						sprintf_s(buffer,128,"%f",mat[r][c]);
						fOut <<"<w>" << buffer <<"</w>";
						fOut << "</r" << r << ">\n";
					}		
				}
				fOut << "</KeyWorldMatrix>\n";
				fOut << "</KeyFrame>\n";
		}
	}
	fOut << "</AnimationData>\n";
	fOut << "</Model>\n";
	
	return MStatus::kSuccess;
}

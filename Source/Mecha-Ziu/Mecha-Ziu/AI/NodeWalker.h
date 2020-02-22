/**************************************************************************
File Name	:	WayNode

Description	:	Node containing information about a point in space
				and directions of traversal
**************************************************************************/
#ifndef COLDE_WAY_NODE_H
#define COLDE_WAY_NODE_H
#include <d3dx9.h>

class CNodeWalker
{
public:
	enum TRAVERSE_TYPE{TRT_NORMAL, TRT_OFFSET, TRT_LINK_CHAIN};
public:
	D3DXMATRIX*			m_matMatrix;			//	The matrix being traversed to
	unsigned short		m_nTraverseType;		//	The type of traversal used for this node
	unsigned char		m_nLinkStep;			//	The current Step in the link
	unsigned char		m_nLinkTotal;			//	The current Step in the link
	D3DXMATRIX*			m_pLinkNode[2];			//	A node this link connects to

public:
	//	Constructor
	CNodeWalker()	:	m_nTraverseType(TRT_NORMAL), 
		m_matMatrix(0), m_nLinkStep(0), m_nLinkTotal(0)
	{	m_pLinkNode[0] = m_pLinkNode[1] = 0;	}


	/*************************************************************************
	Description	:	Request the travel matrix
	**************************************************************************/
	const D3DXMATRIX* ConstMatrix()		{	return m_matMatrix;	}
	D3DXMATRIX* Matrix()				{	return m_matMatrix;	}

	/*************************************************************************
	Description	:	Operators
	**************************************************************************/
	bool operator==(const CNodeWalker& node)
	{
		return (m_matMatrix == node.m_matMatrix) 
			&& (m_nTraverseType == node.m_nTraverseType);
	}
	bool operator!=(const CNodeWalker& node)
	{
		return (m_matMatrix != node.m_matMatrix) 
			|| (m_nTraverseType != node.m_nTraverseType);
	}

	bool operator==(const D3DXMATRIX& node)
	{	return (m_matMatrix == m_matMatrix);	}
	bool operator!=(const D3DXMATRIX& node)
	{	return (m_matMatrix != m_matMatrix);	}
};

#endif
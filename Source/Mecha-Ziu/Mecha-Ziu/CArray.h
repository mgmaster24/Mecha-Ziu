/*****************************************************************************
File Name	:	CArray

Description	:	Stores and manages an array of information
*****************************************************************************/
#ifndef COLD_TRACK_VECTOR_H
#define COLD_TRACK_VECTOR_H
#include <memory>

template<class OBJECT_TYPE>
class CArray
{
private:
	OBJECT_TYPE *m_pArray;			//	The array of array
	unsigned int m_nSize;			//	The size of the array
	unsigned int m_nCapacity;		//	The capacity of the array

public:
	//	Copy constructor, will null the copied from object
	CArray(CArray& copyArray)
	{
		m_pArray = copyArray.m_pArray;
		m_nSize = copyArray.m_nSize;
		m_nCapacity = copyArray.m_nCapacity;

		copyArray.m_pArray = 0;
		copyArray.m_nSize = copyArray.m_nCapacity = 0;
	}

	//	Assignment constructor, will null the copied from object
	CArray &operator=(CArray& copyArray)	
	{
		//	Check that this isnt the same array
		if(this == &copyArray)
			return *this;

		m_pArray = copyArray.m_pArray;
		m_nSize = copyArray.m_nSize;
		m_nCapacity = copyArray.m_nCapacity;

		copyArray.m_pArray = 0;
		copyArray.m_nSize = copyArray.m_nCapacity = 0;

		return *this;
	}
public:
	//	Constructor
	CArray(void) : m_nSize(0), m_nCapacity(0), m_pArray(0)	{	}

	//	Destructor
	~CArray(void)
	{
		//	Free the array memory
		if(m_pArray != 0)
			delete[] m_pArray;
	}

	/**************************************************************************
	Description	:	Returns the size of the vector
	**************************************************************************/
	unsigned int size()		{	return m_nSize;	}

	/**************************************************************************
	Description	:	Return the Object in the List
	**************************************************************************/
	unsigned int capacity()	{	return m_nCapacity;	}

	/**************************************************************************
	Description	:	Return the Object at the index
	**************************************************************************/
	OBJECT_TYPE &GetIndex(unsigned int nIndex)	{	return m_pArray[nIndex];	}

	/**************************************************************************
	Description	:	Stores and manages an array of information
	**************************************************************************/
	void AddObject(OBJECT_TYPE object);

	/**************************************************************************
	Description	:	Remove an object from the vector
	**************************************************************************/
	void RemoveObject(unsigned int nIndex);
	void RemoveObject(OBJECT_TYPE &object);

	/**************************************************************************
	Description	:	Array Index Operator
	**************************************************************************/
	OBJECT_TYPE &operator[](unsigned int nIndex);

	/**************************************************************************
	Description	:	Clears the Array
	**************************************************************************/
	void clear();

	/**************************************************************************
	Description	:	Set the Capacity and Build the Array
	**************************************************************************/
	void SetCapacity(unsigned int nSize);

	/**************************************************************************
	Description	:	Set the object pointer location
	**************************************************************************/
	void SetSize(unsigned int nSize)	{m_nSize = nSize;}

	/**************************************************************************
	Description	:	Copy the data stored in the CArray
	**************************************************************************/
	void Copy(const CArray<OBJECT_TYPE> &copy);
};

template<class OBJECT_TYPE>
void CArray<OBJECT_TYPE>::AddObject(OBJECT_TYPE object)
{
	//	Check that the array is valid
	if(m_pArray == 0)
	{
		//	Create the Array
		m_pArray = new OBJECT_TYPE[1];
		m_nCapacity = 1;
	}	//	Check that the capacity is valid
	else if(m_nCapacity <= m_nSize)
	{
		//	Increase the capacity
		int tempCap = m_nCapacity << 1;

		//	Copy the previous data
		OBJECT_TYPE *tempArray = new OBJECT_TYPE[tempCap];

		//	Copy the previous object
		memcpy(tempArray,m_pArray,sizeof(OBJECT_TYPE) * m_nCapacity);

		//	Set the new capacity
		m_nCapacity = tempCap;

		//	Free the previous object
		delete[] m_pArray;

		//	Set the array to the temp array
		m_pArray = tempArray;
	}

	//	Add the object to the array
	m_pArray[m_nSize] = object;
	//	Incriment the Size
	m_nSize += 1;
}

template<class OBJECT_TYPE>
void CArray<OBJECT_TYPE>::RemoveObject(unsigned int nIndex)
{
	//	Check that the index is in range
	if(nIndex >= m_nSize)
		return;

	//	Switch the objects
	OBJECT_TYPE tempOject = m_pArray[nIndex];
	m_pArray[nIndex] = m_pArray[m_nSize -1];
	m_pArray[m_nSize - 1] = tempOject;

	//	Negate the Size by One
	m_nSize -= 1;
}
template<class OBJECT_TYPE>
void CArray<OBJECT_TYPE>::RemoveObject(OBJECT_TYPE &object)
{
	//	Itterate and find the object
	for(unsigned int itter = 0; itter < m_nSize; itter++)
	{
		//	Check if the ID's Match
		if(m_pArray[itter] == object)
		{
			//	Switch the objects
			OBJECT_TYPE tempOject = m_pArray[itter];
			m_pArray[itter] = m_pArray[m_nSize -1];
			m_pArray[m_nSize - 1] = tempOject;
			
			//	Negate the Size by One
			m_nSize -= 1;
			break;
		}
	}
}

template<class OBJECT_TYPE>
void CArray<OBJECT_TYPE>::clear()
{
	//	Check that the array is valid
	if(m_pArray != 0)
	{
		delete[] m_pArray;
		m_pArray = 0;
	}

	//	Reset Size and Capacity
	m_nSize = m_nCapacity = 0;
}

template<class OBJECT_TYPE>
OBJECT_TYPE &CArray<OBJECT_TYPE>::operator[](unsigned int nIndex)
{
	//	Return the object
	return m_pArray[nIndex];
}

template<class OBJECT_TYPE>
void CArray<OBJECT_TYPE>::SetCapacity(unsigned int nSize)
{
	//	If the Array Currently Exists, Size it
	if(m_pArray != 0)
		delete[] m_pArray;

	//	Create the Array
	m_pArray = new OBJECT_TYPE[nSize];
	m_nCapacity = nSize;
	m_nSize = 0;
}

template<class OBJECT_TYPE>
void CArray<OBJECT_TYPE>::Copy(const CArray<OBJECT_TYPE> &copy)
{
	//	Clear the copy data
	this->clear();

	//	Copy the capacity and size
	this->m_nCapacity = copy.m_nCapacity;
	this->m_nSize = copy.m_nSize;

	//	Check that the pointer is not null
	if(copy.m_pArray != 0)
	{
		//	Allocate the copy data
		this->m_pArray = new OBJECT_TYPE[m_nCapacity];

		//	Copy all the data
		for(unsigned int i = 0; i < m_nCapacity; i++)
			this->m_pArray[i] = copy.m_pArray[i];
	}
	else
		this->m_pArray = 0;
}

#endif
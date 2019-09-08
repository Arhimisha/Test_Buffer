#include "pch.h"
#include <memory>

#ifndef ISPLITTER_H
#define ISPLITTER_H
  


//���������  ISplitter, COM-������ IUnknown ������� �� T
template <typename T>  
class ISplitter {
public:
	// ������������� ������� ��������� - ������� ������������ ���������� ������� � �������, � ������������ ���������� �������
	virtual bool	SplitterInit (IN const int _nMaxBuffers, IN const int _nMaxClients) = 0;

	// ���������� � ��������
	virtual bool	SplitterInfoGet (OUT int* _pnMaxBuffers, OUT int* _pnMaxClients) = 0;

	// ����� ������ � �������
	// ���� ��������� ���������� �������, �� ��� _nTimeOutMsec ���� �� ����������� ������ (������ ������ ������) ����� ���� ���� ���������� ������,
	// ���� ������� ����� ������ ������ � ��������� ����� (� ����������� �� �������� ����� _bForceNew) 
	virtual int 	SplitterPut (IN const T* _pObjPut, IN const bool _bForceNew, IN const int _nTimeOutMsec) = 0;

	// ���������� ��� ������, ��������� ��� ��������
	virtual int		SplitterFlush() = 0;

	// ��������� ������ ������� - ���������� ���������� ������������� �������
	virtual bool	SplitterClientAdd (OUT int* _pnClientID) = 0;

	// ������� ������� �� ��������������, ���� ������ ��������� � �������� �������� ������, �� ��������� ��������
	virtual bool	SplitterClientRemove (IN const int _nClientID) = 0;

	// ���������� ��������
	virtual bool	SplitterClientGetCount (OUT int* _pnCount) = 0;

	// ���������� � ������� �� ������� - ��� ������������� � ���������� ������� (��������)
	virtual bool	SplitterClientGetByIndex (IN  int _nIndex, OUT int* _pnClientID, OUT int* _pnBuffersUsed) = 0;

	// ���������� � ������� �� �������������� - ���������� ������� (��������)
	virtual bool	SplitterClientGetById ( IN int _nClientID, OUT int* _pnBuffersUsed) = 0;

	// �� �������������� ������� ����������� ������, ���� ������ ���� ���, �� ������� _nTimeOutMsec ���� �� ����� ��������� ����� ������, � ������ ���������� ������� �������� - ���������� ������
	virtual int		SplitterGet (IN int _nClientID, OUT T** _ppObjGet, IN int _nTimeOutMsec) = 0;

	// �������� ������� ��������� - ��� �������� ������ ���� ��������
	virtual void	SplitterClose() = 0;

	// ����������
	virtual ~ISplitter() {};
};

#endif ISPLITTER_H
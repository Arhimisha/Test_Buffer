#include "pch.h"
#include <memory>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <future>
#include <thread>
#include <chrono>

#include "isplitter.h"
#include "splitterclientbase.h"

using namespace std::chrono;

#ifndef SPLITTER_H
#define SPLITTER_H

// ������, T - ������ ��� COM-������� IUnknown, ��� T ������ �������� ���������� �������������
template <typename T> // 
class Splitter : public ISplitter<T> {
public:
	
	Splitter();
	Splitter (const Splitter&) = delete;
	Splitter (Splitter&&) = delete;
	Splitter& operator=(const Splitter&) = delete;
	Splitter& operator=(Splitter&&) = delete;
	~Splitter();
	


	// ������������� ������� ��������� - ������� ������������ ���������� ������� � �������, � ������������ ���������� �������
	// ��������� True - �������� ���������������, False - �������� �� ���������������, ��� ��� ��� ��������������� ��� ���� ������������ ������� ������
	bool	SplitterInit(IN const int _nMaxBuffers, IN const int _nMaxClients) override;

	// ���������� � ��������
	// ��������� True - �������� ���������������, False - �������� �� ���������������, ������� ��������� �� ��������
	bool	SplitterInfoGet(OUT int* _pnMaxBuffers, OUT int* _pnMaxClients) override;

	// ����� ������ � �������
	// ���� ��������� ���������� �������, �� ��� _nTimeOutMsec ���� �� ����������� ������ (������ ������ ������) ����� ���� ���� ���������� ������,
	// ���� ������� ����� ������ ������ � ��������� ����� (� ����������� �� �������� ����� _bForceNew) 
	int 	SplitterPut(IN const T* _pObjPut, IN const bool _bForceNew, IN const int _nTimeOutMsec) override;

	// ���������� ��� ������, ��������� ��� ��������
	int		SplitterFlush() override;

	// ��������� ������ ������� - ���������� ���������� ������������� �������
	// ��������� True - ������ ��������, False - �� �������� ��� �������� �� ��������������� 
	bool	SplitterClientAdd(OUT int* _pnClientID) override;

	// ������� ������� �� ��������������, ���� ������ ��������� � �������� �������� ������, �� ��������� ��������
	// ��������� True - ������ ������, False - �� ������ ��� �������� �� ���������������
	bool	SplitterClientRemove(IN const int _nClientID) override;

	// ���������� ��������
	// ��������� True - �������� ���������������, False - �������� �� ���������������, ������� ��������� �� ��������
	bool	SplitterClientGetCount(OUT int* _pnCount) override;

	// ���������� � ������� �� ������� - ��� ������������� � ���������� ������� (��������)
	// ������ - ���������� ����� �� 0 �� �������� ���������� �������� (�� ������������)
	// ��������� True - ������ ������, False -������ �� ������ ��� �������� �� ���������������, ������� ��������� �� ��������
	bool	SplitterClientGetByIndex(IN int _nIndex, OUT int* _pnClientID, OUT int* _pnBuffersUsed) override;

	// ���������� � ������� �� �������������� - ���������� ������� (��������)
	// ��������� True - ������ ������, False -������ �� ������ ��� �������� �� ���������������, ������� ��������� �� ��������
	bool	SplitterClientGetById(IN int _nClientID, OUT int* _pnBuffersUsed) override;

	// �� �������������� ������� ����������� ������, ���� ������ ���� ���, �� ������� _nTimeOutMsec ���� �� ����� ��������� ����� ������, � ������ ���������� ������� �������� - ���������� ������
	int		SplitterGet(IN int _nClientID, OUT T** _ppObjGet, IN int _nTimeOutMsec) override;

	// �������� ������� ��������� - ��� �������� ������ ���� ��������
	void	SplitterClose() override;
	

private:
	//���� ������������� ���������
	std::atomic_bool m_bIsInitialized;

	//���������� ���������� ������� SplitterPut 
	std::atomic<unsigned int> m_uCountOfActive_SplitterPut;

	//���������� ��� ������� SplitterGet ��� �������� ������� ����������� ���������� �������
	std::condition_variable m_cvMessageForSplitterGet;
	std::mutex  m_mutex_cvMessageForSplitterGet;

	//���������� ��� ������� SplitterPut ��� �������� ������� ����������� ���������� �������
	std::condition_variable m_cvMessageForSplitterPut;
	std::mutex  m_mutex_cvMessageForSplitterPut;

	//���� ��������
	SplitterClientsBase m_cClientsBase;
	//������� ��� ������ � ����� ��������
	std::mutex  m_mutex_cClientsBase;

	//������ ������� ������ �� ������, � ������� m_vecObjects, � ������� ���������� ��������� ������ ������ �������
	std::atomic<size_t> m_uCurrentObjectIndex;
	//������������ ������ �������� -������ ������
	std::atomic<size_t> m_uMaxObjectIndex;
	
	//������ ���������� �� �������
	std::vector<std::unique_ptr<T>> m_vecObjects;
	//������� ��� ������ � �������� ������ �� �������
	std::mutex  m_mutex_vecObjects;

	//���� ������������� ���������� SplitterPut
	std::atomic_bool m_bBreakSplitterPut;
	
	//������� ��������� ������� ������� ���������� ����� ���������� (����� ���������� ���������� ������)
	size_t GetNextObjectIndex(IN size_t _nCurrentIndex) const;

	//������� ��������� ���������� ���������� ������� �� �������� ������� ������� - �������� ( � ������ ����������� ������)
	size_t GetCurrentDelay(IN size_t _nCurrentIndex) const;

};


template <typename T>
Splitter<T>::Splitter()
	: m_bIsInitialized(false)
	, m_uCountOfActive_SplitterPut (0)
	, m_cClientsBase()
	, m_uCurrentObjectIndex(0)
	, m_uMaxObjectIndex (0)
	, m_vecObjects (0)
	, m_bBreakSplitterPut(false)
{};

template <typename T>
Splitter<T>::~Splitter()
{};


// ������������� ������� ��������� - ������� ������������ ���������� ������� � �������, � ������������ ���������� �������
// ��������� True - �������� ���������������, False - �������� �� ���������������, ��� ��� ��� ��������������� ��� ���� ������������ ������� ������
template <typename T>
bool	Splitter<T>::SplitterInit(IN const int _nMaxBuffers, IN const int _nMaxClients)
{
	bool bResult (false);
	//�������� ����, ��� �������� ��� �� ���������������
	if (!m_bIsInitialized.load() && _nMaxBuffers > 0 && _nMaxClients > 0 ) {
		m_bIsInitialized.store(true);
		m_uCountOfActive_SplitterPut.store(0);
		m_uCurrentObjectIndex.store(0);	
		//������ ������������ ������ �������� �� 1 ������, ��� ���� ����� ����� 1 �������������� ����������� ������� - �������, ������� ����� �� �������� � ������� ����� ��������� ��������� �����������
		m_uMaxObjectIndex.store(_nMaxBuffers > 0 ? static_cast<unsigned int>(_nMaxBuffers+1) : 0);
		
		size_t uMaxClients = _nMaxClients > 0 ? static_cast<size_t>(_nMaxClients) : 0;

		std::unique_lock <std::mutex> lkClientsBase{ m_mutex_cClientsBase, std::defer_lock };
		std::unique_lock <std::mutex> lkVecObject{ m_mutex_vecObjects, std::defer_lock };
		std::lock(lkClientsBase, lkVecObject);		
		
		m_cClientsBase.SetMaxClients(uMaxClients);
		m_vecObjects.resize(_nMaxBuffers > 0 ? static_cast<size_t>(_nMaxBuffers+1) : 0);

		m_bBreakSplitterPut.store(false);

		bResult = true;
	}
	return bResult;
};



// ���������� � ��������
// ��������� True - �������� ���������������, False - �������� �� ���������������, ������� ��������� �� ��������
template <typename T>
bool	Splitter<T>::SplitterInfoGet(OUT int* _pnMaxBuffers, OUT int* _pnMaxClients)
{
	bool bResult(false);
	//�������� ��������� ���������
	if (m_bIsInitialized.load()) {
		*_pnMaxBuffers = static_cast<int>(m_uMaxObjectIndex.load());
		{
			m_mutex_cClientsBase.lock();
			*_pnMaxClients = static_cast<int>(m_cClientsBase.GetMaxClients());
			m_mutex_cClientsBase.unlock();
		}
		bResult = true;
	}
	return bResult;
};


// ����� ������ � �������
// ���� ��������� ���������� �������, �� ��� _nTimeOutMsec ���� �� ����������� ������ (������ ������ ������) ����� ���� ���� ���������� ������,
// ���� ������� ����� ������ ������ � ��������� ����� (� ����������� �� �������� ����� _bForceNew) 
template <typename T>
int 	Splitter<T>::SplitterPut(IN const T* _pObjPut, IN const bool _bForceNew, IN const int _nTimeOutMsec)
{
	
	int nResult (0);
	if (_pObjPut != nullptr) {
		if (m_bIsInitialized.load()) {
			//������ ������������ ���������� ������� ���� ������� � ��������� � ��������� ���������
			unsigned int uCount = ++m_uCountOfActive_SplitterPut;
			//��������� ���������� �������� ���� �������: ���� �� ����� 1, �� ��������� � ������� - ������� ������,
			//����� ������ ���������, ��� ��� ������ ������� - ����� ���������� ������� ������� SplitterPut ����� �������� � ���������� ������, � ������ 
			//����� ����� ���������� SplitterPut ����� ����������� �� ���������� ����������, � ����� �������� ������ SplitterPut, �� ����� ����������� ����������� ������ ������� ������
			if (1 == uCount) {
				//�������� ������� ������ �������
				size_t uCurrentObjectIndex = m_uCurrentObjectIndex.load();
				size_t uNextCurrentObjectIndex = this->GetNextObjectIndex(uCurrentObjectIndex);
				//���� ������� �������� � �������� ������ ��������� �� �������, �.�. ��, ������� ����� ����������� ��������� ��������
				bool bHasClientsWithCurrentObjectIndex(true);
				//��������� ������� �������� � �������� ������ ��������� �� �������
				{
					m_mutex_cClientsBase.lock();
					bHasClientsWithCurrentObjectIndex = m_cClientsBase.CheckCurrentObjectIndex(uNextCurrentObjectIndex);					
					m_mutex_cClientsBase.unlock();
				}
				//�������� � ��������� _nTimeOutMsec ���� ���� ������� � ������� ��������, �.�. ��, ������� ��� �� ������� ���������� �������� ������
				//�������, ��� ������� ������ ������� ������
				if (bHasClientsWithCurrentObjectIndex) {
					auto timeOut = std::chrono::system_clock::now() + milliseconds{ _nTimeOutMsec };
					//���� ����������� ��������
					std::atomic_bool bHasTimeOut(false);
					//����������� ��������� ��������� ����� ��������
					auto futureTimeOut = std::async(std::launch::async, [&bHasTimeOut, _nTimeOutMsec] {
						std::this_thread::sleep_for(milliseconds{ _nTimeOutMsec });
						bHasTimeOut.store(true);
					});
					//��������� ������ �� ������� ������� ��������� ������ �� ����������� ��������
					//���� ��������� �������, �� ���� bHasClientsWithCurrentObjectIndex �������� True
					//��������� �� ������ ������� m_cvMessageForSplitterPut, ������������ �� ������� SplitterGet
					while (bHasClientsWithCurrentObjectIndex && !bHasTimeOut.load()) {
						std::unique_lock<std::mutex> lkMessage(m_mutex_cvMessageForSplitterPut);
						m_cvMessageForSplitterPut.wait_until(lkMessage, timeOut);
						//��������� ������� ����������� ���������� ������� - ��� ������� ������ ������� ��������� ������
						m_mutex_cClientsBase.lock();
						bHasClientsWithCurrentObjectIndex = m_cClientsBase.CheckCurrentObjectIndex(uNextCurrentObjectIndex);
						m_mutex_cClientsBase.unlock();
					}
				}
				//���� ��������� �� �������� �������, � ����� ��� �������� � ����� ������ �������� ������� ��� ���������� ���� ����������� ������ ������ ������� �������, �� �������� ������
				if (!bHasClientsWithCurrentObjectIndex || _bForceNew) {
					std::unique_ptr<T> upObject(new T);
					*upObject = *_pObjPut;
					//��� �����: ���� ��� �������� �� ���������� �������� � ���� ����
					if (!bHasClientsWithCurrentObjectIndex) {
						m_mutex_vecObjects.lock();
						//������������� ��������� ���� ���������� ���� �������, � ���� �� true, �� �� ��������� ������, � ���� ����������
						if (!m_bBreakSplitterPut.load()) {
							//���������� ������ � �����
							m_vecObjects.at(uCurrentObjectIndex) = std::move(upObject);
							m_uCurrentObjectIndex.store(uNextCurrentObjectIndex);
						}
						else
						{
							m_bBreakSplitterPut.store(false);
							//������������� ������ - ������� �������� � ����� � �������� ������
							nResult = 5;
						}
						m_mutex_vecObjects.unlock();
					}
					else {
						//������ m_vecObjects � m_uCurrentObjectIndex ��� ����������� ������ � ���������
						std::unique_lock <std::mutex> lkClientsBase{ m_mutex_cClientsBase, std::defer_lock };
						std::unique_lock <std::mutex> lkVecObject{ m_mutex_vecObjects, std::defer_lock };
						std::lock(lkClientsBase, lkVecObject);
						//������������� ��������� ���� ���������� ���� �������, � ���� �� true, �� �� ��������� ������, � ���� ����������
						if (!m_bBreakSplitterPut.load()) {
							//���������� ������ � �����
							m_vecObjects.at(uCurrentObjectIndex) = std::move(upObject);
							m_uCurrentObjectIndex.store(uNextCurrentObjectIndex);
							//������� ������� ���� ��������, ������� �� ������ ������� ������
							m_cClientsBase.ChangeCurrentObjectIndex(uNextCurrentObjectIndex, this->GetNextObjectIndex(uNextCurrentObjectIndex));
						}
						else
						{
							m_bBreakSplitterPut.store(false);
							//������������� ������ - ������� �������� � ����� � �������� ������
							nResult = 5;
						}
					}
					//���������� ��������� � ���������� ������ ������� SplitterPut � ������� SplitterGet, ��������� ������ �������.
					m_cvMessageForSplitterGet.notify_all();
				}
				else {
					//������������� ������ - �������� ������� ���������� �������
					nResult = 3;
				}
			}
			else {
				//������������� ������ - ������� �������� ����� ������ ����
				nResult = 2;
			}
			//��������� ���������� �������� ���� �������
			m_uCountOfActive_SplitterPut--;
		}
		else {
			//������������� ������ - �������� �� ���������������
			nResult = 1;
		}
	}
	else {
		//������������� ������ - ��������� _pObjPut �� ���������������
		nResult = 4;
	}
	return nResult;
};


// ���������� ��� ������, ��������� ��� ��������
template <typename T>
int		Splitter<T>::SplitterFlush()
{
	int nResult(0);
	if (m_bIsInitialized.load()) {
		//������ m_vecObjects � m_uCurrentObjectIndex ��� ����������� ������ � ���������
		std::unique_lock <std::mutex> lkClientsBase{ m_mutex_cClientsBase, std::defer_lock };
		std::unique_lock <std::mutex> lkVecObject{ m_mutex_vecObjects, std::defer_lock };
		std::lock(lkClientsBase, lkVecObject);
		//m_bIsInitialized.store(false);
		m_uCurrentObjectIndex.store(0);
		//m_uMaxObjectIndex.store(0);
		size_t uMaxClients = m_cClientsBase.GetMaxClients();
		for (size_t i(0); i < uMaxClients; i++) {
			m_cClientsBase.SetClientCurrentIndex(i,0);
		}
		for (size_t i(0); i < m_vecObjects.size(); i++) {
			m_vecObjects.at(i) = nullptr;
		}
		m_bBreakSplitterPut.store(true);
	}
	else {
		//������������� ������ - �������� �� ���������������
		nResult = 1;
	}
	//������� SplitterGet ����������� ����� �������� ���������, ��� ��� � ���� �������� ������� ������ ����� ����� ������� ������ 
	m_cvMessageForSplitterGet.notify_all();
	// ������� SplitterPut ����������� ��-�� 
	m_cvMessageForSplitterPut.notify_all();
	return nResult;
};


// ��������� ������ ������� - ���������� ���������� ������������� �������
// ��������� True - ������ ��������, False - �� �������� ��� �������� �� ���������������
template <typename T>
bool	Splitter<T>::SplitterClientAdd(OUT int* _pnClientID)
{
	bool bResult(false);
	size_t uClientID(0);
	//�������� ��������� ���������	
	if (m_bIsInitialized.load()) {
		
		size_t uCurrentObjectIndex = static_cast<size_t>(m_uCurrentObjectIndex.load());		
		m_mutex_cClientsBase.lock();
		bResult = m_cClientsBase.AddClient(&uClientID, uCurrentObjectIndex); 
		m_mutex_cClientsBase.unlock();
	}
	if (bResult) {
		*_pnClientID = uClientID;
	}
	return bResult;
};


// ������� ������� �� ��������������, ���� ������ ��������� � �������� �������� ������, �� ��������� ��������
// ��������� True - ������ ������, False - �� ������  ��� �������� �� ���������������
template <typename T>
bool	Splitter<T>::SplitterClientRemove(IN const int _nClientID)
{
	bool bResult(false);
	//�������� ��������� ���������
	if (m_bIsInitialized.load()){
		if (_nClientID >= 0){
			size_t uClientID =  static_cast<size_t>(_nClientID);
			m_mutex_cClientsBase.lock();
			bResult = m_cClientsBase.RemoveClient(uClientID);
			m_mutex_cClientsBase.unlock();
			if (bResult){
				//������� ���������� ��� �������� SplitterGet
				//���� ��� ������� ������� ���� ����� ������� � ������ �������� ������ �������, �� ��� ������ ��� ������ �������� � �������� ���� ������
				m_cvMessageForSplitterGet.notify_all();
			}
		}
	}
	return bResult;
};


// ���������� ��������
// ��������� True - �������� ���������������, False - �������� �� ���������������, ������� ��������� �� ��������
template <typename T>
bool	Splitter<T>::SplitterClientGetCount(OUT int* _pnCount)
{
	bool bResult(false);
	//�������� ��������� ���������
	if (m_bIsInitialized.load()) {
		m_mutex_cClientsBase.lock();
		*_pnCount = m_cClientsBase.GetCurrentClients();
		m_mutex_cClientsBase.unlock();
		bResult = true;
	}
	return bResult;
};


// ���������� � ������� �� ������� - ��� ������������� � ���������� ������� (��������)
// ������ - ���������� ����� �� 0 �� �������� ���������� �������� (�� ������������)
// ��������� True - ������ ������, False -������ �� ������ ��� �������� �� ���������������, ������� ��������� �� ��������
template <typename T>
bool	Splitter<T>::SplitterClientGetByIndex(IN int _nIndex, OUT int* _pnClientID, OUT int* _pnBuffersUsed)
{
	bool bResult(false);
	//�������� ��������� ���������
	if (m_bIsInitialized.load()) {
		size_t uClientID(0);
		size_t uCurrentObjectIndex(0);
		int nCurrentIndex(0);
		m_mutex_cClientsBase.lock();
		size_t uMaxClients = m_cClientsBase.GetMaxClients();
		//������� ���� �������� �������� ���� �� ��������� �� _nIndex ��������� �������
		while (uClientID < uMaxClients && nCurrentIndex <= _nIndex) {
			// �������� ������� ���������� � ������� � ��������� ������
			if (m_cClientsBase.GetClientCurrentIndex(uClientID, &uCurrentObjectIndex)) {
				//���� ���������� ���� - ����������� ������� �������� ��������
				nCurrentIndex++;
			}
			// ���� nCurrentIndex ���� ��������� _nIndex, �� ������ ����������� ������, � �� ����� ����������� uClientID
			if (nCurrentIndex <= _nIndex) {
				uClientID++;
			}
		}
		m_mutex_cClientsBase.unlock();
		//���� nCurrentIndex == _nIndex+1 - ������ ������ ������
		if (nCurrentIndex == _nIndex + 1) {
			*_pnClientID = static_cast<int>(uClientID);
			*_pnBuffersUsed = static_cast<int>(GetCurrentDelay(uCurrentObjectIndex));
			bResult = true;
		}
	}
	return bResult;
};


// ���������� � ������� �� �������������� - ���������� ������� (��������)
// ��������� True - ������ ������, False -������ �� ������ ��� �������� �� ���������������, ������� ��������� �� ��������
template <typename T>
bool	Splitter<T>::SplitterClientGetById(IN int _nClientID, OUT int* _pnBuffersUsed)
{
	bool bResult(false);
	//�������� ��������� ���������
	if (m_bIsInitialized.load()) {
		if (_nClientID >= 0) {
			size_t uClientID = static_cast<size_t>(_nClientID);
			size_t uCurrentObjectIndex(0);
			m_mutex_cClientsBase.lock();
			bResult = m_cClientsBase.GetClientCurrentIndex(uClientID, &uCurrentObjectIndex);
			m_mutex_cClientsBase.unlock();
			if (bResult) {
				*_pnBuffersUsed = static_cast<int>(GetCurrentDelay(uCurrentObjectIndex));
			}
		}
	}
	return bResult;
};


// �� �������������� ������� ����������� ������, ���� ������ ���� ���, �� ������� _nTimeOutMsec ���� �� ����� ��������� ����� ������, � ������ ���������� ������� �������� - ���������� ������
template <typename T>
int		Splitter<T>::SplitterGet(IN int _nClientID, OUT T** _ppObjGet, IN int _nTimeOutMsec)
{
	//��� �������� ���������� ���������� SplitterGet ��� ������ �������, ������� ��� ��� ������� ��� ������ ������� ������� �� ������ ������
	
	int nResult(0);
	if (_ppObjGet != nullptr && *_ppObjGet != nullptr){
		if (m_bIsInitialized.load()) {
			//�������� ������� ������ ������� � ���� ��� ����������
			size_t uClientCurrentObjectIndex(0);
			bool bIsActiveClient(false);
			m_mutex_cClientsBase.lock();
			bIsActiveClient = m_cClientsBase.GetClientCurrentIndex(_nClientID, &uClientCurrentObjectIndex);
			m_mutex_cClientsBase.unlock();
			//��������� ���������� �������, ���� �� �� ������� �� ������������� ������ � ��������� �������
			if (bIsActiveClient) {
				//��������� ������� ������ ������� �� ��������� � ������� �������� ������
				//���� ������� ������ ������� ����� �������� ������� ������ � ���������� ��������� ����� ������
				if (uClientCurrentObjectIndex == m_uCurrentObjectIndex.load()) {
					auto timeOut = std::chrono::system_clock::now() + milliseconds{ _nTimeOutMsec };
					//���� ����������� ��������
					std::atomic_bool bHasTimeOut(false);
					//����������� ��������� ��������� ����� ��������
					auto futureTimeOut = std::async(std::launch::async, [&bHasTimeOut, _nTimeOutMsec] {
						std::this_thread::sleep_for(milliseconds{ _nTimeOutMsec });
						bHasTimeOut.store(true);
					});
					//��������� �������� �� ����� ������ �� ����������� ��������
					//��������� �� ������ ������� m_cvMessageForSplitterGet, �������
					//�������� �� ������� SplitterPut, ��� ���������� � ��������� ������ �������,
					//�������� �� ������� SplitterClientRemove, ��� ���������� ��� ���� �� �������� ������
					//�������� �� ������� SplitterFlush, ��� ���������� ��� ���������� �������� �������� � ���� ��������
					//�������� �� ������� SplitterClose, ��� ���������� ��� ���������� �������� �������� � ���� ��������
					while (!bHasTimeOut.load() && bIsActiveClient && uClientCurrentObjectIndex != m_uCurrentObjectIndex.load()) {
						std::unique_lock<std::mutex> lkMessage(m_mutex_cvMessageForSplitterGet);
						m_cvMessageForSplitterGet.wait_until(lkMessage, timeOut);
						m_mutex_cClientsBase.lock();
						//��������� ���� ���������� ������� � ��� ������� ������
						bIsActiveClient = m_cClientsBase.GetClientCurrentIndex(_nClientID, &uClientCurrentObjectIndex);
						m_mutex_cClientsBase.unlock();
					}
				}
				//����������� ������ ������ � m_vecObjects � m_uCurrentObjectIndex
				{
					//������ m_vecObjects � m_uCurrentObjectIndex ��� ����������� ������ � ���������
					std::unique_lock <std::mutex> lkClientsBase{ m_mutex_cClientsBase, std::defer_lock };
					std::unique_lock <std::mutex> lkVecObject{ m_mutex_vecObjects, std::defer_lock };
					std::lock(lkClientsBase, lkVecObject);
					//��������� ���� ���������� ������� � ��� ������� ������
					bIsActiveClient = m_cClientsBase.GetClientCurrentIndex(_nClientID, &uClientCurrentObjectIndex);
					//��������� �������� ���������� �������, ��� ������ ���� �� ��� ���������� � ������� ������� �������� ������ �������
					if (bIsActiveClient) {
						//��������� ��� ������� ������ ������� �� ����� �������� ������� ������
						//���� �����, �� ������ �� ��������� ����� �������� _nTimeOutMsec , ����� ������ �� ��������
						if (uClientCurrentObjectIndex != m_uCurrentObjectIndex.load()) {
							//�������� ������
							**_ppObjGet = *(m_vecObjects.at(uClientCurrentObjectIndex));
							//J�������� ������� ������ ��� �������
							m_cClientsBase.SetClientCurrentIndex(_nClientID, this->GetNextObjectIndex(uClientCurrentObjectIndex));
							//��������� ���������� � SplitterPut, � ��� ��� ������� ������, ��� ���� ����� SplitterGet �������� ������������ ��� ������ ������� ������� �� ������� ��������� ������
							m_cvMessageForSplitterPut.notify_one();
						}
						else {
							//������������� ������ - ��������� ����� �������� ����� ������
							nResult = 3;
						}
					}
					else {
						//������������� ������ - ������� � ��������� Id �� ������� 
						nResult = 2;
					}
				}
			}
			else {
				//������������� ������ - ������� � ��������� Id �� ������� 
				nResult = 2;
			}
		}
		else {
			//������������� ������ - �������� �� ���������������
			nResult = 1;
		}
	}
	else {
		//������������� ������ - ��������� �� ��������� _ppObjGet �� ���������������
		nResult = 4;
	}
	return nResult;
};



// �������� ������� ��������� - ��� �������� ������ ���� ��������
template <typename T>
void	Splitter<T>::SplitterClose()
{
	if (m_bIsInitialized.load()) {
		//������ m_vecObjects � m_uCurrentObjectIndex ��� ����������� ������ � ���������
		std::unique_lock <std::mutex> lkClientsBase{ m_mutex_cClientsBase, std::defer_lock };
		std::unique_lock <std::mutex> lkVecObject{ m_mutex_vecObjects, std::defer_lock };
		std::lock(lkClientsBase, lkVecObject);
		m_bIsInitialized.store(false);
		m_uCurrentObjectIndex.store(0);
		m_uMaxObjectIndex.store(0);
		size_t uMaxClients = m_cClientsBase.GetMaxClients();
		for (size_t i(0); i < uMaxClients; i++) {
			m_cClientsBase.SetClientCurrentIndex(i, 0);
		}
		for (size_t i(0); i < m_vecObjects.size(); i++) {
			m_vecObjects.at(i) = nullptr;
		}
		m_bBreakSplitterPut.store(true);
	}
	//������� SplitterGet ����������� ����� �������� ���������, ��� ��� � ���� �������� ������� ������ ����� ����� ������� ������ 
	m_cvMessageForSplitterGet.notify_all();
	// ������� SplitterPut ����������� ��-�� 
	m_cvMessageForSplitterPut.notify_all();
};


//������� ��������� ������� ������� ���������� ����� ���������� (����� ���������� ���������� ������)
template <typename T>
size_t Splitter<T>::GetNextObjectIndex(IN size_t _nCurrentIndex) const
{
	size_t nResult(0);
	//�������� ��������� ���������
	if (m_bIsInitialized.load()) {
		nResult = (++_nCurrentIndex) % m_uMaxObjectIndex.load();
	}
	return nResult;
	
}


//������� ��������� ���������� ���������� ������� �� �������� ������� ������� - �������� ( � ������ ����������� ������)
template <typename T>
size_t Splitter<T>::GetCurrentDelay(IN size_t _nCurrentIndex) const 
{
	size_t nResult(0);
	//�������� ��������� ���������
	if (m_bIsInitialized.load()) {
		const size_t uMaxObjectIndex = static_cast<size_t> (m_uMaxObjectIndex.load());
		if (0 < uMaxObjectIndex) {
			nResult = (uMaxObjectIndex + static_cast<size_t>(m_uCurrentObjectIndex.load()) - _nCurrentIndex) % uMaxObjectIndex;
		}		
	}
	return nResult;
};
#endif SPLITTER_H
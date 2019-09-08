#include "pch.h"

#include <iostream>
#include <locale>
#include <vector>
#include <string>
#include <functional>
#include <random>
#include <algorithm>


#include "splitter.h"
#include "test.h"


// �������� �� ��������� ���������� � ������� ���������� � �������
bool EqualClientInfo( IN const SplitterClientInfo& _cClientInfo, IN  size_t _uClientId, IN  size_t _uCurrentObjectIndex, IN  bool _bIsActive) {
	bool bResult = false;

	if (_cClientInfo.GetClientId() == _uClientId
		&& _cClientInfo.GetCurrentObjectIndex() == _uCurrentObjectIndex
		&& _cClientInfo.GetActivity() == _bIsActive)
	{
		bResult = true;
	}
	return bResult;
};



//������ ������������ �����
void PrintFailResult( IN const std::string _strHeader, IN  bool _bRightResult) {
	std::cout << _strHeader;
	if (!_bRightResult) {
		std::cout << ": Fail\n";
	}
	else {
		std::cout  << ": Test Done!\n";
	}
};





//������������ �������� ����������� SplitterClientsBase � ������������ GetMaxClients() � GetCurrentClients()
bool CorrectCreate_SplitterClientsBase(IN const size_t _uMaxClients)
{
	SplitterClientsBase cClientBase(_uMaxClients);
	return cClientBase.GetMaxClients() == _uMaxClients && cClientBase.GetCurrentClients() == 0;
}


//������������ ������� bool SetMaxClients(IN const size_t _uMaxClients);
bool CorrectWork_SplitterClientsBase_SetMaxClients(IN const size_t _uMaxClients)
{
	//�������� ���������� ��������� ������ ������������� ���������� ��������
	bool bCorrectSet(false);	
	SplitterClientsBase cClientBase;
	bCorrectSet = cClientBase.SetMaxClients(_uMaxClients) && cClientBase.GetMaxClients() == _uMaxClients && cClientBase.GetCurrentClients() == 0;
	
	//������� ��������
	bool bCorrectAdd (true);
	for (size_t i = 0; i < _uMaxClients && bCorrectAdd; i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(i);
		bCorrectAdd = cClientBase.AddClient(&nClientId, nCurrentIndexObject) && nClientId == i && cClientBase.GetCurrentClients() == i+1;
	};
	
	//�������� �������� ����������� ���������� �������� ������ ��� ������� ���������� ��������
	bool bCorrectFailSet(true);
	if (_uMaxClients > 0) {
		bCorrectFailSet = !cClientBase.SetMaxClients(_uMaxClients - 1);
	};


	return bCorrectSet && bCorrectAdd && bCorrectFailSet;
};


//������������ �������� ���� ������� bool SplitterClientsBase::GetClientCurrentIndex(IN const size_t _uClientID, OUT size_t *const _uCurrentObjectIndex)
bool CorrectWork_SplitterClientsBase_GetClientCurrentIndex(IN const size_t _uClientID)
{
	size_t nCurrentIndexObject(0);
	SplitterClientsBase cClientBase;
	return !cClientBase.GetClientCurrentIndex(_uClientID, &nCurrentIndexObject);
	
};

//������������ ������� bool SplitterClientsBase::AddClient(OUT size_t *const _uClientID, IN  size_t _uCurrentObjectIndex)
bool CorrectWork_SplitterClientsBase_AddClient(IN const size_t _uMaxClients)
{
	//������� ��������
	bool bCorrectAdd(true);
	SplitterClientsBase cClientBase(_uMaxClients);
	for (size_t i = 0; i < _uMaxClients && bCorrectAdd;i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(i);
		bCorrectAdd = cClientBase.AddClient(&nClientId, nCurrentIndexObject) && nClientId == i && cClientBase.GetCurrentClients() == i + 1;
	};
	//��������� ������������ ��������� ���� �� �������
	bool bCorrectGetInfo(true);
	for (size_t i = 0; i < _uMaxClients && bCorrectGetInfo; i++) {
		size_t nCurrentIndexObject(0);
		bCorrectGetInfo = cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject) && i == nCurrentIndexObject;
	};
	//��������� ���������� ���� �� �������� �� ������������ ��������
	bool bCorrectGetFailInfo(true);
	size_t nLimit(_uMaxClients + 10);
	for (size_t i = _uMaxClients; i < nLimit; i++) {
		size_t nCurrentIndexObject(0);
		bCorrectGetFailInfo = !cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject) && 0 == nCurrentIndexObject;
	}
	return bCorrectAdd && bCorrectGetInfo && bCorrectGetFailInfo;
}


//������������ ������� bool SplitterClientsBase::RemoveClient(OUT size_t *const _uClientID, IN  size_t _uCurrentObjectIndex)
bool CorrectWork_SplitterClientsBase_RemoveClient(IN const size_t _uMaxClients)
{
	//������� ��������
	bool bCorrectAdd(true);
	SplitterClientsBase cClientBase(_uMaxClients);
	for (size_t i = 0; i < _uMaxClients && bCorrectAdd; i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(i);
		bCorrectAdd = cClientBase.AddClient(&nClientId, nCurrentIndexObject) && cClientBase.GetCurrentClients() == i + 1;
	};
	
	//��������� ��������� �������� ��������
	bool bCorrectRemove(true);
	int nMaxClients = static_cast<int> (_uMaxClients);
	auto getRandomIndex = std::bind(std::uniform_int_distribution<>{0, nMaxClients >= 1 ? nMaxClients - 1 : 0}, std::default_random_engine{});
	for (size_t i = 0; i < _uMaxClients && bCorrectRemove; i++) {
		size_t nIndexToDelete = getRandomIndex();
		cClientBase.RemoveClient(nIndexToDelete);
		size_t nCurrentIndexObject(0);
		bCorrectRemove = !cClientBase.GetClientCurrentIndex(nIndexToDelete, &nCurrentIndexObject) && 0 == nCurrentIndexObject ;
	}

	//��������������� ��������
	for (size_t i = 0; i < _uMaxClients; i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(i);
		cClientBase.AddClient(&nClientId, nCurrentIndexObject);
	};
	bool bCorrectActivate = cClientBase.GetCurrentClients() == _uMaxClients;

	//�������� ���������������� �������� � ���������� �������� ��������
	bool bCorrectSecondRemove(true);
	for (size_t i = 0; i < _uMaxClients && bCorrectSecondRemove; i++) {
		cClientBase.RemoveClient(i);
		size_t nCurrentIndexObject(0);
		bCorrectSecondRemove = !cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject) && 0 == nCurrentIndexObject && cClientBase.GetCurrentClients() == _uMaxClients - i - 1;
	}

	return bCorrectAdd && bCorrectRemove && bCorrectActivate && bCorrectSecondRemove;
};



//������������ ������� bool SplitterClientsBase::SetClientCurrentIndex(IN const size_t _uClientID, IN const size_t _uCurrentObjectIndex) 
bool CorrectWork_SplitterClientsBase_SetClientCurrentIndex(IN const size_t _uMaxClients)
{
	//������� ��������
	bool bCorrectAdd(true);
	SplitterClientsBase cClientBase(_uMaxClients);
	for (size_t i = 0; i < _uMaxClients && bCorrectAdd; i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(i);
		bCorrectAdd = cClientBase.AddClient(&nClientId, nCurrentIndexObject);
	};

	//��������� ��������� ������ ���������� CurrentIndexObject ��� ���� ��������
	bool bCorrectSetCurrentIndex(true);
	int nMaxClients = static_cast<int> (_uMaxClients);
	auto getRandomIndex = std::bind(std::uniform_int_distribution<>{0, nMaxClients >= 1 ? nMaxClients - 1 : 0}, std::default_random_engine{});
	for (size_t i = 0; i < _uMaxClients && bCorrectSetCurrentIndex; i++) {
		const size_t nNewCurrentIndex = getRandomIndex();
		size_t nCurrentIndexObject(0);
		bCorrectSetCurrentIndex = cClientBase.SetClientCurrentIndex(i, nNewCurrentIndex) 
								  && cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject) 
								  && nNewCurrentIndex == nCurrentIndexObject;
	}

	//��������� ��������� ������ CurrentIndexObject ��� �������� �� ������������ ��������
	bool bCorrectSetCurrentIndexForNotExistClient(true);
	size_t nLimit(_uMaxClients + 10);
	for (size_t i = _uMaxClients; i < nLimit && bCorrectSetCurrentIndexForNotExistClient; i++) {
		const size_t nNewCurrentIndex = getRandomIndex();
		size_t nCurrentIndexObject(-1);
		bCorrectSetCurrentIndexForNotExistClient = !cClientBase.SetClientCurrentIndex(i, nNewCurrentIndex)
												   && !cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject)
												   && nNewCurrentIndex != nCurrentIndexObject;
	}

	//������������� ���� ��������
	bool bCorrectRemove(true);
	for (size_t i = 0; i < _uMaxClients && bCorrectRemove; i++) {
		cClientBase.RemoveClient(i);
		size_t nCurrentIndexObject(0);
		bCorrectRemove = !cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject) && 0 == nCurrentIndexObject;
	}

	//��������� ��������� ������ CurrentIndexObject ��� ����������������� ��������
	bool bCorrectSetCurrentIndexForNonActiveClient(true);
	for (size_t i = 0; i < _uMaxClients && bCorrectSetCurrentIndexForNonActiveClient; i++) {
		const size_t nNewCurrentIndex = getRandomIndex();
		size_t nCurrentIndexObject(-1);
		bCorrectSetCurrentIndexForNonActiveClient = !cClientBase.SetClientCurrentIndex(i, nNewCurrentIndex)
													&& !cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject)
													&& nNewCurrentIndex != nCurrentIndexObject;
	}
	return bCorrectAdd && bCorrectSetCurrentIndex && bCorrectSetCurrentIndexForNotExistClient && bCorrectRemove && bCorrectSetCurrentIndexForNonActiveClient;
};



//������������ ������� bool SplitterClientsBase::CheckCurrentObjectIndex(IN const size_t _uCurrentIndex);
bool CorrectWork_SplitterClientsBase_CheckCurrentObjectIndex(IN const size_t _uMaxClients)
{
	//������� ��������
	bool bCorrectAdd(true);
	SplitterClientsBase cClientBase(_uMaxClients);
	int nMaxClients = static_cast<int> (_uMaxClients);
	auto getRandomIndex = std::bind(std::uniform_int_distribution<>{0, nMaxClients >= 1 ? nMaxClients - 1 : 0}, std::default_random_engine{});
	for (size_t i = 0; i < _uMaxClients && bCorrectAdd; i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(getRandomIndex());
		bCorrectAdd = cClientBase.AddClient(&nClientId, nCurrentIndexObject);
	}
	//��������� ����������� ������� ��������
	bool bCorrectCheckCurrentIndex(true);
	for (size_t i = 0; i < _uMaxClients && bCorrectCheckCurrentIndex; i++) {
		const size_t nCurrentIndex(getRandomIndex());
		//��������� ������ �� ������� ��������������� ������� � cClientBase
		bool bHasChanges(false);
		for (size_t j = 0; j < _uMaxClients && !bHasChanges; j++) {
			size_t nClientCurrentIndex(0);
			cClientBase.GetClientCurrentIndex(j, &nClientCurrentIndex);
			bHasChanges = nCurrentIndex == nClientCurrentIndex;
		}
		//��������� ���������� ������� ������� ChangeCurrentObjectIndex
		bCorrectCheckCurrentIndex = bHasChanges == cClientBase.CheckCurrentObjectIndex(nCurrentIndex);
	}
	return bCorrectAdd && bCorrectCheckCurrentIndex;
};



//������������ ������� bool SplitterClientsBase::ChangeCurrentObjectIndex(IN const size_t _uCurrentIndex, IN const size_t _uNextCurrentIndex);
bool CorrectWork_SplitterClientsBase_ChangeCurrentObjectIndex(IN const size_t _uMaxClients)
{
	//������� ��������
	bool bCorrectAdd(true);
	SplitterClientsBase cClientBase(_uMaxClients);
	for (size_t i = 0; i < _uMaxClients && bCorrectAdd; i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(i);
		bCorrectAdd = cClientBase.AddClient(&nClientId, nCurrentIndexObject);
	}
	//��������� ������ ������� ��������
	bool bCorrectChangeCurrentIndex(true);
	int nMaxClients = static_cast<int> (_uMaxClients);
	auto getRandomIndex = std::bind(std::uniform_int_distribution<>{0, nMaxClients >= 1 ? nMaxClients - 1 : 0}, std::default_random_engine{});
	for (size_t i = 0; i < _uMaxClients && bCorrectChangeCurrentIndex; i++) {
		const size_t nNewCurrentIndex = getRandomIndex();
		const size_t nOldCurrentIndex = nNewCurrentIndex + 1;
		//��������� ������ �� ��������� ��������� � cClientBase
		bool bHasChanges (false);
		for (size_t j = 0; j < _uMaxClients && !bHasChanges; j++) {
			size_t nCurrentIndexObject(0);
			cClientBase.GetClientCurrentIndex(j, &nCurrentIndexObject);
			bHasChanges = nOldCurrentIndex == nCurrentIndexObject;
		}		
		//��������� ���������� ������� ������� ChangeCurrentObjectIndex
		bCorrectChangeCurrentIndex = bHasChanges == cClientBase.ChangeCurrentObjectIndex( nOldCurrentIndex, nNewCurrentIndex);
		//��������� ������������� ������
		for (size_t k = 0; k < _uMaxClients && bCorrectChangeCurrentIndex; k++) {
			size_t nCurrentIndexObject(0);
			bCorrectChangeCurrentIndex = cClientBase.GetClientCurrentIndex(k, &nCurrentIndexObject)
										 && nCurrentIndexObject != nOldCurrentIndex;
		}
	}

	//������������� ���� ��������
	bool bCorrectRemove(true);
	for (size_t i = 0; i < _uMaxClients && bCorrectRemove; i++) {
		cClientBase.RemoveClient(i);
		size_t nCurrentIndexObject(0);
		bCorrectRemove = !cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject) && 0 == nCurrentIndexObject;
	}

	//��������� ������ ������� �������� ��� ����������������� ��������
	bool bCorrectChangeCurrentIndexForNonActiveClient(true);
	for (size_t i = 0; i < _uMaxClients && bCorrectChangeCurrentIndexForNonActiveClient; i++) {
		const size_t nNewCurrentIndex = getRandomIndex();
		const size_t nOldCurrentIndex = nNewCurrentIndex + 1;
		//��������� ������ �� ��������� ��������� � cClientBase
		bool bHasChanges (false);
		for (size_t j = 0; j < _uMaxClients && !bHasChanges; j++) {
			size_t nCurrentIndexObject(-1);
			cClientBase.GetClientCurrentIndex(j, &nCurrentIndexObject);
			bHasChanges = nOldCurrentIndex == nCurrentIndexObject;
		}
		//��������� ���������� ������� ������� ChangeCurrentObjectIndex - ���������� ������������� ���������
		bCorrectChangeCurrentIndexForNonActiveClient = bHasChanges == cClientBase.ChangeCurrentObjectIndex(nOldCurrentIndex, nNewCurrentIndex);
	}
	return bCorrectAdd && bCorrectChangeCurrentIndex && bCorrectRemove && bCorrectChangeCurrentIndexForNonActiveClient;
}




//������������ �������� ���������� ������ Splitter
bool CorrectCreate_Splitter(IN const int _nMaxBuffers, IN const int _nMaxClients) 
{
	
	bool bCorrectCreate(false);

	Splitter<std::string> cSplitter;
	// ��������� ������������� �� ������������ _nMaxBuffers � _nMaxClients - ��� ������ ���� ������ ����
	bool bIsInitialization(_nMaxBuffers > 0 && _nMaxClients > 0);
	bool bCorrectInitialization = (bIsInitialization == cSplitter.SplitterInit(_nMaxBuffers, _nMaxClients));
	
	int const nUnsignedMaxBuffers(_nMaxBuffers > 0 ? _nMaxBuffers+1 : 0);
	int const nUnsignedMaxClients(_nMaxClients > 0 ? _nMaxClients : 0);	
	
	int nMaxBuffers(0), nMaxClients(0);
	bool bCorrectInfoGet = (bIsInitialization == cSplitter.SplitterInfoGet(&nMaxBuffers, &nMaxClients));
	bCorrectCreate = bIsInitialization ? (nUnsignedMaxBuffers == nMaxBuffers && nUnsignedMaxClients == nMaxClients) : true;
	bool bCorrectReInitialization = bIsInitialization ? !cSplitter.SplitterInit(1, 1) : cSplitter.SplitterInit(1, 1);
	return bCorrectCreate && bCorrectInitialization && bCorrectInfoGet && bCorrectReInitialization;
};

//������������ ���������� ����� ��������, �� �������� � ��������� � ��� ���������� � �� ������ ���������� � ������ Splitter
bool CorrectWork_Splitter_WorkWithClients(IN const int _nMaxBuffers, IN const int _nMaxClients) 
{
	Splitter<std::string> cSplitter;
	cSplitter.SplitterInit(_nMaxBuffers, _nMaxClients);
	
	//��������� �������� ���������
	bool bCorreceAddClient(true);
	std::vector<std::pair<int, bool>> vecClients(0);
	for (int i = 0; i < _nMaxClients && bCorreceAddClient; i++) {
		int nClientId(0);
		bCorreceAddClient = cSplitter.SplitterClientAdd(&nClientId);
		vecClients.push_back(std::pair<int, bool>(nClientId, bCorreceAddClient));
	}

	//������ �������� ���������� ��������
	auto CheckClientsCount = [&]() {
		//��������� ���������� �������� ��������
		int nCountActiveClientsInVector = std::count_if(vecClients.begin(), vecClients.end(), [](std::pair<int, bool> _p) {return _p.second; });
		int nCountActiveClientsInSplitter(0);
		cSplitter.SplitterClientGetCount(&nCountActiveClientsInSplitter);
		return (nCountActiveClientsInSplitter == nCountActiveClientsInVector);
	};
	//������ �������� SplitterClientGetById 
	auto Check_SplitterClientGetById = [&]() {
		bool bResult(true);
		for (size_t i(0); i < vecClients.size() && bResult; i++) {
			int nBufferUsed(-1);
			bool bResultClientGetById = cSplitter.SplitterClientGetById(static_cast<int>(i), &nBufferUsed);
			bool bResultGettingInfo = (nBufferUsed == (bResultClientGetById ? 0 : -1));
			bResult = bResultGettingInfo && bResultClientGetById == vecClients.at(i).second;
		}
		return bResult;
	};
	//������ �������� SplitterClientGetByIndex
	auto Check_SplitterClientGetByIndex = [&]() {
		bool bResult(true);
		int nCountActiveClientsInSplitter(0);
		bool bSplitterInitialized = cSplitter.SplitterClientGetCount(&nCountActiveClientsInSplitter);
		for (int i(0); i < nCountActiveClientsInSplitter && bResult && bSplitterInitialized; i++) {
			int nClientId(-1);
			int nBufferUsed(-1);
			bool bResultClientGetByIndex = cSplitter.SplitterClientGetByIndex(i,&nClientId, &nBufferUsed);
			bool bResultGettingInfo(nBufferUsed == 0 && nClientId >= 0 && vecClients.at(static_cast<size_t>(nClientId)).second);
			bResult = bResultClientGetByIndex && bResultGettingInfo;
		}
		return bResult;
	};

	//�������� ��������� �������� � ������� �����
	bool bCorrectClientsCount(CheckClientsCount() && Check_SplitterClientGetById() && Check_SplitterClientGetByIndex());	
	
	auto getRandomId = std::bind(std::uniform_int_distribution<>{0, _nMaxClients >= 1 ? _nMaxClients - 1 : 0}, std::default_random_engine{});
	//������� ��������� ��������
	for (int i(0); i < _nMaxClients; i+=2) {
		int nDeleteId(getRandomId());
		cSplitter.SplitterClientRemove(nDeleteId);
		vecClients.at(nDeleteId).second = false;
	}
	//�������� �������� � ������� ����� ����� ���������� ��������
	bool bCorrectRemoveClient(CheckClientsCount() && Check_SplitterClientGetById() && Check_SplitterClientGetByIndex());
		
	//������� ���� ��������
	for (int i(0); i < _nMaxClients; i++) {
		cSplitter.SplitterClientRemove(i);
		vecClients.at(static_cast<size_t> (i)).second = false;
	}
	//�������� �������� � ������� �����, ����� ������� ��������
	bool bCorrectRemoveAllClient(CheckClientsCount() && Check_SplitterClientGetById() && Check_SplitterClientGetByIndex());


	return bCorreceAddClient && bCorrectClientsCount && bCorrectRemoveClient && bCorrectRemoveAllClient;
}



//������������ ������� SplitterPut � SplitterGet
//_nWorkTimeMsec - ����� ������������ �������� ��� ���-������, _nDelayBetweenObjectsTimeMsec - �������� ����������������� ����� ������
bool CorrectWork_Splitter_GetAndPut(IN const int _nMaxBuffers, IN const int _nMaxClients, IN const int _nWorkTimeMsec, IN const int _nDelayBetweenObjectsTimeMsec)
{
	//��������
	Splitter<std::string> cSplitter;
	bool bCorrectWorkWithError(false);
	//���������� ��������� nullptr - ������ 4 ��� SplitterGet � SplitterPut
	bool bCorrectWorkWithNullptr(false);
	{
		std::string* pstrObjectForClientNull = nullptr;
		std::string** ppstrObjectForSplitterNull = nullptr;
		std::string* pstrTestStringNull = nullptr;
		std::string** ppstrObjectForSplitterNull2 = &pstrTestStringNull;
		bCorrectWorkWithNullptr =  (   4 == cSplitter.SplitterGet(1, ppstrObjectForSplitterNull, 30)
									&& 4 == cSplitter.SplitterGet(1, ppstrObjectForSplitterNull2, 30)
									&& 4 == cSplitter.SplitterPut(pstrObjectForClientNull, true, 30));
	}
	//�������� �������
	std::string strObjectForSplitter("Test String");
	std::string strObjectForClient("");
	std::string* pstrObjectForClient = &strObjectForClient;
	
	//�������� ������ � �������������������� ������� - ������ 1 ��� SplitterGet � SplitterPut
	bool bCorrectWorkWithoutInitialization(false);
	bCorrectWorkWithoutInitialization = (  1 == cSplitter.SplitterPut(&strObjectForSplitter, true, 30) 
										&& 1 == cSplitter.SplitterGet(1, &pstrObjectForClient, 30));

	//�������������� �������
	bool bSplitterHasInitialization = cSplitter.SplitterInit(_nMaxBuffers, _nMaxClients);
	
	if (_nMaxBuffers > 0, _nMaxClients >0 ) {
		//��������� �������� ���������
		bool bCorreceAddClient(true);
		std::vector<std::pair<int, bool>> vecClients(0);
		for (int i = 0; i < _nMaxClients && bCorreceAddClient; i++) {
			int nClientId(0);
			bCorreceAddClient = cSplitter.SplitterClientAdd(&nClientId);
			vecClients.push_back(std::pair<int, bool>(nClientId, bCorreceAddClient));
		}

		//�������� ������ SplitterGet � �������� Id ������� - ������ 2 ��� SplitterGet
		bool bCorrectWorkWithWrongId(false);
		bCorrectWorkWithWrongId = (2 == cSplitter.SplitterGet(_nMaxClients + 1, &pstrObjectForClient, 30));
		
		//�������� ������ �������� SplitterGet - ������ 3 ��� SplitterGet
		bool bCorrectWorkWithTimeOut_SplitterGet(false);
		bCorrectWorkWithTimeOut_SplitterGet = (3 == cSplitter.SplitterGet(0, &pstrObjectForClient, 30));

		
		//��������� ��������� ����� ���������, �.�. ��� ������� ������� ����� ������ ������
		for (int i = 0; i < _nMaxBuffers; i++) {
			cSplitter.SplitterPut(&strObjectForSplitter, true, 30);
		}
		
		//������ �������� ��������� ������ 2 � 3 � ������� SplitterPut - ������ 2(������ ����� ���� �����������) � ������ 3 (�������)
		auto CheckTimeOutAndUniqueLaunch = [&](bool _bForceNew) {
			//��������� ��������� SplitterPut ������������
			std::vector<std::shared_future< int>> vecFutures;
			for (int i = 0; i < 10; i++) {
				auto fut = std::async(std::launch::async, [&] {
					int nResult = cSplitter.SplitterPut(&strObjectForSplitter, _bForceNew, 30);
					return nResult;
				});
				vecFutures.push_back(fut.share());
			}
			//��������� ���������� ���������� ����������� SplitterPut
			//���� _bForceNew == false, �� ���� �� ������� ������ ������� 3 (������ ��������), � ��������� 2 (������ ������� ����� ���� �����������)
			//���� _bForceNew == true, �� ���� ������� ������ ������� 0 (������� ����������), � ��������� 2 (������ ������� ����� ���� �����������)
			bool bCoorectError(true);
			bool bUnigueResult(false);
			for (size_t i = 0; i < vecFutures.size() && bCoorectError; i++) {
				if (2 == vecFutures.at(i).get()) {
					bCoorectError = true;
				}
				else {
					//���� ���������� ��������� ����������� ���r� ������ ����, �� ���������� ������
					if (!bUnigueResult) {
						bCoorectError = ((_bForceNew ? 0 : 3) == vecFutures.at(i).get());
						bUnigueResult = true;
					}
					else {
						bCoorectError = false;
					}
				}
			}
			return bCoorectError;
		};
		//�������� �������� � ������� ����� ������ ���������� � ������� SplitterPut - ������ 2(������ ����� ���� �����������) � ������ 3 (�������)
		bool bCorrectWorkTimeOutAndUniqueLaunch_SplitterPut( CheckTimeOutAndUniqueLaunch(false) && CheckTimeOutAndUniqueLaunch(true));
		
		//��������� ������ 5 - ���������� SplitterPut �� SplitterFlush();
		bool bCorrectSplitterFlush(false);
		auto fut = std::async(std::launch::async, [&] {
			int nResult = cSplitter.SplitterPut(&strObjectForSplitter, false, 30);
			return nResult;
		});
		cSplitter.SplitterFlush();
		bCorrectSplitterFlush = (5 == fut.get());

		//�������� ����� ��� �������� ����� SplitterFlush()
		//��������� ��������� ����� ���������, �.�. ��� ������� ������� ����� ������ ������
		for (int i = 0; i < _nMaxBuffers; i++) {
			cSplitter.SplitterPut(&strObjectForSplitter, true, 30);
		}
		//�������� �������� � ������� ����� ������ ���������� � ������� SplitterPut - ������ 2(������ ����� ���� �����������) � ������ 3 (�������)
		bool bCorrectWorkTimeOutAndUniqueLaunch_SplitterPut_AfterFlush (CheckTimeOutAndUniqueLaunch(false) && CheckTimeOutAndUniqueLaunch(true));
		
		//�������� �������� ��������
		cSplitter.SplitterClose();
		//�������������� � ��������� �� �����
		bool bCorrectReInitialization(false);
		cSplitter.SplitterInit(_nMaxBuffers, _nMaxClients);
		if (_nMaxBuffers > 0, _nMaxClients > 0) {
			// ��������� ��������� ����� ���������, �.�.��� ������� ������� ����� ������ ������
			for (int i = 0; i < _nMaxBuffers; i++) {
				cSplitter.SplitterPut(&strObjectForSplitter, true, 30);
			}
			//�������� �������� � ������� ����� ������ ���������� � ������� SplitterPut - ������ 2(������ ����� ���� �����������) � ������ 3 (�������)
			bool bCorrectWorkTimeOutAndUniqueLaunch_SplitterPut_AfterReInitialization(CheckTimeOutAndUniqueLaunch(false) && CheckTimeOutAndUniqueLaunch(true));
			//��������� ������ 5 - ���������� SplitterPut �� SplitterFlush();
			bool bCorrectSplitterFlush_AfterReInitialization(false);
			auto fut = std::async(std::launch::async, [&] {
				int nResult = cSplitter.SplitterPut(&strObjectForSplitter, false, 30);
				return nResult;
			});
			cSplitter.SplitterFlush();
			bCorrectSplitterFlush_AfterReInitialization = (5 == fut.get());
		
			bCorrectReInitialization = bCorrectWorkTimeOutAndUniqueLaunch_SplitterPut_AfterReInitialization && bCorrectSplitterFlush_AfterReInitialization;
		}
		else {
			//���� �������� �� ����������������� ��-�� ����� � ���������� �������������
			bCorrectReInitialization = true;
		}


		//���������� ��� ����� �������� ������
		bCorrectWorkWithError =    bCorrectWorkWithNullptr 
								&& bCorrectWorkWithoutInitialization 
								&& bCorrectWorkWithWrongId 
								&& bCorrectWorkWithTimeOut_SplitterGet 
								&& bCorrectWorkTimeOutAndUniqueLaunch_SplitterPut
								&& bCorrectSplitterFlush
								&& bCorrectWorkTimeOutAndUniqueLaunch_SplitterPut_AfterFlush
								&& bCorrectReInitialization ;
	}
	else {
		//���� �������� �� ����������������� ��-�� ����� � ���������� �������������
		bCorrectWorkWithError = bCorrectWorkWithNullptr && bCorrectWorkWithoutInitialization;
	}
	 	
	//�������� ������-������
	std::string strTestString("Test String");
	//���� ����������� ��������
	std::atomic_bool bHasTimeOut(false);
	//������ ��� ������� ��������
	auto WaitTimeOut = [&bHasTimeOut, _nWorkTimeMsec]() {
		std::this_thread::sleep_for(milliseconds{ _nWorkTimeMsec });
		bHasTimeOut.store(true);
	};


	//������������ ������� ������ �������� ��� ������ �������
	bool bCorrectRegularWorkSynchroMode(false);
	Splitter<std::string> cSplitterForLinealWork;
	//�������������� �������
	bool bSplitterForLinealWorkHasInitialization = cSplitterForLinealWork.SplitterInit(_nMaxBuffers, _nMaxClients);
	if (bSplitterForLinealWorkHasInitialization) {
		//��������� ������ ������ ������� � ���������� ������
		int nClientId(0);
		bool bCorrecteAddClient = cSplitterForLinealWork.SplitterClientAdd(&nClientId);		
		bool bCorrectWorkWithOneClientSynchroMode(true);		
		//������ ������� ��������
		auto futureTimeOutForWorkWithOneClientSynchroMode = std::async(std::launch::async, WaitTimeOut);
		//��������� ������� SplitterPut � SplitterGet
		for (int i(0); !bHasTimeOut.load() && bCorrectWorkWithOneClientSynchroMode; i++) {
			std::string strObject = strTestString + std::to_string(i);
			std::string strClientObject("");
			std::string* pstrClientObject = &strClientObject;
			int nCorrectPut =  cSplitterForLinealWork.SplitterPut(&strObject, true, _nDelayBetweenObjectsTimeMsec);
			int nCorrectGet = cSplitterForLinealWork.SplitterGet(nClientId, &pstrClientObject, _nDelayBetweenObjectsTimeMsec);
			bCorrectWorkWithOneClientSynchroMode = 0 == nCorrectPut && 0 == nCorrectGet && strObject == *pstrClientObject;
		}
		//������������ ������ ������ ������� � ����� ��������� ��������� ��������
		int nSecondClientId(0);
		bool bCorrecteAddSecondClient = cSplitterForLinealWork.SplitterClientAdd(&nSecondClientId);
		bool bCorrectWorkWithSecondClientSynchroMode(true);
		//������ ������� ��������
		bHasTimeOut.store(false);
		auto futureTimeOutForWorkWithSecondClientSynchroMode = std::async(std::launch::async, WaitTimeOut);
		//��������� ������� SplitterPut � SplitterGet
		for (int i(0); !bHasTimeOut.load() && bCorrectWorkWithSecondClientSynchroMode; i++) {
			std::string strObject = strTestString + std::to_string(i);
			std::string strClientObject("");
			std::string* pstrClientObject = &strClientObject;
			int nCorrectPut = cSplitterForLinealWork.SplitterPut(&strObject, true, _nDelayBetweenObjectsTimeMsec);
			int nCorrectGet = cSplitterForLinealWork.SplitterGet(nSecondClientId, &pstrClientObject, _nDelayBetweenObjectsTimeMsec);
			bCorrectWorkWithSecondClientSynchroMode = 0 == nCorrectPut && 0 == nCorrectGet && strObject == *pstrClientObject;
		}
		bCorrectRegularWorkSynchroMode = bCorrectWorkWithOneClientSynchroMode && bCorrectWorkWithSecondClientSynchroMode;
	}
	else {
		bCorrectRegularWorkSynchroMode = true;
	}

	
	//������������ ����������� ������ ���������� ��������		
	int nMaxAsynchroClients(50);//������ 2 ������ ���� ���
	bool bCorrectWorkWithFewClientAsynchroMode(true);
	Splitter<std::string> cSplitterForFewClients;
	//�������������� �������
	bool bSplitterForForFewClientsHasInitialization = cSplitterForFewClients.SplitterInit(_nMaxBuffers, _nMaxClients);
	if (bSplitterForForFewClientsHasInitialization) {
		//��������� ������� ���������
		bool bCorrecteAddClient(true);
		std::vector<std::pair<int, bool>> vecClients(0);
		for (int i = 0; i < _nMaxClients && i < nMaxAsynchroClients && bCorrecteAddClient; i++) {
			int nClientId(0);
			bCorrecteAddClient = cSplitterForFewClients.SplitterClientAdd(&nClientId);
			vecClients.push_back(std::pair<int, bool>(nClientId, bCorrecteAddClient));
		}
		
		bool bCorrectAsynchroWork(true);
		//������ ������� ��������
		bHasTimeOut.store(false);
		auto futureTimeOutForWorkWithSecondClientSynchroMode = std::async(std::launch::async, WaitTimeOut);
		//��������� ������� SplitterPut � SplitterGet
		for (int i(0); !bHasTimeOut.load() && bCorrectAsynchroWork; i++) {
			//��������� SplitterGet
			std::string strObject = strTestString + std::to_string(i);
			int nCorrectPut = cSplitterForFewClients.SplitterPut(&strObject, true, _nDelayBetweenObjectsTimeMsec);

			std::vector<std::shared_future< std::string>> vecFutures;
			//���������� ��������� �������� �������� � ��������� 2 ���������� ������
			for (int j = 0; j < _nMaxClients && j < (nMaxAsynchroClients - 2); j++) {
				auto fut = std::async(std::launch::async, [&cSplitterForFewClients, j, _nDelayBetweenObjectsTimeMsec] {
					std::string strClientObject("");
					std::string* pstrClientObject = &strClientObject;
					int nCorrectGet = cSplitterForFewClients.SplitterGet(j, &pstrClientObject, _nDelayBetweenObjectsTimeMsec);
					return strClientObject;
				});
				vecFutures.push_back(fut.share());
			}

			//��������� ��� ��� ������� �������� ������
			bool bCorrectGettingObject(true);
			for (size_t i(0); i < vecFutures.size() && bCorrectGettingObject; i++) {
				bCorrectGettingObject = (strObject == vecFutures.at(i).get());
			}

			bCorrectAsynchroWork = 0 == nCorrectPut && bCorrectGettingObject;
		}		
		bCorrectWorkWithFewClientAsynchroMode = bCorrectAsynchroWork;
	}
	else {
		bCorrectWorkWithFewClientAsynchroMode = true;
	}

	//�������� �������������� ������� �������
	bool bCorrectWorkChaos(true);
	Splitter<std::string> cSplitterForChaos;
	//�������������� �������
	bool bSplitterForForChaosHasInitialization = cSplitterForChaos.SplitterInit(_nMaxBuffers, _nMaxClients);
	if (bSplitterForForChaosHasInitialization) {
		//��������� ������� ���������
		bool bCorrecteAddClient(true);
		std::vector<std::pair<int, bool>> vecClients(0);
		for (int i = 0; i < _nMaxClients && bCorrecteAddClient; i++) {
			int nClientId(0);
			bCorrecteAddClient = cSplitterForChaos.SplitterClientAdd(&nClientId);
			vecClients.push_back(std::pair<int, bool>(nClientId, bCorrecteAddClient));
		}
		bHasTimeOut.store(false);
		auto futureTimeOutForWorkWithSecondClientSynchroMode = std::async(std::launch::async, WaitTimeOut);
		//����������� ������ ������� SplitterPut � SplitterGet
		for (int i(0); !bHasTimeOut.load(); i++) {
			//��������� SplitterGet
			std::string strObject = strTestString + std::to_string(i);
			int nCorrectPut = cSplitterForChaos.SplitterPut(&strObject, true, _nDelayBetweenObjectsTimeMsec);

			std::vector<std::shared_future< std::string>> vecFutures;
			//���������� ��������� �������� �������� � ��������� 2 ���������� ������
			for (int j = 0; j < _nMaxClients && j < (nMaxAsynchroClients - 2); j++) {
				auto fut = std::async(std::launch::async, [&cSplitterForChaos, j, _nDelayBetweenObjectsTimeMsec] {
					std::string strClientObject("");
					std::string* pstrClientObject = &strClientObject;
					int nCorrectGet = cSplitterForChaos.SplitterGet(j, &pstrClientObject, _nDelayBetweenObjectsTimeMsec);
					return strClientObject;
				});
				vecFutures.push_back(fut.share());
			}
			//������������� ��������� SplitterGet 2 ����
			std::string strObject2 = strTestString;
			nCorrectPut = cSplitterForChaos.SplitterPut(&strObject2, true, _nDelayBetweenObjectsTimeMsec);
			nCorrectPut = cSplitterForChaos.SplitterPut(&strObject2, true, _nDelayBetweenObjectsTimeMsec);
			//��� ���������� ��������� �������� �������� � �������� 2 ���������� ������
			for (int j = 0; j < _nMaxClients && j < (nMaxAsynchroClients - 2); j++) {
				auto fut = std::async(std::launch::async, [&cSplitterForChaos, j, _nDelayBetweenObjectsTimeMsec] {
					std::string strClientObject("");
					std::string* pstrClientObject = &strClientObject;
					int nCorrectGet = cSplitterForChaos.SplitterGet(j, &pstrClientObject, _nDelayBetweenObjectsTimeMsec);
					return strClientObject;
				});
				vecFutures.push_back(fut.share());
			}

			//��������� ��� ��� ������������ ����������� ���������� �������� SplitterPut
			bool bCorrectGettingObject(true);
			for (size_t i(0); i < vecFutures.size() && bCorrectGettingObject; i++) {
				nCorrectPut = cSplitterForChaos.SplitterPut(&strObject2, true, _nDelayBetweenObjectsTimeMsec);
				vecFutures.at(i).get();
			}
		}
		bCorrectWorkChaos = true;
	}
	else {
		bCorrectWorkChaos = true;
	}
	return  bCorrectWorkWithError && bCorrectRegularWorkSynchroMode && bCorrectWorkWithFewClientAsynchroMode && bCorrectWorkChaos;
}
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


// Проверка на равенстов информации о клиенте информации о клиенте
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



//Печать проваленного теста
void PrintFailResult( IN const std::string _strHeader, IN  bool _bRightResult) {
	std::cout << _strHeader;
	if (!_bRightResult) {
		std::cout << ": Fail\n";
	}
	else {
		std::cout  << ": Test Done!\n";
	}
};





//Тестирование создания экземпляров SplitterClientsBase и тестирование GetMaxClients() и GetCurrentClients()
bool CorrectCreate_SplitterClientsBase(IN const size_t _uMaxClients)
{
	SplitterClientsBase cClientBase(_uMaxClients);
	return cClientBase.GetMaxClients() == _uMaxClients && cClientBase.GetCurrentClients() == 0;
}


//Тестирование функции bool SetMaxClients(IN const size_t _uMaxClients);
bool CorrectWork_SplitterClientsBase_SetMaxClients(IN const size_t _uMaxClients)
{
	//Проверка корректной установки нового максимального количества клиентов
	bool bCorrectSet(false);	
	SplitterClientsBase cClientBase;
	bCorrectSet = cClientBase.SetMaxClients(_uMaxClients) && cClientBase.GetMaxClients() == _uMaxClients && cClientBase.GetCurrentClients() == 0;
	
	//Создаем клиентов
	bool bCorrectAdd (true);
	for (size_t i = 0; i < _uMaxClients && bCorrectAdd; i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(i);
		bCorrectAdd = cClientBase.AddClient(&nClientId, nCurrentIndexObject) && nClientId == i && cClientBase.GetCurrentClients() == i+1;
	};
	
	//Проверка устновки максимально количества клиентов меньше чем текущее количество клиентов
	bool bCorrectFailSet(true);
	if (_uMaxClients > 0) {
		bCorrectFailSet = !cClientBase.SetMaxClients(_uMaxClients - 1);
	};


	return bCorrectSet && bCorrectAdd && bCorrectFailSet;
};


//Тестирование отсутвия инфы функции bool SplitterClientsBase::GetClientCurrentIndex(IN const size_t _uClientID, OUT size_t *const _uCurrentObjectIndex)
bool CorrectWork_SplitterClientsBase_GetClientCurrentIndex(IN const size_t _uClientID)
{
	size_t nCurrentIndexObject(0);
	SplitterClientsBase cClientBase;
	return !cClientBase.GetClientCurrentIndex(_uClientID, &nCurrentIndexObject);
	
};

//Тестирование Функции bool SplitterClientsBase::AddClient(OUT size_t *const _uClientID, IN  size_t _uCurrentObjectIndex)
bool CorrectWork_SplitterClientsBase_AddClient(IN const size_t _uMaxClients)
{
	//Создаем клиентов
	bool bCorrectAdd(true);
	SplitterClientsBase cClientBase(_uMaxClients);
	for (size_t i = 0; i < _uMaxClients && bCorrectAdd;i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(i);
		bCorrectAdd = cClientBase.AddClient(&nClientId, nCurrentIndexObject) && nClientId == i && cClientBase.GetCurrentClients() == i + 1;
	};
	//Проверяем корректность получения инфы по клиенту
	bool bCorrectGetInfo(true);
	for (size_t i = 0; i < _uMaxClients && bCorrectGetInfo; i++) {
		size_t nCurrentIndexObject(0);
		bCorrectGetInfo = cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject) && i == nCurrentIndexObject;
	};
	//Проверяем отсутствие инфы по заведомо не существующим клиентам
	bool bCorrectGetFailInfo(true);
	size_t nLimit(_uMaxClients + 10);
	for (size_t i = _uMaxClients; i < nLimit; i++) {
		size_t nCurrentIndexObject(0);
		bCorrectGetFailInfo = !cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject) && 0 == nCurrentIndexObject;
	}
	return bCorrectAdd && bCorrectGetInfo && bCorrectGetFailInfo;
}


//Тестирование Функции bool SplitterClientsBase::RemoveClient(OUT size_t *const _uClientID, IN  size_t _uCurrentObjectIndex)
bool CorrectWork_SplitterClientsBase_RemoveClient(IN const size_t _uMaxClients)
{
	//Создаем клиентов
	bool bCorrectAdd(true);
	SplitterClientsBase cClientBase(_uMaxClients);
	for (size_t i = 0; i < _uMaxClients && bCorrectAdd; i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(i);
		bCorrectAdd = cClientBase.AddClient(&nClientId, nCurrentIndexObject) && cClientBase.GetCurrentClients() == i + 1;
	};
	
	//Проверяем рандомное удаление клиентов
	bool bCorrectRemove(true);
	int nMaxClients = static_cast<int> (_uMaxClients);
	auto getRandomIndex = std::bind(std::uniform_int_distribution<>{0, nMaxClients >= 1 ? nMaxClients - 1 : 0}, std::default_random_engine{});
	for (size_t i = 0; i < _uMaxClients && bCorrectRemove; i++) {
		size_t nIndexToDelete = getRandomIndex();
		cClientBase.RemoveClient(nIndexToDelete);
		size_t nCurrentIndexObject(0);
		bCorrectRemove = !cClientBase.GetClientCurrentIndex(nIndexToDelete, &nCurrentIndexObject) && 0 == nCurrentIndexObject ;
	}

	//восстанавливаем клиентов
	for (size_t i = 0; i < _uMaxClients; i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(i);
		cClientBase.AddClient(&nClientId, nCurrentIndexObject);
	};
	bool bCorrectActivate = cClientBase.GetCurrentClients() == _uMaxClients;

	//Проверям последовательное удаление и количество активных клиентов
	bool bCorrectSecondRemove(true);
	for (size_t i = 0; i < _uMaxClients && bCorrectSecondRemove; i++) {
		cClientBase.RemoveClient(i);
		size_t nCurrentIndexObject(0);
		bCorrectSecondRemove = !cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject) && 0 == nCurrentIndexObject && cClientBase.GetCurrentClients() == _uMaxClients - i - 1;
	}

	return bCorrectAdd && bCorrectRemove && bCorrectActivate && bCorrectSecondRemove;
};



//Тестирование Функции bool SplitterClientsBase::SetClientCurrentIndex(IN const size_t _uClientID, IN const size_t _uCurrentObjectIndex) 
bool CorrectWork_SplitterClientsBase_SetClientCurrentIndex(IN const size_t _uMaxClients)
{
	//Создаем клиентов
	bool bCorrectAdd(true);
	SplitterClientsBase cClientBase(_uMaxClients);
	for (size_t i = 0; i < _uMaxClients && bCorrectAdd; i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(i);
		bCorrectAdd = cClientBase.AddClient(&nClientId, nCurrentIndexObject);
	};

	//Проверяем установку нового рандомного CurrentIndexObject для всех клиентов
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

	//Проверяем установку нового CurrentIndexObject для заведомо не существующих клиентов
	bool bCorrectSetCurrentIndexForNotExistClient(true);
	size_t nLimit(_uMaxClients + 10);
	for (size_t i = _uMaxClients; i < nLimit && bCorrectSetCurrentIndexForNotExistClient; i++) {
		const size_t nNewCurrentIndex = getRandomIndex();
		size_t nCurrentIndexObject(-1);
		bCorrectSetCurrentIndexForNotExistClient = !cClientBase.SetClientCurrentIndex(i, nNewCurrentIndex)
												   && !cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject)
												   && nNewCurrentIndex != nCurrentIndexObject;
	}

	//Дизактивируем всех клиентов
	bool bCorrectRemove(true);
	for (size_t i = 0; i < _uMaxClients && bCorrectRemove; i++) {
		cClientBase.RemoveClient(i);
		size_t nCurrentIndexObject(0);
		bCorrectRemove = !cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject) && 0 == nCurrentIndexObject;
	}

	//Проверяем установку нового CurrentIndexObject для дизактивированных клиентов
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



//Тестирование Функции bool SplitterClientsBase::CheckCurrentObjectIndex(IN const size_t _uCurrentIndex);
bool CorrectWork_SplitterClientsBase_CheckCurrentObjectIndex(IN const size_t _uMaxClients)
{
	//Создаем клиентов
	bool bCorrectAdd(true);
	SplitterClientsBase cClientBase(_uMaxClients);
	int nMaxClients = static_cast<int> (_uMaxClients);
	auto getRandomIndex = std::bind(std::uniform_int_distribution<>{0, nMaxClients >= 1 ? nMaxClients - 1 : 0}, std::default_random_engine{});
	for (size_t i = 0; i < _uMaxClients && bCorrectAdd; i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(getRandomIndex());
		bCorrectAdd = cClientBase.AddClient(&nClientId, nCurrentIndexObject);
	}
	//Проверяем определение текущих индексов
	bool bCorrectCheckCurrentIndex(true);
	for (size_t i = 0; i < _uMaxClients && bCorrectCheckCurrentIndex; i++) {
		const size_t nCurrentIndex(getRandomIndex());
		//Проверяем должны ли найтись соответствующие индексы в cClientBase
		bool bHasChanges(false);
		for (size_t j = 0; j < _uMaxClients && !bHasChanges; j++) {
			size_t nClientCurrentIndex(0);
			cClientBase.GetClientCurrentIndex(j, &nClientCurrentIndex);
			bHasChanges = nCurrentIndex == nClientCurrentIndex;
		}
		//Проверяем корректный возврат функции ChangeCurrentObjectIndex
		bCorrectCheckCurrentIndex = bHasChanges == cClientBase.CheckCurrentObjectIndex(nCurrentIndex);
	}
	return bCorrectAdd && bCorrectCheckCurrentIndex;
};



//Тестирование Функции bool SplitterClientsBase::ChangeCurrentObjectIndex(IN const size_t _uCurrentIndex, IN const size_t _uNextCurrentIndex);
bool CorrectWork_SplitterClientsBase_ChangeCurrentObjectIndex(IN const size_t _uMaxClients)
{
	//Создаем клиентов
	bool bCorrectAdd(true);
	SplitterClientsBase cClientBase(_uMaxClients);
	for (size_t i = 0; i < _uMaxClients && bCorrectAdd; i++) {
		size_t nClientId(0);
		size_t nCurrentIndexObject(i);
		bCorrectAdd = cClientBase.AddClient(&nClientId, nCurrentIndexObject);
	}
	//Проверяем замену текущих индексов
	bool bCorrectChangeCurrentIndex(true);
	int nMaxClients = static_cast<int> (_uMaxClients);
	auto getRandomIndex = std::bind(std::uniform_int_distribution<>{0, nMaxClients >= 1 ? nMaxClients - 1 : 0}, std::default_random_engine{});
	for (size_t i = 0; i < _uMaxClients && bCorrectChangeCurrentIndex; i++) {
		const size_t nNewCurrentIndex = getRandomIndex();
		const size_t nOldCurrentIndex = nNewCurrentIndex + 1;
		//Проверяем должны ли произойти изменения в cClientBase
		bool bHasChanges (false);
		for (size_t j = 0; j < _uMaxClients && !bHasChanges; j++) {
			size_t nCurrentIndexObject(0);
			cClientBase.GetClientCurrentIndex(j, &nCurrentIndexObject);
			bHasChanges = nOldCurrentIndex == nCurrentIndexObject;
		}		
		//Проверяем корректный возврат функции ChangeCurrentObjectIndex
		bCorrectChangeCurrentIndex = bHasChanges == cClientBase.ChangeCurrentObjectIndex( nOldCurrentIndex, nNewCurrentIndex);
		//Проверяем произведенную замену
		for (size_t k = 0; k < _uMaxClients && bCorrectChangeCurrentIndex; k++) {
			size_t nCurrentIndexObject(0);
			bCorrectChangeCurrentIndex = cClientBase.GetClientCurrentIndex(k, &nCurrentIndexObject)
										 && nCurrentIndexObject != nOldCurrentIndex;
		}
	}

	//Дизактивируем всех клиентов
	bool bCorrectRemove(true);
	for (size_t i = 0; i < _uMaxClients && bCorrectRemove; i++) {
		cClientBase.RemoveClient(i);
		size_t nCurrentIndexObject(0);
		bCorrectRemove = !cClientBase.GetClientCurrentIndex(i, &nCurrentIndexObject) && 0 == nCurrentIndexObject;
	}

	//Проверяем замену текущих индексов для дизактивированных клиентов
	bool bCorrectChangeCurrentIndexForNonActiveClient(true);
	for (size_t i = 0; i < _uMaxClients && bCorrectChangeCurrentIndexForNonActiveClient; i++) {
		const size_t nNewCurrentIndex = getRandomIndex();
		const size_t nOldCurrentIndex = nNewCurrentIndex + 1;
		//Проверяем должны ли произойти изменения в cClientBase
		bool bHasChanges (false);
		for (size_t j = 0; j < _uMaxClients && !bHasChanges; j++) {
			size_t nCurrentIndexObject(-1);
			cClientBase.GetClientCurrentIndex(j, &nCurrentIndexObject);
			bHasChanges = nOldCurrentIndex == nCurrentIndexObject;
		}
		//Проверяем корректный возврат функции ChangeCurrentObjectIndex - отсутствие необходимости изменений
		bCorrectChangeCurrentIndexForNonActiveClient = bHasChanges == cClientBase.ChangeCurrentObjectIndex(nOldCurrentIndex, nNewCurrentIndex);
	}
	return bCorrectAdd && bCorrectChangeCurrentIndex && bCorrectRemove && bCorrectChangeCurrentIndexForNonActiveClient;
}




//Тестирование создания экземпляра класса Splitter
bool CorrectCreate_Splitter(IN const int _nMaxBuffers, IN const int _nMaxClients) 
{
	
	bool bCorrectCreate(false);

	Splitter<std::string> cSplitter;
	// Результат инициализации от корректности _nMaxBuffers и _nMaxClients - они должны быть больше нуля
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

//Тестирование добавления новых клиентов, их удаления и получения о них информации и их общего количества в классе Splitter
bool CorrectWork_Splitter_WorkWithClients(IN const int _nMaxBuffers, IN const int _nMaxClients) 
{
	Splitter<std::string> cSplitter;
	cSplitter.SplitterInit(_nMaxBuffers, _nMaxClients);
	
	//Заполняем сплиттер клиентами
	bool bCorreceAddClient(true);
	std::vector<std::pair<int, bool>> vecClients(0);
	for (int i = 0; i < _nMaxClients && bCorreceAddClient; i++) {
		int nClientId(0);
		bCorreceAddClient = cSplitter.SplitterClientAdd(&nClientId);
		vecClients.push_back(std::pair<int, bool>(nClientId, bCorreceAddClient));
	}

	//Лямбда проверки количества клиентов
	auto CheckClientsCount = [&]() {
		//Вычисляем количество активных клиентов
		int nCountActiveClientsInVector = std::count_if(vecClients.begin(), vecClients.end(), [](std::pair<int, bool> _p) {return _p.second; });
		int nCountActiveClientsInSplitter(0);
		cSplitter.SplitterClientGetCount(&nCountActiveClientsInSplitter);
		return (nCountActiveClientsInSplitter == nCountActiveClientsInVector);
	};
	//Лямбда проверки SplitterClientGetById 
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
	//Лямбда проверки SplitterClientGetByIndex
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

	//Проверям созданных клиентов с помошью Лямбд
	bool bCorrectClientsCount(CheckClientsCount() && Check_SplitterClientGetById() && Check_SplitterClientGetByIndex());	
	
	auto getRandomId = std::bind(std::uniform_int_distribution<>{0, _nMaxClients >= 1 ? _nMaxClients - 1 : 0}, std::default_random_engine{});
	//Удаляем некоторых клиентов
	for (int i(0); i < _nMaxClients; i+=2) {
		int nDeleteId(getRandomId());
		cSplitter.SplitterClientRemove(nDeleteId);
		vecClients.at(nDeleteId).second = false;
	}
	//Проверям клиентов с помошью Лямбд после рандомного удаления
	bool bCorrectRemoveClient(CheckClientsCount() && Check_SplitterClientGetById() && Check_SplitterClientGetByIndex());
		
	//Удаляем всех клиентов
	for (int i(0); i < _nMaxClients; i++) {
		cSplitter.SplitterClientRemove(i);
		vecClients.at(static_cast<size_t> (i)).second = false;
	}
	//Проверям клиентов с помошью Лямбд, после полного удаления
	bool bCorrectRemoveAllClient(CheckClientsCount() && Check_SplitterClientGetById() && Check_SplitterClientGetByIndex());


	return bCorreceAddClient && bCorrectClientsCount && bCorrectRemoveClient && bCorrectRemoveAllClient;
}



//Тестирование функций SplitterPut и SplitterGet
//_nWorkTimeMsec - время тестирования сплитера для под-тестов, _nDelayBetweenObjectsTimeMsec - задержка междупоступлением новых данных
bool CorrectWork_Splitter_GetAndPut(IN const int _nMaxBuffers, IN const int _nMaxClients, IN const int _nWorkTimeMsec, IN const int _nDelayBetweenObjectsTimeMsec)
{
	//Сплиттер
	Splitter<std::string> cSplitter;
	bool bCorrectWorkWithError(false);
	//Корректная обработка nullptr - Ошибка 4 для SplitterGet и SplitterPut
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
	//Тестовые объекты
	std::string strObjectForSplitter("Test String");
	std::string strObjectForClient("");
	std::string* pstrObjectForClient = &strObjectForClient;
	
	//Проверка работы с неинициализированным буфером - Ошибка 1 для SplitterGet и SplitterPut
	bool bCorrectWorkWithoutInitialization(false);
	bCorrectWorkWithoutInitialization = (  1 == cSplitter.SplitterPut(&strObjectForSplitter, true, 30) 
										&& 1 == cSplitter.SplitterGet(1, &pstrObjectForClient, 30));

	//Инициализируем сплитер
	bool bSplitterHasInitialization = cSplitter.SplitterInit(_nMaxBuffers, _nMaxClients);
	
	if (_nMaxBuffers > 0, _nMaxClients >0 ) {
		//Заполняем сплиттер клиентами
		bool bCorreceAddClient(true);
		std::vector<std::pair<int, bool>> vecClients(0);
		for (int i = 0; i < _nMaxClients && bCorreceAddClient; i++) {
			int nClientId(0);
			bCorreceAddClient = cSplitter.SplitterClientAdd(&nClientId);
			vecClients.push_back(std::pair<int, bool>(nClientId, bCorreceAddClient));
		}

		//Проверка работы SplitterGet с неверным Id клиента - Ошибка 2 для SplitterGet
		bool bCorrectWorkWithWrongId(false);
		bCorrectWorkWithWrongId = (2 == cSplitter.SplitterGet(_nMaxClients + 1, &pstrObjectForClient, 30));
		
		//Проверка работы таймаута SplitterGet - Ошибка 3 для SplitterGet
		bool bCorrectWorkWithTimeOut_SplitterGet(false);
		bCorrectWorkWithTimeOut_SplitterGet = (3 == cSplitter.SplitterGet(0, &pstrObjectForClient, 30));

		
		//Полностью заполняем буфер объектами, т.е. все клиенты ожидают самый старый объект
		for (int i = 0; i < _nMaxBuffers; i++) {
			cSplitter.SplitterPut(&strObjectForSplitter, true, 30);
		}
		
		//Лямбда проверки получения ошибок 2 и 3 у функции SplitterPut - Ошибка 2(запуск более двух экземпляров) и Ошибка 3 (таймаут)
		auto CheckTimeOutAndUniqueLaunch = [&](bool _bForceNew) {
			//Запускаем несколько SplitterPut одновременно
			std::vector<std::shared_future< int>> vecFutures;
			for (int i = 0; i < 10; i++) {
				auto fut = std::async(std::launch::async, [&] {
					int nResult = cSplitter.SplitterPut(&strObjectForSplitter, _bForceNew, 30);
					return nResult;
				});
				vecFutures.push_back(fut.share());
			}
			//Проверяем полученные результаты асинхронных SplitterPut
			//Если _bForceNew == false, то одна из функция должна вернуть 3 (Ошибка Таймаута), а остальные 2 (Ошибка запуска более двух экземпляров)
			//Если _bForceNew == true, то одна функция должна вернуть 0 (штатное выполнение), а остальные 2 (Ошибка запуска более двух экземпляров)
			bool bCoorectError(true);
			bool bUnigueResult(false);
			for (size_t i = 0; i < vecFutures.size() && bCoorectError; i++) {
				if (2 == vecFutures.at(i).get()) {
					bCoorectError = true;
				}
				else {
					//Если уникальный результат встречается болrе одного раза, то возвращаем ошибку
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
		//Проверка таймаута и запуска более одного экземпляра у функции SplitterPut - Ошибка 2(запуск более двух экземпляров) и Ошибка 3 (таймаут)
		bool bCorrectWorkTimeOutAndUniqueLaunch_SplitterPut( CheckTimeOutAndUniqueLaunch(false) && CheckTimeOutAndUniqueLaunch(true));
		
		//проверяем ошибку 5 - прерывание SplitterPut из SplitterFlush();
		bool bCorrectSplitterFlush(false);
		auto fut = std::async(std::launch::async, [&] {
			int nResult = cSplitter.SplitterPut(&strObjectForSplitter, false, 30);
			return nResult;
		});
		cSplitter.SplitterFlush();
		bCorrectSplitterFlush = (5 == fut.get());

		//Повторям тесты для сплитера после SplitterFlush()
		//Полностью заполняем буфер объектами, т.е. все клиенты ожидают самый старый объект
		for (int i = 0; i < _nMaxBuffers; i++) {
			cSplitter.SplitterPut(&strObjectForSplitter, true, 30);
		}
		//Проверка таймаута и запуска более одного экземпляра у функции SplitterPut - Ошибка 2(запуск более двух экземпляров) и Ошибка 3 (таймаут)
		bool bCorrectWorkTimeOutAndUniqueLaunch_SplitterPut_AfterFlush (CheckTimeOutAndUniqueLaunch(false) && CheckTimeOutAndUniqueLaunch(true));
		
		//Проверка закрытия сплитера
		cSplitter.SplitterClose();
		//Инициализируем и проверяем по новой
		bool bCorrectReInitialization(false);
		cSplitter.SplitterInit(_nMaxBuffers, _nMaxClients);
		if (_nMaxBuffers > 0, _nMaxClients > 0) {
			// Полностью заполняем буфер объектами, т.е.все клиенты ожидают самый старый объект
			for (int i = 0; i < _nMaxBuffers; i++) {
				cSplitter.SplitterPut(&strObjectForSplitter, true, 30);
			}
			//Проверка таймаута и запуска более одного экземпляра у функции SplitterPut - Ошибка 2(запуск более двух экземпляров) и Ошибка 3 (таймаут)
			bool bCorrectWorkTimeOutAndUniqueLaunch_SplitterPut_AfterReInitialization(CheckTimeOutAndUniqueLaunch(false) && CheckTimeOutAndUniqueLaunch(true));
			//проверяем ошибку 5 - прерывание SplitterPut из SplitterFlush();
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
			//Если сплиттер не инициализировался из-за нулей в аргументах инициализации
			bCorrectReInitialization = true;
		}


		//Агрегируем все флаги проверок ошибок
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
		//Если сплиттер не инициализировался из-за нулей в аргументах инициализации
		bCorrectWorkWithError = bCorrectWorkWithNullptr && bCorrectWorkWithoutInitialization;
	}
	 	
	//Тестовая строка-Объект
	std::string strTestString("Test String");
	//Флаг наступления таймаута
	std::atomic_bool bHasTimeOut(false);
	//Лямбда для отсчета таймаута
	auto WaitTimeOut = [&bHasTimeOut, _nWorkTimeMsec]() {
		std::this_thread::sleep_for(milliseconds{ _nWorkTimeMsec });
		bHasTimeOut.store(true);
	};


	//Тестирование штатной работы сплитера для одного клиента
	bool bCorrectRegularWorkSynchroMode(false);
	Splitter<std::string> cSplitterForLinealWork;
	//Инициализируем сплитер
	bool bSplitterForLinealWorkHasInitialization = cSplitterForLinealWork.SplitterInit(_nMaxBuffers, _nMaxClients);
	if (bSplitterForLinealWorkHasInitialization) {
		//Тестируем работу одного клиента в синхронном режиме
		int nClientId(0);
		bool bCorrecteAddClient = cSplitterForLinealWork.SplitterClientAdd(&nClientId);		
		bool bCorrectWorkWithOneClientSynchroMode(true);		
		//Запуск отсчета таймаута
		auto futureTimeOutForWorkWithOneClientSynchroMode = std::async(std::launch::async, WaitTimeOut);
		//Тестируем функции SplitterPut и SplitterGet
		for (int i(0); !bHasTimeOut.load() && bCorrectWorkWithOneClientSynchroMode; i++) {
			std::string strObject = strTestString + std::to_string(i);
			std::string strClientObject("");
			std::string* pstrClientObject = &strClientObject;
			int nCorrectPut =  cSplitterForLinealWork.SplitterPut(&strObject, true, _nDelayBetweenObjectsTimeMsec);
			int nCorrectGet = cSplitterForLinealWork.SplitterGet(nClientId, &pstrClientObject, _nDelayBetweenObjectsTimeMsec);
			bCorrectWorkWithOneClientSynchroMode = 0 == nCorrectPut && 0 == nCorrectGet && strObject == *pstrClientObject;
		}
		//Тестирование работы одного клиента с одним постоянно отстающим клиентом
		int nSecondClientId(0);
		bool bCorrecteAddSecondClient = cSplitterForLinealWork.SplitterClientAdd(&nSecondClientId);
		bool bCorrectWorkWithSecondClientSynchroMode(true);
		//Запуск отсчета таймаута
		bHasTimeOut.store(false);
		auto futureTimeOutForWorkWithSecondClientSynchroMode = std::async(std::launch::async, WaitTimeOut);
		//Тестируем функции SplitterPut и SplitterGet
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

	
	//Тестирование асинхронной работы нескольких клиентов		
	int nMaxAsynchroClients(50);//Больше 2 должно быть тут
	bool bCorrectWorkWithFewClientAsynchroMode(true);
	Splitter<std::string> cSplitterForFewClients;
	//Инициализируем сплитер
	bool bSplitterForForFewClientsHasInitialization = cSplitterForFewClients.SplitterInit(_nMaxBuffers, _nMaxClients);
	if (bSplitterForForFewClientsHasInitialization) {
		//Заполняем сплитер клиентами
		bool bCorrecteAddClient(true);
		std::vector<std::pair<int, bool>> vecClients(0);
		for (int i = 0; i < _nMaxClients && i < nMaxAsynchroClients && bCorrecteAddClient; i++) {
			int nClientId(0);
			bCorrecteAddClient = cSplitterForFewClients.SplitterClientAdd(&nClientId);
			vecClients.push_back(std::pair<int, bool>(nClientId, bCorrecteAddClient));
		}
		
		bool bCorrectAsynchroWork(true);
		//Запуск отсчета таймаута
		bHasTimeOut.store(false);
		auto futureTimeOutForWorkWithSecondClientSynchroMode = std::async(std::launch::async, WaitTimeOut);
		//Тестируем функции SplitterPut и SplitterGet
		for (int i(0); !bHasTimeOut.load() && bCorrectAsynchroWork; i++) {
			//Запускаем SplitterGet
			std::string strObject = strTestString + std::to_string(i);
			int nCorrectPut = cSplitterForFewClients.SplitterPut(&strObject, true, _nDelayBetweenObjectsTimeMsec);

			std::vector<std::shared_future< std::string>> vecFutures;
			//Асинхронно запускаем ожидания клиентов и оставляем 2 постоянных ждунов
			for (int j = 0; j < _nMaxClients && j < (nMaxAsynchroClients - 2); j++) {
				auto fut = std::async(std::launch::async, [&cSplitterForFewClients, j, _nDelayBetweenObjectsTimeMsec] {
					std::string strClientObject("");
					std::string* pstrClientObject = &strClientObject;
					int nCorrectGet = cSplitterForFewClients.SplitterGet(j, &pstrClientObject, _nDelayBetweenObjectsTimeMsec);
					return strClientObject;
				});
				vecFutures.push_back(fut.share());
			}

			//Проверяем что все клиенты получили объект
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

	//Проверка множественного запуска функций
	bool bCorrectWorkChaos(true);
	Splitter<std::string> cSplitterForChaos;
	//Инициализируем сплитер
	bool bSplitterForForChaosHasInitialization = cSplitterForChaos.SplitterInit(_nMaxBuffers, _nMaxClients);
	if (bSplitterForForChaosHasInitialization) {
		//Заполняем сплитер клиентами
		bool bCorrecteAddClient(true);
		std::vector<std::pair<int, bool>> vecClients(0);
		for (int i = 0; i < _nMaxClients && bCorrecteAddClient; i++) {
			int nClientId(0);
			bCorrecteAddClient = cSplitterForChaos.SplitterClientAdd(&nClientId);
			vecClients.push_back(std::pair<int, bool>(nClientId, bCorrecteAddClient));
		}
		bHasTimeOut.store(false);
		auto futureTimeOutForWorkWithSecondClientSynchroMode = std::async(std::launch::async, WaitTimeOut);
		//Хаотический запуск функций SplitterPut и SplitterGet
		for (int i(0); !bHasTimeOut.load(); i++) {
			//Запускаем SplitterGet
			std::string strObject = strTestString + std::to_string(i);
			int nCorrectPut = cSplitterForChaos.SplitterPut(&strObject, true, _nDelayBetweenObjectsTimeMsec);

			std::vector<std::shared_future< std::string>> vecFutures;
			//Асинхронно запускаем ожидания клиентов и оставляем 2 постоянных ждунов
			for (int j = 0; j < _nMaxClients && j < (nMaxAsynchroClients - 2); j++) {
				auto fut = std::async(std::launch::async, [&cSplitterForChaos, j, _nDelayBetweenObjectsTimeMsec] {
					std::string strClientObject("");
					std::string* pstrClientObject = &strClientObject;
					int nCorrectGet = cSplitterForChaos.SplitterGet(j, &pstrClientObject, _nDelayBetweenObjectsTimeMsec);
					return strClientObject;
				});
				vecFutures.push_back(fut.share());
			}
			//Дополнительно запускаем SplitterGet 2 раза
			std::string strObject2 = strTestString;
			nCorrectPut = cSplitterForChaos.SplitterPut(&strObject2, true, _nDelayBetweenObjectsTimeMsec);
			nCorrectPut = cSplitterForChaos.SplitterPut(&strObject2, true, _nDelayBetweenObjectsTimeMsec);
			//еще Асинхронно запускаем ожидания клиентов и оставляя 2 постоянных ждунов
			for (int j = 0; j < _nMaxClients && j < (nMaxAsynchroClients - 2); j++) {
				auto fut = std::async(std::launch::async, [&cSplitterForChaos, j, _nDelayBetweenObjectsTimeMsec] {
					std::string strClientObject("");
					std::string* pstrClientObject = &strClientObject;
					int nCorrectGet = cSplitterForChaos.SplitterGet(j, &pstrClientObject, _nDelayBetweenObjectsTimeMsec);
					return strClientObject;
				});
				vecFutures.push_back(fut.share());
			}

			//Проверяем что все синхропотоки завершились паралельно запуская SplitterPut
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
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

// Буффер, T - Замена для COM-объекта IUnknown, тип T должен обладать копирующим присваиванием
template <typename T> // 
class Splitter : public ISplitter<T> {
public:
	
	Splitter();
	Splitter (const Splitter&) = delete;
	Splitter (Splitter&&) = delete;
	Splitter& operator=(const Splitter&) = delete;
	Splitter& operator=(Splitter&&) = delete;
	~Splitter();
	


	// Инициализация объекта сплиттера - задаётся максимальное количество буферов в очереди, и максимальное количество выходов
	// Результат True - сплиттер инициализирован, False - сплиттер не инициализирован, так как уже инициализирован или даны некорректные входные данные
	bool	SplitterInit(IN const int _nMaxBuffers, IN const int _nMaxClients) override;

	// Информация о сплитере
	// Результат True - сплиттер инициализирован, False - сплиттер не инициализирован, входные параметры не менялись
	bool	SplitterInfoGet(OUT int* _pnMaxBuffers, OUT int* _pnMaxClients) override;

	// Кладём данные в очередь
	// Если превышено количество буферов, то ждём _nTimeOutMsec пока не освободятся буфера (клиент заберёт данные) после чего либо возвращаем ошибку,
	// либо убираем самый старый объект и добавляем новый (в зависимости от значения флага _bForceNew) 
	int 	SplitterPut(IN const T* _pObjPut, IN const bool _bForceNew, IN const int _nTimeOutMsec) override;

	// Сбрасываем все буфера, прерываем все ожидания
	int		SplitterFlush() override;

	// Добавляем нового клиента - возвращаем уникальный идентификатор клиента
	// Результат True - клиент добавлен, False - не добавлен или сплиттер не инициализирован 
	bool	SplitterClientAdd(OUT int* _pnClientID) override;

	// Удаляем клиента по идентификатору, если клиент находится в процессе ожидания буфера, то прерываем ожидание
	// Результат True - клиент уделен, False - не удален или сплиттер не инициализирован
	bool	SplitterClientRemove(IN const int _nClientID) override;

	// Количество клиентов
	// Результат True - сплиттер инициализирован, False - сплиттер не инициализирован, входные параметры не менялись
	bool	SplitterClientGetCount(OUT int* _pnCount) override;

	// Информация о клиенте по индексу - его идентификатор и количество буферов (задержка)
	// Индекс - порядковый номер от 0 до текущего количества клиентов (не включительно)
	// Результат True - клиент найден, False -клиент не найден или сплиттер не инициализирован, входные параметры не менялись
	bool	SplitterClientGetByIndex(IN int _nIndex, OUT int* _pnClientID, OUT int* _pnBuffersUsed) override;

	// Информация о клиенте по идентификатору - количество буферов (задержка)
	// Результат True - клиент найден, False -клиент не найден или сплиттер не инициализирован, входные параметры не менялись
	bool	SplitterClientGetById(IN int _nClientID, OUT int* _pnBuffersUsed) override;

	// По идентификатору клиента запрашиваем данные, если данных пока нет, то ожидаем _nTimeOutMsec пока не будут добавлены новые данные, в случае превышения времени ожидания - возвращаем ошибку
	int		SplitterGet(IN int _nClientID, OUT T** _ppObjGet, IN int _nTimeOutMsec) override;

	// Закрытие объекта сплиттера - все ожидания должны быть прерваны
	void	SplitterClose() override;
	

private:
	//Флаг инициализации сплиттера
	std::atomic_bool m_bIsInitialized;

	//Количество запущенных функций SplitterPut 
	std::atomic<unsigned int> m_uCountOfActive_SplitterPut;

	//Оповещение для функции SplitterGet для проверки условий продолжения выполнения функции
	std::condition_variable m_cvMessageForSplitterGet;
	std::mutex  m_mutex_cvMessageForSplitterGet;

	//Оповещение для функции SplitterPut для проверки условий продолжения выполнения функции
	std::condition_variable m_cvMessageForSplitterPut;
	std::mutex  m_mutex_cvMessageForSplitterPut;

	//База клиентов
	SplitterClientsBase m_cClientsBase;
	//Мьютекс для работы с базой клиентов
	std::mutex  m_mutex_cClientsBase;

	//Индекс Текущей ссылки на объект, в векторе m_vecObjects, в который необходимо проводить запись нового объекта
	std::atomic<size_t> m_uCurrentObjectIndex;
	//Максимальный Индекс объектов -размер буфера
	std::atomic<size_t> m_uMaxObjectIndex;
	
	//Вектор указателей на объекты
	std::vector<std::unique_ptr<T>> m_vecObjects;
	//Мьютекс для работы с вектором ссылок на объекты
	std::mutex  m_mutex_vecObjects;

	//Флаг необходимости прерывания SplitterPut
	std::atomic_bool m_bBreakSplitterPut;
	
	//Функция получения индекса объекта следующего после указанного (буфер циклически использует вектор)
	size_t GetNextObjectIndex(IN size_t _nCurrentIndex) const;

	//Функция получения отставания указанного индекса от текущего индекса очереди - задержка ( с учетом цикличности буфера)
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


// Инициализация объекта сплиттера - задаётся максимальное количество буферов в очереди, и максимальное количество выходов
// Результат True - сплиттер инициализирован, False - сплиттер не инициализирован, так как уже инициализирован или даны некорректные входные данные
template <typename T>
bool	Splitter<T>::SplitterInit(IN const int _nMaxBuffers, IN const int _nMaxClients)
{
	bool bResult (false);
	//Проверка того, что сплиттер еще не инициализирован
	if (!m_bIsInitialized.load() && _nMaxBuffers > 0 && _nMaxClients > 0 ) {
		m_bIsInitialized.store(true);
		m_uCountOfActive_SplitterPut.store(0);
		m_uCurrentObjectIndex.store(0);	
		//Делаем максимальный индекс объектов на 1 больше, для того чтобы иметь 1 дополнительный технический элемент - текущий, который никто не ожтижает и который будет замененет следующим поступивщим
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



// Информация о сплитере
// Результат True - сплиттер инициализирован, False - сплиттер не инициализирован, входные параметры не менялись
template <typename T>
bool	Splitter<T>::SplitterInfoGet(OUT int* _pnMaxBuffers, OUT int* _pnMaxClients)
{
	bool bResult(false);
	//Проверка инициации сплиттера
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


// Кладём данные в очередь
// Если превышено количество буферов, то ждём _nTimeOutMsec пока не освободятся буфера (клиент заберёт данные) после чего либо возвращаем ошибку,
// либо убираем самый старый объект и добавляем новый (в зависимости от значения флага _bForceNew) 
template <typename T>
int 	Splitter<T>::SplitterPut(IN const T* _pObjPut, IN const bool _bForceNew, IN const int _nTimeOutMsec)
{
	
	int nResult (0);
	if (_pObjPut != nullptr) {
		if (m_bIsInitialized.load()) {
			//Делаем преинкремент количества запуска этой функции и сохраняем в локальную переменую
			unsigned int uCount = ++m_uCountOfActive_SplitterPut;
			//Проверяем количество запусков этой функции: если не равен 1, то завершаем с ошибкой - двойной запуск,
			//Нужно именно завершать, так как второй вариант - ждать завершения первого запуска SplitterPut может привести к блокировки буфера, в случае 
			//когда новые экземпляры SplitterPut будет запускаться до завершения предыдущих, и будут работать только SplitterPut, не давая возможности запуститься другим функция класса
			if (1 == uCount) {
				//Получаем текущий индекс буффера
				size_t uCurrentObjectIndex = m_uCurrentObjectIndex.load();
				size_t uNextCurrentObjectIndex = this->GetNextObjectIndex(uCurrentObjectIndex);
				//Флаг наличия клиентов с индексом равным следующим за текущим, т.е. те, которые имеют максимально возможную задержку
				bool bHasClientsWithCurrentObjectIndex(true);
				//Проверяем наличие клиентов с индексом равным следующим за текущим
				{
					m_mutex_cClientsBase.lock();
					bHasClientsWithCurrentObjectIndex = m_cClientsBase.CheckCurrentObjectIndex(uNextCurrentObjectIndex);					
					m_mutex_cClientsBase.unlock();
				}
				//Ожидание с таймаутом _nTimeOutMsec если есть клиенты с текущим индексом, т.е. те, которые еще не забрали подлежащий удалению объект
				//Ожидаем, что клиенты успеют забрать объект
				if (bHasClientsWithCurrentObjectIndex) {
					auto timeOut = std::chrono::system_clock::now() + milliseconds{ _nTimeOutMsec };
					//Флаг наступления таймаута
					std::atomic_bool bHasTimeOut(false);
					//Ассинхронно запускаем установку флага таймаута
					auto futureTimeOut = std::async(std::launch::async, [&bHasTimeOut, _nTimeOutMsec] {
						std::this_thread::sleep_for(milliseconds{ _nTimeOutMsec });
						bHasTimeOut.store(true);
					});
					//Проверяем успели ли клиенты забрать последний объект до наступления таймаута
					//Если наструпил таймаут, то флаг bHasClientsWithCurrentObjectIndex остается True
					//Реагируем на каждой событие m_cvMessageForSplitterPut, отправляемое из функции SplitterGet
					while (bHasClientsWithCurrentObjectIndex && !bHasTimeOut.load()) {
						std::unique_lock<std::mutex> lkMessage(m_mutex_cvMessageForSplitterPut);
						m_cvMessageForSplitterPut.wait_until(lkMessage, timeOut);
						//Проверяем условия продолжения выполнения функции - все клиенты успели забрать последний объект
						m_mutex_cClientsBase.lock();
						bHasClientsWithCurrentObjectIndex = m_cClientsBase.CheckCurrentObjectIndex(uNextCurrentObjectIndex);
						m_mutex_cClientsBase.unlock();
					}
				}
				//Если спрлиттер по прежнему активен, а также нет клиентов с самым старым индексом объекта или установлен флаг безусловной замены самого старого объекта, то заменяем объект
				if (!bHasClientsWithCurrentObjectIndex || _bForceNew) {
					std::unique_ptr<T> upObject(new T);
					*upObject = *_pObjPut;
					//Две ветки: если нет клиентов со устаревшим индексом и если есть
					if (!bHasClientsWithCurrentObjectIndex) {
						m_mutex_vecObjects.lock();
						//Дополнительно проверяем флаг прерывания этой функции, и если он true, то не сохраняем объект, а флаг сбрасываем
						if (!m_bBreakSplitterPut.load()) {
							//Записываем объект в буфер
							m_vecObjects.at(uCurrentObjectIndex) = std::move(upObject);
							m_uCurrentObjectIndex.store(uNextCurrentObjectIndex);
						}
						else
						{
							m_bBreakSplitterPut.store(false);
							//Устанавливаем ошибку - Функция прервана в связи с отчиской буфера
							nResult = 5;
						}
						m_mutex_vecObjects.unlock();
					}
					else {
						//Захват m_vecObjects и m_uCurrentObjectIndex для единоличной работы с объектами
						std::unique_lock <std::mutex> lkClientsBase{ m_mutex_cClientsBase, std::defer_lock };
						std::unique_lock <std::mutex> lkVecObject{ m_mutex_vecObjects, std::defer_lock };
						std::lock(lkClientsBase, lkVecObject);
						//Дополнительно проверяем флаг прерывания этой функции, и если он true, то не сохраняем объект, а флаг сбрасываем
						if (!m_bBreakSplitterPut.load()) {
							//Записываем объект в буфер
							m_vecObjects.at(uCurrentObjectIndex) = std::move(upObject);
							m_uCurrentObjectIndex.store(uNextCurrentObjectIndex);
							//Двигаем индексы всех клиентов, которые не смогли забрать объект
							m_cClientsBase.ChangeCurrentObjectIndex(uNextCurrentObjectIndex, this->GetNextObjectIndex(uNextCurrentObjectIndex));
						}
						else
						{
							m_bBreakSplitterPut.store(false);
							//Устанавливаем ошибку - Функция прервана в связи с отчиской буфера
							nResult = 5;
						}
					}
					//Отправляем сообщение о завершении работы функции SplitterPut в функции SplitterGet, ожидающие нового объекта.
					m_cvMessageForSplitterGet.notify_all();
				}
				else {
					//Устанавливаем ошибку - превышен таймаут выполнения функции
					nResult = 3;
				}
			}
			else {
				//Устанавливаем ошибку - функция запущена более одного раза
				nResult = 2;
			}
			//Уменьшаем количество запусков этой функции
			m_uCountOfActive_SplitterPut--;
		}
		else {
			//Устанавливаем ошибку - сплиттер не инициализирован
			nResult = 1;
		}
	}
	else {
		//Устанавливаем ошибку - Указатель _pObjPut не инициализирован
		nResult = 4;
	}
	return nResult;
};


// Сбрасываем все буфера, прерываем все ожидания
template <typename T>
int		Splitter<T>::SplitterFlush()
{
	int nResult(0);
	if (m_bIsInitialized.load()) {
		//Захват m_vecObjects и m_uCurrentObjectIndex для единоличной работы с объектами
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
		//Устанавливаем ошибку - сплиттер не инициализирован
		nResult = 1;
	}
	//Функции SplitterGet завершаться после отправки сообщения, так как у всех клиентов текущий индекс будет равен индексу буфера 
	m_cvMessageForSplitterGet.notify_all();
	// Функция SplitterPut завершается из-за 
	m_cvMessageForSplitterPut.notify_all();
	return nResult;
};


// Добавляем нового клиента - возвращаем уникальный идентификатор клиента
// Результат True - клиент добавлен, False - не добавлен или сплиттер не инициализирован
template <typename T>
bool	Splitter<T>::SplitterClientAdd(OUT int* _pnClientID)
{
	bool bResult(false);
	size_t uClientID(0);
	//Проверка инициации сплиттера	
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


// Удаляем клиента по идентификатору, если клиент находится в процессе ожидания буфера, то прерываем ожидание
// Результат True - клиент уделен, False - не удален  или сплиттер не инициализирован
template <typename T>
bool	Splitter<T>::SplitterClientRemove(IN const int _nClientID)
{
	bool bResult(false);
	//Проверка инициации сплиттера
	if (m_bIsInitialized.load()){
		if (_nClientID >= 0){
			size_t uClientID =  static_cast<size_t>(_nClientID);
			m_mutex_cClientsBase.lock();
			bResult = m_cClientsBase.RemoveClient(uClientID);
			m_mutex_cClientsBase.unlock();
			if (bResult){
				//Оправка оповещения все функциям SplitterGet
				//Если для данного клиента есть такая функция в режиме ожидания нового объекта, то она увидит что клиент отключен и завершит свою работу
				m_cvMessageForSplitterGet.notify_all();
			}
		}
	}
	return bResult;
};


// Количество клиентов
// Результат True - сплиттер инициализирован, False - сплиттер не инициализирован, входные параметры не менялись
template <typename T>
bool	Splitter<T>::SplitterClientGetCount(OUT int* _pnCount)
{
	bool bResult(false);
	//Проверка инициации сплиттера
	if (m_bIsInitialized.load()) {
		m_mutex_cClientsBase.lock();
		*_pnCount = m_cClientsBase.GetCurrentClients();
		m_mutex_cClientsBase.unlock();
		bResult = true;
	}
	return bResult;
};


// Информация о клиенте по индексу - его идентификатор и количество буферов (задержка)
// Индекс - порядковый номер от 0 до текущего количества клиентов (не включительно)
// Результат True - клиент найден, False -клиент не найден или сплиттер не инициализирован, входные параметры не менялись
template <typename T>
bool	Splitter<T>::SplitterClientGetByIndex(IN int _nIndex, OUT int* _pnClientID, OUT int* _pnBuffersUsed)
{
	bool bResult(false);
	//Проверка инициации сплиттера
	if (m_bIsInitialized.load()) {
		size_t uClientID(0);
		size_t uCurrentObjectIndex(0);
		int nCurrentIndex(0);
		m_mutex_cClientsBase.lock();
		size_t uMaxClients = m_cClientsBase.GetMaxClients();
		//считаем всех активных клиентов пока не доберемся до _nIndex активного клиента
		while (uClientID < uMaxClients && nCurrentIndex <= _nIndex) {
			// Проверям наличие информации о клиенте и считываем данные
			if (m_cClientsBase.GetClientCurrentIndex(uClientID, &uCurrentObjectIndex)) {
				//Если информация есть - увеличиваем счетчик активных клиентов
				nCurrentIndex++;
			}
			// Если nCurrentIndex стал превышать _nIndex, то найден необходимый клиент, и не нужно увеличивать uClientID
			if (nCurrentIndex <= _nIndex) {
				uClientID++;
			}
		}
		m_mutex_cClientsBase.unlock();
		//Если nCurrentIndex == _nIndex+1 - найден нужный клиент
		if (nCurrentIndex == _nIndex + 1) {
			*_pnClientID = static_cast<int>(uClientID);
			*_pnBuffersUsed = static_cast<int>(GetCurrentDelay(uCurrentObjectIndex));
			bResult = true;
		}
	}
	return bResult;
};


// Информация о клиенте по идентификатору - количество буферов (задержка)
// Результат True - клиент найден, False -клиент не найден или сплиттер не инициализирован, входные параметры не менялись
template <typename T>
bool	Splitter<T>::SplitterClientGetById(IN int _nClientID, OUT int* _pnBuffersUsed)
{
	bool bResult(false);
	//Проверка инициации сплиттера
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


// По идентификатору клиента запрашиваем данные, если данных пока нет, то ожидаем _nTimeOutMsec пока не будут добавлены новые данные, в случае превышения времени ожидания - возвращаем ошибку
template <typename T>
int		Splitter<T>::SplitterGet(IN int _nClientID, OUT T** _ppObjGet, IN int _nTimeOutMsec)
{
	//Нет проверки количества запущенных SplitterGet для одного клиента, считаем что все запросы для одного клиента исходят из одного потока
	
	int nResult(0);
	if (_ppObjGet != nullptr && *_ppObjGet != nullptr){
		if (m_bIsInitialized.load()) {
			//Получаем текущий индекс клиента и флаг его активности
			size_t uClientCurrentObjectIndex(0);
			bool bIsActiveClient(false);
			m_mutex_cClientsBase.lock();
			bIsActiveClient = m_cClientsBase.GetClientCurrentIndex(_nClientID, &uClientCurrentObjectIndex);
			m_mutex_cClientsBase.unlock();
			//Проверяем активность клиента, если он не активен то устанавливаем ошибку и завершаем функцию
			if (bIsActiveClient) {
				//Проверяем текущий индекс клиента по сравнению с текущим индексом буфера
				//Если текущий индекс клиента равен текущиму индексу буфера и необходимо дождаться новых данных
				if (uClientCurrentObjectIndex == m_uCurrentObjectIndex.load()) {
					auto timeOut = std::chrono::system_clock::now() + milliseconds{ _nTimeOutMsec };
					//Флаг наступления таймаута
					std::atomic_bool bHasTimeOut(false);
					//Ассинхронно запускаем установку флага таймаута
					auto futureTimeOut = std::async(std::launch::async, [&bHasTimeOut, _nTimeOutMsec] {
						std::this_thread::sleep_for(milliseconds{ _nTimeOutMsec });
						bHasTimeOut.store(true);
					});
					//Проверяем появился ли новый объект до наступления таймаута
					//Реагируем на каждой событие m_cvMessageForSplitterGet, которые
					//Приходят из функции SplitterPut, для оповещения о появлении нового объекта,
					//Приходят из функции SplitterClientRemove, для оповещения что один из клиентов удален
					//Приходят из функций SplitterFlush, для оповещения что необходимо прервать ожидания у всех клиентов
					//Приходят из функций SplitterClose, для оповещения что необходимо прервать ожидания у всех клиентов
					while (!bHasTimeOut.load() && bIsActiveClient && uClientCurrentObjectIndex != m_uCurrentObjectIndex.load()) {
						std::unique_lock<std::mutex> lkMessage(m_mutex_cvMessageForSplitterGet);
						m_cvMessageForSplitterGet.wait_until(lkMessage, timeOut);
						m_mutex_cClientsBase.lock();
						//Обновляем флаг активность клиента и его текущий индекс
						bIsActiveClient = m_cClientsBase.GetClientCurrentIndex(_nClientID, &uClientCurrentObjectIndex);
						m_mutex_cClientsBase.unlock();
					}
				}
				//Критическая секция работы с m_vecObjects и m_uCurrentObjectIndex
				{
					//Захват m_vecObjects и m_uCurrentObjectIndex для единоличной работы с объектами
					std::unique_lock <std::mutex> lkClientsBase{ m_mutex_cClientsBase, std::defer_lock };
					std::unique_lock <std::mutex> lkVecObject{ m_mutex_vecObjects, std::defer_lock };
					std::lock(lkClientsBase, lkVecObject);
					//Обновляем флаг активность клиента и его текущий индекс
					bIsActiveClient = m_cClientsBase.GetClientCurrentIndex(_nClientID, &uClientCurrentObjectIndex);
					//Повторная проверка активности клиента, для случая если он мог измениться в течении времени ожидания нового объекта
					if (bIsActiveClient) {
						//Проверяем что текущий индекс клиента не равен текущему индексу буфера
						//Если равен, то значит за указанное время ожидания _nTimeOutMsec , новый объект не появился
						if (uClientCurrentObjectIndex != m_uCurrentObjectIndex.load()) {
							//Забираем объект
							**_ppObjGet = *(m_vecObjects.at(uClientCurrentObjectIndex));
							//Jбновляем текущий индекс для клиента
							m_cClientsBase.SetClientCurrentIndex(_nClientID, this->GetNextObjectIndex(uClientCurrentObjectIndex));
							//Отправить оповещение в SplitterPut, о том что забрали объект, для того чтобы SplitterGet проверил освобождение что больше никакие клиенты не ожидают удаляемый объект
							m_cvMessageForSplitterPut.notify_one();
						}
						else {
							//Устанавливаем ошибку - Превышено время ожидания новых данных
							nResult = 3;
						}
					}
					else {
						//Устанавливаем ошибку - Клиента с указанным Id не найдено 
						nResult = 2;
					}
				}
			}
			else {
				//Устанавливаем ошибку - Клиента с указанным Id не найдено 
				nResult = 2;
			}
		}
		else {
			//Устанавливаем ошибку - сплиттер не инициализирован
			nResult = 1;
		}
	}
	else {
		//Устанавливаем ошибку - Указатель на указатель _ppObjGet не инициализирован
		nResult = 4;
	}
	return nResult;
};



// Закрытие объекта сплиттера - все ожидания должны быть прерваны
template <typename T>
void	Splitter<T>::SplitterClose()
{
	if (m_bIsInitialized.load()) {
		//Захват m_vecObjects и m_uCurrentObjectIndex для единоличной работы с объектами
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
	//Функции SplitterGet завершаться после отправки сообщения, так как у всех клиентов текущий индекс будет равен индексу буфера 
	m_cvMessageForSplitterGet.notify_all();
	// Функция SplitterPut завершается из-за 
	m_cvMessageForSplitterPut.notify_all();
};


//Функция получения индекса объекта следующего после указанного (буфер циклически использует вектор)
template <typename T>
size_t Splitter<T>::GetNextObjectIndex(IN size_t _nCurrentIndex) const
{
	size_t nResult(0);
	//Проверка инициации сплиттера
	if (m_bIsInitialized.load()) {
		nResult = (++_nCurrentIndex) % m_uMaxObjectIndex.load();
	}
	return nResult;
	
}


//Функция получения отставания указанного индекса от текущего индекса очереди - задержка ( с учетом цикличности буфера)
template <typename T>
size_t Splitter<T>::GetCurrentDelay(IN size_t _nCurrentIndex) const 
{
	size_t nResult(0);
	//Проверка инициации сплиттера
	if (m_bIsInitialized.load()) {
		const size_t uMaxObjectIndex = static_cast<size_t> (m_uMaxObjectIndex.load());
		if (0 < uMaxObjectIndex) {
			nResult = (uMaxObjectIndex + static_cast<size_t>(m_uCurrentObjectIndex.load()) - _nCurrentIndex) % uMaxObjectIndex;
		}		
	}
	return nResult;
};
#endif SPLITTER_H
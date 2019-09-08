#include "pch.h"

#include <vector>
#include <iostream>
#include <limits>
#include <string>

#include "splitterclientbase.h"



//Функции класса SplitterClientInfo


SplitterClientInfo::SplitterClientInfo() 
	: m_uClientId(0)
	, m_uCurrentObjectIndex(0)
	, m_bIsActive(false) 
{};

SplitterClientInfo::SplitterClientInfo( IN size_t _nClientId, IN  size_t _nCurrentObjectIndex, IN  bool _bIsActive)
	: m_uClientId(_nClientId)
	, m_uCurrentObjectIndex(_nCurrentObjectIndex)
	, m_bIsActive(_bIsActive) 
{};

SplitterClientInfo::~SplitterClientInfo() 
{};
	
size_t SplitterClientInfo::GetClientId() const { return m_uClientId; };
size_t SplitterClientInfo::GetCurrentObjectIndex() const { return m_uCurrentObjectIndex; };
bool SplitterClientInfo::GetActivity() const { return m_bIsActive; };


void SplitterClientInfo::SetClientId ( IN const size_t _uClientId) { m_uClientId = _uClientId; };
void SplitterClientInfo::SetCurrentObjectIndex ( IN const size_t _uCurrentObjectIndex) { m_uCurrentObjectIndex = _uCurrentObjectIndex; };
void SplitterClientInfo::SetActivity ( IN const bool _bIsActive) { m_bIsActive = _bIsActive; };

//Скопировать информацию о клиенте
void SplitterClientInfo::CopyInfo ( IN const SplitterClientInfo& _clientInfo)
{
	m_uClientId = _clientInfo.GetClientId();
	m_uCurrentObjectIndex = _clientInfo.GetCurrentObjectIndex();
	m_bIsActive = _clientInfo.GetActivity();
}

//Обнулить информацию  о клиенте
void SplitterClientInfo::ClearInfo() 
{
	m_uClientId = 0;
	m_uCurrentObjectIndex = 0;
	m_bIsActive = false;
}

//Переместить информацию о клиенте 
void SplitterClientInfo::MoveInfo( OUT SplitterClientInfo& _clientInfo) 
{
	this->CopyInfo(_clientInfo);
	_clientInfo.ClearInfo();
}






//Функции класса SplitterClientsBase 

SplitterClientsBase::SplitterClientsBase()
	:m_vecClientInfo(0),
	m_uMaxClients(0)
{};



SplitterClientsBase::SplitterClientsBase(IN const size_t _uMaxClients)
	:m_vecClientInfo(0),
	m_uMaxClients (_uMaxClients)
{};



SplitterClientsBase::~SplitterClientsBase() 
{};


//Максимальное количество клиентов
size_t SplitterClientsBase::GetMaxClients() const
{
	return m_uMaxClients;
};


//Установить максимальное количество клиентов.
//Результат: True - выполнено; False - ошибка, текущее количество клиентов больше чем новое максимальное количество клиентов
bool SplitterClientsBase::SetMaxClients(IN const size_t _uMaxClients)
{
	bool bResult = true;
	if (m_vecClientInfo.size() <= _uMaxClients) {
		m_uMaxClients = _uMaxClients;
	}
	else {
		bResult = false;
	}
	return bResult;
};

//Текущее количество клиентов активных и не активных
size_t SplitterClientsBase::GetCurrentClients() const
{
	size_t nCount(0);
	for (auto& clientInfo : m_vecClientInfo) {
		if (clientInfo.GetActivity()) {
			nCount++;
		}
	}
	return nCount;
};

//Добавление нового клиента. Результат: True - клиент добавлен; False - клиент не добавлен 
bool SplitterClientsBase::AddClient(OUT size_t *const _uClientID, IN  size_t _uCurrentObjectIndex)
{
	bool bResult = false;
	//добавляем клиента в существующую неактивную запись и активируем её
	for (auto& client : m_vecClientInfo) {
		if (!client.GetActivity()) {
			client.SetCurrentObjectIndex(_uCurrentObjectIndex);
			client.SetActivity(true);
			*_uClientID = client.GetClientId();
			bResult = true;
		}
	}
	//добавляем нового клиента если все записи активныеи не превышен лимит. SplitterClientInfo::m_uClientID == индексу записи в векторе
	if (!bResult && m_vecClientInfo.size() < m_uMaxClients) {
		*_uClientID = m_vecClientInfo.size();
		m_vecClientInfo.push_back(SplitterClientInfo(*_uClientID, _uCurrentObjectIndex, true));
		bResult = true;
	}	
	return bResult;
};



//Удаление клиента (дизактивация записи). Результат: True - клиент удален; False - клиента с таким Id не существует или он уже дизактивирован
bool SplitterClientsBase::RemoveClient(IN const size_t _uClientID)
{
	bool bResult = false;
	//Считаем что SplitterClientInfo::m_uClientID == индексу записи в векторе
	if (m_vecClientInfo.size() > _uClientID && m_vecClientInfo.at(_uClientID).GetActivity()){
		// ClientID у записи не очищаем, для возможной последующей активации
		m_vecClientInfo.at(_uClientID).SetActivity(false);
		m_vecClientInfo.at(_uClientID).SetCurrentObjectIndex(0);
		bResult = true;
	}
	return bResult;
};


//Информация о клиенте по идентификатору. Результат: True - информация найдена; False - клиента с таким Id не существует или он уже дизактивирован
bool SplitterClientsBase::GetClientCurrentIndex(IN const size_t _uClientID, OUT size_t *const _uCurrentObjectIndex) const
{
	bool bResult = false;
	//Считаем что SplitterClientInfo::m_uClientID == индексу записи в векторе
	if (m_vecClientInfo.size() > _uClientID && m_vecClientInfo.at(_uClientID).GetActivity())	{
		*_uCurrentObjectIndex = m_vecClientInfo.at(_uClientID).GetCurrentObjectIndex();
		bResult = true;
	}	
	return bResult;
};


//Изменение текущего индекса буфера для клиента. Результат: True - индекс изменен; False -  клиента с таким Id не существует или он уже дизактивирован
bool SplitterClientsBase::SetClientCurrentIndex(IN const size_t _uClientID, IN const size_t _uCurrentObjectIndex)
{
	bool bResult = false;
	//Считаем что SplitterClientInfo::m_uClientID == индексу записи в векторе
	if (m_vecClientInfo.size() > _uClientID && m_vecClientInfo.at(_uClientID).GetActivity()){
		m_vecClientInfo.at(_uClientID).SetCurrentObjectIndex(_uCurrentObjectIndex);
		bResult = true;
	}
	return bResult;
};


//Проверка текущего индекса буфера для всех активных клиентов на равернсво _uCurrentIndex 
//Результат: True - равенства есть, False - равенств нет
bool SplitterClientsBase::CheckCurrentObjectIndex (IN const size_t _uCurrentIndex)
{
	bool bResult = false;
	for (std::size_t i = 0; i < m_vecClientInfo.size(); i++ ) {
		if (m_vecClientInfo.at(i).GetActivity() && _uCurrentIndex == m_vecClientInfo.at(i).GetCurrentObjectIndex()) {
			bResult = true;
		}
	}
	return bResult;
};


//Замена текущего индекса буфера для всех активных клиентов на _uNextCurrentIndex, если он равен _uCurrentIndex 
//Результат: True - замены произведены, False - замен нет
bool SplitterClientsBase::ChangeCurrentObjectIndex (IN const size_t _uCurrentIndex, IN const size_t _uNextCurrentIndex)
{
	bool bResult = false;
	for (std::size_t i = 0; i < m_vecClientInfo.size(); i++) {
		if (m_vecClientInfo.at(i).GetActivity() && _uCurrentIndex == m_vecClientInfo.at(i).GetCurrentObjectIndex()) {
			m_vecClientInfo.at(i).SetCurrentObjectIndex(_uNextCurrentIndex);
		bResult = true;
		}
	}
	return bResult;
};
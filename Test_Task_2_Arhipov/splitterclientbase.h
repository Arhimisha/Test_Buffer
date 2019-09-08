#include "pch.h"
#include <vector>

#ifndef SPLITTERCLIENTBASE_H
#define SPLITTERCLIENTBASE_H
//Класс для хранения информации о клиенте сплиттера
class SplitterClientInfo {
public:

	SplitterClientInfo();
	SplitterClientInfo( IN size_t _uClientId, IN  size_t _uCurrentObjectIndex, IN  bool _bIsActive);
	~SplitterClientInfo();

	SplitterClientInfo(const SplitterClientInfo&) = default;
	SplitterClientInfo(SplitterClientInfo&&) = default;
	SplitterClientInfo& operator=(const SplitterClientInfo&) = default;
	SplitterClientInfo& operator=(SplitterClientInfo&&) = default;

	size_t GetClientId() const;
	size_t GetCurrentObjectIndex() const;
	bool GetActivity() const;


	void SetClientId( IN const size_t _uClientId);
	void SetCurrentObjectIndex( IN const size_t _uCurrentObjectIndex);
	void SetActivity( IN const bool _bIsActive);

	//Скопировать информацию о клиенте
	void CopyInfo( IN const SplitterClientInfo& _clientInfo);

	//Обнулить информацию  о клиенте
	void ClearInfo();

	//Переместить информацию о клиенте
	void MoveInfo( OUT SplitterClientInfo& _clientInfo);
	
private:
	//Id клиента
	size_t m_uClientId;
	//Текущее индекс буфера
	size_t m_uCurrentObjectIndex;
	//Флаг активности клиента
	bool m_bIsActive;

};


//Класс для управления информацией о клиентах сплиттера
class SplitterClientsBase {
public:
	SplitterClientsBase();
	SplitterClientsBase(IN const size_t _uMaxClients);
	~SplitterClientsBase();

	SplitterClientsBase(const SplitterClientsBase&) = delete;
	SplitterClientsBase(SplitterClientsBase&&) = delete;
	SplitterClientsBase& operator=(const SplitterClientsBase&) = delete;
	SplitterClientsBase& operator=(SplitterClientsBase&&) = delete;

	//Максимальное количество клиентов
	size_t GetMaxClients() const;

	//Установить максимальное количество клиентов.
	//Результат: True - выполнено; False - ошибка, текущее количество клиентов больше чем новое максимальное количество клиентов
	bool SetMaxClients(IN const size_t _uMaxClients);

	//Текущее количество клиентов
	size_t GetCurrentClients() const;

	//Добавление нового клиента. Результат: True - клиент добавлен; False - клиент не добавлен 
	bool AddClient(OUT size_t *const _uClientID, IN size_t _uCurrentObjectIndex);

	//Удаление клиента (дизактивация записи). Результат: True - клиент удален; False - клиента с таким Id не существует или он уже дизактивирован
	bool RemoveClient(IN const size_t _uClientID);

	//Информация о клиенте по идентификатору. Результат: True - информация найдена; False - клиента с таким Id не существует или он уже дизактивирован
	bool GetClientCurrentIndex(IN const size_t _uClientID, OUT size_t *const _uCurrentObjectIndex) const;

	//Изменение текущего индекса буфера для клиента. Результат: True - индекс изменен; False -  клиента с таким Id не существует или он уже дизактивирован
	bool SetClientCurrentIndex(IN const size_t _uClientID, IN const size_t _uCurrentObjectIndex);

	//Проверка текущего индекса буфера для всех активных клиентов на равернсво _uCurrentIndex 
	//Результат: True - равенства есть, False - равенств нет
	bool CheckCurrentObjectIndex(IN const size_t _uCurrentIndex);

	//Замена текущего индекса буфера для всех активных клиентов на _uNextCurrentIndex, если он равен _uCurrentIndex 
	//Результат: True - замены произведены, False - замен нет
	bool ChangeCurrentObjectIndex(IN const size_t _uCurrentIndex, IN const size_t _uNextCurrentIndex);


private:
	//Максимальное количество клиентов
	std::size_t m_uMaxClients;
	//Контейнер с клиентами
	std::vector<SplitterClientInfo> m_vecClientInfo;

	
};



#endif SPLITTERCLIENTBASE_H
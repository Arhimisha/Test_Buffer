#include "pch.h"
#include <memory>

#ifndef ISPLITTER_H
#define ISPLITTER_H
  


//Интерфейс  ISplitter, COM-объект IUnknown заменен на T
template <typename T>  
class ISplitter {
public:
	// Инициализация объекта сплиттера - задаётся максимальное количество буферов в очереди, и максимальное количество выходов
	virtual bool	SplitterInit (IN const int _nMaxBuffers, IN const int _nMaxClients) = 0;

	// Информация о сплитере
	virtual bool	SplitterInfoGet (OUT int* _pnMaxBuffers, OUT int* _pnMaxClients) = 0;

	// Кладём данные в очередь
	// Если превышено количество буферов, то ждём _nTimeOutMsec пока не освободятся буфера (клиент заберёт данные) после чего либо возвращаем ошибку,
	// либо убираем самый старый объект и добавляем новый (в зависимости от значения флага _bForceNew) 
	virtual int 	SplitterPut (IN const T* _pObjPut, IN const bool _bForceNew, IN const int _nTimeOutMsec) = 0;

	// Сбрасываем все буфера, прерываем все ожидания
	virtual int		SplitterFlush() = 0;

	// Добавляем нового клиента - возвращаем уникальный идентификатор клиента
	virtual bool	SplitterClientAdd (OUT int* _pnClientID) = 0;

	// Удаляем клиента по идентификатору, если клиент находится в процессе ожидания буфера, то прерываем ожидание
	virtual bool	SplitterClientRemove (IN const int _nClientID) = 0;

	// Количество клиентов
	virtual bool	SplitterClientGetCount (OUT int* _pnCount) = 0;

	// Информация о клиенте по индексу - его идентификатор и количество буферов (задержка)
	virtual bool	SplitterClientGetByIndex (IN  int _nIndex, OUT int* _pnClientID, OUT int* _pnBuffersUsed) = 0;

	// Информация о клиенте по идентификатору - количество буферов (задержка)
	virtual bool	SplitterClientGetById ( IN int _nClientID, OUT int* _pnBuffersUsed) = 0;

	// По идентификатору клиента запрашиваем данные, если данных пока нет, то ожидаем _nTimeOutMsec пока не будут добавлены новые данные, в случае превышения времени ожидания - возвращаем ошибку
	virtual int		SplitterGet (IN int _nClientID, OUT T** _ppObjGet, IN int _nTimeOutMsec) = 0;

	// Закрытие объекта сплиттера - все ожидания должны быть прерваны
	virtual void	SplitterClose() = 0;

	// Деструктор
	virtual ~ISplitter() {};
};

#endif ISPLITTER_H
#include "pch.h"
#ifndef TEST_H
#define TEST_H



// Проверка на равенстов информации о клиенте информации о клиенте
bool EqualClientInfo( IN const SplitterClientInfo& _cClientInfo, IN const size_t _uClientId, IN const size_t _uCurrentObjectIndex, IN const bool _bIsActive);

//Печать проваленного теста
void PrintFailResult(IN const std::string _strHeader,IN const bool _bRightResult = false);

//Тестирование создания экземпляров SplitterClientsBase и тестирование GetMaxClients() и GetCurrentClients()
bool CorrectCreate_SplitterClientsBase(IN const size_t _uMaxClients);

//Тестирование функции bool SetMaxClients(IN const size_t _uMaxClients);
bool CorrectWork_SplitterClientsBase_SetMaxClients(IN const size_t _uClientID);

//Тестирование функции bool SplitterClientsBase::GetClientCurrentIndex(IN const size_t _uClientID, OUT size_t *const _uCurrentObjectIndex)
bool CorrectWork_SplitterClientsBase_GetClientCurrentIndex(IN const size_t _uClientID);

//Тестирование Функции bool SplitterClientsBase::AddClient(OUT size_t *const unClientID, IN  size_t _uCurrentObjectIndex)
bool CorrectWork_SplitterClientsBase_AddClient(IN const size_t _uMaxClients);


//Тестирование Функции bool SplitterClientsBase::RemoveClient(OUT size_t *const _uClientID, IN  size_t _uCurrentObjectIndex)
bool CorrectWork_SplitterClientsBase_RemoveClient(IN const size_t _uMaxClients);

//Тестирование Функции bool SplitterClientsBase::SetClientCurrentIndex(IN const size_t _uClientID, IN const size_t _uCurrentObjectIndex) 
bool CorrectWork_SplitterClientsBase_SetClientCurrentIndex(IN const size_t _uMaxClients);

//Тестирование Функции bool SplitterClientsBase::CheckCurrentObjectIndex(IN const size_t _uCurrentIndex);
bool CorrectWork_SplitterClientsBase_CheckCurrentObjectIndex(IN const size_t _uMaxClients);

//Тестирование Функции bool SplitterClientsBase::ChangeCurrentObjectIndex(IN const size_t _uCurrentIndex, IN const size_t _uNextCurrentIndex);
bool CorrectWork_SplitterClientsBase_ChangeCurrentObjectIndex(IN const size_t _uMaxClients);


//Тестирование создания экземпляра класса Splitter
bool CorrectCreate_Splitter(IN const int _nMaxBuffers, IN const int _nMaxClients);

//Тестирование добавления новых клиентов, их удаления и получения о них информации и их общего количества в классе Splitter
bool CorrectWork_Splitter_WorkWithClients(IN const int _nMaxBuffers, IN const int _nMaxClients);


//Тестирование функций SplitterPut и SplitterGet
//_nWorkTimeMsec - время тестирования сплитера для под-тестов, _nDelayBetweenObjectsTimeMsec - задержка междупоступлением новых данных
bool CorrectWork_Splitter_GetAndPut(IN const int _nMaxBuffers, IN const int _nMaxClients, IN const int _nWorkTimeMsec, IN const int _nDelayBetweenObjectsTimeMsec);

#endif TEST_H

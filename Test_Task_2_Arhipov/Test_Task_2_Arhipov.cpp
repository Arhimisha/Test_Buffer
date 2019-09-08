// Test_Task_2_Arhipov.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"

#include <iostream>
#include <locale>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <fstream>

#include "splitter.h"
#include "test.h"

/*
class Item
{
public:
	Item() { std::cout << "Item " << m_nNomber <<" acquired\n"; }
	Item(int _nNomber): m_nNomber(_nNomber) { std::cout << "Item " << m_nNomber << " acquired\n"; }
	~Item() { std::cout << "Item " << m_nNomber << " destroyed\n"; }
private:
	int m_nNomber = 0;
};
*/

int main()
{
	setlocale(LC_ALL, "rus");
    
		
	//Start test for class SplitterClientInfo
	SplitterClientInfo clientInfo1;
	std::cout << "Create (SplitterClientInfo) Client 1";
	PrintFailResult("", EqualClientInfo(clientInfo1, 0, 0, false));
	SplitterClientInfo clientInfo2(2, 2, true);
	std::cout << "Create (SplitterClientInfo) Client 2";
	PrintFailResult("", EqualClientInfo(clientInfo2, 2, 2, true));
	SplitterClientInfo clientInfo3(3, 3, false);
	std::cout << "Create (SplitterClientInfo) Client 3";
	PrintFailResult("", EqualClientInfo(clientInfo3, 3, 3, false));
	
	clientInfo1.CopyInfo(clientInfo2);
	std::cout << "Execute SplitterClientInfo::CopyInfo() from Client 2 to Client 1";
	PrintFailResult("", EqualClientInfo(clientInfo1, 2, 2, true));

	clientInfo1.ClearInfo();
	std::cout << "Execute SplitterClientInfo::ClearInfo() Client 1";
	PrintFailResult("", EqualClientInfo(clientInfo1, 0, 0, false));

	clientInfo2.MoveInfo(clientInfo3);
	std::cout << "Execute SplitterClientInfo::MoveInfo() from Client 3 to Client 2";
	PrintFailResult("", EqualClientInfo(clientInfo2, 3, 3, false) && EqualClientInfo(clientInfo3, 0, 0, false));


	//Start test for class SplitterClientsBase	
	std::cout << "Create Default (SplitterClientsBase) Client Base";
	bool bAreAllTestsCorrect(true);
	
	SplitterClientsBase cClientBase;
	bAreAllTestsCorrect = (cClientBase.GetMaxClients() == 0 && cClientBase.GetCurrentClients() == 00);
	PrintFailResult("", bAreAllTestsCorrect);
	
	
	std::cout << "Create diferent (SplitterClientsBase) Client Base";
	bool bCorrectCreate = true;
	for (size_t i = 0; i <= 1000 && bCorrectCreate; i++) {
		bCorrectCreate = CorrectCreate_SplitterClientsBase(i);
	}
	PrintFailResult("", bCorrectCreate);
	bAreAllTestsCorrect = bAreAllTestsCorrect && bCorrectCreate;

	std::cout << "Test SplitterClientsBase::SetMaxClient";
	bool bCorrectSetMaxClients = true;
	for (size_t i = 0; i <= 100 && bCorrectSetMaxClients; i++) {
		bCorrectSetMaxClients = CorrectWork_SplitterClientsBase_SetMaxClients(i);
	};
	PrintFailResult("", bCorrectSetMaxClients);
	bAreAllTestsCorrect = bAreAllTestsCorrect && bCorrectSetMaxClients;


	std::cout << "Test SplitterClientsBase::GetClientInfo";
	bool bCorrectGetClientInfo = true;
	for (size_t i = 0; i <= 1000 && bCorrectGetClientInfo; i++) {
		bCorrectGetClientInfo = CorrectWork_SplitterClientsBase_GetClientCurrentIndex(i);
	}
	PrintFailResult("", bCorrectGetClientInfo);
	bAreAllTestsCorrect = bAreAllTestsCorrect && bCorrectGetClientInfo;


	std::cout << "Test SplitterClientsBase::AddClient";
	bool bCorrectAddClient = true;
	for (size_t i = 0; i <= 100 && bCorrectAddClient; i++) {
		bCorrectAddClient = CorrectWork_SplitterClientsBase_AddClient(i);
	}
	PrintFailResult("", bCorrectAddClient);
	bAreAllTestsCorrect = bAreAllTestsCorrect && bCorrectAddClient;


	std::cout << "Test SplitterClientsBase::RemoveClient";
	bool bCorrectRemoveClient = true;
	for (size_t i = 0; i <= 100 && bCorrectRemoveClient; i++) {
		bCorrectRemoveClient = CorrectWork_SplitterClientsBase_RemoveClient(i);
	}
	PrintFailResult("", bCorrectRemoveClient);
	bAreAllTestsCorrect = bAreAllTestsCorrect && bCorrectRemoveClient;


	std::cout << "Test SplitterClientsBase::SetClientCurrentIndex";
	bool bCorrectSetCurrentIndex = true;
	for (size_t i = 0; i <= 100 && bCorrectSetCurrentIndex; i++) {
		bCorrectSetCurrentIndex = CorrectWork_SplitterClientsBase_SetClientCurrentIndex(i);
	}
	PrintFailResult("", bCorrectSetCurrentIndex);
	bAreAllTestsCorrect = bAreAllTestsCorrect && bCorrectSetCurrentIndex;



	std::cout << "Test SplitterClientsBase::ChangeOldCurrentIndex";
	bool bCorrectCheckCurrentIndex = true;
	for (size_t i = 0; i <= 100 && bCorrectCheckCurrentIndex; i++) {
		bCorrectCheckCurrentIndex = CorrectWork_SplitterClientsBase_CheckCurrentObjectIndex(i);
	}
	PrintFailResult("", bCorrectCheckCurrentIndex);
	bAreAllTestsCorrect = bAreAllTestsCorrect && bCorrectCheckCurrentIndex;


	std::cout << "Test SplitterClientsBase::ChangeOldCurrentIndex";
	bool bCorrectChangeCurrentIndex = true;
	for (size_t i = 0; i <= 100 && bCorrectChangeCurrentIndex; i++) {
		bCorrectChangeCurrentIndex = CorrectWork_SplitterClientsBase_ChangeCurrentObjectIndex(i);
	}
	PrintFailResult("", bCorrectChangeCurrentIndex);
	bAreAllTestsCorrect = bAreAllTestsCorrect && bCorrectChangeCurrentIndex;

	//End test for class SplitterClientsBase
	

	//Start test for class Splitter

	std::cout << "Test Create Splitter";
	bool bCorrectCreateSplitter(true);
	for (int i = -10; i < 100 && bCorrectCreateSplitter; i++) {
		bCorrectCreateSplitter = CorrectCreate_Splitter(i + 1, i);
	}
	PrintFailResult("", bCorrectCreateSplitter);
	bAreAllTestsCorrect = bAreAllTestsCorrect && bCorrectCreateSplitter;


	std::cout << "Test Work with Splitters Client";
	bool bCorrectSplitterWorkWithClients(true);
	for (int i = 0; i <= 1000 && bCorrectSplitterWorkWithClients; i+=100) {
		bCorrectSplitterWorkWithClients = CorrectWork_Splitter_WorkWithClients(i + 1, i);
	}
	PrintFailResult("", bCorrectSplitterWorkWithClients);
	bAreAllTestsCorrect = bAreAllTestsCorrect && bCorrectSplitterWorkWithClients;


	std::cout << "Test Work with Splitters SplitterPut() and SplitterGet()";
	bool bCorrectWorkSplitter_GetAndPut(true);
	for (int i = 1; i <= 200 && bCorrectWorkSplitter_GetAndPut; i += 99) {
		bCorrectWorkSplitter_GetAndPut = CorrectWork_Splitter_GetAndPut(i + 1, i, 1000, 40)&& CorrectWork_Splitter_GetAndPut(10, i, 1000, 40) && CorrectWork_Splitter_GetAndPut(i , 10, 1000, 40);
	}
	PrintFailResult("", bCorrectWorkSplitter_GetAndPut);
	bAreAllTestsCorrect = bAreAllTestsCorrect && bCorrectWorkSplitter_GetAndPut;

	//End test for class Splitter
	
	
	//Берем строку из файла
	std::ifstream sourseFile("Test_Files\\Source.txt");
	std::string strObject("");
	std::getline(sourseFile, strObject);
	
	//Время симулящии в милисекундах
	const int nSimulationTimeMsec = 10000;
	//Время между новыми объектами поступающими в буфер
	const int nDelayObjectMsec = 30;
	//Количество клиентов сплитера - делать больше трех
	const int nMaxClients(100);
	//Количество буферов
	const int nMaxObjects(100);

	//Симуляция работы
	std::cout << "Simulation..." << "\n";
	Splitter<std::string> cSplitter;
	cSplitter.SplitterInit(nMaxObjects, nMaxClients);

	//Заполняем клиентами
	bool bCorrecteAddClient(true);
	std::vector<std::pair<int, bool>> vecClients(0);
	for (int i = 0; i < nMaxClients && bCorrecteAddClient; i++) {
		int nClientId(0);
		bCorrecteAddClient = cSplitter.SplitterClientAdd(&nClientId);
		vecClients.push_back(std::pair<int, bool>(nClientId, bCorrecteAddClient));
	}

	//Устанавливаем общий таймаут симуляции
	std::atomic_bool bHasGeneralTimeOut(false);
	auto futureGeneralTimeOut = std::async(std::launch::async, [&bHasGeneralTimeOut, nSimulationTimeMsec] {
		std::this_thread::sleep_for(milliseconds{ nSimulationTimeMsec });
		bHasGeneralTimeOut.store(true);
	});
	//Запускаем асинхронно SplitterPut
	auto futureWorkSplitterPut = std::async(std::launch::async, [&bHasGeneralTimeOut, &cSplitter, &strObject, nDelayObjectMsec] {
		//выполняем пока не наступил таймаут
		while (!bHasGeneralTimeOut.load()) {
			//запускаем таймер задержки кадра
			auto futureDelay = std::async(std::launch::async, [nDelayObjectMsec] {
				std::this_thread::sleep_for(milliseconds{ nDelayObjectMsec });
			});
			cSplitter.SplitterPut( &strObject, true, nDelayObjectMsec);
			//Ожидаем завершение задержки
			futureDelay.get();
		}		
	});
	//Делем клиентов на три группы,
	//1 - всегда забираю объекты
	//2 - забираю объекты с опозданием
	//3 - не забирают объекты
	if (nMaxClients > 3) {
		int nFirstThird = (int)(nMaxClients / 3);
		int nSecondThird = nFirstThird * 2;

		//Запускаем клиентов которые всегда забирают
		auto futureWorkSplitterGetFirstThird = std::async(std::launch::async, [&bHasGeneralTimeOut, &cSplitter, nFirstThird, nDelayObjectMsec] {
			//выполняем пока не наступил таймаут
			while (!bHasGeneralTimeOut.load()) {
				std::vector<std::shared_future< std::string>> vecFutures;
				//Асинхронно запускаем ожидания клиентов
				for (int j = 0; j < nFirstThird; j++) {
					auto fut = std::async(std::launch::async, [&cSplitter, j, nDelayObjectMsec] {
						std::string strClientObject("");
						std::string* pstrClientObject = &strClientObject;
						int nCorrectGet = cSplitter.SplitterGet(j, &pstrClientObject, nDelayObjectMsec);
						return strClientObject;
					});
					vecFutures.push_back(fut.share());
				}
				//Ожидаем что все клиенты получили объект
				for (size_t i(0); i < vecFutures.size(); i++) {
					 vecFutures.at(i).get();
					//std::cout << vecFutures.at(i).get() << "\n";
				}
				
			}
		});

		//Запускаем клиентов которые  забирают c задержкой
		auto futureWorkSplitterGetSecondThird = std::async(std::launch::async, [&bHasGeneralTimeOut, &cSplitter, nFirstThird, nSecondThird, nDelayObjectMsec] {
			//выполняем пока не наступил таймаут
			while (!bHasGeneralTimeOut.load()) {
				//Делаем паузу 
				std::this_thread::sleep_for(milliseconds{ nDelayObjectMsec });
				std::vector<std::shared_future< std::string>> vecFutures;
				//Асинхронно запускаем ожидания клиентов 
				for (int j = nFirstThird; j < nSecondThird; j++) {
					auto fut = std::async(std::launch::async, [&cSplitter, j, nDelayObjectMsec] {
						std::string strClientObject("");
						std::string* pstrClientObject = &strClientObject;
						int nCorrectGet = cSplitter.SplitterGet(j, &pstrClientObject, nDelayObjectMsec);
						return strClientObject;
					});
					vecFutures.push_back(fut.share());
				}
				//Ожидаем что все клиенты получили объект
				for (size_t i(0); i < vecFutures.size(); i++) {
					vecFutures.at(i).get();
					//std::cout << vecFutures.at(i).get() << "\n";
				}
			}
		});


		futureWorkSplitterGetFirstThird.get();
		futureWorkSplitterGetSecondThird.get();
	}

	futureGeneralTimeOut.get();
	futureWorkSplitterPut.get();

	//SpecialTests
	//Тестирование потоков
	/*{
		std::condition_variable m_cvMessageForSplitterPut;
		std::mutex m_mutex_cvMessageForSplitterPut;
		std::mutex m_Print;
		std::atomic_bool bHasTimeOut(false);

		int nCurrentObjectIndex = 1;
		int _nTimeOutMsec(100);
		int _nTimeForCheck(10);

		bool bHasClientsWithCurrentObjectIndex(true);
		std::vector<std::shared_future< void>> vecFutures;
		if (bHasClientsWithCurrentObjectIndex) {
			std::cout << "Main_tread_id:" << std::this_thread::get_id() << "\n";
			auto timeOut = std::chrono::system_clock::now() + milliseconds{ _nTimeOutMsec };


			auto futureTimeOut = std::async(std::launch::async, [&bHasTimeOut, _nTimeOutMsec] {
				std::this_thread::sleep_for(milliseconds{ _nTimeOutMsec });
				bHasTimeOut.store(true);
			});

			for (int i = 0; i < 10; i++) {
				auto fut = std::async([_nTimeForCheck, &m_cvMessageForSplitterPut, &m_Print, i] {
					std::this_thread::sleep_for(milliseconds{ _nTimeForCheck });
					m_cvMessageForSplitterPut.notify_one();
					m_Print.lock();
					std::cout << i << "  " << "tread_id:" << std::this_thread::get_id() << "\n";
					m_Print.unlock();
				});
				vecFutures.push_back(fut.share());
			}
			while (bHasClientsWithCurrentObjectIndex && !bHasTimeOut.load()) {
				std::unique_lock<std::mutex> lkMessage(m_mutex_cvMessageForSplitterPut);
				m_cvMessageForSplitterPut.wait_until(lkMessage, timeOut);
				m_Print.lock();
				std::cout << "Check\n";
				m_Print.unlock();
			}

		}
	}*/
	//Тестирование умных указателей
	/*{
		std::unique_ptr<Item> item1(new Item(1)); // выделение Item
		std::unique_ptr<Item> item2(new Item(2)); // присваивается значение nullptr

		std::cout << "item1 is " << (item1 ? "not null\n" : "null\n");
		std::cout << "item2 is " << (item2 ? "not null\n" : "null\n");

		// item2 = item1; // не скомпилируется: семантика копирования отключена
		item2 = std::move(item1); // item2 теперь владеет item1, а для item1 присваивается значение null

		std::cout << "Ownership transferred\n";

		std::cout << "item1 is " << (item1 ? "not null\n" : "null\n");
		std::cout << "item2 is " << (item2 ? "not null\n" : "null\n");
	}*/
	//Тестирование заполнения вектора
	/*{
		std::cout << "\nТестирование заполнения вектора\n";
		std::vector<std::unique_ptr<std::string>> vecUniquePtr(0);
		auto  PrintVector = [&] {
			int i(0);
			for (auto& elem : vecUniquePtr) {
				if (elem) {
					std::cout << i << ": " << *elem << "\n";
				}
				else {
					std::cout << i << ": nullptr\n";
				}
				i++;
			}
			
			std::cout << "\nUsual for\n";
			
			for (size_t i = 0; i < vecUniquePtr.size(); i++) {
				if (vecUniquePtr.at(i)) {
					std::cout << i << ": " << *(vecUniquePtr.at(i)) << "\n";
				}
				else {
					std::cout << i << ": nullptr\n";
				}
			}
		};
		PrintVector();
		vecUniquePtr.resize(5);
		PrintVector();
	}
	*/
	//Тестирование Функции получения задержки
	/*{
		int l_nCurrentIndex(0);
		size_t l_uResultGetCurrentDelay(0);
		std::atomic<bool> l_bIsInitialized(true);
		std::atomic<size_t> l_uMaxObjectIndex(0);
		std::atomic<size_t> l_uCurrentObjectIndex(0);

		auto GetCurrentDelay = [&]()
		{
			size_t nResult(0);
			//Проверка инициации сплиттера
			if (l_bIsInitialized.load()) {
				const size_t uMaxObjectIndex = static_cast<size_t> (l_uMaxObjectIndex.load());
				if (0 < uMaxObjectIndex) {
					nResult = (uMaxObjectIndex + static_cast<size_t>(l_uCurrentObjectIndex.load()) - l_nCurrentIndex) % uMaxObjectIndex;
				}				
			}
			return nResult;
		};
		std::cout << "Локальное Тестирование GetCurrentDelay\n";
		l_uMaxObjectIndex.store(20);
		l_uCurrentObjectIndex.store(10);
		std::cout << "l_uMaxObjectIndex: " << l_uMaxObjectIndex << " | l_uCurrentObjectIndex: " << l_uCurrentObjectIndex << "\n";
		for (; l_nCurrentIndex <= l_uMaxObjectIndex; l_nCurrentIndex++) {
			//l_uResultGetCurrentDelay = GetCurrentDelay();
			std::cout << "l_nCurrentIndex: " << l_nCurrentIndex << " | Delay: " << GetCurrentDelay() << "\n";
		}
	}*/
	//Тестирование работы алгоритмов для контейнеров
	/*{
		bool bActivity(false);	
		std::vector<std::pair<int, bool>> vecClients(0);
		for (int i(0); i < 12; i++) {
			bActivity = !bActivity;
			vecClients.push_back(std::pair<int, bool>(i, bActivity));
		}
		int nCountActiveClients = std::count_if(vecClients.begin(), vecClients.end(), [](std::pair<int, bool> _p) {return _p.second; });
		std::cout<< "CountActiveClients: " << nCountActiveClients << "\n";
	}*/
	//Тестирование определение Id по индексу
	/*{
		size_t uClientID(0);
		size_t uCurrentObjectIndex(0);
		int nCurrentIndex(0);
		size_t uMaxClients(10);
		int _nIndex(5);
		//считаем всех активных клиентов пока не доберемся до _nIndex активного клиента
		while (uClientID < uMaxClients && nCurrentIndex <= _nIndex) {
			nCurrentIndex++;
			if (nCurrentIndex <= _nIndex) {
				uClientID++;
			}
			std::cout << "uClientID: " << uClientID << " | nCurrentIndex: " << nCurrentIndex << "\n";
		};

		std::cout << "After All:\n" << "uClientID: " << uClientID << " | nCurrentIndex: " << nCurrentIndex << "\n";
	}*/
	//Тестирование вывода в файлы
	/*{
	std::ofstream outf("Test_Files\\Source.txt");
	for (int i(0); i < 1024; i++) {
		outf << "Test ";
	}
	outf << "\n";		
	}*/
	


	if (bAreAllTestsCorrect) {
		std::cout << "All tests are correct!\n";
	}
	return 0;
}



// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.

#include "pch.h"
#include <vector>

#ifndef SPLITTERCLIENTBASE_H
#define SPLITTERCLIENTBASE_H
//����� ��� �������� ���������� � ������� ���������
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

	//����������� ���������� � �������
	void CopyInfo( IN const SplitterClientInfo& _clientInfo);

	//�������� ����������  � �������
	void ClearInfo();

	//����������� ���������� � �������
	void MoveInfo( OUT SplitterClientInfo& _clientInfo);
	
private:
	//Id �������
	size_t m_uClientId;
	//������� ������ ������
	size_t m_uCurrentObjectIndex;
	//���� ���������� �������
	bool m_bIsActive;

};


//����� ��� ���������� ����������� � �������� ���������
class SplitterClientsBase {
public:
	SplitterClientsBase();
	SplitterClientsBase(IN const size_t _uMaxClients);
	~SplitterClientsBase();

	SplitterClientsBase(const SplitterClientsBase&) = delete;
	SplitterClientsBase(SplitterClientsBase&&) = delete;
	SplitterClientsBase& operator=(const SplitterClientsBase&) = delete;
	SplitterClientsBase& operator=(SplitterClientsBase&&) = delete;

	//������������ ���������� ��������
	size_t GetMaxClients() const;

	//���������� ������������ ���������� ��������.
	//���������: True - ���������; False - ������, ������� ���������� �������� ������ ��� ����� ������������ ���������� ��������
	bool SetMaxClients(IN const size_t _uMaxClients);

	//������� ���������� ��������
	size_t GetCurrentClients() const;

	//���������� ������ �������. ���������: True - ������ ��������; False - ������ �� �������� 
	bool AddClient(OUT size_t *const _uClientID, IN size_t _uCurrentObjectIndex);

	//�������� ������� (������������ ������). ���������: True - ������ ������; False - ������� � ����� Id �� ���������� ��� �� ��� ��������������
	bool RemoveClient(IN const size_t _uClientID);

	//���������� � ������� �� ��������������. ���������: True - ���������� �������; False - ������� � ����� Id �� ���������� ��� �� ��� ��������������
	bool GetClientCurrentIndex(IN const size_t _uClientID, OUT size_t *const _uCurrentObjectIndex) const;

	//��������� �������� ������� ������ ��� �������. ���������: True - ������ �������; False -  ������� � ����� Id �� ���������� ��� �� ��� ��������������
	bool SetClientCurrentIndex(IN const size_t _uClientID, IN const size_t _uCurrentObjectIndex);

	//�������� �������� ������� ������ ��� ���� �������� �������� �� ��������� _uCurrentIndex 
	//���������: True - ��������� ����, False - �������� ���
	bool CheckCurrentObjectIndex(IN const size_t _uCurrentIndex);

	//������ �������� ������� ������ ��� ���� �������� �������� �� _uNextCurrentIndex, ���� �� ����� _uCurrentIndex 
	//���������: True - ������ �����������, False - ����� ���
	bool ChangeCurrentObjectIndex(IN const size_t _uCurrentIndex, IN const size_t _uNextCurrentIndex);


private:
	//������������ ���������� ��������
	std::size_t m_uMaxClients;
	//��������� � ���������
	std::vector<SplitterClientInfo> m_vecClientInfo;

	
};



#endif SPLITTERCLIENTBASE_H
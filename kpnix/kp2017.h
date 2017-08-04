const int	MAX_MVV = 2;											// ����� ������� �����/������(���)
const int	MAX_BLOCK_IN_MVV = 3;									// ����� ������ � ����� ���
																	   
const int	MAX_MTS_IN_BLOCK = 8;									// ����� ������� �� � �����
const int	MAX_MTS_IN_MVV = MAX_BLOCK_IN_MVV*MAX_MTS_IN_BLOCK;		// ����� ������� � ���
const int	MAX_MTU_IN_BLOCK = MAX_MTS_IN_BLOCK;					// ����� ������� �� � �����
const int	MAX_MTU_IN_MVV = MAX_BLOCK_IN_MVV*MAX_MTU_IN_BLOCK;		// ����� ������� �� � ���
																	   
const int	MAX_MTS = MAX_MVV*MAX_MTS_IN_MVV;						// ��� ����� ������� ��
const int	MAX_MTU = MAX_MVV*MAX_MTU_IN_MVV;						// ��� ����� ������� ��
																	   
const int	MTS_ROW_CNT = 4;										// ����� ����� � ������ ��
const int	MTS_COL_CNT = 8;										// ����� �������� � ������ ��
const int	MAX_TS_IN_MTS = MTS_ROW_CNT*MTS_COL_CNT;				// ����� �� � ������
																	   
const int	MTU_ROW_CNT = 4;										// ����� ����� � ������ ��
const int	MTU_COL_CNT = 8;										// ����� �������� � ������ ��
const int	MAX_TU_IN_MTU = MTU_ROW_CNT*MTU_COL_CNT;				// ����� �� � ������

const int	MAX_MODUL_IN_BLOCK = 8;
const int	MAX_MODUL_IN_MVV = MAX_BLOCK_IN_MVV*MAX_MODUL_IN_BLOCK;	// ����� ������� � ���;									//����� ������ � ����� ���
const int	MAX_MODUL = MAX_MVV*MAX_MODUL_IN_MVV;					// ��� ����� ������� �� - ��

const int	TSSIZE = MAX_MTS * MTS_ROW_CNT;							// ����������� �������� ��

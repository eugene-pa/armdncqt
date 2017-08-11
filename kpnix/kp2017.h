const int	MAX_MVV = 2;											// число модулей ввода/вывода(МВВ)
const int	MAX_BLOCK_IN_MVV = 3;									// число блоков в одном МВВ
																	   
const int	MAX_MTS_IN_BLOCK = 8;									// число модулей ТС в блоке
const int	MAX_MTS_IN_MVV = MAX_BLOCK_IN_MVV*MAX_MTS_IN_BLOCK;		// число модулей в МВВ
const int	MAX_MTU_IN_BLOCK = MAX_MTS_IN_BLOCK;					// число модулей ТУ в блоке
const int	MAX_MTU_IN_MVV = MAX_BLOCK_IN_MVV*MAX_MTU_IN_BLOCK;		// число модулей ТУ в МВВ
																	   
const int	MAX_MTS = MAX_MVV*MAX_MTS_IN_MVV;						// мах число модулей ТС
const int	MAX_MTU = MAX_MVV*MAX_MTU_IN_MVV;						// мах число модулей ТУ
																	   
const int	MTS_ROW_CNT = 4;										// число строк в модуле ТС
const int	MTS_COL_CNT = 8;										// число столбцов в модуле ТС
const int	MAX_TS_IN_MTS = MTS_ROW_CNT*MTS_COL_CNT;				// число ТС в модуле
																	   
const int	MTU_ROW_CNT = 4;										// число строк в модуле ТУ
const int	MTU_COL_CNT = 8;										// число столбцов в модуле ТУ
const int	MAX_TU_IN_MTU = MTU_ROW_CNT*MTU_COL_CNT;				// число ТУ в модуле

const int	MAX_MODUL_IN_BLOCK = 8;
const int	MAX_MODUL_IN_MVV = MAX_BLOCK_IN_MVV*MAX_MODUL_IN_BLOCK;	// число модулей в МВВ;									//число блоков в одном МВВ
const int	MAX_MODUL = MAX_MVV*MAX_MODUL_IN_MVV;					// мах число модулей ТУ - ТС

const int	TSSIZE = MAX_MTS * MTS_ROW_CNT;							// размерность массивов ТС

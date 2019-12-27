// SudokuGame.cpp: implementation of the CSudokuGame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sudoku.h"
#include "SudokuGame.h"
#include "helpfunct.h"
#include "RandomFill.h"
#include <list>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCellField::CCellField()
{
	Init();
}

CSudokuGame::CSudokuGame()
{
	m_SolutionCnt = 0;
	m_bSearchOneSolution = false;
	Init();
}

CSudokuGame::~CSudokuGame()
{
	ResetMovesToMake();
}

void CSudokuGame::Reset()
{
	ZeroMemory( m_StartPosition , COL_CNT * ROW_CNT * sizeof( int ) );
	ZeroMemory( m_CurrentPlayedCells , COL_CNT * ROW_CNT * sizeof( CCell* ) );

	m_CellField.Reset();

	ResetMovesToMake();
	ResetForbidden();
}

void CCellField::Reset()
{
	int	i, j;
	for( i = 0 ; i < VAL_CNT ; i++ )
	{
		for( j = 0 ; j < VAL_CNT ; j++ )
		{
			m_Cells[i][j].m_Val = 0;
		}
	}
}

void CSudokuGame::ResetMovesToMake()
{
	CToPlay* pToPlay;
	std::vector<CToPlay*>::iterator it;
	for( it = m_MovesToMake.begin() ; it != m_MovesToMake.end() ; it++ )
	{
		pToPlay = *it;
		delete pToPlay;
	}
	m_MovesToMake.clear();
}

void CSudokuGame::Init()
{
	ZeroMemory( m_StartPosition , VAL_CNT * VAL_CNT * sizeof( int ) );
	ResetDone();
	//m_CellField.Init();
}

void CCellField::Init()
{
	int			i, j;
	CCondition* pCond;
	int			CondIdx, CellIdx;
	CCell*		pCell;

	for( i = 0 ; i < VAL_CNT ; i++ )
	{
		for( j = 0 ; j < VAL_CNT ; j++ )
		{
			pCell = &m_Cells[i][j];
			pCell->m_Col = i;
			pCell->m_Row = j;

			//condition for a cell:
			CondIdx = ROWS_IN_FIELD * ( i / COLS_IN_FIELD ) + j / ROWS_IN_FIELD;
			pCond = &m_Conditions[0][CondIdx];
			pCell->m_Conditions[0] = pCond;
			CellIdx = ROWS_IN_FIELD * ( i - COLS_IN_FIELD * ( i / COLS_IN_FIELD ) ) + j % ROWS_IN_FIELD; 
			ASSERT( pCond->m_Cells[CellIdx] == NULL );
			pCond->m_Cells[CellIdx] = pCell;
			pCond->m_Desc.Format( "3x3-Feld[%d][%d]", i/COLS_IN_FIELD + 1, j/ROWS_IN_FIELD + 1 );

			//condition for a column:
			pCond = &m_Conditions[1][i];
			pCell->m_Conditions[1] = pCond;
			ASSERT( pCond->m_Cells[j] == NULL );
			pCond->m_Cells[j] = pCell;
			pCond->m_Desc.Format( "Spalte %d", i + 1 );

			//condition for a row:
			pCond = &m_Conditions[2][j];
			pCell->m_Conditions[2] = pCond;
			ASSERT( pCond->m_Cells[i] == NULL );
			pCond->m_Cells[i] = pCell;
			pCond->m_Desc.Format( "Reihe %d", j + 1 );
		}
	}

	CPartConditions*	pPartCond;
	CCondition*			pCellCond, *pColCond, *pRowCond;

	//fill pCond->m_arPartConditions
	for( i = 0 ; i < VAL_CNT ; i++ )
	{
		pCellCond = &m_Conditions[0][i];//cells
		pPartCond = new CPartConditions;
		pCellCond->m_arPartConditions.push_back( pPartCond );
		pPartCond = new CPartConditions;
		pCellCond->m_arPartConditions.push_back( pPartCond );

		pColCond = &m_Conditions[1][i];//cols
		pPartCond = new CPartConditions;
		pColCond->m_arPartConditions.push_back( pPartCond );

		pRowCond = &m_Conditions[2][i];//cols
		pPartCond = new CPartConditions;
		pRowCond->m_arPartConditions.push_back( pPartCond );				
	}

	for( i = 0 ; i < VAL_CNT ; i++ )
	{
		for( j = 0 ; j < VAL_CNT ; j++ )
		{
			pCell = &m_Cells[i][j];
			pCellCond = pCell->m_Conditions[0];
			pColCond = pCell->m_Conditions[1];
			pRowCond = pCell->m_Conditions[2];

			pPartCond = pCellCond->m_arPartConditions[0];//3*(3-elem-columns)
			ASSERT( pPartCond->m_PartConditions[i%COLS_IN_FIELD].m_pCells[j%ROWS_IN_FIELD] == NULL );
			pPartCond->m_PartConditions[i%COLS_IN_FIELD].m_pCells[j%ROWS_IN_FIELD] = pCell;
			ASSERT( pPartCond->m_PartConditions[i%COLS_IN_FIELD].pCond == NULL && ( j%ROWS_IN_FIELD == 0 )
				||  pPartCond->m_PartConditions[i%COLS_IN_FIELD].pCond == pColCond && ( j%ROWS_IN_FIELD != 0 ) );
			pPartCond->m_PartConditions[i%COLS_IN_FIELD].pCond = pColCond;
			pPartCond->m_PartConditions[i%COLS_IN_FIELD].m_bCol = true;

			pPartCond = pCellCond->m_arPartConditions[1];//3*(3-elem-rows)
			ASSERT( pPartCond->m_PartConditions[j%ROWS_IN_FIELD].m_pCells[i%COLS_IN_FIELD] == NULL );
			pPartCond->m_PartConditions[j%ROWS_IN_FIELD].m_pCells[i%COLS_IN_FIELD] = pCell;
			ASSERT( pPartCond->m_PartConditions[j%ROWS_IN_FIELD].pCond == NULL && ( i%COLS_IN_FIELD == 0 )
				||  pPartCond->m_PartConditions[j%ROWS_IN_FIELD].pCond == pRowCond && ( i%COLS_IN_FIELD != 0 ) );
			pPartCond->m_PartConditions[j%ROWS_IN_FIELD].pCond = pRowCond;
			pPartCond->m_PartConditions[j%ROWS_IN_FIELD].m_bCol = false;

			pPartCond = pColCond->m_arPartConditions[0];//3*(3-elem-cols)
			ASSERT( pPartCond->m_PartConditions[j/ROWS_IN_FIELD].m_pCells[j%ROWS_IN_FIELD] == NULL );
			pPartCond->m_PartConditions[j/ROWS_IN_FIELD].m_pCells[j%ROWS_IN_FIELD] = pCell;
			ASSERT( pPartCond->m_PartConditions[j/ROWS_IN_FIELD].pCond == NULL && ( j%ROWS_IN_FIELD == 0 )
				||  pPartCond->m_PartConditions[j/ROWS_IN_FIELD].pCond == pCellCond && ( j%ROWS_IN_FIELD != 0 ) );
			pPartCond->m_PartConditions[j/ROWS_IN_FIELD].pCond = pCellCond;
			pPartCond->m_PartConditions[j/ROWS_IN_FIELD].m_bCol = true;

			pPartCond = pRowCond->m_arPartConditions[0];//3*(3-elem-rows)
			ASSERT( pPartCond->m_PartConditions[i/COLS_IN_FIELD].m_pCells[i%COLS_IN_FIELD] == NULL );
			pPartCond->m_PartConditions[i/COLS_IN_FIELD].m_pCells[i%COLS_IN_FIELD] = pCell;
			ASSERT( pPartCond->m_PartConditions[i/COLS_IN_FIELD].pCond == NULL && ( i%COLS_IN_FIELD == 0 )
				||  pPartCond->m_PartConditions[i/COLS_IN_FIELD].pCond == pCellCond && ( i%COLS_IN_FIELD != 0 ) );
			pPartCond->m_PartConditions[i/COLS_IN_FIELD].pCond = pCellCond;
			pPartCond->m_PartConditions[i/COLS_IN_FIELD].m_bCol = false;
		}
	}
}

#ifdef _DEBUG
void CCellField::TraceCells()
{
	::AfxTrace( "\n" );
	int i;
	for( i = 0 ; i < 9 ; i++ )
	{
		::AfxTrace( "%d %d %d %d %d %d %d %d %d\n",
		m_Cells[0][i].m_Val,
		m_Cells[1][i].m_Val,
		m_Cells[2][i].m_Val,
		m_Cells[3][i].m_Val,
		m_Cells[4][i].m_Val,
		m_Cells[5][i].m_Val,
		m_Cells[6][i].m_Val,
		m_Cells[7][i].m_Val,
		m_Cells[8][i].m_Val );
	}
}
#endif

int CSudokuGame::Solve( int MaxLevel , int& MaxUsedLevel , int& NumberOfSolutions , bool bUndoActions )
{
	ResetMovesToMake();
	ResetForbidden();

	int			FoundCellCnt;
	int			SumFoundCells = 0;
	int			i;
	int			level;
	CToPlay*	pToPlay = NULL;

	MaxUsedLevel = 0;
	NumberOfSolutions = 1;

	int FreeCellCnt = GetFreeCellCnt();

	bool bContradiction = false;

	do
	{
		FoundCellCnt = 0;
		ZeroMemory( m_CurrentPlayedCells , COL_CNT * ROW_CNT * sizeof( CCell* ) );

		FoundCellCnt = SimpleSolve( FreeCellCnt - SumFoundCells , &level );
		if( FoundCellCnt == -1 )
		{
			bContradiction = true;
			break;
		}

		if( level > MaxLevel )
			break;

		::AfxTrace( "\nFoundCells: %d from %d" , FoundCellCnt , FreeCellCnt - SumFoundCells );

		if( FoundCellCnt )
		{
			MaxUsedLevel = __max( MaxUsedLevel , level );
			//play it for the next loop:
			for( i = 0 ; i < FoundCellCnt ; i++ )
			{
				pToPlay = m_MovesToMake[SumFoundCells+i];
				pToPlay->GetCell()->SetVal( pToPlay->GetVal() );
			}
			SumFoundCells += FoundCellCnt;
		}
		else if( MaxLevel > 4 )
		{
			for( i = 5 ; i < 8 ; i++ )
			{
				if( i > MaxLevel )
					break;

				FoundCellCnt = DoOperations( i , FreeCellCnt - SumFoundCells );
				if( FoundCellCnt == -1 )
				{
					bContradiction = true;
					break;
				}

				if( FoundCellCnt )
				{
					MaxUsedLevel = __max( i , MaxUsedLevel );
					//play it for the next loop:
					for( i = 0 ; i < FoundCellCnt ; i++ )
					{
						pToPlay = m_MovesToMake[SumFoundCells+i];
						pToPlay->GetCell()->SetVal( pToPlay->GetVal() );
					}
					SumFoundCells += FoundCellCnt;
					break;
				}
			}

			if( bContradiction )
				break;

			if( i == 9 )
			{
				MaxUsedLevel = 9;
				NumberOfSolutions = DoComputerSolution( false , false );
				return NumberOfSolutions;
			}
		}

	} while( FoundCellCnt > 0 );


	if( bUndoActions )
	{
		//undo all actions:
		std::vector<CToPlay*>::iterator it;
		for( it = m_MovesToMake.begin() ; it != m_MovesToMake.end() ; it++ )
		{
			pToPlay = *it;
			pToPlay->GetCell()->Erase();
		}
	}

	return bContradiction ? -1 : SumFoundCells;
}

int CSudokuGame::SimpleSolve( int RemainingCells , int* pLevel )
{
	int FoundCellCnt = 0;

	int CurFoundCells = OnlyOneCellLeft();
	if( CurFoundCells == -1 )
		return -1;//contradiction
	FoundCellCnt += CurFoundCells;

	CurFoundCells = OnlyOneValLeft();
	if( CurFoundCells == -1 )
		return -1;//contradiction
	FoundCellCnt += CurFoundCells;

	if( !pLevel )
		return FoundCellCnt;

	//returns the solving level 1, 2, 3 or 4
	*pLevel = 1;
	if( FoundCellCnt > 0  && RemainingCells > 10 )
	{
		double RelativeFoundCells = FoundCellCnt / (double)RemainingCells;
		if( RelativeFoundCells > 0.2 )//>20%
			*pLevel = 1;
		else if( RelativeFoundCells > 0.08)//>8%
			*pLevel = 2;
		else if( RelativeFoundCells > 0.032 )//>3.2%
			*pLevel = 3;
		else
			*pLevel = 4;
	}

	return FoundCellCnt;
}

bool CSudokuGame::IsAlreadyFound( CCell* pCell )
{
	return m_CurrentPlayedCells[pCell->m_Col][pCell->m_Row] != NULL;
}

void CSudokuGame::AddFoundCell( CCell* pCell , int v , CString& desc )
{
	m_CurrentPlayedCells[pCell->m_Col][pCell->m_Row] = pCell;

	CToPlay* pToPlay = new CToPlay;
	pToPlay->SetCell( pCell );
	pToPlay->SetVal( v );
	pToPlay->SetDesc( desc );
	m_MovesToMake.push_back( pToPlay );
}

int CSudokuGame::OnlyOneCellLeft()
{
	int			i, j, k;
	CCondition* pCond;
	CCell*		pCell;
	int			iRet = 0;
	CString		desc;
	
	for( i = 0 ; i < 3 ; i++ )
	{
		for( j = 0 ; j < VAL_CNT ; j++ )
		{
			pCond = GetCond( i , j );
			for( k = 1 ; k < VAL_CNT_1 ; k++ )
			{
				if( !pCond->IsFree( k ) )
					continue;

				pCell = pCond->OnlyOneCellLeft( k );
				if( pCell )
				{
					if( pCell == (CCell*)INVALID_PTR )
						return -1;//contradiction. no field left for k.

					if( IsAlreadyFound( pCell ) )
						continue;

					iRet++;
					desc.Format( "Nur ein Feld frei für Zahl '%d' in %s" , k , pCond->GetDesc() );
					AddFoundCell( pCell , k , desc );
				}
			}
		}
	}

	return iRet;
}

int CSudokuGame::OnlyOneValLeft()
{
	int			i, j;
	CCell*		pCell;
	int			iRet = 0;
	int			v;
	CString		desc;

	for( i = 0 ; i < VAL_CNT ; i++ )
	{
		for( j = 0 ; j < VAL_CNT ; j++ )
		{
			pCell = GetCell( i , j );
			if( !pCell->IsFree() )
				continue;

			v = pCell->GetOnlyPossibleValue();
			if( v < 0 )
				continue;
			if( v == 0 )
				return -1;//contradiction. not solvable,
			else
			{
				if( IsAlreadyFound( pCell ) )
					continue;
				iRet++;
				desc.Format( "'%d' ist der einzig mögliche Wert für das Feld[%d][%d]" , v , pCell->m_Col+1 , pCell->m_Row+1 );
				AddFoundCell( pCell , v , desc );
			}
		}
	}

	return iRet;
}

void CSudokuGame::ResetForbidden()
{
	int			i, j;
	CCell*		pCell;

	for( i = 0 ; i < VAL_CNT ; i++ )
	{
		for( j = 0 ; j < VAL_CNT ; j++ )
		{
			pCell = GetCell( i , j );
			pCell->ResetForbidden();
		}
	}

	ResetDone();
}

void CSudokuGame::ResetDone()
{
	memset( m_bIndConclDone, 0 , VAL_CNT * HOR_FIELD_CNT * 2 * sizeof(int) );
	memset( m_bFieldDone, 0 , VAL_CNT * 3 * sizeof(int) );
}

void CSudokuGame::SaveToFile( CStdioFile& file )
{
	CString line, str;
	int i, j;
	int val;

	//save initial position:
	file.WriteString( "<--Initial Position:-->\n");
	for( j = 0 ; j < VAL_CNT ; j++ )
	{
		line.Empty();
		for( i = 0 ; i < VAL_CNT ; i++ )
		{
			val = m_StartPosition[i][j];
			if( val )
				str.Format( "%d;" , val );
			else
				str = " ;";
			line += str;
		}
		line += "\n";
		file.WriteString( (LPCTSTR)line );
	}

	//save current position:
	file.WriteString( "<--Current Position:-->\n");
	for( j = 0 ; j < VAL_CNT ; j++ )
	{
		line.Empty();
		for( i = 0 ; i < VAL_CNT ; i++ )
		{
			val = GetCell( i , j )->GetVal();
			if( val )
				str.Format( "%d;" , val );
			else
				str = " ;";
			line += str;
		}
		line += "\n";
		file.WriteString( (LPCTSTR)line );
	}
}

void CSudokuGame::LoadFromFile( CStdioFile& file , int version )
{
	CString line, str;
	int i, j;
	int val;
	char buf[2];
	buf[1] = 0;

	Reset();

	//load initial position:
	file.ReadString( line );
	ASSERT( line == "<--Initial Position:-->" );
	
	for( j = 0 ; j < VAL_CNT ; j++ )
	{
		//line.Empty();
		file.ReadString( line );
		for( i = 0 ; i < VAL_CNT ; i++ )
		{
			buf[0] = line.GetAt(2*i);
			val = atoi( buf );
			if( val >= 0 && val < VAL_CNT_1 )
				m_StartPosition[i][j] = val;
			else
			{
				ASSERT(0);
				m_StartPosition[i][j] = 0;
			}
		}
	}

	//load initial position:
	file.ReadString( line );
	ASSERT( line == "<--Current Position:-->" );

	CCell* pCell;
	
	for( j = 0 ; j < VAL_CNT ; j++ )
	{
		//line.Empty();
		file.ReadString( line );
		for( i = 0 ; i < VAL_CNT ; i++ )
		{
			buf[0] = line.GetAt(2*i);
			val = atoi( buf );
			pCell = GetCell( i , j );
			if( val > 0 && val < VAL_CNT_1 )
				pCell->SetVal( val );
			else
			{
				ASSERT( val == 0 );
				pCell->Erase();
			}
			
			pCell->SetInitial( m_StartPosition[i][j] != 0 );
		}
	}
}

CToPlay* CSudokuGame::GetOutput( int idx )
{
	if( idx < 0 || idx >= m_MovesToMake.size() )
		return NULL;

	return m_MovesToMake[idx];
}

int CSudokuGame::GetFreeCellCnt()
{
	int			i, j;
	int			FreeCellCnt = 0;
	CCell*		pCell;

	for( i = 0 ; i < VAL_CNT ; i++ )
	{
		for( j = 0 ; j < VAL_CNT ; j++ )
		{
			pCell = GetCell( i , j );
			if( pCell->IsFree() )
				FreeCellCnt++;
		}
	}

	return FreeCellCnt;
}

int	CSudokuGame::DoOperations( int level , int CurFreeCellCnt )
{
	int		FoundCellCnt;
	bool	bForBidDone;

	switch( level )
	{
	case 5:
			FoundCellCnt = DoIndirectConclusion( level , CurFreeCellCnt , bForBidDone );
			if( FoundCellCnt == -1 )
				return -1;
			else if( FoundCellCnt > 0 )
				return FoundCellCnt;

			FoundCellCnt = Do67GroupForbids( level , CurFreeCellCnt );
			if( FoundCellCnt == -1 )
				return -1;
			else if( FoundCellCnt > 0 )
				return FoundCellCnt;

			return 0;

	case 6:
		return DoLevel6( CurFreeCellCnt );

	default:
		ASSERT(0);//not implemented yet
		return 0;
	}
}

int CSudokuGame::DoLevel6( int CurFreeCellCnt )
{
	int		FoundCellCnt;
	bool	bGroupForbidDone = false;
	bool	bIndConcForbidDone = false;

	do
	{
		FoundCellCnt = DoGroupForbids( CurFreeCellCnt , bGroupForbidDone );
		if( FoundCellCnt == -1 )
			return -1;
		else if( FoundCellCnt > 0 )
			return FoundCellCnt;

		FoundCellCnt = DoIndirectConclusion( 6 , CurFreeCellCnt , bIndConcForbidDone );
		if( FoundCellCnt == -1 )
			return -1;
		else if( FoundCellCnt > 0 )
			return FoundCellCnt;

	} while( bGroupForbidDone || bIndConcForbidDone );
	
	return 0;
}

int	CSudokuGame::DoComputerSolution( bool bSearchOneSolution , bool bSimulation )
{
	/*if( bSearchOneSolution )
	{
		find one solution and then return. Result is in the cells.
	}
	else
	{
		count in Variable m_SolutionCnt the number of solutions. 
		The last found solution remains in the cells after return. 
	}*/

	//for increasing performance we try it simple at first:
	int MaxUsedLevel, NumberOfSolutions;
	Solve( MAX_HUMAN_LEVEL , MaxUsedLevel , NumberOfSolutions , true );

	m_SolutionCnt = 0;

	CCell* pFirstCell = ConnectFreeCells();
	if( !pFirstCell )
		return 0;

	int ret = 0;

	if( bSearchOneSolution )
	{
		ret = FindOneSolution( pFirstCell ) ? 1 : 0;
	}
	else
	{
		FindAllSolutions( pFirstCell );
		if( m_SolutionCnt && !bSimulation )
			FindOneSolution( pFirstCell );
		ret = m_SolutionCnt;//returns the number of solutions
	}
	
	if( bSimulation )
	{
		CCell* pCell;
		for( pCell = pFirstCell ; pCell != NULL ; pCell = pCell->m_pNextFreeCell )
			pCell->Erase();
	}

	return ret;
}

CCell* CSudokuGame::ConnectFreeCells()
{
	int			i, j;
	CCell*		pCell;
	CCell*		pFirstCell = NULL;
	CCell*		pPrevCell = NULL;

	for( i = 0 ; i < COL_CNT ; i++ )
	{
		for( j = 0 ; j < ROW_CNT ; j++ )
		{
			pCell = GetCell( i , j );
			if( !pCell->IsFree() )
				continue;

			if( !pFirstCell )
			{
				pFirstCell = pCell;
				pPrevCell = pCell;
			}
			else
			{
				pPrevCell->m_pNextFreeCell = pCell;
				pPrevCell = pCell;
			}			
		}
	}

	if( pPrevCell )
		pPrevCell->m_pNextFreeCell = NULL;

	return pFirstCell;
}

void CSudokuGame::FindAllSolutions( CCell* pCell )
{
	//number of solutions is in m_SolutionCnt
	for( int v = 1 ; v < VAL_CNT_1 ; v++ )
	{
		if( !pCell->IsFree( v , NULL , false ) )
			continue;

		pCell->SetVal( v );
		
		if( pCell->m_pNextFreeCell )
		{
			FindAllSolutions( pCell->m_pNextFreeCell );
			pCell->Erase();
		}
		else
		{
			m_SolutionCnt++;
			pCell->Erase();
			return;
		}
	}
}

bool CSudokuGame::FindOneSolution( CCell* pCell )
{
	//returns after the first solution is found
	for( int v = 1 ; v < VAL_CNT_1 ; v++ )
	{
		if( !pCell->IsFree( v , NULL , false ) )
			continue;

		pCell->SetVal( v );
		
		if( pCell->m_pNextFreeCell )
		{
			if( FindOneSolution( pCell->m_pNextFreeCell ) )
				return true;
			pCell->Erase();
		}
		else
			return true;
	}

	return false;
}

int CSudokuGame::Do67GroupForbids( int level , int CurFreeCellCnt )
{
	//returns true if a number found using conditions with already has 6-7 values set
	//in a general view this is not a special solution method, 
	//but for user it is much easier to see, if only a few cells are free.

	int FoundCellCnt = 0;

	CForbidAction*	pAction = new CForbidAction;

	//fill the list of forbidden values
	CCondition* pCond, *pCondToForbid;
	int			FreeCellCnt;
	bool		bCol;
	int			i, j;
	CCell*		pCell;

	int size = m_MovesToMake.size();

	for( i = 0 ; i < 3 ; i++ )
	{
		for( j = 0 ; j < VAL_CNT ; j++ )
		{
			pCond = GetCond( i , j );
			if( FieldDone( j , i ) )
				continue;//forbid already done. don't do 2x

			FreeCellCnt = pCond->GetFreeCellCnt();
			if( FreeCellCnt < 2 || FreeCellCnt > 3 )
				continue;

			if( pCond->GetFreePartCondCnt( bCol ) != 1 )
				continue;

			pCell = pCond->GetFirstFreeCell();
			if( i == 0 )
				pCondToForbid = bCol ? pCell->m_Conditions[1] : pCell->m_Conditions[2];
			else
				pCondToForbid = pCell->m_Conditions[0];
			
			pCond->GetFreeValues( pAction->ForbiddenValues );
			pCondToForbid->GetFreeCells( pAction->CellsToForbid , &pAction->ForbiddenValues );
			if( !pAction->DoForbid( CurFreeCellCnt ) )
				continue;//No new information. Forbid is implicitly already made before.

			FoundCellCnt = SimpleSolve( CurFreeCellCnt , NULL );
			if( FoundCellCnt == -1 )
			{
				delete pAction;
				return -1;
			}
			if( FoundCellCnt )
			{
				CToPlay* pToPlay = m_MovesToMake[size];
				pToPlay->SetAction( pAction );
				CString str;
				str.Format( " (Verbot für '%s' in %s)",  (LPCTSTR)pAction->GetForbiddenValStr() , pCondToForbid->GetDesc() );
				pToPlay->GetDesc() += str;
				SetFieldDone( j , i );
				return FoundCellCnt;
			}
			
			pAction->UndoForbid( CurFreeCellCnt );			
		}
	}

	delete pAction;
	return 0;
}

int	CSudokuGame::DoGroupForbids( int CurFreeCellCnt , bool& bForbidDone )
{
	bForbidDone = false;

	CCondition* pCond;
	int i, j;
	int	nDoneForbids = 0;
	for( i = 0 ; i < 3 ; i++ )
	{
		for( j = 0 ; j < VAL_CNT ; j++ )
		{
			pCond = &m_CellField.m_Conditions[i][j];
			bForbidDone = pCond->DoGroupForbid( CurFreeCellCnt );
			if( bForbidDone )
				break;
		}
	}

	if( !bForbidDone )
		return 0;

	int CurFoundCells = OnlyOneCellLeft();
	if( CurFoundCells )
		return CurFoundCells;//contradiction

	return OnlyOneValLeft();
}

int CSudokuGame::DoIndirectConclusion( int level , int CurFreeCellCnt , bool& bForBidDone )
{
	//returns FoundCellCnt using indirect conclusion
	//the logic of the following implementation hardcodes HOR_CELL_CNT = VER_CELL_CNT = 3;
	//if not we would have not theSetRow/Col, the forbiddenRow/col, ...

	/*old implementation:
	//do it for cols:
	int FoundCellCnt = DoIndConclColRow( level > 5 , CurFreeCellCnt , true , level < 6 , bForBidDone );
	if( FoundCellCnt != 0 )
		return FoundCellCnt;

	//do it for rows:
	return DoIndConclColRow( level > 5 , CurFreeCellCnt , false , level < 6 , bForBidDone );*/

	CCondition*		pCond, *pOtherCond;
	sConditionPart* pPartCond;
	int				i, j, k, r, v;
	CCell*			pCell, *pOtherCell;
	bool			bWithForbidden = level > 5;
	bool			bForbidDone = false;

	for( i = 0 ; i < 3 ; i++ )
	{
		for( j = 0 ; j < VAL_CNT ; j++ )
		{
			pCond = GetCond( i , j );
			for( v = 1 ; v < VAL_CNT_1 ; v++ )
			{
				if( !pCond->IsFree( v ) )
					continue;

				pPartCond = pCond->GetOnlyFreePartCond( v , bWithForbidden );
				if( !pPartCond )
					continue;
				
				pOtherCond = pPartCond->pCond;
				for( k = 0 ; k < VAL_CNT ; k++ )
				{
					pOtherCell = pOtherCond->m_Cells[k];
					if( !pOtherCell->IsFree( v , NULL , bWithForbidden ) )
						continue;

					for( r = 0 ; r < 3 ; r++ )
					{
						if( pPartCond->m_pCells[r] == pOtherCell )
							break;
					}

					if( r < 3 )
						continue;

					if( pOtherCell->SetForbidden( v , CurFreeCellCnt ) )
						bForbidDone = true;
				}
			}
		}
	}

	return 0;
}

int CSudokuGame::DoForbid( CForbidAction* pAction , CCondition* pCond , int v , int PartIdx , bool bCol , int CurFreeCellCnt , bool bSimulation , bool& bForBidDone )
{
	int size = m_MovesToMake.size();

	pCond->GetFreeCells( pAction->CellsToForbid , v , PartIdx , bCol );

	if( !pAction->DoForbid( CurFreeCellCnt ) )
	{
		//No new information. Forbid is explicitly or implicitly already made before.
		bForBidDone = false;
		return 0;
	}

	bForBidDone = true;

	int FoundCellCnt = SimpleSolve( CurFreeCellCnt , NULL );
	if( FoundCellCnt == -1 )
	{
		return -1;
	}
	if( FoundCellCnt )
	{
		CToPlay* pToPlay = m_MovesToMake[size];
		pToPlay->SetAction( pAction );
		CString str;
		if( PartIdx == -1 )
			str.Format( " (Verbot für '%s' in %s)",  (LPCTSTR)pAction->GetForbiddenValStr() , pCond->GetDesc() );
		else
		{
			CString t = bCol ? "Spalte" : "Reihe";
			int idx = pAction->GetForbiddenColRow( bCol ) % 3;
			CString p = ( idx == 0 ) ? "ersten" : ( ( idx == 1 ) ? "zweiten" : "dritten" );
			str.Format( " (Verbot für '%s' in der %s %s in %s )",  (LPCTSTR)pAction->GetForbiddenValStr() , (LPCTSTR)p , (LPCTSTR)t , pCond->GetDesc() );
		}
		pToPlay->GetDesc() += str;
		return FoundCellCnt;
	}
	
	if( bSimulation )
		pAction->UndoForbid( CurFreeCellCnt );

	return 0;
}

bool CSudokuGame::FillGame()
{
	//return DoComputerSolution() > 0;

	//create the numbers [1;2;...;9] in random order:
	int		val[9];
	int		i, r, t;
	for( i = 0; i < 9 ; i++ )
	{
		val[i] = i + 1;
	}

	for( i = 8 ; i >= 0 ; i-- )
	{
		r = random64( i + 1 );
		t = val[i];
		val[i] = val[r];
		val[r] = t;
	}

	CRandomFillVal	Filler[9];

	CRandomFillVal* f;
	for( i = 0; i < 9 ; i++ )
	{
		f = &Filler[i];
		f->m_Depth = i;
		f->m_Val = val[i];
		f->m_pNext = i == 8 ? NULL : &Filler[i+1];
		f->m_pPrev = i == 0 ? NULL : &Filler[i-1];
	}

	CRandomFillVal::ResetCells();

	for( i = 0; i < VAL_CNT ; i++ )
	{
		Filler[i].Init();
	}

	C3Rows* pFirstRow = &Filler[0].m_3Rows[0];
	if( !pFirstRow->Fill() )
	{
		CRandomFillVal::ResetCells();
		return false;
	}

	Reset();

	int j;
	CCell* pCell;
	for( i = 0; i < COL_CNT ; i++ )
	{
		for( j = 0; j < ROW_CNT ; j++ )
		{
			pCell = GetCell( i , j );
			pCell->SetVal( CRandomFillVal::m_Cells[i][j].GetVal() );
			pCell->ClearGroups();
		}	
	}

	return true;
}

bool CSudokuGame::CreateGame( int level, int* pTargetCellsCnt /*= NULL*/ )
{
	for(;;)
	{
		if( !FillGame() )//fill the 81-cells by random
			return false;

		/*#ifdef _DEBUG
			m_CellField.TraceCells();
		#endif*/

		BuildGroups();//find the most probable unresolvable substructures in order to increase performance of RandomRemoval()

		int n = 81;
		if( pTargetCellsCnt )
			n = __min( COL_CNT * ROW_CNT - *pTargetCellsCnt , n );

		if( RandomRemoval( level , n ) == level )
			break;

		DeleteGroups();
	}

	SetAsInitialPos();
	DeleteGroups();
	return true;
}

void CSudokuGame::BuildGroups()
{
	/*this function searches for the most often occuring groups.
	A group is substructure which is not resolvable, if no field of the group is set.
	We seach for the 4-Group( µ = 9 for a 9x9-game ):	a	b
														b	a
	and for the 6-Group( µ = 3 for a 9x9-game ):		a	c	b
														b	a	c
	There are other groups too. But it is not task of this function to search for all possible groups. 
	We only want search for the most often occuring groups in order to recognize enough forbidden cells for random removal.
	It is enough, if the probability to produce an ambigous result during random-removal is kept small.*/

	m_Groups.clear();//groups for CreateGame

	int			i, j, k, n, r, s;
	int			v, w, x;
	CCell*		pCell;
	CCell*		pCell2 , *pCell3 , *pCell4, *pCell5, *pCell6;
	int			c0;
	int*		pGroup;

	for( i = 0 ; i < 6 ; i++ )//6 = COL_CNT/ROW_CNT - 3 
	{
		if( i%3 == 2 )
			continue;

		c0 = 3 * ( i / 3 );
		s = 3 + 2*c0 - i;
		for( j = 0 ; j < 8 ; j++ )//6 = COL_CNT/ROW_CNT - 3 
		{
			pCell = GetCell( i , j );
			v = pCell->GetVal();
			for( k = c0 + 1 ; k < s ; k++ )
			{
				pCell2 = GetCell( k , j );
				w = pCell2->GetVal();
				for( n = j+1 ; n < ROW_CNT ; n++ )
				{
					pCell3 = GetCell( k , n );
					if( pCell3->GetVal() == v )
						break;
				}
				if( n == ROW_CNT )
					continue;

				pCell4 = GetCell( i , n );
				x = pCell4->GetVal();
				if( x == w )
				{
					//is 4-group
					pGroup = new int;
					*pGroup = 4;
					pCell->AddGroup( pGroup );
					pCell2->AddGroup( pGroup );
					pCell3->AddGroup( pGroup );
					pCell4->AddGroup( pGroup );
					m_Groups.push_back( pGroup );//store it for deletion later
				}
				else if( i < 3 )//3 = COL_CNT/ROW_CNT - 6 
				{
					//search for the 6-group descibed above:
					for( r = j+1 ; r < ROW_CNT ; r++ )
					{
						pCell5 = GetCell( i , r );
						if( pCell5->GetVal() == w )
							break;
					}

					if( r == ROW_CNT )
						continue;

					pCell6 = GetCell( k , r );
					if( pCell6->GetVal() == x )
					{
						//is A-6-group
						pGroup = new int;
						*pGroup = 6;
						pCell->AddGroup( pGroup );
						pCell2->AddGroup( pGroup );
						pCell3->AddGroup( pGroup );
						pCell4->AddGroup( pGroup );
						pCell5->AddGroup( pGroup );
						pCell6->AddGroup( pGroup );
						m_Groups.push_back( pGroup );//store it for deletion later
					}
				}
			}
		}
	}

	int r0;
	for( j = 0 ; j < 6 ; j++ )//6 = COL_CNT/ROW_CNT - 3 
	{
		if( j%3 == 2 )
			continue;

		r0 = 3 * ( j / 3 );
		s = 3 + 2*r0 - j;
		for( i = 0 ; i < 8 ; i++ )//6 = COL_CNT/ROW_CNT - 3 
		{
			pCell = GetCell( i , j );
			v = pCell->GetVal();
			for( k = r0 + 1 ; k < s ; k++ )
			{
				pCell2 = GetCell( i , k );
				w = pCell2->GetVal();
				for( n = i+1 ; n < COL_CNT ; n++ )
				{
					pCell3 = GetCell( n , k );
					if( pCell3->GetVal() == v )
						break;
				}
				if( n == COL_CNT )
					continue;

				pCell4 = GetCell( n , j );
				x = pCell4->GetVal();
				if( x == w )
				{
					//is 4-group
					pGroup = new int;
					*pGroup = 4;
					pCell->AddGroup( pGroup );
					pCell2->AddGroup( pGroup );
					pCell3->AddGroup( pGroup );
					pCell4->AddGroup( pGroup );
					m_Groups.push_back( pGroup );//store it for deletion later
				}
				else if( j < 3 )//3 = COL_CNT/ROW_CNT - 6 
				{
					//search for the 6-group descibed above:
					for( r = i+1 ; r < COL_CNT ; r++ )
					{
						pCell5 = GetCell( r , j );
						if( pCell5->GetVal() == w )
							break;
					}

					if( r == COL_CNT )
						continue;

					pCell6 = GetCell( r , k );
					if( pCell6->GetVal() == x )
					{
						//is A-6-group
						pGroup = new int;
						*pGroup = 6;
						pCell->AddGroup( pGroup );
						pCell2->AddGroup( pGroup );
						pCell3->AddGroup( pGroup );
						pCell4->AddGroup( pGroup );
						pCell5->AddGroup( pGroup );
						pCell6->AddGroup( pGroup );
						m_Groups.push_back( pGroup );//store it for deletion later
					}
				}
			}
		}
	}
}

int CSudokuGame::RandomRemoval( int level , int TargetRemoveCnt )
{
	//successivly removes values from an initially full game.
	//after each removal it is tested, whether the sudoku is still solvable(NumberOfSolutions == 1) within the given level.
	//the functions ends, if no more cell can be removed or the TargetRemoveCnt is reached.
	int		rNum;
	int		nRemoved = 0;
	CCell*	pCell;
	int		NumberOfSolutions;
	int		i, j, v;
	CCell*	Cells[CELL_CNT];
	int		MaxUsedLevel = 0;
	int		CurUsedLevel;
	int		FoundCellCnt = 1;

	int		CellCnt = CELL_CNT;
	for( i = 0 ; i < COL_CNT ; i++ )
	{
		for( j = 0 ; j < ROW_CNT ; j++ )
		{
			Cells[ COL_CNT * i + j ] = GetCell( i , j );
		}
	}

	bool	bDoBruteForce = level > MAX_HUMAN_LEVEL;

	while( nRemoved < TargetRemoveCnt && CellCnt > 0 )
	{
		rNum = random64( CellCnt );
		pCell = Cells[rNum];
		CellCnt--;

		if( !pCell->IsNeeded() )
		{
			v = pCell->GetVal();
			pCell->Erase();//do remove for test
			nRemoved++;

			#ifdef _DEBUG
				m_CellField.TraceCells();
			#endif

			if( bDoBruteForce )
				NumberOfSolutions = DoComputerSolution( false , true );
			else
			{
				FoundCellCnt = Solve( level , CurUsedLevel , NumberOfSolutions , true );
				if( FoundCellCnt == nRemoved && NumberOfSolutions )
					MaxUsedLevel = __max( MaxUsedLevel , CurUsedLevel );
			}

			if( NumberOfSolutions == 0 )
			{
				ASSERT(0);//bug
				return 0;
			}

			if( NumberOfSolutions == 1 && ( bDoBruteForce || FoundCellCnt == nRemoved ) )
			{
				//confirm removal:
				pCell->DecrementGroups();
			}
			else
			{
				//undo removal:
				pCell->SetVal( v );
				nRemoved--;
			}
		}
		else
		{
			#ifdef _DEBUG
				int t = 3;//only to be able to set a breakpoint
				t++;
			#endif
		}

		Cells[rNum] = Cells[CellCnt];
		Cells[CellCnt] = pCell;
	}

	return bDoBruteForce ? level : MaxUsedLevel;
}

void CSudokuGame::DeleteGroups()
{
	std::list<int*>::iterator it;
	for( it = m_Groups.begin() ; it != m_Groups.end() ; it++ )
	{
		delete *it;
	}

	m_Groups.clear();
}

void CSudokuGame::SetAsInitialPos()
{
	int		i, j, v;
	CCell*	pCell;
	for( i = 0 ; i < COL_CNT ; i++ )
	{
		for( j = 0 ; j < ROW_CNT ; j++ )
		{
			pCell = GetCell( i , j );
			if( pCell )
			{
				v = pCell->GetVal();
				pCell->SetInitial( v != 0 );
			}
			else
				v = 0;

			m_StartPosition[i][j] = v;
		}
	}
}

void CSudokuGame::ResetToInitialPos()
{
	int		i, j, v;
	CCell*	pCell;
	for( i = 0 ; i < COL_CNT ; i++ )
	{
		for( j = 0 ; j < ROW_CNT ; j++ )
		{
			pCell = GetCell( i , j );
			if( pCell )
			{
				v = m_StartPosition[i][j];
				if( v )
					pCell->SetVal( v );
				else
					pCell->Erase();

				pCell->SetInitial( v != 0 );
			}
		}
	}
}
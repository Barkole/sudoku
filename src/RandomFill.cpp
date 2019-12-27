// RandomFill.cpp: implementation of the CRandomFill class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sudoku.h"
#include "Cell.h"
#include "RandomFill.h"
#include "helpfunct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*static*/ CCell	CRandomFillVal::m_Cells[COL_CNT][ROW_CNT];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNextCell::CNextCell() : m_pCell(NULL), m_pNextCells(NULL)
{
}

CNextCell::~CNextCell()
{
	delete m_pNextCells;
}

CNextCellList::CNextCellList() : cnt(0), m_pNCells(NULL)
{
}

void CNextCellList::SetSize( int s )
{
	if( cnt )
		delete[] m_pNCells;

	if( s )
	{
		cnt = s;
		m_pNCells = new CNextCell*[cnt];
	}
	else
		m_pNCells = NULL;

}

CNextCellList::~CNextCellList()
{
	if( cnt )
	{
		int i;
		for( i = 0 ; i < cnt ; i++ )
		{
			delete m_pNCells[i];//delete the content if ith cell
		}
	}
	delete[] m_pNCells;//delete the array itself
}

C3Rows::C3Rows()
{
	memset( m_pCells , 0 , 9 * sizeof( CNextCell* ) );
	m_Idx = 0;
	m_pNext = NULL;
	m_Val = 0;
	m_pUsedCols = NULL;
}

C3Rows::~C3Rows()
{
	for( int i = 0 ; i < 9 ; i++ )
	{
		delete m_pCells[i];
	}
}

void C3Rows::Init()
{
	//the idea of all this classes (CNextCell, CNextCellList, C3Row) and the following complex initialisation is to 
	//move "forbidden"-logic out of the actual backtracking algorithm. So the algorithm will get easier and faster.
	//The price is that we need a bit more memory and this init-function must be implemented.
	//the following code hardcodes that we have a 3x3 sudoku by logic. It massively uses that 3-2 == 1, so we need no lists....

	int i, j, k , n;
	CNextCell*		pNCell;
	CNextCellList*	pList2[9][6];//3x3x6 3 elemental last rows 3(i/3) x 3(i%3) x 6(j)
	CNextCellList*	pList;
	int row;
	int col;

	row = 3 * m_Idx;//first row
	for( i = 0 ; i < 9 ; i++ )
	{
		pNCell = new CNextCell;
		pNCell->m_pCell = &CRandomFillVal::m_Cells[i][row];
		pNCell->m_pNextCells = new CNextCellList;
		pNCell->m_pNextCells->SetSize( 6 );
		m_pCells[i] = pNCell;
	}

	row = 3 * m_Idx + 2;//last row
	for( i = 0 ; i < 9 ; i++ )
	{
		col = 3 * ( i/3 );
		for( j = 0 ; j < 6 ; j++ )
		{
			if( j > 2 )
				k = 3 * ( ( ( col + j ) % 9 ) / 3 );
			else
				k = 3 * ( ( ( col + j + 6 ) % 9 ) / 3 );
			pList = new CNextCellList;
			pList->SetSize( 3 );
			for( n = 0 ; n < 3 ; n++ )
			{
				pNCell = new CNextCell;
				pNCell->m_pCell = &CRandomFillVal::m_Cells[k+n][row];
				pList->m_pNCells[n] = pNCell;
			}
			Shuffle( (void**)pList->m_pNCells , 3 );
			pList2[i][j] = pList;
		}
	}

	row = 3 * m_Idx + 1;//Middle row
	int jdx;
	for( i = 0 ; i < 9 ; i++ )
	{
		col = 3 * ( i/3 ) + 3;
		pList = m_pCells[i]->m_pNextCells;
		for( j = 0 ; j < 6 ; j++ )
		{
			pNCell = new CNextCell;
			jdx = ( col + j ) % 9;
			pNCell->m_pCell = &CRandomFillVal::m_Cells[jdx][row];
			pNCell->m_pNextCells = pList2[i][j];
			pList->m_pNCells[j] = pNCell;
		}
		Shuffle( (void**)pList->m_pNCells , 6 );
	}

	Shuffle( (void**)m_pCells , 9 );

	/*#ifdef _DEBUG
		CNextCell*		pNCell2;
		CNextCellList*	pList3;
		for( i = 0 ; i < 9 ; i++ )
		{
			pNCell = m_pCells[i];
			::AfxTrace( "\n [%d][%d]" , pNCell->m_pCell->m_Col , pNCell->m_pCell->m_Row );
			pList = pNCell->m_pNextCells;
			for( j = 0 ; j < 6 ; j++ )
			{
				pNCell2 = pList->m_pNCells[j];
				::AfxTrace( "\n\t [%d][%d]" , pNCell2->m_pCell->m_Col , pNCell2->m_pCell->m_Row );
				pList3 = pNCell2->m_pNextCells;
				for( k = 0 ; k < 3 ; k++ )
				{
					::AfxTrace( "\n\t\t [%d][%d]" , pList3->m_pNCells[k]->m_pCell->m_Col , pList3->m_pNCells[k]->m_pCell->m_Row );
				}
			}
		}
	#endif*/
}

bool C3Rows::Fill()
{
	int				i, j, k;
	CNextCell*		pNCell;
	CCell*			pCell_i, *pCell_j, *pCell_k;
	int				col;
	C3Rows*			pNext = m_pNext;
	CNextCellList*	pNextCells_j, *pNextCells_k;

	for( i = 0 ; i < 9 ; i++ )
	{
		pNCell = m_pCells[i];
		pCell_i = pNCell->m_pCell;

		if( !pCell_i->IsFree() )
			continue;

		col = pCell_i->m_Col;
		if( m_pUsedCols[col] )
			continue;

		pCell_i->m_Val = m_Val;
		m_pUsedCols[col] = 1;
		//CRandomFillVal::TraceCells();

		pNextCells_j = pNCell->m_pNextCells;
		for( j = 0 ; j < 6 ; j++ )
		{
			pNCell = pNextCells_j->m_pNCells[j];
			pCell_j = pNCell->m_pCell;

			if( !pCell_j->IsFree() )
				continue;

			col = pCell_j->m_Col;
			if( m_pUsedCols[col] )
				continue;

			//play:
			pCell_j->m_Val = m_Val;
			m_pUsedCols[col] = 1;
			//CRandomFillVal::TraceCells();
			
			pNextCells_k = pNCell->m_pNextCells;
			for( k = 0 ; k < 3 ; k++ )
			{
				pNCell = pNextCells_k->m_pNCells[k];
				pCell_k = pNCell->m_pCell;

				if( !pCell_k->IsFree() )
					continue;

				col = pCell_k->m_Col;
				if( m_pUsedCols[col] )
					continue;

				pCell_k->m_Val = m_Val;
				m_pUsedCols[col] = 1;
				//CRandomFillVal::TraceCells();
				
				if( !pNext || pNext->Fill() )
					return true;//success!

				//undo play:
				pCell_k->m_Val = 0;
				m_pUsedCols[col] = 0;
			}

			//undo play:
			pCell_j->m_Val = 0;
			m_pUsedCols[pCell_j->m_Col] = 0;
		}

		//undo play:
		pCell_i->m_Val = 0;
		m_pUsedCols[pCell_i->m_Col] = 0;
	}

	return false;
}

CRandomFillVal::CRandomFillVal()
{
	m_Val = 0;
	m_pPrev = NULL;
	m_pNext = NULL;
	m_Depth = 0;
}

CRandomFillVal::~CRandomFillVal()
{
}

void CRandomFillVal::Init()
{
	C3Rows* p3Row;
	memset( m_UsedCols , 0 , 9 * sizeof( int ) );
	for( int i = 0 ; i < 3 ; i++ )
	{
		p3Row = &m_3Rows[i];
		p3Row->m_Idx = i;
		if( i < 2 )
			p3Row->m_pNext = &m_3Rows[i+1];
		else
			p3Row->m_pNext = m_pNext ? &m_pNext->m_3Rows[0] : NULL;

		p3Row->m_Val = m_Val;//copy from CRandomFill
		p3Row->m_pUsedCols = m_UsedCols;//copy from CRandomFill
		p3Row->Init();
	}
}

/*static*/ void	CRandomFillVal::ResetCells()
{
	int		i, j;
	for( i = 0 ; i < 9 ; i++ )
	{
		for( j = 0 ; j < 9 ; j++ )
		{
			m_Cells[i][j].m_Col = i;
			m_Cells[i][j].m_Row = j;
			m_Cells[i][j].Erase();
		}
	}
}

#ifdef _DEBUG
/*static*/ void	CRandomFillVal::TraceCells()
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
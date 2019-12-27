// Condition.cpp: implementation of the CCondition class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sudoku.h"
#include "Cell.h"
#include "Condition.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCondition::CCondition()
{
	ZeroMemory( m_Cells , VAL_CNT * sizeof( CCell* ) );
}

CCondition::~CCondition()
{
	std::vector<CPartConditions*>::iterator it;
	for( it = m_arPartConditions.begin() ; it != m_arPartConditions.end() ; it++ )
	{
		delete *it;
	}
}

CCell*	CCondition::OnlyOneCellLeft( int v )
{
	if( !IsFree( v ) )
	{
		ASSERT(0);//don't call for already set values.
		return NULL;
	}

	CCell* pCell;
	CCell* pTheCell = (CCell*)INVALID_PTR;
	int		PossibleValCnt = 0;
	for( int i = 0 ; i < VAL_CNT ; i++ )
	{
		pCell = m_Cells[i];
		if( !pCell->IsFree() )
		{
			ASSERT( pCell->m_Val == i+1 );
			continue;//val is already set
		}

		if( pCell->IsFree( v , this , true ) )
		{
			PossibleValCnt++;
			if( PossibleValCnt > 1 )
				return NULL;
			else
				pTheCell = pCell;
		}
	}

	return pTheCell;
}

void CCondition::SetVal( CCell* pCell , int v )
{
	//Initially the fields are unsorted, 
	//but whenever we set a field with value v, we move that field to m_Cells[v-1]
	for( int i = 0 ; i < VAL_CNT ; i++ )
	{
		if( pCell == m_Cells[i] )
		{
			if( i == v-1 )
				return;
			
			//swap
			m_Cells[i] = m_Cells[v-1];
			m_Cells[v-1] = pCell;
			return;
		}
	}
}
	
int CCondition::GetFreeCellCnt()
{
	int			i;
	int			FreeCellCnt = 0;

	for( i = 0 ; i < VAL_CNT ; i++ )
	{
		if( m_Cells[i]->m_Val == 0 )
			FreeCellCnt++;
	}

	return FreeCellCnt;
}

bool sConditionPart::IsFull()
{
	//is full with numbers.
	for( int i = 0 ; i < COLS_IN_FIELD ; i++ )
	{
		if( m_pCells[i]->IsFree() )
			return false;
	}
	return true;
}

bool sConditionPart::IsFull( int v, bool bWithForbids )
{
	//is full for value v.
	for( int i = 0 ; i < COLS_IN_FIELD ; i++ )
	{
		if( m_pCells[i]->IsFree( v , NULL , bWithForbids ) )
			return false;
	}
	return true;
}

int	CCondition::GetFreePartCondCnt( bool& bCol )
{
	//bCol is only valid, if the condition is a field
	bCol = false;

	int PartCondsSets = m_arPartConditions.size();

	int					FreePartConds = 3;
	CPartConditions*	pPartConditions;
	int cur;

	for( int i = 0 ; i < PartCondsSets ; i++ )
	{
		pPartConditions = m_arPartConditions[i];
		cur = pPartConditions->GetFreePartCondCnt();
		if( cur < FreePartConds )
		{
			FreePartConds = cur;
			bCol = (i == 0);//using that i == 0 are columns for fields
		}
	}

	return FreePartConds;
}

CCell* CCondition::GetFirstFreeCell()
{
	for( int i = 0 ; i < VAL_CNT ; i++ )
	{
		if( m_Cells[i]->IsFree() )
			return m_Cells[i];
	}

	return NULL;
}

void CCondition::GetFreeValues( std::vector<sValCellPair>& FreeValues )
{
	int i;

	FreeValues.clear();
	for( i = 0 ; i < VAL_CNT ; i++ )
	{
		if( m_Cells[i]->m_Val == 0  )
		{
			sValCellPair pair;
			pair.v = i+1;
			pair.pCell = m_Cells[i];
			FreeValues.push_back( pair );
		}
	}
}

void CCondition::GetFreeCells( std::vector<CCell*>& FreeCells , std::vector<sValCellPair>* pNotThisOnes )
{
	FreeCells.clear();

	if( pNotThisOnes && pNotThisOnes->size() > 0 )
	{
		std::vector<sValCellPair>::iterator it;
		CCell* pCell;
		for( int i = 0 ; i < VAL_CNT ; i++ )
		{
			pCell = m_Cells[i];
			if( pCell->IsFree() )
			{
				for( it = pNotThisOnes->begin() ; it != pNotThisOnes->end() ; it++ )
				{
					sValCellPair& pair = *it;
					if( pair.pCell == pCell )
						break;
				}

				if( it == pNotThisOnes->end() )
					FreeCells.push_back( pCell );
			}
		}
	}
	else
	{
		for( int i = 0 ; i < VAL_CNT ; i++ )
		{
			if( m_Cells[i]->IsFree() )
				FreeCells.push_back( m_Cells[i] );
		}
	}
}

void CCondition::GetFreeCells( std::vector<CCell*>& FreeCells , int ForbiddenValue , int NotPartIdx , bool bCol )
{
	FreeCells.clear();

	if( NotPartIdx < 0 || NotPartIdx > 2 )
	{
		ASSERT(0);
		return;
	}

	int		PartCondsSets = m_arPartConditions.size();
	int		idx;

	if( PartCondsSets > 1 )
		idx = bCol ? 0 : 1;
	else
		idx = 0;

	int		i, j;
	CPartConditions* pPartConditions = m_arPartConditions[idx];
	for( i = 0 ; i < COLS_IN_FIELD ; i++ )
	{
		if( i == NotPartIdx )
			continue;

		sConditionPart& part = pPartConditions->m_PartConditions[i];
		for( j = 0 ; j < COLS_IN_FIELD ; j++ )
		{
			if( part.m_pCells[j]->IsFree( ForbiddenValue , NULL , true ) )
				FreeCells.push_back( part.m_pCells[j] );
		}
	}
}

int CCondition::AddFreeCells( int v , CCell** ppFreeCells , int CellIdx , int CellCnt )
{
	int			i;
	CCell*		pCell;
	int			FreeCellCnt = CellIdx;//assuming that the cells with idx < CellIdx are not free for other values.
	for( i = CellIdx ; i < CellCnt ; i++ )
	{
		 pCell = ppFreeCells[i];
		 if( pCell->IsFree( v , NULL , true ) )
		 {
			 if( i != FreeCellCnt )
			 {
				//swap:
				ppFreeCells[i] = ppFreeCells[FreeCellCnt];
				ppFreeCells[FreeCellCnt] = pCell;
			 }
			 FreeCellCnt++;
		 }
	}

	return FreeCellCnt;
}

int	CPartConditions::GetFreePartCondCnt()
{
	int FreePartConds = 0;
	for( int i = 0 ; i < 3 ; i++ )
	{
		if( !m_PartConditions[i].IsFull() )
			FreePartConds++;
	}
	return FreePartConds;
}

sConditionPart* CCondition::GetOnlyFreePartCond( int v , bool bWithForbids )
{
	int					PartCondCnt = m_arPartConditions.size();
	std::vector<CPartConditions*>::iterator it;
	CPartConditions*	pPartConditions;
	sConditionPart*		pConditionPart;

	for( it = m_arPartConditions.begin() ; it != m_arPartConditions.end() ; it++ )
	{
		pPartConditions = *it;
		pConditionPart = pPartConditions->GetOnlyFreePartCond( v , bWithForbids );
		if( pConditionPart )
			return pConditionPart;
	}

	return NULL; 
}

sConditionPart*	CPartConditions::GetOnlyFreePartCond( int v , bool bWithForbids )
{
	sConditionPart* pConditionPart = NULL;
	for( int i = 0 ; i < 3 ; i++ )
	{
		if( !m_PartConditions[i].IsFull( v , bWithForbids ) )
		{
			if( pConditionPart == NULL )
				pConditionPart = &m_PartConditions[i];
			else
				return NULL;
		}
	}
	return pConditionPart;
}

bool CCondition::DoGroupForbid( int CurFreeCellCnt )
{//returns true is  a forbid was made

	std::vector<sValCellPair> FreeValCells;
	GetFreeValues( FreeValCells );
	int FreePlaces =  FreeValCells.size();
	if( FreePlaces < 4 )
		return false;

	//work with static arrays to avoid performance-killing permanent allocating/deleting of lists:
	int i, j, k, r;
	CCell**	pFreeCells = new CCell*[FreePlaces];
	int*	FreeVals = new int[FreePlaces];
	for( i = 0 ; i < FreePlaces ; i++ )
	{
		pFreeCells[i] = FreeValCells[i].pCell;
		FreeVals[i] = FreeValCells[i].v;
	}

	bool bRet = false;

	//search for 2-elemental groups first:
	//2.1: group because rest of cells is forbidden
	/*for all pairs (v, w ) of free values:
		if( GetFreeCells( v , w ) == 2 )
			Set forbidden in this 2 free cells all values of the complement of (v,w).	
	Example:
	? ? ?
	? ? ?
	? ? 9
	-----
	1 2
	2 1
	The right 2 cells build a group for the values 1 and 2.*/
	int FreeCellCnt;
	CCell* pCell;
	int v, w;
	for( i = 0 ; i < FreePlaces-1 ; i++ )
	{
		v = FreeValCells[i].v;
		FreeCellCnt = AddFreeCells( v , pFreeCells , 0 , FreePlaces );
		if( FreeCellCnt > 2 )
			continue;

		for( j = i+1 ; j < FreePlaces ; j++ )
		{
			w = FreeValCells[j].v;
			if( AddFreeCells( w , pFreeCells , FreeCellCnt , FreePlaces ) == 2 )
			{
				//group found.
				//Set forbidden in this 2 free cells all values of the complement of (v,w):
				for( k = 0 ; k < 2 ; k++ )
				{
					pCell = pFreeCells[k];
					for( r = 0 ; r < FreePlaces ; r++ )
					{
						if( r == i || r == j )
							continue;

						if( pCell->IsFree( FreeValCells[r].v , NULL , true ) )
						{
							if( pCell->SetForbidden( FreeValCells[r].v , CurFreeCellCnt ) )
								bRet = true;//return true, if indeed a new forbid was found
						}
					}
				}

				if( bRet )
				{
					delete[] pFreeCells;
					delete[] FreeVals;
					return true;//one forbid is enough for this round
				}
			}
		}		
	}

	if( FreePlaces < 5 )
		return bRet;//if FreePlaces == 4, we get the same as in 2.1 

	//2.2: group because rest of values is forbidden
	/*for all pairs (pCell, pCell2) of free cells:
		if( GetFreeValues( pCell , pCell2 ) == 2 )
			Set forbidden in this 2 free values in the complement of (pCell, pCell2).	
	Example:
	7 ? ? | 4
	? 8 ? |	4
	? ? 9 
	-----
		1
		2
		3
	The right 2 cells build a group for the values 5 and 6.*/
	CCell*	pCell2;
	int		FreeValCnt;
	for( i = 0 ; i < FreePlaces ; i++ )
	{
		pCell = FreeValCells[i].pCell;
		FreeValCnt = pCell->AddFreeValues( FreeVals , 0 , FreePlaces );
		if( FreeValCnt > 2 )
			continue;

		for( j = i+1 ; j < FreePlaces ; j++ )
		{
			pCell2 = FreeValCells[j].pCell;
			if( pCell2->AddFreeValues( FreeVals , FreeValCnt , FreePlaces ) == 2 )
			{
				//group found.
				//Set forbidden in this 2 free cells all values of the complement of (v,w):
				for( k = 0 ; k < 2 ; k++ )
				{
					v = FreeVals[k];
					for( r = 0 ; r < FreePlaces ; r++ )
					{
						if( r == i || r == j )
							continue;

						if( FreeValCells[r].pCell->IsFree( v , NULL , true ) )
						{
							if( FreeValCells[r].pCell->SetForbidden( v , CurFreeCellCnt ) )
								bRet = true;//return true, if indeed a new forbid was found
						}
					}
				}

				if( bRet )
				{
					delete[] pFreeCells;
					delete[] FreeVals;
					return true;//one forbid is enough for this round
				}
			}
		}		
	}

	delete[] pFreeCells;
	delete[] FreeVals;
	return false;
}
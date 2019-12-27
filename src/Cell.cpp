// Cell.cpp: implementation of the CCell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sudoku.h"
#include "Cell.h"
#include "Condition.h"
#include "helpfunct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCell::CCell()
{
	m_Val = 0;
	m_Col = -1;
	m_Row = -1;
	m_pNextFreeCell = NULL;
	m_Conditions[0] = NULL;
	m_Conditions[1] = NULL;
	m_Conditions[2] = NULL;

	ZeroMemory( m_Forbidden , (VAL_CNT_1) * sizeof(int) );

	m_pNextFreeCell = NULL;
	m_bInitial = false;
}

CCell::~CCell()
{
}

bool CCell::IsFree( int v , CCondition* pTheCond , bool bWithForbidden )
{
	if( m_Val )
		return m_Val == v;

	if( bWithForbidden && m_Forbidden[v] )
		return false;

	CCondition* pCond;
	for( int i = 0 ; i < 3 ; i++ )
	{
		pCond = m_Conditions[i];
		if( pCond == pTheCond )
			continue;

		if( !pCond->IsFree( v ) )
			return false;
	}

	return true;
}

int	CCell::GetOnlyPossibleValue()
{
	if( m_Val )
	{
		ASSERT(0);//shouldn't be called, if already set
		return m_Val;
	}

	BYTE		val[VAL_CNT_1];
	memset( val , 1 , VAL_CNT_1 );
	int			i, j;
	CCondition*	pCond;

	for( i = 1 ; i < VAL_CNT_1 ; i++ )
	{
		if( m_Forbidden[i] )
		{
			val[i] = 0;
			continue;
		}

		for( j = 0 ; j < 3 ; j++ )
		{
			pCond = m_Conditions[j];
			if( !pCond->IsFree( i ) )
			{
				val[i] = 0;
				break;
			}
		}
	}

	int ret = 0;
	int PossibleValCnt = 0;
	for( i = 1 ; i < VAL_CNT_1 ; i++ )
	{
		if( val[i] )
		{
			PossibleValCnt++;
			ret = i;
			if( PossibleValCnt > 1 )
				return -1;
		}
	}

	return ret;
}

void CCell::SetVal( int v )
{
	if( v < 1 || v > VAL_CNT )
	{
		ASSERT(0);
		return;
	}

	m_Val = v;

	int			j;
	CCondition*	pCond;
	for( j = 0 ; j < 3 ; j++ )
	{
		pCond = m_Conditions[j];
		pCond->SetVal( this , v );
	}
}

int CForbidAction::DoForbid( int CurFreeCellCnt )
{
	std::vector<sValCellPair>::iterator	it;
	std::vector<CCell*>::iterator		cit;
	CCell*								pCell;
	int									v;

	ForbidCells.clear();
	
	for( it = ForbiddenValues.begin() ; it != ForbiddenValues.end() ; it++ )
	{
		sValCellPair& pair = *it;
		v = pair.v;
		for( cit = CellsToForbid.begin() ; cit != CellsToForbid.end() ; cit++ )
		{
			pCell = *cit;
			if( pCell->SetForbidden( pair.v , CurFreeCellCnt ) )
				ForbidCells.push_back( pCell );
		}	
	}

	return ForbidCells.size();
}

void CForbidAction::UndoForbid( int CurFreeCellCnt )
{
	std::vector<sValCellPair>::iterator		it;
	std::vector<CCell*>::iterator	cit;
	CCell*							pCell;

	for( it = ForbiddenValues.begin() ; it != ForbiddenValues.end() ; it++ )
	{
		sValCellPair& pair = *it;
		for( cit = CellsToForbid.begin() ; cit != CellsToForbid.end() ; cit++ )
		{
			pCell = *cit;
			pCell->EraseForbidden( pair.v , CurFreeCellCnt );
		}
	}

	CellsToForbid.clear();
	ForbiddenValues.clear();
}

int	CForbidAction::GetForbiddenColRow( bool bCol )
{
	std::vector<CCell*>::iterator	it;
	CCell*						pCell;

	for( it = ForbidCells.begin() ; it != ForbidCells.end() ; it++ )
	{
		pCell = *it;
		return bCol ? pCell->m_Col : pCell->m_Row;
	}

	ASSERT(0);
	return -1;
}

CString	CForbidAction::GetForbiddenValStr()
{
	std::vector<sValCellPair>::iterator	it;
	CString		res;
	CString		c;
	
	for( it = ForbiddenValues.begin() ; it != ForbiddenValues.end() ; it++ )
	{
		sValCellPair& pair = *it;
		c.Format( "%d" , pair.v );
		res += c;
	}

	return res;	
}


bool CCell::IsNeeded()
{
	int* pGroup;
	std::list<int*>::iterator it;
	for( it = m_Groups.begin() ; it != m_Groups.end() ; it++ )
	{
		pGroup = *it;
		if( *pGroup < 2 )
			return true;
	}

	return false;
}

void CCell::DecrementGroups()
{
	int* pGroup;
	std::list<int*>::iterator it;
	for( it = m_Groups.begin() ; it != m_Groups.end() ; it++ )
	{
		pGroup = *it;
		(*pGroup)--;
	}
}

int	CCell::AddFreeValues( int* pFreeVals , int ValIdx , int ValCnt )
{
	//for group-building
	int			i;
	int			v;
	int			FreeValCnt = ValIdx;//assuming that the cells with idx < CellIdx are not free for other values.
	for( i = ValIdx ; i < ValCnt ; i++ )
	{
		 v = pFreeVals[i];
		 if( IsFree( v , NULL , true ) )
		 {
			 if( i != FreeValCnt )
			 {
				//swap:
				pFreeVals[i] = pFreeVals[FreeValCnt];
				pFreeVals[FreeValCnt] = v;
			 }
			 FreeValCnt++;
		 }
	}

	return FreeValCnt;
}
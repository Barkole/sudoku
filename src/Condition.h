// Condition.h: interface for the CCondition class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONDITION_H__1AE36C4B_CAA5_4210_B176_990D8447C9BA__INCLUDED_)
#define AFX_CONDITION_H__1AE36C4B_CAA5_4210_B176_990D8447C9BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

struct sCellValPair
{
	sCellValPair() : pCell(NULL), val(0) {}
	CCell*	pCell;
	int		val;
};

class CCondition;

struct sConditionPart
{
	CCondition* pCond;
	CCell*		m_pCells[COLS_IN_FIELD];//Hardcoding COLS_IN_FIELD == ROWS_IN_FIELD. If COLS_IN_FIELD != ROWS_IN_FIELD we must distinguish more. 
									//But i am to lazy to impl a game i do not have... (we should have as m_pCells a list with variable length then)
	bool		m_bCol;

	sConditionPart() : pCond(NULL), m_bCol(false) { ZeroMemory( m_pCells , COLS_IN_FIELD * sizeof(CCell*) ); }

	bool		IsFull();//is full with numbers.
	bool		IsFull( int v , bool bWithForbids );
};

class CPartConditions
{
public:
	sConditionPart	m_PartConditions[3];
	int				GetFreePartCondCnt();
	sConditionPart*	GetOnlyFreePartCond( int v , bool bWithForbids );
};

class CCondition  
{
public:
	CCondition();
	virtual ~CCondition();

	CCell*			m_Cells[VAL_CNT];

	CCell*			OnlyOneCellLeft( int v );
	inline bool		IsFree( int v )	{ return m_Cells[v-1]->m_Val == 0; }
	inline CCell*	GetCell( int v ) { return (m_Cells[v-1]->m_Val == 0) ? NULL : m_Cells[v-1]; }
	void			SetVal( CCell* pCell , int v );
	int				GetFreeCellCnt();
	int				GetFreePartCondCnt( bool& bCol );
	CCell*			GetFirstFreeCell();
	void			GetFreeValues( std::vector<sValCellPair>& FreeValues );
	void			GetFreeCells( std::vector<CCell*>& FreeCells , std::vector<sValCellPair>* pNotThisOnes );
	void			GetFreeCells( std::vector<CCell*>& FreeCells , int ForbiddenValue , int NotPartIdx , bool bCol );
	int				AddFreeCells( int v , CCell** ppFreeCells , int CellIdx , int CellCnt );//for group-building
	sConditionPart*	GetOnlyFreePartCond( int v , bool bWithForbids );
	bool			DoGroupForbid( int CurFreeCellCnt );//returns true is  a forbid was made

	CString			m_Desc;
	LPCTSTR			GetDesc()	{ return (LPCTSTR)m_Desc; }

	std::vector<CPartConditions*>	m_arPartConditions;
};

#endif // !defined(AFX_CONDITION_H__1AE36C4B_CAA5_4210_B176_990D8447C9BA__INCLUDED_)

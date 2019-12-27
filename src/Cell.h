// Cell.h: interface for the CCell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIELD_H__08547F19_AA32_4E51_B408_A4809D8C828C__INCLUDED_)
#define AFX_FIELD_H__08547F19_AA32_4E51_B408_A4809D8C828C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//conatnts:
#define COLS_IN_FIELD 3
#define ROWS_IN_FIELD 3
#define HOR_FIELD_CNT 3
#define VER_FIELD_CNT 3
#define COL_CNT 9 //COLS_IN_FIELD * HOR_FIELD_CNT
#define ROW_CNT 9 //ROWS_IN_FIELD * VER_FIELD_CNT
#define	VAL_CNT 9 //values per cell: = COLS_IN_FIELD * ROWS_IN_FIELD
#define	CELL_CNT 81
#define	VAL_CNT_1 10 //VAL_CNT +1 , one based
#define INVALID_PTR 0xFFFFFFFF
/*A normal sudoku has ( 3 x 3 )^2 = 81 cells.
Initially i planned to implement a generic sudoku with ( n x m )^2 cells.
But during implementation i detected that several things would become more complex.
Many code assumes by algorithm that columns and rows are symmetric.
Additionally a generic implementation would need a case-sensitive handling at many places:
switch( n )
{
case 2:
do it much more simple...
case 3: 
do it like now..
default://n >= 4
replace several integers in the code by lists of integers...
}

But on the other side the more simple functions like CCellField::Init() work well for generic constants.
Therfore i didn't remove it and the result is a mixture between hardcoding 3x3 and a generic solution...*/






#include <vector>
#include <list>

class CCondition;
class CCell;

int ftoint( double x );

struct sValCellPair
{
	int		v;
	CCell*	pCell;
	sValCellPair() : v(0), pCell(NULL) {}
	sValCellPair( const sValCellPair& src ) : v(src.v), pCell(src.pCell) {}
};

class CForbidAction
{
public:
	std::vector<sValCellPair>	ForbiddenValues;//list of values to Forbid
	std::vector<CCell*>			CellsToForbid;//list of cells the forbidden values should be applied to
	std::vector<CCell*>			ForbidCells;//the list of cells where the forbid really brings new information

	CForbidAction() {}

	int				DoForbid( int CurFreeCellCnt );
	void			UndoForbid( int CurFreeCellCnt );
	CString			GetForbiddenValStr();
	int				GetForbiddenColRow( bool bCol );
};

class CCell  
{
public:
	CCell();
	virtual ~CCell();

	int				m_Val;
	inline	int		GetVal()	{ return m_Val; }
			void	SetVal( int v );
	inline	bool	IsFree()	{ return m_Val == 0; }
	inline	void	Erase()		{ m_Val = 0; }

	int		m_Col;
	int		m_Row;

	CCondition*	m_Conditions[3];

	CCell*			m_pNextFreeCell;//next free cell for computersolution

	//for creating a game:
	std::list<int*> m_Groups;//this cell is part of the following groups. Value *int indicates the number of group elements still in game.
	void			AddGroup( int* pGroup ) { m_Groups.push_back( pGroup ); }
	void			ClearGroups()			{ m_Groups.clear(); }
	void			DecrementGroups();
	bool			IsNeeded();

	int				AddFreeValues( int* ppFreeVals , int ValIdx , int ValCnt );//for group-building

	int		m_Forbidden[VAL_CNT_1];//m_Forbidden[v] > 0 means: "Is forbidden". m_Forbidden[v] == FreeCellCnt, when the forbid is set
	bool	m_bInitial;//= true for the an initial filled cell (black numbers); = false for useredited or computersolved values (gray

	bool	IsInitial()				{ return m_bInitial; }
	void	SetInitial( bool i )	{ m_bInitial = i; }


	bool			IsFree( int v , CCondition* pTheCond , bool bWithForbidden );
	int				GetOnlyPossibleValue();
	inline bool		SetForbidden( int v , int CurFreeCellCnt )	
	{ 
		if( !m_Forbidden[v] )
		{
			m_Forbidden[v] = CurFreeCellCnt;
			return true;
		}
		else
			return false;
	}

	inline void	EraseForbidden( int v , int CurFreeCellCnt )
	{
		if( m_Forbidden[v] == CurFreeCellCnt )
			m_Forbidden[v] = 0;
	}
	void		ResetForbidden() { ZeroMemory( m_Forbidden , (VAL_CNT_1) * sizeof(int) ); }
};

#endif // !defined(AFX_FIELD_H__08547F19_AA32_4E51_B408_A4809D8C828C__INCLUDED_)

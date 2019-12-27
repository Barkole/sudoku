// SudokuGame.h: interface for the CSudokuGame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUDOKUGAME_H__6CEE444E_E6F1_4155_AD4C_79DB4253C846__INCLUDED_)
#define AFX_SUDOKUGAME_H__6CEE444E_E6F1_4155_AD4C_79DB4253C846__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Cell.h"
#include "Condition.h"

#define MAX_HUMAN_LEVEL 5 //todo increase to 6 after level 6 is implemented

class CToPlay//one move to play
{
protected:
	CCell*			m_pCell;//field for which value is to play
	int				m_Val;//value to play
	CString			m_Desc;//description of this move
	CForbidAction*	m_pAction;//if the found cell needs forbids for argumentation, pAction != describes the action

public:

	CToPlay() : m_pCell(NULL) , m_Val(0) , m_pAction(NULL) {}
	CToPlay( CCell* pF , int v ) : m_pCell(pF) , m_Val(v) , m_pAction(NULL) {} 
	CToPlay( const CToPlay& src ) : m_pCell(src.m_pCell) , m_Val(src.m_Val), m_Desc(src.m_Desc), m_pAction(src.m_pAction) {}
	~CToPlay()	{ delete m_pAction; }

	CCell*			GetCell() { return m_pCell; }
	void			SetCell( CCell* c ) { m_pCell = c; }
	int				GetVal() { return m_Val; }
	void			SetVal( int v ) { m_Val = v; }
	CString&		GetDesc() { return m_Desc; }
	void			SetDesc( const CString& s ) { m_Desc = s; }
	CForbidAction*	GetAction() { return m_pAction; }
	void			SetAction( CForbidAction* a ) { m_pAction = a; }
};

class CCellField //all cells of the game including their conditions
{

public:

	CCell			m_Cells[COL_CNT][ROW_CNT];
	CCondition		m_Conditions[3][VAL_CNT];

					CCellField();
					CCellField( const CCellField& src );

	void			Init();
	void			Reset();

	#ifdef _DEBUG
		void		TraceCells();
	#endif
	
	inline	CCell*		GetCell( int i , int j )	{ return &m_Cells[i][j]; }
	inline	CCondition*	GetCond( int i , int j )	{ return &m_Conditions[i][j]; }
};

class CSudokuGame  
{
public:
					CSudokuGame();
	virtual			~CSudokuGame();

	std::vector<CToPlay*> m_MovesToMake;

	CCellField		m_CellField;

	inline	CCell*		GetCell( int i , int j )	{ return &m_CellField.m_Cells[i][j]; }
	inline	CCondition*	GetCond( int i , int j )	{ return &m_CellField.m_Conditions[i][j]; }

	int				m_bIndConclDone[VAL_CNT][HOR_FIELD_CNT][2];
	int				m_bFieldDone[VAL_CNT][3];

	CCell*			m_CurrentPlayedCells[COL_CNT][ROW_CNT];

	inline int		IndConclDone( int v , int k , bool bCol )	{ return m_bIndConclDone[v][k][bCol?1:0]; }
	inline int		FieldDone( int v , int k )					{ return m_bFieldDone[v][k]; }
	void			SetIndConclDone( int v , int k , bool bCol ) { m_bIndConclDone[v][k][bCol?1:0] = 1; }
	void			SetFieldDone( int v , int k )				{ m_bFieldDone[v][k] = 1; }

	int				m_StartPosition[COL_CNT][ROW_CNT];

	int				m_SolutionCnt;
	bool			m_bSearchOneSolution;

	void			Reset();
	void			ResetDone();
	void			ResetMovesToMake();
	void			ResetForbidden();
	void			SetAsInitialPos();

	void			Init();
	int				OnlyOneCellLeft();
	int				OnlyOneValLeft();
	void			SaveToFile( CStdioFile& file );
	void			LoadFromFile( CStdioFile& file , int version );

	int				Solve( int MaxLevel , int& MaxUsedLevel , int& NumberOfSolutions , bool bUndoActions );
	//solves current situation. Result is pushed to m_MovesToMake. NumberOfSolutions is only valid if MaxLevel = Maximum (=Computersolution())

	int				SimpleSolve( int RemainingCells , int* pLevel );//returns the number of found cells and writes, if pLevel, in pLevel the level (1, 2, or 3) 
	int				DoOperations( int level , int CurFreeCellCnt );
	int				DoComputerSolution( bool bSearchOneSolution , bool bSimulation );//returns the number of solutions
	CCell*			ConnectFreeCells();//connects the free cells to a simple linked list as preparation of SolveBruteForce()
	void			FindAllSolutions( CCell* pCell );//number of solutions is in m_SolutionCnt
	bool			FindOneSolution( CCell* pCell );//returns after the first solution is found
	int				Do67GroupForbids( int level , int CurFreeCellCnt );//returns FoundCellCnt using 6-7-forbids
	int				DoLevel6( int CurFreeCellCnt );
	int				DoGroupForbids( int CurFreeCellCnt , bool& bForbidDone );
	int				DoIndirectConclusion( int level , int CurFreeCellCnt , bool& bForbidDone );//returns FoundCellCnt using indirect conclusions
	int				DoForbid( CForbidAction* pAction , CCondition* pCond , int v , int PartIdx , 
								bool bCol , int CurFreeCellCnt , bool bSimulation , bool& bForBidDone );

	CToPlay*		GetOutput( int idx );

	bool			IsAlreadyFound( CCell* pCell );
	void			AddFoundCell( CCell* pCell , int v , CString& desc );
	int				GetFreeCellCnt();

	bool			FillGame();
	bool			CreateGame( int level, int* pTargetCellsCnt = NULL );
	int				RandomRemoval( int level, int TargetRemoveCnt );//returns the max used Level

	std::list<int*> m_Groups;//groups for CreateGame
	void			BuildGroups();
	void			DeleteGroups();
	void			ResetToInitialPos();
};

#endif // !defined(AFX_SUDOKUGAME_H__6CEE444E_E6F1_4155_AD4C_79DB4253C846__INCLUDED_)

// RandomFill.h: interface for the CRandomFill class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RANDOMFILL_H__42880719_F536_45E0_B785_914ADAB8B211__INCLUDED_)
#define AFX_RANDOMFILL_H__42880719_F536_45E0_B785_914ADAB8B211__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNextCellList;

class CNextCell//cell+info about the list of following cells in the next row.
{
public:
	CNextCell();
	virtual ~CNextCell();

	CCell*			m_pCell;
	CNextCellList*	m_pNextCells;	
};

class CNextCellList
{
public:
	CNextCellList();
	virtual ~CNextCellList();

	void		SetSize( int s );

	CNextCell**	m_pNCells;
	int			cnt;//count, how many cells we have in m_pNCells
};
	 
class C3Rows// 3 consequent rows of the game
{
public:

	C3Rows();
	virtual ~C3Rows();

	int				m_Idx;

	CNextCell*		m_pCells[9];

	C3Rows*			m_pNext;
	int				m_Val;//copy from CRandomFill
	int*			m_pUsedCols;//is int[9], copy from CRandomFill

	void			Init();
	bool			Fill();
};

class CRandomFillVal//fills a game completely with m_Val ( at 9 places in a 3x3 Sudoku ) 
{
public:
	CRandomFillVal();
	virtual ~CRandomFillVal();

	int				m_Val;
	CRandomFillVal*	m_pPrev;//pointer to next instance, which will fill the game with another m_Val
	CRandomFillVal*	m_pNext;//pointer to previous instance, which has filled the game with another m_Val
	int				m_Depth;//depth in the hierachy of the 9 CRandomFillVal-instances == the number of free cells per row

	static CCell	m_Cells[COL_CNT][ROW_CNT];// the (81) cells of the game. static, all 9 instances have the same cells
	static void		ResetCells();
	#ifdef _DEBUG
		static void		TraceCells();
	#endif

	C3Rows			m_3Rows[3];//the 3x3 rows of the game

	int				m_UsedCols[9];

	void			Init();
};

#endif // !defined(AFX_RANDOMFILL_H__42880719_F536_45E0_B785_914ADAB8B211__INCLUDED_)

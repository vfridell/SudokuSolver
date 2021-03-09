#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include "SudokuSolver.h"

void makeAlignedPairs(int blockId);
	
unsigned short board[9][9];
/*									 {{5,0,7,0,0,0,0,6,0},
                             {9,0,0,0,4,0,0,0,0},
                             {0,8,0,0,0,5,3,0,0},
                             {0,0,0,0,0,6,0,4,0},
                             {0,2,4,3,0,9,5,1,0},
                             {0,9,0,8,0,0,0,0,0},
                             {0,0,8,1,0,0,0,3,0},
                             {0,0,0,0,6,0,0,0,1},
                             {0,4,0,0,0,0,2,0,7}};
*/

// should end up with size == 27
std::vector<ThreeThree> vBoard;

// all the aligned pairs of cells
AlignedPairs alignedPairs;

void printBoard()
{
	for (int r=0; r<9; ++r)
	{
		for (int c=0; c<9; ++c)
		{
			std::cout << board[r][c] << " ";
		}
		std::cout << std::endl;
	}
}

unsigned short getBlockId(int row, int col)
{
	// convert from base 3 to base 10
	return ( (row / 3) * 3 ) + (col / 3);
}
	
ThreeThree loadBlock(int row, int col)
{
	std::vector<unsigned short> v;
	locVector locs;
	for(int r=0; r<3; ++r)
	{
		for(int c=0; c<3; ++c)
		{
			v.push_back(board[row + r][col + c]);
			locs.push_back(std::make_pair(row + r, col + c));
		}
	}
	
	ThreeThree t(v);
	t.setLocations(locs);
	t.setId(getBlockId(row, col));
	t.setType(eBlock);
	return t;
}

ThreeThree loadRow(int row)
{
	std::vector<unsigned short> v;
	locVector locs;
	for(int c=0; c<9; ++c)
	{
		v.push_back(board[row][c]);
		locs.push_back(std::make_pair(row,c));
	}
	
	ThreeThree t(v);
	t.setLocations(locs);
	t.setId(row);
	t.setType(eRow);
	return t;
}

ThreeThree loadCol(int col)
{
	std::vector<unsigned short> v;
	locVector locs;
	for(int r=0; r<9; ++r)
	{
		v.push_back(board[r][col]);
		locs.push_back(std::make_pair(r,col));
	}
	
	ThreeThree t(v);
	t.setLocations(locs);
	t.setId(col);
	t.setType(eCol);
	return t;
}

void loadBoard()
{
	//blocks start at 0, 3, or 6
	for(int r=0; r<=6; r+=3)
	{
		for(int c=0; c<=6; c+=3)
		{
			vBoard.push_back(loadBlock(r,c));
		}
	}

	//load rows and cols
	for(int r=0; r<9; ++r)
	{
		vBoard.push_back(loadRow(r));
	}
	for(int c=0; c<9; ++c)
	{
		vBoard.push_back(loadCol(c));
	}

	// fill the alignedPairs list
	for(int i=0; i<9; ++i)
	{
		makeAlignedPairs(i);
	}
}

void clearBoard()
{
	vBoard.clear();
	alignedPairs.clear();
}

std::vector<unsigned short> getPossibleValues(unsigned short mask)
{
	std::vector<unsigned short> resultVector;
	for(unsigned short i = 0; i<9; ++i)
	{
		if ( mask & (0x01 << i) )
		{
			// not a possible value
		}
		else
		{
			resultVector.push_back(i+1);
		}
	}
	return resultVector;
}

bool changeBoard(int row, int col, unsigned short mask)
{
	std::vector<unsigned short> pv = getPossibleValues(mask);
	if (pv.size() == 1)
	{
		std::cout << "Setting board["
					 << row << "][" << col 
					 << "] = " << pv[0] << std::endl;
		board[row][col] = pv[0];
		return true;
	}
	return false;				
}

ThreeThree* getThreeThree(ThreeThreeType type, int id)
{
	std::vector<ThreeThree>::iterator itr = vBoard.begin();
	for ( ; itr != vBoard.end(); ++itr)
	{
		if (itr->getType() == type && itr->getId() == id)
		{
			return &(*itr);
		}
	}
	return 0;
}

void makeAlignedPairs(int blockId)
{
	// go through all the cells in this block and
	// keep a count of how many 0 cells could possibly 
	// be each number 0-9
	int possibleValueCount[10] = {0,0,0,0,0,0,0,0,0,0};
	ThreeThree* block = getThreeThree(eBlock, blockId);
	locVector locs = block->getLocations();
	locVector::const_iterator locItr = locs.begin();
	for ( ; locItr != locs.end(); ++locItr)
	{
		int focusCellRow = locItr->first;
		int focusCellCol = locItr->second;
		if (board[focusCellRow][focusCellCol] == 0)
		{
			Cell cell(focusCellRow,focusCellCol);
			cell.initialize();
			std::vector<unsigned short> pvv = getPossibleValues(cell.getCompositeMask());
			for(std::vector<unsigned short>::iterator itr=pvv.begin(); itr<pvv.end(); ++itr)
			{
				possibleValueCount[*itr] += 1;
			}
		}
	} // end for each cell in this block

	// find values for which only 2 cells could take them
	for (unsigned short value=1; value<10; ++value)
	{
		if (possibleValueCount[value] == 2)
		{
			locItr = locs.begin();
			for ( ; locItr != locs.end(); ++locItr)
			{
				int focusCellRow = locItr->first;
				int focusCellCol = locItr->second;
				if (board[focusCellRow][focusCellCol] == 0)
				{
					Cell cell(focusCellRow,focusCellCol);
					cell.initialize();
					std::vector<unsigned short> pvv = getPossibleValues(cell.getCompositeMask());
					for(std::vector<unsigned short>::iterator itr=pvv.begin(); itr<pvv.end(); ++itr)
					{
						if (*itr == (value))
						{
							alignedPairs.add(focusCellRow, focusCellCol, blockId, value);
						}
					}
				}
			} // end for each cell in this block
		}
	} // end for each value 1-9
}

int main(int argc, char* argv[])
{
	// read puzzle board from file
	if (argc>1)
	{
		std::ifstream fin(argv[1]);
		for (int r=0; r<9; ++r)
		{
			for (int c=0; c<9; ++c)
			{
				fin >> board[r][c];
			}
		}
	}
	else
	{
		std::cout << "Usage: " << argv[0] << " puzzlefile" << std::endl;
		exit(1);
	}

	
	bool changeMade;
	do
	{
		changeMade = false;

		// set up the ThreeThree vBoard
		loadBoard();
	
		// check for cells that only have one possible value
		for (int r=0; r<9 && !changeMade; ++r)
		{
			for (int c=0; c<9 && !changeMade; ++c)
			{
				// skip any cells that we know the value of
				if (board[r][c] == 0)
				{
					Cell cell(r,c);
					cell.initialize();
					changeMade = changeBoard(r, c, cell.getCompositeMask());
					if (changeMade)
					{
						std::cout << "Cell [" << r << "][" << c << "] CMask = "
						  		    << cell.getCompositeMask() << std::endl;
					}
				}
			}
		}
		
		if (!changeMade)
		{
			// check for ThreeThrees where only 
			// one cell has a particular possible value
			std::vector<ThreeThree>::iterator itr = vBoard.begin();
			for ( ; itr != vBoard.end() && !changeMade; ++itr)
			{
				locVector locs = itr->getLocations();
				locVector::iterator locItr = locs.begin();
				for ( ; locItr != locs.end() && !changeMade; ++locItr)
				{
					int focusCellRow = locItr->first;
					int focusCellCol = locItr->second;
//					std::cout << "FocusCell: [" << focusCellRow
//					  			 << "][" << focusCellCol
//								 << ']' << std::endl;
					if (board[focusCellRow][focusCellCol] == 0)
					{
						unsigned short dblCompositeMask = itr->getDoubleCompositeMask(focusCellRow, focusCellCol);
						// & the doubleComposite with the focusCellComposite
						Cell cell(focusCellRow,focusCellCol);
						cell.initialize();
						unsigned short focusCompositeMask = cell.getCompositeMask();
						unsigned short resultMask = focusCompositeMask & dblCompositeMask;
						// xor
						resultMask ^= dblCompositeMask;
						// get the complement
						resultMask = ~resultMask;
						changeMade = changeBoard(focusCellRow, focusCellCol, resultMask);
						if (changeMade)
						{
							std::cout << "Cell [" << focusCellRow << "][" << focusCellCol 
								       << "] CMask = " << std::hex << focusCompositeMask
										 << " ThreeThree (" << itr->getTypeString() 
										 << "," << itr->getId() << ") "
										 << " DCMask= " << dblCompositeMask
										 << " resultMask= " << resultMask << std::endl;
						}
					}
				} // end for each cell in this ThreeThree
			} // end for each ThreeThree in vBoard
		}	
			
		// DEBUG print the aligned pairs
		//alignedPairs.print();
	
		// must clear the vBoard
		std::cout << vBoard.size() << std::endl;
		clearBoard();

	} while(changeMade);


	//output the final board
	printBoard();
	
	return 1;
}

bool ThreeThree::Validate()
{
   const unsigned short compareMask = 0x1FF;
	unsigned short mask = getMask();
	if (mask == compareMask)
		return true;
	else
		return false;
}

unsigned short ThreeThree::getMask()
{
	if (data_.size() != 9)
	{
		std::cout << "ThreeThree::getMask(): ERROR invalid vector"
					 << std::endl;
		return 0x00;
	}

	unsigned short mask = 0x00;
	std::vector<unsigned short>::iterator itr = data_.begin();
	for ( ; itr != data_.end(); ++itr)
	{
		unsigned short s = 0x01;
		unsigned short checkBit = (s << ( (*itr) - 1 ));
		if (mask & checkBit)
		{
			std::cout << "ThreeThree::getMask(): ERROR trying to set bit twice: mask="
						 << std::setbase(2) << mask << " checkBit=" << checkBit 
						 << std::endl;
			exit(1);
		}
		mask += checkBit;
	}
	
	return mask;
}

unsigned short ThreeThree::getDoubleCompositeMask(int focusCellRow, int focusCellCol)
{
	// get double-compositeMask excepting this cell
	unsigned short dblCompositeMask = 0x01ff;
	locVector::iterator locItr = locations_.begin();
	for ( ; locItr != locations_.end(); ++locItr)
	{
		int row = locItr->first;
		int col = locItr->second;
		if (row == focusCellRow && col == focusCellCol)
		{
			continue;
		}
		
		/*std::cout << "inner ThreeThree cell: ["
					 << row << "][" << col
					 << ']' << std::endl;
					 */

		if (board[row][col] == 0)
		{
			Cell cell(row,col);
			cell.initialize();
			dblCompositeMask &= cell.getCompositeMask();
		}
	} 
	
	return dblCompositeMask;
}

std::string ThreeThree::getTypeString()
{
	switch(type_)
	{
		case eRow:
			return "Row";
		case eCol:
			return "Col";
		case eBlock:
			return "Block";
		default:
			return "InvalidType";
	}
}

void Cell::initialize()
{
	rowThreeThree_ = getThreeThree(eRow, row_);
	colThreeThree_ = getThreeThree(eCol, col_);
	blockThreeThree_ = getThreeThree(eBlock, getBlockId(row_, col_));
	setCompositeMask();
}

void Cell::setCompositeMask()
{
	compositeMask_ = rowThreeThree_->getMask();
	compositeMask_ |= colThreeThree_->getMask();
	compositeMask_ |= blockThreeThree_->getMask();

	apMask_ = alignedPairs.getAlignedPairsMask(row_, col_);
	compositeMask_ |= apMask_;
}

unsigned short Cell::getCompositeMask()
{
	return compositeMask_;
}

void AlignedPairs::add(int row, int col, int blockId, unsigned short value)
{
	bool foundHalf = false;

	std::vector<AlignedPair>::iterator itr;
	for (itr = pairs_.begin(); itr!=pairs_.end(); ++itr)
	{
		if(itr->blockId == blockId && itr->value == value)
		{
			// the passed info is the other half of the found pair
			itr->setB(row, col);
			itr->setPairType();
			if (!itr->isAligned())
			{
				pairs_.erase(itr);
			}
			return;
		}
	}
	
	// we need to create a new entry
	AlignedPair newP;
	newP.setBlockAndValue(blockId, value);
	newP.setA(row, col);
	pairs_.push_back(newP);
	return;
}

bool AlignedPairs::get(int blockId, unsigned short value, AlignedPair &p)
{
	std::vector<AlignedPair>::iterator itr;
	for (itr = pairs_.begin(); itr!=pairs_.end(); ++itr)
	{
		if(itr->blockId == blockId && itr->value == value)
		{
			p = *itr;
			return true;
		}
	}
	return false;
}

bool AlignedPair::isQuadAligned(AlignedPair &i)
{
	return ( i.value == value && ((i.row_A == row_A && i.row_B == row_B) || (i.col_A == col_A && i.col_B == col_B)) && i.blockId != blockId);
}

void AlignedPairs::print()
{
	std::vector<AlignedPair>::iterator itr;
	for (itr = pairs_.begin(); itr!=pairs_.end(); ++itr)
	{
		itr->print();
	}
}

void AlignedPair::print()
{
	std::cout << "AlignedPair: block=" << blockId
				 << " [" << row_A << "][" << col_A
				 << "] & [" << row_B << "][" << col_B 
				 << "] value=" << value
				 << std::endl;
}

bool AlignedPairs::isQuadAligned(AlignedPair &p)
{
	std::vector<AlignedPair>::iterator itr;
	for (itr = pairs_.begin(); itr!=pairs_.end(); ++itr)
	{
		if (p.isQuadAligned(*itr))
		{
			return true;
		}
	}
	return false;
}
	
unsigned short AlignedPairs::getAlignedPairsMask(int row, int col)
{
	// the mask to return
	unsigned short mask = 0;
	
	// storage for finding quad-aligned pairs
	std::map<unsigned short, int> valueCounts;
	
	std::vector<AlignedPair>::iterator itr;
	for (itr = pairs_.begin(); itr!=pairs_.end(); ++itr)
	{
		// make sure this cell isn't in this aligned pair
		if ( (row == itr->row_A && col == itr->col_A) ||
			  (row == itr->row_B && col == itr->col_B) )
		{
			continue;
		}
		
		if (eRow == itr->pairType)
		{
			if (row == itr->row_A)
			{
				// this cell is row aligned with this aligned pair
				// This means that this cells possible values cannot
				// include this aligned pair's value
				mask |= (0x0001 << (itr->value - 1));
			}
			/*else if(col == itr->col_A || col == itr->col_B)
			{
				// this cell shares a col with this row-aligned pair
				// If this pair is quad-aligned then it will restrict
				// possible values
				if (isQuadAligned(*itr))
				{
					// TODO Make sure this cell isn't in the Quad Aligned group
					mask |= (0x0001 << (itr->value - 1));
				}
			}*/
			
		}
		else if (eCol == itr->pairType)
		{
			if (col == itr->col_A)
			{
				// this cell is col aligned with this aligned pair
				mask |= (0x0001 << (itr->value - 1));
			}
			/*else if(row == itr->row_A || row == itr->row_B)
			{
				// this cell shares a row with this col-aligned pair
				// If this pair is quad-aligned then it will restrict
				// possible values
				if (isQuadAligned(*itr))
				{
					// TODO Make sure this cell isn't in the Quad Aligned group
					mask |= (0x0001 << (itr->value - 1));
				}
			}*/
		}
				
	} // end for each aligned pair

	return mask;
}

void AlignedPair::setPairType()
{
	if (row_A == row_B)
	{
		pairType = eRow;
	}
	else if(col_A == col_B)
	{
		pairType = eCol;
	}
	else
	{
		pairType = eBlock;
	}
}

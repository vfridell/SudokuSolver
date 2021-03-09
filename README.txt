Sudoku Solver
1/6/06
by Vern Fridell

The puzzle is played on a board like the one below 9x9.  The goal is to arrange the grid so that every row, column, and 3x3 box contains every digit from 1 to 9 inclusive.  So for every cell there are 9*3=27 cells that it must agree with (have a different value than).

unsigned short board[][9] = {{5,0,7,0,0,0,0,6,0},
                             {9,0,0,0,4,0,0,0,0},
                             {0,8,0,0,0,5,3,0,0},
                             {0,0,0,0,0,6,0,4,0},
                             {0,2,4,3,0,9,5,1,0},
                             {0,9,0,8,0,0,0,0,0},
                             {0,0,8,1,0,0,0,3,0},
                             {0,0,0,0,6,0,0,0,1},
                             {0,4,0,0,0,0,2,0,7}};

A ThreeThree is a single row, column, or block.  There are 27 ThreeThrees on a board.  A ThreeThree is uniquely identified by its type (row, col, or block) and id.  The id is the row, col, or block number.  Rows and cols are numbered 0-8 exactly like a c-style array.  Blocks are numbered 0-8 beginning in the upper left and ending in the lower right, incrementing columns before rows.  

Each ThreeThree has a Mask.  A Mask is a bit-field of all values already set in that ThreeThree.  So, the Mask for a particular ThreeThree tells you what values are (and aren't) defined for that one ThreeThree.  So the Mask of row(0) = 001110000 (0x0070).  Mask(block(8)) = 001000111 (0x0047).

A cell is a single location on the board.

Each cell with a zero value has a Composite Mask (CMask).
A CMask is a bit-field composed of a bitwise-or of the Masks of all the ThreeThrees that cell belongs to.  Each cell belongs to 3 ThreeThrees.  The CMask for a cell tells you what all the possible values are for that cell.  For example, in the board above the cell at [4][4] the possible value list is {7} and so the CMask is (110111111).  CMask([0][1]) = (111111010).  CMask() is undefined for a cell with a value > 0.

If the CMask only contains a single 0 bit, then that cell's value has been deduced.

Each ThreeThree also has a double composite mask (DCMask) for each cell in that ThreeThree.  A DCMask is the bitwise-and of all CMasks for all the cells in that ThreeThree with the exception of the focus cell.  The DCMask reveals which possible values are missing from all but one of the cells of a given ThreeThree. 

Within the board there may be Aligned Pairs (APs).  An AP is composed of two cells within a Block which share a possible value that no others in that block have.  Furthermore, the cells must share a row or column.  An AP represents an either/or for that possible value within that block.  It also restricts the cells in the row or column that it is aligned on from having that particular possible value.

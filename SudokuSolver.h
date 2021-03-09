#include <vector>
#include <map>

typedef std::vector<std::pair<int,int> > locVector;

enum ThreeThreeType {eRow, eCol, eBlock};

class ThreeThree
{
	public:
	ThreeThree(std::vector<unsigned short>&v) {data_ = v;}
	~ThreeThree() {}
	
	bool Validate();
	unsigned short getMask();
	void setId(unsigned short s) {id_ = s;}
	unsigned short getId() {return id_;}
	void setType(ThreeThreeType t) {type_ = t;}
	ThreeThreeType getType() {return type_;}
	std::string getTypeString();
	void setLocations(locVector& l) {locations_ = l;}
	locVector getLocations() {return locations_;}
	unsigned short getDoubleCompositeMask(int focusCellRow, int focusCellCol);

	private:
	std::vector<unsigned short> data_;
	locVector locations_;
	int id_;
	ThreeThreeType type_;
};


class Cell
{
	public:
	Cell(int row, int col) {row_ = row; col_ = col;}
	~Cell() {}

	void initialize();
	unsigned short getCompositeMask();
	
	private:
	void setCompositeMask();

	int row_;
	int col_;
	ThreeThree *rowThreeThree_;
	ThreeThree *colThreeThree_;
	ThreeThree *blockThreeThree_;
	unsigned short compositeMask_;
	unsigned short apMask_;
};

class AlignedPair
{
	public:
	AlignedPair() {}
	~AlignedPair() {}

	bool isQuadAligned(AlignedPair &i);
	bool isAligned() {return row_A == row_B || col_A == col_B;}
	void setBlockAndValue(int b, unsigned short v) {blockId = b; value = v;}
	void setA(int row, int col) {row_A = row; col_A = col;}
	void setB(int row, int col) {row_B = row; col_B = col;}
	void setPairType();
	void print();
	 	
	ThreeThreeType pairType;
	unsigned short value;
	int blockId;
	int row_A;
	int col_A;
	int row_B;
	int col_B;
};

class AlignedPairs
{
	public:
	AlignedPairs() {}
	~AlignedPairs() {}

	void add(int row, int col, int blockId, unsigned short value);
	bool get(int blockId, unsigned short value, AlignedPair &p);
	void clear() {pairs_.clear();}
	void print();
	unsigned short getAlignedPairsMask(int row, int col);
	bool isQuadAligned(AlignedPair&);
	
	private:
		
	std::vector<AlignedPair> pairs_;
};

#pragma once
class Cell
{
private:
	int row;
	int col;
	int actualCost;
	Cell* parent;

public:
	Cell();
	Cell(int r, int c, Cell* p);
	int getActualCost()  { return actualCost; }
	void setActualCost(int cost) { actualCost = cost; }
	int getRow() { return row; }
	int getCol() { return col; }
	Cell* getParent() { return parent; }
};


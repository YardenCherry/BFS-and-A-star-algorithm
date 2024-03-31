
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <queue>
#include <vector>
#include "Cell.h"
#include <iostream>

using namespace std;

const int MSZ = 100;

const int SPACE = 0;
const int WALL = 1;
const int START = 2;
const int TARGET = 3;
const int GRAY = 4;
const int BLACK = 5;
const int PATH = 6;

int targetRow, targetCol;
int maze[MSZ][MSZ] = {0}; // space
bool runBFS = false, runAStar = false;


int heuristic(int row, int col, int targetRow, int targetCol) {
	return abs(row - targetRow) + abs(col - targetCol);
}

struct CompareCells {
	bool operator()(Cell* a, Cell* b)const {
		// Compare cells based on their heuristic values
		return heuristic(a->getRow(), a->getCol(), targetRow, targetCol) >
			heuristic(b->getRow(), b->getCol(), targetRow, targetCol);
	}
};

// Update the CompareCells functor
struct CompareTotalCells {
	bool operator()(Cell* a, Cell* b) const {
		int totalCostA = a->getActualCost() + heuristic(a->getRow(), a->getCol(), targetRow, targetCol);
		int totalCostB = b->getActualCost() + heuristic(b->getRow(), b->getCol(), targetRow, targetCol);
		return totalCostA > totalCostB;
	}
};
priority_queue<Cell*, vector<Cell*>, CompareCells> grays;
priority_queue<Cell*, vector<Cell*>, CompareTotalCells> yellows;





void InitMaze();

void init()
{
	glClearColor(0.5,0.5,0.5,0);// color of window background
	glOrtho(0, MSZ, 0, MSZ, -1, 1); // set the coordinates system

	srand(time(0));

	InitMaze();
}

void InitMaze()
{
	int i, j;

	for (i = 0;i < MSZ;i++)
	{
		maze[0][i] = WALL;
		maze[MSZ-1][i] = WALL;
		maze[i][0] = WALL;
		maze[i][MSZ - 1] = WALL;
	}

	for(i=1;i<MSZ-1;i++)
		for (j = 1;j < MSZ - 1;j++)
		{
			if (i % 2 == 1) // mostly spaces
			{
				if (rand() % 10 < 2) // WALL
					maze[i][j] = WALL;
				else maze[i][j] = SPACE;
			}
			else // even lines
			{
				if (rand() % 10 < 4) // space
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
		}
	maze[MSZ / 2][MSZ / 2] = START;


	targetRow = rand() % MSZ;
	targetCol = rand() % MSZ;
	maze[targetRow][targetCol] = TARGET;

	Cell* pc = new Cell(MSZ / 2, MSZ / 2,nullptr);  // pointer to a cell
	grays.push(pc); // insert first cell to grays
	yellows.push(pc);

}

void DrawMaze()
{
	int i, j;

	for(i=0;i<MSZ;i++)
		for (j = 0;j < MSZ;j++)
		{
			switch (maze[i][j]) // set convinient color of a cell
			{
			case SPACE:
				glColor3d(0.1, 0.1, 0.1); // white
				break;
			case WALL:
				glColor3d(0,0, 1); // black
				break;
			case START:
				glColor3d(0.5, 0.5, 1); // blue
				break;
			case TARGET:
				glColor3d(1, 0, 0); // red
				break;
			case GRAY:
				glColor3d(1, 0.7, 0); // orange
				break;
			case BLACK:
				glColor3d(0.8, 1, 0.8); // green
				break;
			case PATH:
				glColor3d(1, 0, 1); // magenta
				break;

			}// switch

			glBegin(GL_POLYGON);
			glVertex2d(j, i);
			glVertex2d(j, i + 1);
			glVertex2d(j+1, i + 1);
			glVertex2d(j+1 , i );
			glEnd();
		}
}

void RestorePath(Cell* pc)
{
	while (pc != nullptr)
	{
		if (maze[pc->getRow()][pc->getCol()] != TARGET && maze[pc->getRow()][pc->getCol()] != START) {
			maze[pc->getRow()][pc->getCol()] = PATH;
			
		}
		pc = pc->getParent();
	}
}

// BFS a cell on row,col can be either WHITE or TARGET
bool CheckNeighbour(int row, int col, Cell* pCurrent) {
	Cell* pn = nullptr;
	if (maze[row][col] == TARGET) {
		runBFS = false;
		RestorePath(pCurrent);
		return false;
	}
	else {
		pn = new Cell(row, col, pCurrent);
		grays.push(pn); // Enqueue based on priority
		maze[row][col] = GRAY;
		return true;
	}
}
bool CheckAndAddNeighbor(int row, int col, Cell* pCurrent) {
	if (maze[row][col] == TARGET) {
		runAStar = false;
		RestorePath(pCurrent);
		return false;
	}
	if (maze[row][col] != WALL && maze[row][col] != BLACK) {
		Cell* pn = new Cell(row, col, pCurrent);
		pn->setActualCost(pCurrent->getActualCost() + 1); // Assuming each step has a cost of 1
		yellows.push(pn);
		maze[row][col] = GRAY;
	}
}
void RunAStarIteration() {
	Cell* pc;
	int row, col;
	bool go_on = true;
	if (yellows.empty()) {
		runAStar = false;
		cout << "yellows are empty. There is no solution\n";
		return;
	}
	else {
		pc = yellows.top();
		yellows.pop();
		if (maze[pc->getRow()][pc->getCol()] != START) maze[pc->getRow()][pc->getCol()] = BLACK;

		row = pc->getRow();
		col = pc->getCol();

		if (maze[row][col] == TARGET) {
			runAStar = false;
			RestorePath(pc);
			return;
		}

		// Now check all neighbors of pc and enqueue them based on their total cost

		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == TARGET)
			go_on = CheckAndAddNeighbor(row + 1, col, pc);
		if (go_on && (maze[row - 1][col] == SPACE || maze[row - 1][col] == TARGET))
			go_on = CheckAndAddNeighbor(row - 1, col, pc);
		if (go_on && (maze[row][col - 1] == SPACE || maze[row][col - 1] == TARGET))
			go_on = CheckAndAddNeighbor(row, col - 1, pc);
		if (go_on && (maze[row][col + 1] == SPACE || maze[row][col + 1] == TARGET))
			go_on = CheckAndAddNeighbor(row, col + 1, pc);
	}
}
void RunBFSIteration() {
	Cell* pc;
	int row, col;
	bool go_on = true;

	if (grays.empty()) {
		runBFS = false;
		cout << "Grays are empty. There is no solution\n";
		return;
	}
	else {
		pc = grays.top(); // Dequeue based on priority
		grays.pop();
		if (maze[pc->getRow()][pc->getCol()] != START) maze[pc->getRow()][pc->getCol()] = BLACK;

		row = pc->getRow();
		col = pc->getCol();

		if (maze[row][col] == TARGET) {
			runBFS = false;
			RestorePath(pc);
			return;
		}

		// Now check all neighbors of pc and enqueue them based on priority
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == TARGET)
			go_on = CheckNeighbour(row + 1, col, pc);
		if (go_on && (maze[row - 1][col] == SPACE || maze[row - 1][col] == TARGET))
			go_on = CheckNeighbour(row - 1, col, pc);
		if (go_on && (maze[row][col - 1] == SPACE || maze[row][col - 1] == TARGET))
			go_on = CheckNeighbour(row, col - 1, pc);
		if (go_on && (maze[row][col + 1] == SPACE || maze[row][col + 1] == TARGET))
			go_on = CheckNeighbour(row, col + 1, pc);
	}
}



// drawings are here
void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer with background color

	DrawMaze();

	glutSwapBuffers(); // show all
}

void idle() 
{

	if (runBFS)
		RunBFSIteration();
	if (runAStar)
		RunAStarIteration();

	glutPostRedisplay(); // call to display indirectly
}

void menu(int choice)
{
	if (choice == 1) // BFS
		runBFS = true;
	if (choice ==2 ) // AStar
		runAStar = true;
}

void main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(900, 900);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("BFS Example");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle);  // updates function
	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("run BestFirstSearch", 1);
	glutAddMenuEntry("run AStar", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
}
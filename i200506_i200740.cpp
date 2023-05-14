#include<iostream>
#include <cmath>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

//* /////////////////////////////////////////      GLOBAL VARIABLES       ///////////////////////////////////////////
int sub_count = 0;
sem_t semaphores[3];
pthread_mutex_t l;
int sub_index = 0;
int error_index = 0;
int invalid_row_entries = 0;
int invalid_col_entries = 0;
int invalid_sub_entries = 0;
int total_invalid_entries = 0;
pthread_mutex_t mut;
pthread_t solutions[4]; //solution[0] : solve_row, solution[1] : solve_col, solution[2] : solve_subGrid, solutions[3] = solve_board
int board[9][9] =
    {
	    {6, 2, 2, 5, 2, 9, 1, 8, 7},
            {5, 1, 9, 7, 2, 8, 6, 3, 4},
            {8, 3, 7, 6, 1, 4, 2, 9, 5},
            {1, 4, 1, 8, 6, 5, 7, 2, 9},
            {9, 5, 8, 2, 4, 7, 3, 6, 1},
            {7, 6, 2, 3, 9, 1, 4, 5, 8},
            {3, 7, 1, 9, 5, 6, 8, 4, 2},
            {4, 9, 6, 1, 8, 2, 5, 7, 3},
            {2, 8, 5, 4, 7, 3, 9, 1, 6}
    };
//-----------------------------function to initialize semaphores
void initSemaphore()
{
        sem_init(&semaphores[0],0,1);
        sem_init(&semaphores[1],0,1);
        sem_init(&semaphores[2],0,1);
}
//-----------------------------function to print sudoku title
void sudoku()
{
    cout << "\t  ___ ___ ___ ___ ___ ___ ___ ___ ___ \n";
    cout << "\t |                                   |\n";
    cout << "\t |           SUDOKU PUZZLE           |\n";
    cout << "\t |___ ___ ___ ___ ___ ___ ___ ___ ___| \n";

}
void sudokuGame()
{
    cout << "\n\n\t  ___*___*___*___*___*___*___*___*___ \n";
    cout << "\t |                                   |\n";
    cout << "\t |        WELCOME TO THE GAME        |\n";
    cout << "\t |___*___*___*___*___*___*___*___*___| \n\n";

}
//-
//-----------------------------function for priniting sudoku puzzle
void printPuzzle()
{
    sudoku();
    for(int i = 0; i < 9; i++)
    {
    	cout<<"\t";
        for(int j = 0; j < 9; j++)
            cout << " | " <<board[i][j];
        cout << " |\n";
        cout << "\t |___|___|___|___|___|___|___|___|___|\n";
    }
    cout<<endl;
}
///////////////////////////////////////////      STRUCTURES       ///////////////////////////////////////////
//--------------------------structure that keeps record of the starting row and col and the grid entries
typedef struct
{
    int row;
    int col;
    int (* grid)[9];
} puzzleParameters;
//--------------------------structure that keeps record of the error entries that are more than 9 and less than 1
typedef struct
{
    char is_valid='n';
    int value;
    int x;
    int y;
} errorEntries;
//--------------------------structure that keeps record of the inavlid entries in each subgrid
typedef struct
{
    char is_valid='n';
    int value;
    int x;
    int y;
} subInvalidEntries;
//--------------------------structure that keeps record of the inavlid entries in each column
typedef struct
{
    char is_valid='n';
    int value;
    int x;
    int y;
} colInvalidEntries;
//--------------------------structure that keeps record of the inavlid entries in each row
typedef struct
{
    char is_valid='n';
    int value;
    int x;
    int y;
} rowInvalidEntries;
//--------------------------making arrays to store invalid entries
rowInvalidEntries invalid_row[81];
colInvalidEntries invalid_col[81];
subInvalidEntries invalid_sub[81];
errorEntries error_entries[81];

///////////////////////////////////////////      FUNCTIONS USED IN PHASE 1       ///////////////////////////////////////////
//-----------------------------function to check if each row has invalid entries or not
void * checkRows(void * args)
{
    cout<<" __________________________________\n";
    cout<<"|                                  |\n";
    cout<<"|-------THREAD FOR ROWS (id)-------|\n";
    cout<<"|__________________________________|\n";
    int invalid_entry = 0; int row_index = 0;
    puzzleParameters * object = (puzzleParameters *) args;
    int startRow = object->row;
    int startCol = object->col;
    for (int i = startRow; i < 9; ++i)
    {
        cout<<"\n*-------|       Row "<<i+1<<"       |-------*\n";
        int row[10] = {0};
        for (int j = startCol; j < 9; ++j)
        {
            int val = object->grid[i][j];
            // checking if value is more than 9 or less than 1
            if(val>9 || val<1)
            {
                //incrementing in error entries and displaying them if condition is true
                sem_wait(&semaphores[0]);
		error_entries[error_index].is_valid='y';
		error_entries[error_index].value=val;
		error_entries[error_index].x=j;
		error_entries[error_index].y=i;
		error_index++;
		sem_post(&semaphores[0]) ;
            	invalid_entry++;
            	invalid_row_entries++;
            	cout<<"\t>> Error["<<i<<"]["<<j<<"]  = "<<val<<endl;
            }
            else
            {
                //if the value is not 0, it means there is a duplicate value which is invalid
		    if (row[val] != 0)
		    {
                // incrementing in invalid entries and displayind them if duplicate detected
		    	cout<<"\t>> Error["<<i<<"]["<<j<<"]  = "<<val<<endl;
			invalid_row[row_index].is_valid='y';
			invalid_row[row_index].value=val;
			invalid_row[row_index].x=i;
			invalid_row[row_index].y=j;
			row_index++;
		    	invalid_entry++;
		    	invalid_row_entries++;
		    }
		    else
		        row[val] = 1;
           }
        }
        if (invalid_entry>0)
    		cout<<"\t~> Total Errors = "<<invalid_entry<<endl;
    	else
    		cout<<"\t>> No Error"<<endl;
    	invalid_entry = 0;
    }
    if (invalid_row_entries>0)
    	return (void *) 0;
    return (void *) 1;
}
//-----------------------------function to check if each column has invalid entries or not
void * checkCols(void * args)
{
    cout<<" _____________________________________\n";
    cout<<"|                                     |\n";
    cout<<"|-------THREAD FOR COLUMNS (id)-------|\n";
    cout<<"|_____________________________________|\n";
    int invalid_entry = 0; int col_index = 0;
    puzzleParameters * object = (puzzleParameters *) args;
    int startCol = object->col;
    int startRow = object->row;
    for (int i = startCol; i < 9; ++i)
    {
        cout<<"\n*-------|       Column "<<i+1<<"       |-------*\n";
        int col[10] = {0};
        for (int j = startRow; j < 9; ++j)
        {
            int val = object->grid[j][i];
            // checking if value is more than 9 or less than 1
            if(val>9 || val<1)
            {
                //incrementing in error entries and displaying them if condition is true
            	invalid_entry++;
            	invalid_col_entries++;
            	cout<<"\t>> Error["<<j<<"]["<<i<<"]  = "<<val<<endl;
            }
            else
            {
                //if the value is not 0, it means there is a duplicate value which is invalid
		    if (col[val] != 0)
		    {
                // incrementing in invalid entries and displayind them if duplicate detected
		    	cout<<"\t>> Error["<<j<<"]["<<i<<"]  = "<<val<<endl;
		    	invalid_col[col_index].is_valid='y';
                	invalid_col[col_index].value=val;
                	invalid_col[col_index].x=j;
                	invalid_col[col_index].y=i;
                	col_index++;
		    	invalid_entry++;
		    	invalid_col_entries++;
		    }
		    else
		        col[val] = 1;
            }
        }
        if (invalid_entry>0)
    		cout<<"\t~> Total Errors = "<<invalid_entry<<endl;
    	else
    		cout<<"\t>> No Error"<<endl;
    	invalid_entry = 0;
    }
    if (invalid_col_entries>0)
    	return (void *) 0;
    return (void *) 1;
}
//----------------------------- function to display title for subgrid
void coutSubgrid()
{
    cout<<" _____________________________________\n";
    cout<<"|                                     |\n";
    cout<<"|-------THREAD FOR SUBGRID (id)-------|\n";
    cout<<"|_____________________________________|\n";
}

//-----------------------------function to check if each sub grid has invalid entries or not
void * checkSubGrid(void * args)
{
    cout<<"\n*-------|       Subgrid "<<++sub_count<<"       |-------*\n";
    int invalid_entry = 0; int invalid_entries = 0;
    puzzleParameters * object = (puzzleParameters *) args;
    int startRow = object->row;
    int startCol = object->col;
    int subgrid[10] = {0};
    // iterating in the top 3 values first then the second and then third respectively
    for (int i = startRow; i < startRow + 3; ++i)
    {
        for (int j = startCol; j < startCol + 3; ++j)
        {
            int val = object->grid[i][j];
            // checking if value is more than 9 or less than 1
            if(val>9 || val<1)
            {
                //incrementing in error entries and displaying them if condition is true
            	invalid_entry++;
            	invalid_sub_entries++;
            	invalid_entries++;
            	cout<<"\t>> Error["<<i<<"]["<<j<<"]  = "<<val<<endl;
            }
            else
            {
                //if the value is not 0, it means there is a duplicate value which is invalid
            if (subgrid[val] != 0)
            {
                // incrementing in invalid entries and displayind them if duplicate detected
                cout<<"\t>> Error["<<i<<"]["<<j<<"]  = "<<val<<endl;
          	sem_wait(&semaphores[1]);
		invalid_sub[sub_index].is_valid='y';
		invalid_sub[sub_index].value=val;
		invalid_sub[sub_index].x=i;
		invalid_sub[sub_index].y=j;
		sub_index++;
		sem_post(&semaphores[1]);
            	invalid_entry++;
            	invalid_entries++;
            	pthread_mutex_lock(&l);
            	invalid_sub_entries++;
            	pthread_mutex_unlock(&l);
            }
            else
                subgrid[val] = 1;
           }
        }
        if (invalid_entry>0)
    		cout<<"\t~> Total Errors = "<<invalid_entry<<endl;
    	invalid_entry = 0;
    }
    // if there are no entries, display respectively
    if(invalid_entries==0)
    	cout<<"\t>> No Error"<<endl;
    if (invalid_sub_entries>0)
    	return (void *) 0;
    return (void *) 1;
}
///////////////////////////////////////     PHASE 1     ///////////////////////////////////////////////////////
void phase1()
{
    puzzleParameters *list[9];
    int i = 0;
    int j = 0;
    int check = 3;
    for(int index = 0; index < 9; index++)
    {
    	/* Starting row and col for 9 subgrids
     		     ________________________
		    |  0,0  |  0,3  |  0,6  |
		    |_______|_______|_______|
		    |  3,0  |  3,3  |  3,6  |
		    |_______|_______|_______|
		    |  6,0  |  6,3  |  6,6  |
		    |_______|_______|_______|

    	*/
    	//checking if index is 3, increment i,check by 3 and renew j
    	if(index == check)
    	{
    		j = 0;
    		i += 3;
    		check += 3;
    	}
    	puzzleParameters* p = (puzzleParameters *) malloc(sizeof(puzzleParameters));
    	p->row = i;
    	p->col = j;
    	p->grid = board;

    	list[index] = p;
    	j += 3;
    }
    //initializing 11 threads
    pthread_t threads[11];
    void * check_rows;
    void * check_cols;
    void * subgrids[9];

    //checking for validity in rows
    pthread_create(&threads[0], NULL, checkRows, (void *)list[0]);
    pthread_join(threads[0], &check_rows);
    //displaying total invalid entries of rows
    cout<<"** "<<invalid_row_entries<<" invalid enteries in all rows\n";

    //checking for validity in columns
    pthread_create(&threads[1], NULL, checkCols, (void *) list[0]);
    pthread_join(threads[1], &check_cols);
    //displaying total invalid entries of columns
    cout<<"** "<<invalid_col_entries<<" invalid enteries in all columns\n";

    //checking for validity in all 9 subgrids
    coutSubgrid();
    for(int i = 0; i<9; i++)
    {
	    pthread_create(&threads[i+2], NULL, checkSubGrid, (void *) list[i]);
	    pthread_join(threads[i+2], &subgrids[i]);
	    pthread_mutex_destroy(&l);

    }
    //displaying total invalid entries of subgrids
    cout<<"** "<<invalid_sub_entries<<" invalid enteries in all subgrids\n";
}
///////////////////////////////////////////      FUNCTIONS USED IN PHASE 2       ///////////////////////////////////////////
//-----------------------------function to solve the error and invalid entries in each grid
void* solveSubs(void* subs)
{
    puzzleParameters* p = (puzzleParameters*)subs;
    int* temp = new int[10];
    //iterating through each 9 entries one by one in the subgrid (first by row followed by column)
    for(int i = p->row; i <  p->row + 3; i++)
    {
        for(int j = p->col; j < p->col+3; j++)
        {
        //solving and saving in a temp array
		temp[board[i][j]]++;
        }
    }
    //returning the solved temp array
    pthread_exit((void*)temp);
}
//-----------------------------function to solve the error and invalid entries in each column
void* solveCols(void* col)
{
    int* column = (int*)col;
    int temp[10] = {0};
    //iterating through each 9 entries in a column
    for(int i = 0; i < 9; i++)
    {
         //solving and saving in a temp array
        temp[board[i][*column]]++;
    }
     //returning the solved temp array
    pthread_exit((void*)temp);
}

//-----------------------------function to solve the error and invalid entries in each row
void* solveRows(void* row)
{
    int* rownum = (int*)row;
    int temp[10] = {0};
    //iterating through each 9 entries in a row
    for(int i = 0; i < 9; i++)
    {
        //solving and saving in a temp array
        temp[board[*rownum][i]]++;
    }
    //returning the solved temp array
    pthread_exit((void*)temp);
}

// -----------------------------function that calls the above functions to solve the sudoku puzzle
void* solveBoard(void* sub)
{
    void* temp[3];
    int* boards[3];
    int* invalid_indexes = (int*)sub;
    puzzleParameters p;

    //each subgrid's start is being decided here by saving the starting index of the invalid entry in p.row and p.col
    p.row = invalid_row[invalid_indexes[0]].x / 3 * 3;
    p.col = invalid_col[invalid_indexes[1]].y / 3 * 3;

    //solution[0] : solve_row, temp1[0] : returned_rows, board[0] : solution_of_rows
    pthread_create(&solutions[0], NULL, solveRows, (void*)&invalid_row[invalid_indexes[0]].x);
    pthread_join(solutions[0], &temp[0]);
    boards[0] = (int*)temp[0];

    //solution[1] : solve_col, temp1[1] : returned_cols, board[1] : solution_of_cols
    pthread_create(&solutions[1], NULL, solveCols, (void*)&invalid_col[invalid_indexes[1]].y);
    pthread_join(solutions[1], &temp[1]);
    boards[1] = (int*)temp[1];

    //solution[2] : solve_subGrid, temp1[2] : returned_subGrids, board[2] : solution_of_subgrids
    pthread_create(&solutions[2], NULL, solveSubs, (void*)&p);
    pthread_join(solutions[2], &temp[2]);
    boards[2] = (int*)temp[2];

    for(int i = 1; i < 10; i++)
    {
        if(boards[0][i] == 0 && boards[1][i] == 0 && boards[2][i] == 0)
        {
            sem_wait(&semaphores[2]);
            board[invalid_row[invalid_indexes[0]].x][invalid_col[invalid_indexes[1]].y] = i;
            sem_post(&semaphores[2]);
        }
    }
    return NULL;
}
/////////////////////////////////////////////     PHASE 2    //////////////////////////////////////////////////////////
void phase2()
{
    //check if the sum of invalid/error enteries is more than 0 that means sodoku has to be solved so phase 2 functions are called
    if((invalid_sub_entries+invalid_row_entries+invalid_col_entries)>0)
    {
    	int* invalid_indexes = new int[3];
    	bool match1 = false;
        bool match2 = false;
        int index_c = 0;
        int index_s = 0;
        printf("INVALID SUDOKU PUZZLE\n\n");

        // run for 81 times to iterate through the whole grid
        for(int r=0;r<81;r++)
        {
            // check for invalid entries
            if(invalid_row[r].is_valid=='y')
            {
                // run again for 81 times to iterate through the whole grid (this time keeping record of invalid entries)
                for(int c = 0; c < 81; c++)
                {
                    //if the invalid value in the subgrid is equal to the invalid value in the row then theres a match found
                    if(invalid_sub[c].value == invalid_row[r].value && invalid_sub[c].is_valid == 'y')
                    {
                        index_s = c;
                        // cout << "IDXS: " << idxs << endl;
                        invalid_sub[c].is_valid = 'n';
                        match1 = true;
                    }
                    //if the invalid value in the subgrid is equal to the invalid value in the column then theres a match found
                    if(invalid_col[c].value == invalid_row[r].value && invalid_col[c].is_valid == 'y')
                    {
                        index_c = c;
                        // cout << "IDXC: " << idxc << endl;
                        invalid_col[c].is_valid = 'n';
                        match2 = true;
                    }
                    //when both above matches are true we call the above function which solves the whole sudoku board
                    if(match1 && match2)
                    {
                        invalid_indexes[0] = r;
                        invalid_indexes[1] = index_c;
                        invalid_indexes[2] = index_s;
                        //solutions[3] = solve_board
                        //the array of size 3 containing invalid entries is passed in the function to be solved
                        pthread_create(&solutions[3], NULL, solveBoard, (void*)invalid_indexes);
                        pthread_join(solutions[3], NULL);
                        //both matches are set to false after the board is solved to exit the if statement
                        match1 = false;
                        match2 = false;
                    }
                }
            }
        }
        printf("Solving...\n");
        printf("\nAFTER SOLVING SUDOKU PUZZLE\n");
    }
    else
        printf("\nALREADY VALID SUDOKU PUZZLE\n\n");
    invalid_row_entries = 0;
    invalid_col_entries = 0;
    invalid_sub_entries = 0;
}
/////////////////////////////////////////////    MAIN    ////////////////////////////////////////////////
int main(void)
{
	sudokuGame();
	initSemaphore();//initializing semaphores calling this function
	printPuzzle();//printing sudoku puzzle before solving it
	phase1();//calling phase 1 in which it will check if puzzle is valid or not
	phase2();//calling phase 2 i which it solves the invalid entires present in board
	printPuzzle();//now printing the error free puzzle
	phase1();//again calling phase 1 so that it can check validity of solved puzzle
}



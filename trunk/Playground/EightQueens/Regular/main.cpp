#include <iostream>
#include <string>
#include <sstream>
#include <vector>


class ChessBoard
{
public:
    ChessBoard() :
        mFields(64, 0)
    {
    }

    int get(unsigned row, unsigned col) const
    { return mFields[8 * row + col]; }

    void set(unsigned row, unsigned col, int value)
    {
        if (get(row, col) == 0)
        {
            mFields[8 * row + col] = value;
        }
    }

    void setQueen(unsigned row, unsigned col, int value)
    {
        // Mark queen as negative value.
        set(row, col, -value);

        // Mark vertical line as occupied
        for (unsigned r = 0; r < 8; ++r)
        {
            set(r, col, value);
        }

        // Mark horizontal line as occupied
        for (unsigned c = 0; c < 8; ++c)
        {
            set(row, c, value);
        }

        // Mark diagonal as occupied
        int r1_offset = int(row) - int(col);
        int r2_offset = row + col;

        for (int i = 0; i < 8; ++i)
        {
            int r = r1_offset + i;
            if (r >= 0 && r < 8)
            {
                set(r, i, value);
            }


            int r2 = r2_offset - i;
            if (r2 >= 0 && r2 < 8)
            {
                set(r2, i, value);
            }
        }
    }

private:
    std::vector<int> mFields;
};


bool PutQueen(ChessBoard & board, int rowOffset = 0, int colOffset = 0, int remaining = 8)
{
    if (remaining == 0)
    {
        return true;
    }

    for (unsigned row = rowOffset; row < 8; ++row)
    {
        for (unsigned col = colOffset; col < 8; ++col)
        {
            colOffset = 0; // set to 0 again
            if (board.get(row, col) == 0)
            {
                ChessBoard backup = board;

                board.setQueen(row, col, 8 - remaining + 1);
                if (PutQueen(board, row, col + 1, remaining - 1))
                {
                    return true;
                }
                else
                {
                    // Revert
                    board = std::move(backup);
                }
            }
        }
    }
    return false;
}


void PrintBoard(const ChessBoard & board)
{
    for (unsigned r = 0; r < 8; ++r)
    {
        for (unsigned c = 0; c < 8; ++c)
        {
            if (c != 0)
            {
                std::cout << " ";
            }
            int v = board.get(r, c);
            if (v != 0)
            {
                std::cout << v;
            }
            else
            {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
}


int main(int argc, char ** argv)
{
    ChessBoard board;
    PutQueen(board);
    
    ChessBoard printable;

    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            int value = board.get(r, c);
            if (value < 0)
            {
                printable.set(r, c, -value);
            }
        }
    }
    PrintBoard(printable);
    return 0;
}

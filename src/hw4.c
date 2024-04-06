#include "hw4.h"

void initialize_game(ChessGame *game) {

    //create our chess game board
    char initalizeGameBoard[8][8] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };

    //copy everything into from initalized to the chessboard from the struct
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            game -> chessboard[i][j] = initalizeGameBoard[i][j];
        }
    }

    //set move count to 0
    game -> moveCount = 0;
    
    //set captured count to 0
    game -> capturedCount = 0;

    //set current player to white
    game -> currentPlayer = WHITE_PLAYER;

}

void chessboard_to_fen(char fen[], ChessGame *game) {
    (void)fen;
    (void)game;
}

/* helper function - checking direction of movement based on color */ 
int color_movement(int src_row, int dest_row, int current_player) {
    
    //check movement for white player
    if(current_player == WHITE_PLAYER) {
        if(src_row > dest_row) {
            return 1; //valid moving upwards
        } else if(src_row < dest_row) {
            return -1; //invalid moving backwards
        }   
    }

    //check movement for black player
    else if(current_player == BLACK_PLAYER) {
         if(src_row < dest_row) {
            return 2; //valid moving downwards
        } else if(src_row > dest_row) {
            return -1; //invalid moving backwards 
        }  
    }
    return 0;
}

/* verifies the pawn is moving forward by one or two squares, and in the correct direction based on color */
bool is_valid_pawn_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {

    //check if there is no piece at the location
    if(game -> chessboard[src_row][src_col] != piece && game -> chessboard[src_row][src_col] != toupper(piece)) {
        return false;
    }

    //check if the piece is not a pawn
    if(piece != 'p' && piece != 'P') {
        return false;
    }

    //check the direction based on the players color (call helper)
    int currentPlayer = game -> currentPlayer;
    int direction = color_movement(src_row, dest_row, currentPlayer);

    //check invalid direction 
    if(direction == 0 || direction == -1) { 
        return false;
    }

    //check white pawn
    else if(direction == 1) {
        //check if it moves one and the next spot is open
        if(src_row == dest_row + 1 && game -> chessboard[dest_row][dest_col] == '.' && src_col == dest_col) {
            return true;
        } 
        //check if it moves two squares and the next spot is open
        else if(src_row == dest_row + 2 && src_row == 6 && game -> chessboard[dest_row + 1][dest_col] == '.' 
                && game -> chessboard[dest_row][dest_col] == '.') {
            return true;
        } 
        //if there is the a pawn of the opp color diag to it and if so then capture the pawn
        else if((src_row == dest_row + 1 && (src_col == dest_col - 1 || src_col == dest_col + 1)) && game -> chessboard[dest_row][dest_col] != '.' 
                    && game->chessboard[dest_row][dest_col] != 'P') {
            return true;
        }
    }

    //check black pawn
    else if(direction == 2) {
        //check if it moves one and the next spot is open
        if(src_row == dest_row - 1 && game -> chessboard[dest_row][dest_col] == '.') {
            return true;
        }
        //check if it moves two squares and the next spot is open
        else if(src_row == dest_row - 2 && src_row == 1 && game -> chessboard[dest_row - 1][src_col] == '.' 
                && game -> chessboard[dest_row][dest_col] == '.') {
            return true;
        } 
        //if there is the a pawn of the opp color diag to it and if so then capture the pawn
        if((src_row == dest_row - 1 && (src_col == dest_col - 1 || src_col == dest_col + 1)) && game -> chessboard[dest_row][dest_col] != '.' 
                    && game->chessboard[dest_row][dest_col] != 'P') {
            return true;
        }
    }
    //otherwise invalid 
    return false;
}

/* verifies that the rook is moving horizontally or vertically and there is an empty path */
bool is_valid_rook_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {

    char piece = game -> chessboard[src_row][src_col];

    //check if the piece is not a rook
    if(piece != 'r' && piece != 'R' && piece != 'q' && piece != 'Q') {
        return false;
    }

    //check if it is moving horizontally or vertically 
    if(src_row != dest_row && src_col != dest_col) {
        return false;
    }
    
    int horizPath;
    int vertPath;

    //check nothing is in the way of the horizontal path
    if(src_row == dest_row) {
        if(dest_col > src_col) {
            horizPath = 1;
        } else {
            horizPath = -1;
        }

        for(int i = src_col + horizPath; i != dest_col; i += horizPath) {
            if(game -> chessboard[src_row][i] != '.') {
                return false;
            }
        }
    }

    //check nothing is in the way of the vertical path
    else {
        if(dest_row > src_row) {
            vertPath = 1;
        } else {
            vertPath = -1;
        }

        for(int i = src_row + vertPath; i != dest_row; i += vertPath) {
            if(game -> chessboard[i][dest_col] != '.') {
                return false;
            }
        }
    }
    return true;
}

/* verifies the knight is moving in an L shape (two squares vertically and one square horizontally, or two squares horizontally and one square vertically) */
bool is_valid_knight_move(int src_row, int src_col, int dest_row, int dest_col) {

    int row = abs(src_row - dest_row);
    int col = abs(src_col - dest_col);

    bool checkLShape = false;
    checkLShape = ((row == 2 && col == 1) || (row == 1 && col == 2));

    if(checkLShape) {
        return true;
    } else {
        return false;
    }
}

/* verifies the bishop is moving diagonally and there is a clear path */
bool is_valid_bishop_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {

    if(src_row == dest_row || src_col == dest_col) {
        return false;
    }

    //check to make sure row and col length are the same
    int rowDifference = abs(dest_row - src_row);
    int colDifference = abs(dest_col - src_col);

    if(rowDifference != colDifference) {
        return false;
    }

    //direction of movement
    int row, col;
    if(dest_row > src_row) {
        row = 1;
    } else {
        row = -1;
    }

    if(dest_col > src_col) {
        col = 1;
    } else {
        col = -1;
    }

    for(int i = row + src_row, j = col + src_col; i != dest_row && j != dest_col; i += row, j += col) {
        if(game -> chessboard[i][j] != '.') {
            return false;
        }
    }

    return true;
}

/* verifies that the queen is moving like a rook (horizontally or vertically) or like a bishop (diagonally) */
bool is_valid_queen_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {

    //queen can move like a rook or a bishop
    if((is_valid_rook_move(src_row, src_col, dest_row, dest_col, game) == true) || (is_valid_bishop_move(src_row, src_col, dest_row, dest_col, game) == true)) {
        return true;
    } else {
        return false;
    }
}

/* verifies that the king is moving one square in any direction (horizontally, vertically, or diagonally) */
bool is_valid_king_move(int src_row, int src_col, int dest_row, int dest_col) {

    //find the difference between the sourc and dest in rows and cols
    int rows, cols;
    rows = src_row - dest_row;
    cols = src_col - dest_col;

    //check if the diff in rows and cols is <= 1
    if((rows == 0 || rows == 1 || rows == -1) && (cols == 0 || cols == 1 || cols == -1)) {
            return true;
    } 
    else {
        return false;
    }
}

/* call each function we made for validity and check if they work */
bool is_valid_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {

    //check the validity of each piece 
    if(piece == 'p' || piece == 'P') {
        return is_valid_pawn_move(piece, src_row, src_col, dest_row, dest_col, game);
    } 
    else if(piece == 'r' || piece == 'R') {
        return is_valid_rook_move(src_row, src_col, dest_row, dest_col, game);

    } else if(piece == 'n' || piece == 'N') {
        return is_valid_knight_move(src_row, src_col, dest_row, dest_col);

    } else if(piece == 'b' || piece == 'B') {
        return is_valid_bishop_move(src_row, src_col, dest_row, dest_col, game);

    } else if (piece == 'q' || piece == 'Q') {
        return is_valid_queen_move(src_row, src_col, dest_row, dest_col, game);

    } else if (piece == 'k' || piece == 'K') {
        return is_valid_king_move(src_row, src_col, dest_row, dest_col);

    } else {
        return false;
    }
    return false;

}

void fen_to_chessboard(const char *fen, ChessGame *game) {
    (void)fen;
    (void)game;
}

int parse_move(const char *move, ChessMove *parsed_move) {

    char srcCol = move[0];
    char srcRow = move[1];
    char destCol = move[2];
    char destRow = move[3];

    //parse_move_invalid_format
    if(((int)strlen(move) != 4 && (int)strlen(move) != 5) ||
        ((srcCol < 'a' || srcCol > 'h') || (destCol < 'a' || destCol > 'h'))) {
        return PARSE_MOVE_INVALID_FORMAT;
    }

    //parse_move_out_of_bounds
    if(srcRow < '1' || srcRow > '8' || destRow < '1' || destRow > '8') {
        return PARSE_MOVE_OUT_OF_BOUNDS;
    }

    //parse_move_invalid_destination
    if((int)strlen(move) == 5) {
        char dest5Col = move[4];
        if(dest5Col != 'q' && dest5Col != 'r' && dest5Col != 'b' && dest5Col != 'n') {
            return PARSE_MOVE_INVALID_PROMOTION;
        }
        if(destRow != '1' || destRow != '8') {
            return PARSE_MOVE_INVALID_DESTINATION;
        }
    }

    //parse_move_invalid_promotion
    // if((int)strlen(move) == 5) {
    //     char dest5Col = move[4];
    //     if(dest5Col != 'q' && dest5Col != 'r' && dest5Col != 'b' && dest5Col != 'n') {
    //         return PARSE_MOVE_INVALID_PROMOTION;
    //     }
    // }

    (void)move;
    (void)parsed_move;
    return 0;
}

int make_move(ChessGame *game, ChessMove *move, bool is_client, bool validate_move) {
    (void)game;
    (void)move;
    (void)is_client;
    (void)validate_move;
    return -999;
}

int send_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    (void)game;
    (void)message;
    (void)socketfd;
    (void)is_client;
    return -999;
}

int receive_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    (void)game;
    (void)message;
    (void)socketfd;
    (void)is_client;
    return -999;
}

int save_game(ChessGame *game, const char *username, const char *db_filename) {
    (void)game;
    (void)username;
    (void)db_filename;
    return -999;
}

int load_game(ChessGame *game, const char *username, const char *db_filename, int save_number) {
    (void)game;
    (void)username;
    (void)db_filename;
    (void)save_number;
    return -999;
}

void display_chessboard(ChessGame *game) {
    printf("\nChessboard:\n");
    printf("  a b c d e f g h\n");
    for (int i = 0; i < 8; i++) {
        printf("%d ", 8 - i);
        for (int j = 0; j < 8; j++) {
            printf("%c ", game -> chessboard[i][j]);
        }
        printf("%d\n", 8 - i);
    }
    printf("  a b c d e f g h\n");
}

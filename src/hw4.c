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

    int empty = 0; 
    char color;
    fen[0] = '\0';

    //color of the current plater
    if(game -> currentPlayer == WHITE_PLAYER) {
        color = 'w';
    } else if(game -> currentPlayer == BLACK_PLAYER) {
        color = 'b';
    }

    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {

            //piece at the current position
            char piece = game -> chessboard[row][col];
        
            if(piece == '.') {
                empty++;
            }
            else if(piece!='.'){
                if(empty > 0){
                    sprintf(fen + strlen(fen) , "%c", '0' + empty);
                    
                }
                sprintf(fen+strlen(fen), "%c", piece);
                empty = 0;
            }
        }

        if (empty > 0) {
            sprintf(fen + strlen(fen), "%d", empty);
            empty = 0;
        }

        // slash at the end of each row
        if(row < 7) {
            strcat(fen, "/");
        }
    }

    sprintf(fen + strlen(fen), "%s", " ");
    sprintf(fen + strlen(fen) , "%c", color);

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
   
    int row = 0;
    int col = 0;
    int empty = 0;
    char color = fen[strlen(fen) - 1];

    //find color
    if(color == 'w') {
        game -> currentPlayer = WHITE_PLAYER;
    } 
    else if(color == 'b') {
        game -> currentPlayer = BLACK_PLAYER;
   }

    //parse the string and move row by row
    for(int i = 0; fen[i] != '\0' && row < 8; i++) {
        char piece = fen[i];
      
        if(piece == '/') {
            row++;
            col = 0; //reset the column
        }
        else if(piece >= '1' && piece <= '8'){
            empty = piece - '0';
            
            while(empty != 0) {
                game -> chessboard[row][col] = '.';
                col++;
                empty--; //decrement until none left
            }
        }
	    else { 
            //place the piece
		    game -> chessboard[row][col++] = piece;
	    }
    }
}

int parse_move(const char *move, ChessMove *parsed_move) {

    char srcCol = move[0];
    char srcRow = move[1];
    char destCol = move[2];
    char destRow = move[3];
    char dest5Col = move[4];

    //parse_move_invalid_format
    if(((int)strlen(move) != 4 && (int)strlen(move) != 5) ||
        ((srcCol < 'a' || srcCol > 'h') || (destCol < 'a' || destCol > 'h'))) {
        return PARSE_MOVE_INVALID_FORMAT;
    }

    //parse_move_out_of_bounds
    if(srcRow < '1' || srcRow > '8' || destRow < '1' || destRow > '8') {
        return PARSE_MOVE_OUT_OF_BOUNDS;
    }  

    //parse_move_invalid_destination and promotion
    if((int)strlen(move) == 5) {
        if(dest5Col != 'q' && dest5Col != 'r' && dest5Col != 'b' && dest5Col != 'n') {
            return PARSE_MOVE_INVALID_PROMOTION;
        }
        if(destRow != '1' && destRow != '8') {
            return PARSE_MOVE_INVALID_DESTINATION;
        }
    }

    //assign parsed values for start square
    parsed_move -> startSquare[0] = srcCol;
    parsed_move -> startSquare[1] = srcRow;
    parsed_move -> startSquare[2] = '\0';

    //assign the parsed values for end square
    parsed_move -> endSquare[0] = destCol;
    parsed_move -> endSquare[1] = destRow;
    parsed_move -> endSquare[2] = dest5Col;
    parsed_move -> endSquare[3] = '\0';

    return 0;
}

/* helper functon - find the respective row and col */
void find_row_and_col(const char *square, int *row, int *col) {
    *col = (square[0] - 'a');
    *row = 7 - (square[1] - '1');
}

int make_move(ChessGame *game, ChessMove *move, bool is_client, bool validate_move) {

    //initialize all my variables first
    int srcRow, srcCol, destRow, destCol;
    find_row_and_col(move -> startSquare, &srcRow, &srcCol);
    find_row_and_col(move -> endSquare, &destRow, &destCol);
    char piece = game -> chessboard[srcRow][srcCol];
    char destPiece = game -> chessboard[destRow][destCol];
    char promotedPiece;

    if (destPiece != '.') {
        destPiece = game -> chessboard[destRow][destCol];
    } else {
        destPiece = '.';
    }   

    //find if it is white or black
    int isWhite = 0, isBlack = 0;
    if (is_client == true) {
        isWhite = 1;
    } else if(is_client == false) {
        isBlack = 1;
    }

    if(validate_move) {

        //move_out_of_turn
        if((isWhite == 1) && game -> currentPlayer != WHITE_PLAYER) {
            return MOVE_OUT_OF_TURN;
        }
        else if((isBlack == 1) && game -> currentPlayer != BLACK_PLAYER) {
            return MOVE_OUT_OF_TURN;
        }

        //move_nothing
        if(piece == '.') {
            return MOVE_NOTHING;
        }

        //move_wrong_color
        if(game -> currentPlayer == WHITE_PLAYER && islower(piece)) {
            return MOVE_WRONG_COLOR; //this is returning for the last test case
        }

        if(game -> currentPlayer == BLACK_PLAYER && isupper(piece)) {
            return MOVE_WRONG_COLOR;
        }

        //move_sus
        if(isupper(piece) && isupper(destPiece)) {
            return MOVE_SUS;
        }
        if((islower(piece) && islower(destPiece))) {
            return MOVE_SUS;
        }
        
        //move_not_a_pawn
        if (strlen(move -> startSquare) + strlen(move -> endSquare) == 5 && (piece != 'P' && piece != 'p')) {
            return MOVE_NOT_A_PAWN;
        }

        //move_missing_promotion
        if (strlen(move -> startSquare) + strlen(move -> endSquare) == 4 && (destRow == 0 || destRow == 7) && (piece == 'P' || piece == 'p')) {
            return MOVE_MISSING_PROMOTION;
        }

        //move_wrong
        if(!is_valid_move(piece, srcRow, srcCol, destRow, destCol, game) == true) {
            return MOVE_WRONG;
        }

        if(piece == 'P' && destRow == 7) {
            promotedPiece = move -> endSquare[2]; //white
            game -> chessboard[destRow][destCol] = promotedPiece;
        }
        if(piece == 'p' && destRow == 0) {
            promotedPiece = move -> endSquare[2]; //black
            game -> chessboard[destRow][destCol] = promotedPiece;
        }
    } 

    //update game -> moves
    game -> moves[game -> moveCount] = *move;
    game -> moveCount++;

    if(destPiece != '.') {
        game -> capturedPieces[game -> capturedCount] = destPiece;
        game -> capturedCount++;
    }

    //update game -> chessboard
    game -> chessboard[destRow][destCol] = piece;
    game -> chessboard[srcRow][srcCol] = '.';

    //update the next player (white goes to black and vice versa)
    if(isWhite) {
        game -> currentPlayer = BLACK_PLAYER;
    } else {
        game -> currentPlayer = WHITE_PLAYER;
    }

    return 0;
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

/* open the file and generate the fen string */
int save_game(ChessGame *game, const char *username, const char *db_filename) {

    FILE *fname = fopen(db_filename, "a");
    if(username[0] == '\0') {
        return -1;
    } //no username 

    for(int i = 0; username[i] != '0'; i++) {
        if(username[i] == ' ') {
            fclose(fname);
            return -1;
        }
    }

    char fen[BUFFER_SIZE];
    chessboard_to_fen(fen, game);
    fprintf(fname, "%s:%s\n", username, fen);
    fclose(fname);

    return 0;
}

int load_game(ChessGame *game, const char *username, const char *db_filename, int save_number) {

    FILE *file = fopen(db_filename, "r");

    if(file == NULL) {
		return -1;
    } //null file

    if(save_number <= 0) {
        return -1;
    } //invalid save

    char fen[100] = {'\0'};
    char read[100] = {'\0'};
    int count = 0;
    char player;

    while(fscanf(file, "%s:%s %c", read, fen, &player) > 0) {
        if(strcmp(read, username) == 0) {
            count++;
            if(count == save_number) {
                //load the game here
                fen_to_chessboard(fen, game);
                return 0;
                break;
            }
        }
    }

    (void)game;
    fclose(file);
    return -1;
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

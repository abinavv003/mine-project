#include <stdio.h>   
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_PLAYERS 6
#define PROFILE_FILE "player_profiles5.dat"

// Player structure
typedef struct {
    char name[30];
    int moves;
    int cells_revealed; 
    double high_score; 
} Player;


char highest_scorer[30] = "";
double highest_score = 0.0;

int main() {
    Player players[MAX_PLAYERS] = {0}; 
    int player_count = 0;
    Player *current_player = NULL;

    // Default board size
    int rows = 10, cols = 10, mines = 20;

    // Load existing profiles
    loadProfiles(players, &player_count);

    int choice;
    while (1) {
        printf("\n--- Main Menu ---\n");
        printf("1. Play Game\n");
        printf("2. Set Board Size\n");
        printf("3. View Leaderboard\n");
        printf("4. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                createOrSelectProfile(players, &player_count, &current_player);
                if (current_player) {
                    playGame(players, player_count, current_player, rows, cols, mines);
                    saveProfiles(players, player_count);
                }
                break;
            case 2:
                customizeBoardSize(&rows, &cols, &mines);
                break;
            case 3:
                displayLeaderboard(players, player_count);
                break;
            case 4:
                printf("Exiting the game. Goodbye!\n");
                return 0;
            default:
                printf("Invalid option. Please try again.\n");
        }
    }

    return 0;
}


void customizeBoardSize(int *rows, int *cols, int *mines) {
    printf("Enter custom number of rows: ");
    scanf("%d", rows);
    printf("Enter custom number of columns: ");
    scanf("%d", cols);
    printf("Enter number of mines: ");
    scanf("%d", mines);
}


void initializeBoard(char **board, char **revealed, int rows, int cols) {
	int i,j;
    for ( i = 0; i < rows; i++) {
        for ( j = 0; j < cols; j++) {
            board[i][j] = '0';
            revealed[i][j] = '*';  
        }
    }
}


void placeMines(char **board, int rows, int cols, int mines) {
    srand(time(NULL));
    int count = 0;

    while (count < mines) {
        int row = rand() % rows;
        int col = rand() % cols;

        if (board[row][col] != 'M') {
            board[row][col] = 'M';
            count++;
        }
    }
}


void printBoard(char **board, char **revealed, int rows, int cols) {
    printf("   ");
    int i;
    for ( i = 0; i < cols; i++) {
        printf("%d ", i);
    }
    printf("\n");

    for ( i = 0; i < rows; i++) {
    	int j;
        printf("%d  ", i);
        for (j = 0; j < cols; j++) {
            printf("%c ", revealed[i][j]);
        }
        printf("\n");
    }
}


int isValid(int row, int col, int rows, int cols) {
    return (row >= 0 && row < rows && col >= 0 && col < cols);
}


int revealCell(char **board, char **revealed, int row, int col, int rows, int cols, Player *current_player) {
    if (!isValid(row, col, rows, cols) || revealed[row][col] != '*') {
        return 0;  
    }

    revealed[row][col] = board[row][col];  

    
    if (board[row][col] == 'M') {
        return 1;  
    }

    
    current_player->cells_revealed++; 
    return 0;  
}


// Game loop
void playGame(Player *players, int player_count, Player *current_player, int rows, int cols, int mines) {
    char **board = (char **)malloc(rows * sizeof(char *));
    char **revealed = (char **)malloc(rows * sizeof(char *));
    int i;
    
    for ( i = 0; i < rows; i++) {
        board[i] = (char *)malloc(cols * sizeof(char));
        revealed[i] = (char *)malloc(cols * sizeof(char));
    }

    current_player->cells_revealed = 0; 
    initializeBoard(board, revealed, rows, cols);
    placeMines(board, rows, cols, mines);

    int row, col;
    while (1) {
        printBoard(board, revealed, rows, cols);

        printf("Enter row and column to reveal (e.g. 1 2): ");
        scanf("%d %d", &row, &col);

        if (revealCell(board, revealed, row, col, rows, cols, current_player)) {
            printf("Game Over! You hit a mine.\n");
            printBoard(board, board, rows, cols);  
            break;
        }

        
        if (current_player->cells_revealed == (rows * cols - mines)) {
            printf("Congratulations %s! You won the game!\n", current_player->name);
            break;
        }
    }

    
    if (current_player->high_score < current_player->cells_revealed) {
        current_player->high_score = current_player->cells_revealed;
        
        strcpy(highest_scorer, current_player->name);
        highest_score = current_player->high_score;
    }

   
    for (i = 0; i < rows; i++) {
        free(board[i]);
        free(revealed[i]);
    }
    free(board);
    free(revealed);
}


void createOrSelectProfile(Player *players, int *player_count, Player **current_player) {
    int choice;
    char name[30];

    printf("1. Create new profile\n2. Select existing profile\n");
    scanf("%d", &choice);
    getchar();  

    if (choice == 1) {
        if (*player_count >= MAX_PLAYERS) {
            printf("Maximum player profiles reached.\n");
            return;
        }
        printf("Enter player name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';  
        strcpy(players[*player_count].name, name);
        players[*player_count].moves = 0;
        players[*player_count].cells_revealed = 0;
        players[*player_count].high_score = 0;
        *current_player = &players[*player_count];
        (*player_count)++;
    } else if (choice == 2) {
        printf("Enter player name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';  

        
        int i,found = 0;
        for ( i = 0; i < *player_count; i++) {
            if (strcmp(players[i].name, name) == 0) {
                *current_player = &players[i];
                found = 1;
                printf("Profile selected: %s\n", (*current_player)->name);
                break;
            }
        }
        if (!found) {
            printf("Profile not found.\n");
        }
    } else {
        printf("Invalid choice.\n");
    }
}


void displayLeaderboard(Player *players, int player_count) {
    printf("\n--- Leaderboard ---\n");
    int i;
    for (i = 0; i < player_count; i++) {
        printf("%s: %.0f cells revealed\n", players[i].name, players[i].high_score);
    }
}


void saveProfiles(Player *players, int player_count) {
    FILE *file = fopen(PROFILE_FILE, "wb");
    if (file) {
        fwrite(players, sizeof(Player), player_count, file);
        fclose(file);
    } else {
        printf("Error saving profiles.\n");
    }
}


void loadProfiles(Player *players, int *player_count) {
    FILE *file = fopen(PROFILE_FILE, "rb");
    if (file) {
        *player_count = fread(players, sizeof(Player), MAX_PLAYERS, file);
        fclose(file);
    } else {
        printf("No existing profiles found.\n");
    }
}


int countAdjacentMines(char **board, int row, int col, int rows, int cols) {
    int i,j, count = 0;
    for (i = row - 1; i <= row + 1; i++) {
        for (j = col - 1; j <= col + 1; j++) {
            if (isValid(i, j, rows, cols) && board[i][j] == 'M') {
                count++;
            }
        }
    }
    return count;
}


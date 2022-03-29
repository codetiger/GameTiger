#include "../core/common.h"
#include "../core/screen.h"
#include "../core/image.h"
#include "../core/keyboard.h"

enum BLOCKTYPE {
    T_BLOCK = 1, 
    I_BLOCK = 2, 
    O_BLOCK = 3, 
    J_BLOCK = 4, 
    L_BLOCK = 5, 
    Z_BLOCK = 6, 
    S_BLOCK = 7
};

#define BOARD_WIDTH 12
#define BOARD_HEIGHT 20
#define BLOCK_SIZE 4
#define totalBlockTypes 7


struct Block{
    bool cells[16];
    uint8_t size;
    BLOCKTYPE type;
    
    Block() {
        size = 4;
        bool ca[16] = {
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        };
        memcpy(cells, ca, sizeof(ca));
    }

    Block(BLOCKTYPE t) {
        type = t; 
        if(type == T_BLOCK) {
            size = 3;
            bool ca[16] = {
                1, 1, 1, 0,
                0, 1, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0
            };
            memcpy(cells, ca, sizeof(ca));
        } else if(type == I_BLOCK) {
            size = 4;
            bool ca[16] = {
                1, 0, 0, 0,
                1, 0, 0, 0,
                1, 0, 0, 0,
                1, 0, 0, 0
            };
            memcpy(cells, ca, sizeof(ca));
        } else if(type == O_BLOCK) {
            size = 2;
            bool ca[16] = {
                1, 1, 0, 0,
                1, 1, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0
            };
            memcpy(cells, ca, sizeof(ca));
        } else if(type == J_BLOCK) {
            size = 3;
            bool ca[16] = {
                0, 1, 0, 0,
                0, 1, 0, 0,
                1, 1, 0, 0,
                0, 0, 0, 0
            };
            memcpy(cells, ca, sizeof(ca));
        } else if(type == L_BLOCK) {
            size = 3;
            bool ca[16] = {
                1, 0, 0, 0,
                1, 0, 0, 0,
                1, 1, 0, 0,
                0, 0, 0, 0
            };
            memcpy(cells, ca, sizeof(ca));
        } else if(type == Z_BLOCK) {
            size = 3;
            bool ca[16] = {
                1, 1, 0, 0,
                0, 1, 1, 0,
                0, 0, 0, 0,
                0, 0, 0, 0
            };
            memcpy(cells, ca, sizeof(ca));
        } else if(type == S_BLOCK) {
            size = 3;
            bool ca[16] = {
                0, 1, 1, 0,
                1, 1, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0
            };
            memcpy(cells, ca, sizeof(ca));
        }
    }
    
    void rotate() {
        //Transpose
        for (int r = 0; r < size; r++) {
            for (int c = r; c < size; c++) {
                int temp = cells[r * BLOCK_SIZE + c];
                cells[r * BLOCK_SIZE + c] = cells[c * BLOCK_SIZE + r];
                cells[c * BLOCK_SIZE + r] = temp;
            }
        }

        //Reverse elements on row order
        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size/2; c++) {
                int temp = cells[r * BLOCK_SIZE + c];
                cells[r * BLOCK_SIZE + c] = cells[r * BLOCK_SIZE + (size-c-1)];
                cells[r * BLOCK_SIZE + (size-c-1)] = temp;
            }
        }
    }

    void print() {
        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size; c++) {
                printf("%d ", cells[r * size + c]);
            }
            printf("\n");
        }
        printf("\n");
    }
};

class TetrisScreen : public Screen
{
private:
    uint8_t board[BOARD_HEIGHT*BOARD_WIDTH];
    struct Block currentBlock, nextBlock;
    
    GameState gameState;
    uint8_t gameSpeed = 1;
    uint16_t highScore = 0;
    uint16_t score = 0;

    timetype lastUpdate;
    Image *font2;

    int8_t currentBlockX, currentBlockY;
    Color blockColors[8] = {
        Color(0, 0, 0),
        Color(190, 0, 192),
        Color(34, 255, 255),
        Color(255, 255, 9),

        Color(0, 0, 183),
        Color(252, 97, 9),
        Color(251, 0, 6),
        Color(35, 255, 7),
    };
    
    void initializeBlockTypes();
    bool canMove(int8_t newPosX, int8_t newPosY, bool rotate);
    void createNewBlock();
    void removeRow(uint8_t row);
    void printBoard();

    void checkColFull();
    void checkGameOver();
public:
    TetrisScreen(void (*returnCallBack)(int8_t menu), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore);
    ~TetrisScreen();

    void update();
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);

};

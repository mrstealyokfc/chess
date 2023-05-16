#ifndef __CHESS_H
#define __CHESS_H

#include<stdint.h>


#define X_MASK     0x000000ff
#define Y_MASK     0x0000ff00
#define PIECE_TYPE 0x00ff0000
#define PIECE_TEAM 0xff000000

typedef uint32_t peice;


typedef struct{
	uint8_t width;
	uint8_t height;
} board;

void submit_move(uint32_t game_id, uint16_t move);

uint32_t register_game();

void unregister_game();

#endif

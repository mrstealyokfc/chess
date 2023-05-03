


typedef uint32_t peice;

typedef struct{
	uint8_t width;
	uint8_t height;
} board;

void submit_move(uint32_t game_id, uint16_t move);

uint32_t register_game();

void unregister_game();

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// フィールドの縦の大きさ
#define ROW 20
// フィールドの横の大きさ
#define COLUMN 60

// fieldの中身
#define NONE 0
#define PLAYER_TRUE 1
#define ROBOT_TRUE 2
#define SCRAP_TRUE 3

// ロボット・プレイヤーの状態
#define NONE 0
#define ALIVE 1
#define DEAD -1

// レベル1でのロボット生成数
#define ROBOTCOEF 5
// ロボットの最大数
#define ROBOTMAXNUM 40

// ゲームの状態
#define CLEAR 1
#define PLAYING 0
#define GAMEOVER -1

typedef struct pos {
  int x;
  int y;
  int state;
} POS;

extern char getChar(void);
void init_char(POS *player, POS robot[ROBOTMAXNUM]);
void init_field(int field[ROW][COLUMN]);
void place_char(int field[ROW][COLUMN], int robot_num);
void shuffle_field(int field[ROW][COLUMN]);
void make_char(POS *player, POS robot[ROBOTMAXNUM], int field[ROW][COLUMN]);
void disp_field(int field[ROW][COLUMN], int level, int score);
void disp_horizontal_border(void);

void move_player(POS *player, int field[ROW][COLUMN]);
int check_movable(int key, POS player, int field[ROW][COLUMN]);
void teleport(POS *player, int field[ROW][COLUMN]);
void move_robot(POS player, POS robot[ROBOTMAXNUM]);

void make_field(POS player, POS robot[ROBOTMAXNUM], int field[ROW][COLUMN]);
void check_player_state(POS player, POS robot[ROBOTMAXNUM], int *game_state);
void check_robot_state(POS robot[ROBOTMAXNUM], int robot_num, int *game_state, int *score);
void disp_gameover(int level, int score);

int main(void)
{
  POS player, robot[ROBOTMAXNUM];
  int field[ROW][COLUMN];
  int game_state = PLAYING;
  int level = 0;
  int score = 0;
  int robot_num = 0;

  srand((unsigned)time(NULL));

  while (game_state != GAMEOVER) {
    level++;
    init_char(&player, robot);
    init_field(field);

    if (robot_num < ROBOTMAXNUM) robot_num = level*ROBOTCOEF;
    place_char(field, robot_num);
    make_char(&player, robot, field);

    game_state = PLAYING;
    while (game_state == PLAYING) {
      disp_field(field, level, score);
      move_player(&player, field);
      move_robot(player, robot);
      check_player_state(player, robot, &game_state);
      check_robot_state(robot, robot_num, &game_state, &score);
      make_field(player, robot, field);
    }

    if (game_state == CLEAR) score += level * 10;
  }

  disp_gameover(level, score);

  return 0;
}

void init_char(POS *player, POS robot[ROBOTMAXNUM])
{
  int i;

  player->x = 0;
  player->y = 0;
  player->state = NONE;

  for (i = 0; i < ROBOTMAXNUM; i++) {
    robot[i].x = 0;
    robot[i].y = 0;
    robot[i].state = NONE;
  }
}

void init_field(int field[ROW][COLUMN])
{
  int x, y;

  for (y = 0; y < ROW; y++) {
    for (x = 0; x < COLUMN; x++) {
      field[y][x] = NONE;
    }
  }
}

void place_char(int field[ROW][COLUMN], int robot_num)
{
  int i;

  field[0][0] = PLAYER_TRUE;
  for (i = 0; i < robot_num; i++){
    field[0][i+1] = ROBOT_TRUE;
  }

  shuffle_field(field);
}

void shuffle_field(int field[ROW][COLUMN])
{
  int x, y, x_tmp, y_tmp, tmp;

  for (y = 0; y < ROW; y++) {
    for (x = 0; x < COLUMN; x++) {
      x_tmp = rand() % COLUMN;
      y_tmp = rand() % ROW;
      tmp = field[y][x];
      field[y][x] = field[y_tmp][x_tmp];
      field[y_tmp][x_tmp] = tmp;
    }
  }
}

void make_char(POS *player, POS robot[ROBOTMAXNUM], int field[ROW][COLUMN])
{
  int x, y;
  int i = 0;

  for (y = 0; y < ROW; y++) {
    for (x = 0; x < COLUMN; x++) {
      if (field[y][x] == PLAYER_TRUE) {
        player->x = x;
        player->y = y;
        player->state = ALIVE;
      }else if (field[y][x] == ROBOT_TRUE) {
        robot[i].x = x;
        robot[i].y = y;
        robot[i].state = ALIVE;
        i++;
      }
    }
  }
}

void disp_field(int field[ROW][COLUMN], int level, int score)
{
  int x, y;

  disp_horizontal_border();

  for (y = 0; y < ROW; y++) {
    printf("|");
    for (x = 0; x < COLUMN; x++) {
      switch (field[y][x]) {
        case NONE:
          printf(" ");
          break;
        case PLAYER_TRUE:
          printf("@");
          break;
        case ROBOT_TRUE:
          printf("+");
          break;
        case SCRAP_TRUE:
          printf("*");
          break;
      }
    }
    printf("|");
    puts("");
  }

  disp_horizontal_border();

  printf("(level:%d score:%d):?\n", level, score);
}

void disp_horizontal_border(void)
{
  int x, y;

  for (x = 0; x < COLUMN+2; x++) {
    if (x == 0 || x == COLUMN+1) {
      printf("+");
    }else {
      printf("-");
    }
  }
  puts("");
}

void move_player(POS *player, int field[ROW][COLUMN])
{
  int key;

  do {
    key = getChar() - '0';
  } while(check_movable(key, *player, field));

  switch (key) {
    case 1:
      player->y += 1;
      player->x -= 1;
      break;
    case 2:
      player->y += 1;
      break;
    case 3:
      player->y += 1;
      player->x += 1;
      break;
    case 4:
      player->x -= 1;
      break;
    case 6:
      player->x += 1;
      break;
    case 7:
      player->y -= 1;
      player->x -= 1;
      break;
    case 8:
      player->y -= 1;
      break;
    case 9:
      player->y -= 1;
      player->x += 1;
      break;
    case 0:
      teleport(player, field);
      break;
  }
}

int check_movable(int key, POS player, int field[ROW][COLUMN])
{
  switch (key) {
    case 1: return (player.y == ROW-1 || player.x == 0 || field[player.y+1][player.x-1] != NONE);
    case 2: return (player.y == ROW-1 || field[player.y+1][player.x] != NONE);
    case 3: return (player.y == ROW-1 || player.x == COLUMN-1 || field[player.y+1][player.x+1] != NONE);
    case 4: return (player.x == 0 || field[player.y][player.x-1] != NONE);
    case 5: return 0;
    case 6: return (player.x == COLUMN-1 || field[player.y][player.x+1] != NONE);
    case 7: return (player.y == 0 || player.x == 0 || field[player.y-1][player.x-1] != NONE);
    case 8: return (player.y == 0 || field[player.y-1][player.x] != NONE);
    case 9: return (player.y == 0 || player.x == COLUMN-1 || field[player.y-1][player.x+1] != NONE);
    case 0: return 0;
    default: return 1;
  }
}

void teleport(POS *player, int field[ROW][COLUMN])
{
  int x, y, x_tmp, y_tmp, tmp;

  for (y = 0; y < ROW; y++) {
    for (x = 0; x < COLUMN; x++) {
      x_tmp = rand() % COLUMN;
      y_tmp = rand() % ROW;
      if (field[y_tmp][x_tmp] == PLAYER_TRUE && field[y][x] == NONE) {
        tmp = field[y][x];
        field[y][x] = field[y_tmp][x_tmp];
        field[y_tmp][x_tmp] = tmp;
        player->x = x;
        player->y = y;
      }else if (field[y_tmp][x_tmp] == NONE && field[y][x] == PLAYER_TRUE) {
        tmp = field[y][x];
        field[y][x] = field[y_tmp][x_tmp];
        field[y_tmp][x_tmp] = tmp;
        player->x = x_tmp;
        player->y = y_tmp;
      }
    }
  }
}

void move_robot(POS player, POS robot[ROBOTMAXNUM])
{
  int i;

  for (i = 0; i < ROBOTMAXNUM; i++) {
    if (robot[i].state == ALIVE) {
      if (robot[i].x < player.x) {
        robot[i].x += 1;
      }else if (robot[i].x > player.x) {
        robot[i].x -= 1;
      }

      if (robot[i].y < player.y) {
        robot[i].y += 1;
      }else if (robot[i].y > player.y) {
        robot[i].y -= 1;
      }
    }
  }
}

void make_field(POS player, POS robot[ROBOTMAXNUM], int field[ROW][COLUMN])
{
  int i;

  init_field(field);

  field[player.y][player.x] = PLAYER_TRUE;

  for (i = 0; i < ROBOTMAXNUM; i++) {
    if (robot[i].state == ALIVE) {
      field[robot[i].y][robot[i].x] = ROBOT_TRUE;
    }else if (robot[i].state == DEAD) {
      field[robot[i].y][robot[i].x] = SCRAP_TRUE;
    }
  }
}

void check_player_state(POS player, POS robot[ROBOTMAXNUM], int *game_state)
{
  int i;

  for (i = 0; i < ROBOTMAXNUM; i++) {
    if (robot[i].state == ALIVE && robot[i].x == player.x && robot[i].y == player.y) {
      player.state = DEAD;
      *game_state = GAMEOVER;
    }
  }
}

void check_robot_state(POS robot[ROBOTMAXNUM], int robot_num, int *game_state, int *score)
{
  int i, j;
  static int dead_robot_num = 0;

  for (i = 0; i < ROBOTMAXNUM; i++) {
    for (j = i+1; j < ROBOTMAXNUM; j++) {
      if (robot[j].state != NONE && robot[i].state != NONE &&
         robot[j].x == robot[i].x && robot[j].y == robot[i].y) {
        if (robot[i].state == ALIVE) {
          dead_robot_num++;
          (*score)++;
        }
        if (robot[j].state == ALIVE) {
          dead_robot_num++;
          (*score)++;
        }
        robot[i].state = DEAD;
        robot[j].state = DEAD;
      }
    }
  }

  if (dead_robot_num >= robot_num) {
    *game_state = CLEAR;
    dead_robot_num = 0;
  }
}

void disp_gameover(int level, int score)
{
  printf("Game over... (level:%d score:%d)\n", level, score);
}

/**
 * @file Yamato.cpp
 * @author Dreamsbootleg
 * @brief Battleship AI
 * @date 12/8/2025
 */

#include <fstream>
#include <iomanip>
#include <random>

#include "Yamato.h"

#define BONUS_HIT_TWO 20
#define BONUS_HIT_ONE 10
#define HEAT_WEIGHT 3.0
#define SET_W 8

// Write your AI's name here. Please don't make it more than 64 bytes.
#define AI_NAME "Yamato"

// Write your name(s) here. Please don't make it more than 64 bytes.
#define AUTHOR_NAMES "Dreamsbootleg"

/*================================================================================
 * Starts up the entire match. Do NOT change anything here unless you really
 *understand what you are doing.
 *================================================================================*/

int main(int argc, char *argv[]) {
    // player must have the socket path as an argument.
    if (argc != 2) {
        printf("%s Error: Requires socket name! (line: %d)\n", AI_NAME,
               __LINE__);
        return -1;
    }
    char *socket_path = argv[1];

    // set random seed
    srand(getpid());

    PlayerExample my_player = PlayerExample();
    return my_player.play_match(socket_path, AI_NAME, AUTHOR_NAMES);
}

PlayerExample::PlayerExample() : Player() { return; }

PlayerExample::~PlayerExample() { return; }

/*================================================================================
 * This is like a constructor for the entire match.
 * You probably don't want to make changes here unless it is something that is
 *done once at the beginning of the entire match..
 *================================================================================*/

void PlayerExample::handle_setup_match(PlayerNum player, int board_size) {
    this->player = player;
    this->board_size = board_size;
    create_boards();
    score_data_ship[0][0] = -10;
    score_data_ship[1][0] = -10;
    score_data_ship[2][0] = -10;
    return;
}
/*================================================================================
 * This is like a constructor for one game/round within the entire match.
 * Add anything here that you need to do at the beginning of each game.
 *================================================================================*/

void PlayerExample::handle_start_game() {
    game_counter++;
    //sdt::cerr << "Game #: " << game_counter << "\n";
    clear_boards();
    return;
}

/*================================================================================
 * Example of how to decide where to place a ship of length ship_length and
 * inform the contest controller of your decision.
 *
 * If you place your ship even a bit off the board or collide with a previous
 *ship that you placed in this round, you instantly forfeit the round.
 *
 * TLDR: set ship.len, ship.row, and ship.col to good values and return ship.
 *================================================================================*/

Ship PlayerExample::choose_ship_place(int ship_length) {
    if (ship_length == 3) {
        //sdt::cerr << "choosing ship";
        Ship ship = ship_place_3();
        //sdt::cerr << ship.col << "," << ship.row << "," << ship.dir << ","
                  << ship.len;
        return ship;
    } else {
        Ship ship = ship_place_random(ship_length);
        return ship;
    }
}

bool PlayerExample::collision(Ship ship) {
    if (ship.dir == HORIZONTAL) {
        for (int i = 0; i < ship.len; i++) {
            if (ship_board[ship.row][ship.col + i] == SHIP)
                return true;
        }
        return false;
    } else {
        for (int i = 0; i < ship.len; i++) {
            if (ship_board[ship.row + i][ship.col] == SHIP)
                return true;
        }
        return false;
    }
}

void PlayerExample::mark_ship(Ship ship) {
    if (ship.dir == HORIZONTAL) {
        for (int i = 0; i < ship.len; i++) {
            this->ship_board[ship.row][ship.col + i] = SHIP;
        }
    } else {
        for (int i = 0; i < ship.len; i++) {
            this->ship_board[ship.row + i][ship.col] = SHIP;
        }
    }
    ship_length_list.insert(ship.len);
}

Ship PlayerExample::ship_place_random(int ship_length) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rdir(0, 1);
    Ship ship;
    ship.len = ship_length;
    while (true) {
        ship.dir = (rdir(gen) % 2 == 0) ? HORIZONTAL : VERTICAL;
        if (ship.dir == HORIZONTAL) {
            std::uniform_int_distribution<> rrow(0, board_size - 1);
            ship.row = rrow(gen);
            std::uniform_int_distribution<> rcol(0, board_size - ship.len);
            ship.col = rcol(gen);
        } else {
            std::uniform_int_distribution<> rrow(0, board_size - ship.len);
            ship.row = rrow(gen);
            std::uniform_int_distribution<> rcol(0, board_size - 1);
            ship.col = rcol(gen);
        }
        if (!collision(ship)) {
            mark_ship(ship);
            return ship;
        }
    }
}

Ship PlayerExample::ship_place_3() {
    int min = 100;
    Ship ship;
    ship.len = 3;
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size - 3; col++) {
            if ((score_data_ship[row][col] + score_data_ship[row][col + 1] +
                 score_data_ship[row][col + 2]) < min &&
                (ship_board[row][col] != SHIP &&
                 ship_board[row][col + 1] != SHIP &&
                 ship_board[row][col + 2] != SHIP)) {
                min = score_data_ship[row][col] +
                      score_data_ship[row][col + 1] +
                      score_data_ship[row][col + 2];
                ship.col = col;
                ship.row = row;
                ship.dir = HORIZONTAL;
            }
        }
    }
    for (int row = 0; row < board_size - 3; row++) {
        for (int col = 0; col < board_size; col++) {
            if ((score_data_ship[row][col] + score_data_ship[row + 1][col] +
                 score_data_ship[row + 2][col]) < min &&
                (ship_board[row][col] != SHIP &&
                 ship_board[row + 1][col] != SHIP &&
                 ship_board[row + 2][col] != SHIP)) {
                min = score_data_ship[row][col] +
                      score_data_ship[row + 1][col] +
                      score_data_ship[row + 2][col];
                ship.col = col;
                ship.row = row;
                ship.dir = VERTICAL;
            }
        }
    }
    if (ship.dir == HORIZONTAL) {
        score_data_ship[ship.row][ship.col] = 0;
        score_data_ship[ship.row][ship.col + 1] = 0;
        score_data_ship[ship.row][ship.col + 2] = 0;
    } else {
        score_data_ship[ship.row][ship.col] = 0;
        score_data_ship[ship.row + 1][ship.col] = 0;
        score_data_ship[ship.row + 2][ship.col] = 0;
    }
    mark_ship(ship);
    return ship;
}

void PlayerExample::evaluate_board_ship() {
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
        }
    }
}
/*================================================================================
 * Example of how to decide where to shoot and inform the contest controller
 * of your decision.
 *================================================================================*/

Shot PlayerExample::choose_shot() {
    shot_counter++;
    return probable_shot();
}

Shot PlayerExample::probable_shot() {
    Shot shot;
    int best_value = -1;
    int chosen_row = -1;
    int chosen_col = -1;
    evaluate_board_hit();
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            if (score_data_hit[row][col] > best_value) {
                best_value = score_data_hit[row][col];
                shot.row = row;
                shot.col = col;
                chosen_row = row;
                chosen_col = col;
            }
        }
    }

    print_evaluated_board(chosen_row, chosen_col);
    /*if (shot_counter == 1) {
        shot.row = 0;
        shot.col = 0;
        return shot;
    }
    if (shot_counter == 2) {
        shot.row = 0;
        shot.col = 1;
        return shot;
    }
    if (shot_counter == 3) {
        shot.row = 0;
        shot.col = 4;
        return shot;
    }
    if (shot_counter == 4) {
        shot.row = 0;
        shot.col = 5;
        return shot;
    }*/
    return shot;
}

void PlayerExample::evaluate_board_hit() {
    // loops through the board and evaluates based on row_value and
    // col_value
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            score_data_hit[row][col] =
                (row_value(row, col) + col_value(row, col)) +
                (HEAT_WEIGHT * normalized_heat_map_hit[row][col]);
        }
    }

    // if two HITS in a row horizontaly, gives bonus points
    for (int row = 0; row < board_size; row++) {
        for (int col = 1; col < board_size - 1; col++) {
            if (shot_board[row][col] == HIT &&
                shot_board[row][col - 1] == HIT &&
                shot_board[row][col + 1] == WATER) {
                score_data_hit[row][col + 1] += BONUS_HIT_TWO;
            }
            if (shot_board[row][col] == HIT &&
                shot_board[row][col + 1] == HIT &&
                shot_board[row][col - 1] == WATER) {
                score_data_hit[row][col - 1] += BONUS_HIT_TWO;
            }
        }
    }

    // if two HITS in a row vertically, gives bonus points
    for (int row = 1; row < board_size - 1; row++) {
        for (int col = 0; col < board_size; col++) {
            if (shot_board[row][col] == HIT &&
                shot_board[row - 1][col] == HIT &&
                shot_board[row + 1][col] == WATER) {
                score_data_hit[row + 1][col] += BONUS_HIT_TWO;
            }
            if (shot_board[row][col] == HIT &&
                shot_board[row + 1][col] == HIT &&
                shot_board[row - 1][col] == WATER) {
                score_data_hit[row - 1][col] += BONUS_HIT_TWO;
            }
        }
    }

    // if hit, gives bonus points to surrounding area
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            if (shot_board[row][col] == HIT) {
                if (col + 1 < board_size && shot_board[row][col + 1] == WATER)
                    score_data_hit[row][col + 1] += BONUS_HIT_ONE;
                if (col - 1 >= 0 && shot_board[row][col - 1] == WATER)
                    score_data_hit[row][col - 1] += BONUS_HIT_ONE;
                if (row + 1 < board_size && shot_board[row + 1][col] == WATER)
                    score_data_hit[row + 1][col] += BONUS_HIT_ONE;
                if (row - 1 >= 0 && shot_board[row - 1][col] == WATER)
                    score_data_hit[row - 1][col] += BONUS_HIT_ONE;
            }
        }
    }
}

int PlayerExample::row_value(int row, int col) {
    if (shot_board[row][col] != WATER)
        return 0;
    int width = 1;
    int bonus = 0;
    for (int r = row - 1; r >= 0 && row - r < *ship_length_list.rbegin(); r--) {
        if (shot_board[r][col] == MISS || shot_board[r][col] == KILL)
            break;
        width++;
        if (shot_board[r][col] == HIT)
            bonus += 5;
    }
    for (int r = row + 1;
         r < board_size && r - row < *ship_length_list.rbegin(); r++) {
        if (shot_board[r][col] == MISS || shot_board[r][col] == KILL)
            break;
        width++;
        if (shot_board[r][col] == HIT)
            bonus += 5;
    }
    if (width < *ship_length_list.rbegin())
        return 0;
    else
        return width + bonus;
}

int PlayerExample::col_value(int row, int col) {
    if (shot_board[row][col] != WATER)
        return 0;
    int width = 1;
    int bonus = 0;
    for (int c = col - 1; c >= 0 && col - c < *ship_length_list.rbegin(); c--) {
        if (shot_board[row][c] == MISS || shot_board[row][c] == KILL)
            break;
        width++;
        if (shot_board[row][c] == HIT)
            bonus += 5;
    }
    for (int c = col + 1;
         c < board_size && c - col < *ship_length_list.rbegin(); c++) {
        if (shot_board[row][c] == MISS || shot_board[row][c] == KILL)
            break;
        width++;
        if (shot_board[row][c] == HIT)
            bonus += 5;
    }
    if (width < *ship_length_list.rbegin())
        return 0;
    else
        return width + bonus;
}

void PlayerExample::use_heat_map_hit() {
    int max{0};
    int min{0};
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            if (heat_map_hit[row][col] > max)
                max = heat_map_hit[row][col];
            if (heat_map_hit[row][col] < min)
                min = heat_map_hit[row][col];
        }
    }
    if (max == min) {
        return;
    }
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            normalized_heat_map_hit[row][col] =
                (float)(heat_map_hit[row][col] - min) / (max - min);
        }
    }
}

void PlayerExample::use_heat_map_ship() {
    int max{0};
    int min{0};
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            if (heat_map_ship[row][col] > max)
                max = heat_map_ship[row][col];
            if (heat_map_ship[row][col] < min)
                min = heat_map_ship[row][col];
        }
    }
    if (max == min) {
        return;
    }
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            normalized_heat_map_ship[row][col] =
                (float)(heat_map_ship[row][col] - min) / (max - min);
        }
    }
}

Shot PlayerExample::random_shot() {
    Shot shot;
    std::random_device rd;
    std::mt19937 gen(rd());
    while (true) {
        std::uniform_int_distribution<> srow(0, board_size - 1);
        std::uniform_int_distribution<> scol(0, board_size - 1);
        shot.row = srow(gen);
        shot.col = scol(gen);
        if (!(shot_board[shot.row][shot.col] != WATER))
            return shot;
    }
}

void PlayerExample::print_evaluated_board(int chosen_row, int chosen_col) {
    //sdt::cerr << "Shot #: " << shot_counter << "\n";
    //sdt::cerr << "Shot Chosen: " << chosen_row << "," << chosen_col << "\n";
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            //sdt::cerr << std::setw(SET_W);
            //sdt::cerr << score_data_hit[row][col];
        }
        //sdt::cerr << "\n";
    }
    //sdt::cerr << "\n";
    //sdt::cerr << "ship_queue: ";
    auto ship_length_list_copy = ship_length_list;
    for (auto i : ship_length_list_copy) {
        //sdt::cerr << i;
    }
    //sdt::cerr << endl;
}

void PlayerExample::print_heat_map_hit() {
    //sdt::cerr << "Heat Map Hit: \n";
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            switch (shot_board[row][col]) {
                case HIT:
                    heat_map_hit[row][col]++;
                    break;
                case KILL:
                    heat_map_hit[row][col]++;
                    break;
                case MISS:
                    heat_map_hit[row][col]--;
                    break;
            }
            //sdt::cerr << std::setw(SET_W);
            //sdt::cerr << heat_map_hit[row][col];
        }
        //sdt::cerr << "\n";
    }
    //sdt::cerr << std::endl;
    use_heat_map_hit();
}

void PlayerExample::print_heat_map_ship() {
    //sdt::cerr << "Heat Map: Ship: \n";
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            switch (ship_board[row][col]) {
                case HIT:
                    heat_map_ship[row][col]++;
                    break;
                case KILL:
                    heat_map_ship[row][col]++;
                    break;
                case MISS:
                    heat_map_ship[row][col]--;
                    break;
            }
            //sdt::cerr << std::setw(SET_W);
            //sdt::cerr << heat_map_ship[row][col];
        }
        //sdt::cerr << "\n";
    }
    //sdt::cerr << std::endl;
}
/*================================================================================
 * This function is called to inform your AI of the result of a previous
 *shot, as well as where the opponent has shot.
 *================================================================================*/

void PlayerExample::handle_shot_return(PlayerNum player, Shot &shot) {
    // Results of your AI's shot was returned, store it
    if (player == this->player) {
        this->shot_board[shot.row][shot.col] = shot.value;
        //sdt::cerr << "Shot Result: " << shot.value << "\n" << endl;
    }

    // Your AI is informed of where the opponent AI shot, store it
    // NOTE: Opponent shots are stored in ship_board, not shot_board
    else {
        this->ship_board[shot.row][shot.col] = shot.value;
    }
    return;
}

/*================================================================================
 * This function is called to update your shot_board (results of your
 *shots at opponent) when an opponent ship has been killed, OR to update
 *your ship_board (where you keep track of your ships) to show that your
 *ship was killed.
 *================================================================================*/

void PlayerExample::handle_ship_dead(PlayerNum player, Ship &ship) {
    // store the ship that was killed
    for (int i = 0; i < ship.len; i++) {
        if (player == this->player) {  // your ship is dead
            if (ship.dir == HORIZONTAL)
                this->ship_board[ship.row][ship.col + i] = KILL;
            else if (ship.dir == VERTICAL)
                this->ship_board[ship.row + i][ship.col] = KILL;
        } else {  // their ship is dead
            if (ship.dir == HORIZONTAL)
                this->shot_board[ship.row][ship.col + i] = KILL;
            else if (ship.dir == VERTICAL)
                this->shot_board[ship.row + i][ship.col] = KILL;
        }
    }

    if (player != this->player) {
        auto it = ship_length_list.find(ship.len);
        if (it != ship_length_list.end())
            ship_length_list.erase(it);

        if (ship.dir == HORIZONTAL) {
            //sdt::cerr << "Ship Dead: Horizontal" << "\n";
            for (int i = 0; i < ship.len; i++) {
                //sdt::cerr << ship.row << "," << ship.col + i << "\n";
            }
            //sdt::cerr << "Ship Length: " << ship.len << "\n";
        } else {
            //sdt::cerr << "Ship Dead: Vertical" << "\n";
            for (int i = 0; i < ship.len; i++) {
                //sdt::cerr << ship.row + i << "," << ship.col << "\n";
            }
            //sdt::cerr << "Ship Length: " << ship.len << "\n";
        }
    }
    return;
}

/*================================================================================
 * This function runs at the end of a particular game/round.
 * Do anything here that needs to be done at the end of a game/round in
 *the match.
 *================================================================================*/

void PlayerExample::handle_game_over()

{
    shot_counter = 0;
    print_heat_map_hit();
    print_heat_map_ship();
    ship_length_list.clear();
    return;
}

/*================================================================================
 * This function is called by the AI's destructor and runs at the end of
 *the entire match.
 *================================================================================*/

void PlayerExample::handle_match_over() {
    delete_boards();
    return;
}

/*================================================================================
 * This function sets up all boards at the beginning of the whole match.
 * Add setup here for any boards you create.
 *================================================================================*/

void PlayerExample::create_boards() {
    int size = this->board_size;
    // dynamically create an array of pointers.
    this->ship_board = new char *[size];
    this->shot_board = new char *[size];
    // this->int_board = new int*[size];

    // dynamically allocate memory of size board_size for each row.
    for (int i = 0; i < size; i++) {
        this->ship_board[i] = new char[size];
        this->shot_board[i] = new char[size];
    }
    return;
}

/*================================================================================
 * This function resets boards between rounds.
 *================================================================================*/

void PlayerExample::clear_boards() {
    // assign WATER to the boards
    for (int i = 0; i < this->board_size; i++) {
        for (int j = 0; j < this->board_size; j++) {
            this->ship_board[i][j] = WATER;
            this->shot_board[i][j] = WATER;
        }
    }
    return;
}

/*================================================================================
 * This function is called by the AI's destructor and runs at the end of
 *the entire match.
 *================================================================================*/

void PlayerExample::delete_boards() {
    // deallocates memory using the delete operator

    for (int i = 0; i < this->board_size; i++) {
        delete[] this->ship_board[i];
        delete[] this->shot_board[i];
    }
    delete[] this->ship_board;
    delete[] this->shot_board;
    return;
}
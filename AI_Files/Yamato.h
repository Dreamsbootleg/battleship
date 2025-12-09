/**
 * @file Yamato.cpp
 * @author Dreamsbootleg
 * @brief Battleship AI
 * @date 12/8/2025
 */

#ifndef Yamato_H
#define Yamato_H

#include <queue>
#include <set>

#include "protected/Player.h"

// PlayerExample inherits from/extends Player

// Rename this Class with your own class!
// Make sure to change all references to
// the `PlayerExample` class in both
// this file, and your c++ file

// #define UNKNOWN -1 // Never shot here before
// #define WATER 0    // Shot here and missed
// #define HIT 1      // Shot here and hit a ship
// #define KILL 2     // Shot here and sunk a ship
class PlayerExample : public Player {
   public:
    PlayerExample();
    ~PlayerExample();
    void handle_setup_match(PlayerNum player, int board_size);
    void handle_start_game();
    std::multiset<int> ship_length_list;

    Ship choose_ship_place(int ship_length);
    Ship ship_place_3();
    Ship ship_place_4();
    Ship ship_place_5();
    Ship ship_place_random(int ship_length);

    bool collision(Ship ship);
    void mark_ship(Ship ship);

    Shot choose_shot();
    Shot random_shot();
    Shot probable_shot();
    int row_value(int row, int col);
    int col_value(int row, int col);
    Shot search_shot();

    void evaluate_board_hit();
    void evaluate_board_ship();
    void use_heat_map_hit();
    void use_heat_map_ship();
    void print_evaluated_board(int chosen_row, int chosen_col);
    void print_heat_map_hit();
    void print_heat_map_ship();

    float score_data_ship[10][10];
    float heat_map_ship[10][10]{0};
    float normalized_heat_map_ship[10][10]{0};

    float score_data_hit[10][10];
    float heat_map_hit[10][10]{0};
    float normalized_heat_map_hit[10][10]{0};

    int game_counter{0};
    int shot_counter{0};

    void handle_shot_return(PlayerNum player, Shot &shot);
    void handle_ship_dead(PlayerNum player, Ship &ship);

    void handle_game_over();
    void handle_match_over();

   private:
    PlayerNum player;
    int board_size;
    char **ship_board;
    char **shot_board;
    void create_boards();
    void clear_boards();
    void delete_boards();
};

#endif

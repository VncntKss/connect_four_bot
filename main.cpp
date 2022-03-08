#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <math.h>
#include <limits>
#include <tuple>
#include <chrono>
#include <unordered_map>

#define EMPTY_PIECE 0
#define PLAYER_PIECE 1
#define OPPONENT_PIECE 2
#define INF std::numeric_limits<int>::max();

typedef std::vector<std::vector<short>> Board;

std::string await_bot_response(const std::string &path) {
  std::fstream file_handle;
  std::string file_content;
  do {
    file_handle.open(path);
    file_handle >> file_content;
    file_handle.close();
  } while (file_content.empty() or file_content[0] == '>');

  return file_content.substr(1, file_content.size());
}

void trunc_file(const std::string& path) {
    std::ofstream file_handle(path, std::ios::trunc);
    file_handle.close();
}

void write_response(const std::string& path, unsigned short value) noexcept {
    std::ofstream file_handle(path, std::ios::trunc);
    file_handle << ">" + std::to_string(value);
    file_handle.close();
}

int get_next_pos(Board &board, int pos) { 
    int i = 0;
    while(board[pos][i] != EMPTY_PIECE) i++;
    return i;
}

void place_piece(Board &board, int pos, int piece) {
  int height = get_next_pos(board, pos);
  board[pos][height] = piece;
}

void print_board(Board board) {
  for (int j = 0; j <= 3; j++) {
    for (int i = 0; i <= 15; i++) {
      std::cout << board[i][j] << " ";
      if (i % 4 == 3)
        std::cout << "\n";
    }
    std::cout << "-------"
              << "\n";
  }


  std::cout << "\n";
}

std::vector<int> get_possible_locations(Board &board) {
  std::vector<int> possible_locations;
  for (int i = 0; i < 16; i++) {
    if(get_next_pos(board, i) < 4){
      possible_locations.push_back(i);
    }
  };
  return possible_locations;
}

std::string get_string_from_board(Board &board) {
  std::string result;
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 4; j++) {
      result += board[i][j];
    }
  }
  return result;
}

bool is_winning_move(Board board, int pos, int piece) {
  int height = get_next_pos(board, pos);

  place_piece(board, pos, piece);
  
  int start1 = pos%4;
  int start3 = floor(pos/4);
  int start2 = start3 * 4;
  //horizontal top to bottom
  if((piece == board[start1][height]) && (piece == board[start1 + 4][height]) && (piece == board[start1 + 8][height]) && (piece == board[start1 + 12][height])) return true;
  //horizontal left to right
  if((piece == board[start2][height]) && (piece == board[start2 + 1][height]) && (piece == board[start2 + 2][height]) && (piece == board[start2 + 3][height])) return true;
  //vertical 
  if((piece == board[pos][0]) && (piece == board[pos][1]) && (piece == board[pos][2]) && (piece == board[pos][3])) return true;
  //diagonals
  if(start1 == start3){
    //horizontal diagonal
    if((piece == board[0][height]) && (piece == board[5][height]) && (piece == board[10][height]) && (piece == board[15][height])) return true;
    //space diagonal from 0 0 to 15 3
    if((piece == board[0][0]) && (piece == board[5][1]) && (piece == board[10][2]) && (piece == board[15][3])) return true;
    //from 0 3 to 15 0
    if((piece == board[0][3]) && (piece == board[5][2]) && (piece == board[10][1]) && (piece == board[15][0])) return true;
  }else if(start1 == 3-start3){
    //other horizontal diagonal
    if((piece == board[3][height]) && (piece == board[6][height]) && (piece == board[9][height]) && (piece == board[12][height])) return true;
    //space diagonal from 3 3 to 12 0
    if((piece == board[3][3]) && (piece == board[6][2]) && (piece == board[9][1]) && (piece == board[12][0])) return true;
    //space diagonal from 3 0 to 12 3
    if((piece == board[3][0]) && (piece == board[6][1]) && (piece == board[9][2]) && (piece == board[12][3])) return true;
  }

  if(height == start1){
    if((piece == board[start3][0]) && (piece == board[start3 + 1][1]) && (piece == board[start3 + 2][2]) && (piece == board[start3 + 3][3])) return true;     
  }else if(height == 3 - start1){
    if((piece == board[start3][3]) && (piece == board[start3 + 1][2]) && (piece == board[start3 + 2][1]) && (piece == board[start3 + 3][0])) return true; 
  }
  
  if(height == start3){
    if((piece == board[start1][0]) && (piece == board[start1 + 4][1]) && (piece == board[start1 + 8][2]) && (piece == board[start1 + 12][3])) return true;     
  }else if(height == 3 - start1){
    if((piece == board[start1][3]) && (piece == board[start1 + 4][2]) && (piece == board[start1 + 8][1]) && (piece == board[start1 + 12][0])) return true; 
  }

  return false;
}

int evaluate_window(Board& board, int piece){
    auto check_vertical = [piece, board](int pos) {
        int result = 1;
        for (auto i = 0; i < 4; i++){
            if(board[pos][i] == piece){
                result *= 2;
            }else if(board[pos][i] != EMPTY_PIECE){
              return 0;
            }
        }
        return result;
    };

    auto check_horizontal1 = [piece, board](int x, int z) {
        int result = 1;
        for (auto i = 0; i < 4; i++){
            if(board[i * 4 + x][z] == piece){
                result *= 2;
            }else if(board[i * 4 + x][z] != EMPTY_PIECE){
              return 0;
            }
        }
        return result;
    };

    auto check_horizontal2 = [piece, board](int y, int z) {
        int result = 1;
        for (auto i = 0; i < 4; i++){
            if(board[i + 4 * y][z] == piece){
                result *= 2;
            }else if(board[i + 4 * y][z] != EMPTY_PIECE){
              return 0;
            }
        }
        return result;
    };

    auto check_diagonal1 = [piece, board](int pos) {
        int result = 1;
        for (auto i = 0; i < 4; i++){
            if(board[i * 4 + pos][i] == piece){
                result *= 2;
            }else if(board[i * 4 + pos][i] != EMPTY_PIECE){
              return 0;
            }
        }
        return result;
    };

    auto check_diagonal2 = [piece, board](int pos) {
        int result = 1;
        for (auto i = 0; i < 4; i++){
            if(board[i * 4 + pos][3 - i] == piece){
                result *= 2;
            }else if(board[i * 4 + pos][3 - i] != EMPTY_PIECE){
              return 0;
            }
        }
        return result;
    };

    auto check_diagonal3 = [piece, board](int y) {
        int result = 1;
        for (auto i = 0; i < 4; i++){
            if(board[i + y][i] == piece){
                result *= 2;
            }else if(board[i + y][i] != EMPTY_PIECE){
              return 0;
            }
        }
        return result;
    };

    auto check_diagonal4 = [piece, board](int y) {
        int result = 1;
        for (auto i = 0; i < 4; i++){
            if(board[i + y][3 - i] == piece){
                result *= 2;
            }else if(board[i + y][3 - i] != EMPTY_PIECE){
              return 0;
            }
        }
        return result;
    };

    auto check_space_diagonal = [piece, board]() {
        int res1 = 1, res2 = 1, res3 = 1, res4 = 1;
        for (int i = 0; i < 4; i++)
        {
          if(board[i * 5][i] == piece){
            res1 *= 2;
          }else if(board[i * 5][i] != EMPTY_PIECE){
            res1 = 0;
          }
          if(board[i * 5][3 - i] == piece){
            res2 *= 2;
          }else if(board[i * 5][3 - i] != EMPTY_PIECE){
            res2 = 0;
          }
          if(board[i * 3 + 3][i] == piece){
            res3 *= 2;
          }else if(board[i * 3 + 3][i] != EMPTY_PIECE){
            res3 = 0;
          }
          if(board[i * 3 + 3][3 - i] == piece){
            res4 *= 2;
          }else if(board[i * 3 + 3][3 - i] != EMPTY_PIECE){
            res4 = 0;
          }
        }  
        return res1 + res2 + res3 + res4;      
    };
    
    int result = 0;

    for (auto i = 0; i < 16; i++){
      result += check_vertical(i);
      result += check_horizontal1(i / 4, i % 4);
      result += check_horizontal2(i / 4, i % 4);
    }

    for (size_t i = 0; i < 4; i++)
    {
      result += check_diagonal1(i);
      result += check_diagonal2(i);
      result += check_diagonal3(i);
      result += check_diagonal4(i);
    }
    

    result += check_space_diagonal();

    return result;
}

std::tuple<int, int> alpha_beta(Board board, int depth, int alpha, int beta, bool is_bot) {
    auto possible_locations = get_possible_locations(board);
    if(depth > 5) std::cout << depth;
    if(depth == 0){
      return std::tuple<int, int>(0, evaluate_window(board, PLAYER_PIECE));
    }
    if(is_bot){
        int value = -INF;
        int position = 0;
        for(auto pos : possible_locations){
            if(is_winning_move(board, pos, PLAYER_PIECE)){
                return std::tuple<int, int>(pos, 1000000);
            }
            Board copy = board;
            place_piece(copy, pos, PLAYER_PIECE);
            int score = std::get<1>(alpha_beta(copy, depth-1, alpha, beta, false));
            if(value < score){
                value = score;
                position = pos;
            };
            alpha = std::max(alpha, value);
            if(alpha >= beta) break;
        }
        if(std::abs(value))std::cout << value << std::endl;
        return std::tuple<int,int>(position, value);    
    }else{
        int value = INF;
        int position = 0;
        for(auto pos : possible_locations){
            if(is_winning_move(board, pos, OPPONENT_PIECE)){
                return std::tuple<int, int>(pos, -1000000);
            }
            Board copy = board;
            place_piece(copy, pos, OPPONENT_PIECE);
            int score = std::get<1>(alpha_beta(copy, depth-1, alpha, beta, true));
            if(value > score){
                value = score;
                position = pos;
            };
            beta = std::min(beta, value);
            if(alpha >= beta) break;        
        }
        return std::tuple<int,int>(position, value);  
    }

}

int main(int argc, char **argv) {

  if (argc < 3) {
    std::cerr << "Not enough arguments supplied!"
              << "\n";
    exit(1);
  }

  std::cout << std::boolalpha;

  std::string player = argv[1];
  std::string path = argv[2];
  const int depth = 5;

  Board board(16, std::vector<short>(4));

  std::fstream game_file;
  game_file.open(path);

  std::unordered_map<std::string, int> hashmap;

  while (true) {
    std::string response = await_bot_response(path);
    trunc_file(path);


    if (response == "end") {
        std::cout << "Game end!" << std::endl;
        exit(0);
    }else if (response != "start") place_piece(board, std::stoi(response), OPPONENT_PIECE);
    
    int inf = INF;

    std::cout << "Starting turn..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    int pos, score;
    auto result = alpha_beta(board, 6, -inf, inf, true);
    auto end = std::chrono::high_resolution_clock::now();

    pos = std::get<0>(result);
    score = std::get<1>(result);



    std::cout << "Duration: " << std::chrono::duration_cast<std::chrono::seconds>(end-start).count() << std::endl;

    place_piece(board, pos, PLAYER_PIECE);
    write_response(path, pos);
  }
  return 0;
}
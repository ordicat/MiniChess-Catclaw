#include <iostream>
#include <sstream>
#include <cstdint>

#include "./state.hpp"
#include "../config.hpp"

static const int move_table_rook_bishop[8][7][2] = {
  {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}},
  {{0, -1}, {0, -2}, {0, -3}, {0, -4}, {0, -5}, {0, -6}, {0, -7}},
  {{1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}},
  {{-1, 0}, {-2, 0}, {-3, 0}, {-4, 0}, {-5, 0}, {-6, 0}, {-7, 0}},
  {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}},
  {{1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}, {7, -7}},
  {{-1, 1}, {-2, 2}, {-3, 3}, {-4, 4}, {-5, 5}, {-6, 6}, {-7, 7}},
  {{-1, -1}, {-2, -2}, {-3, -3}, {-4, -4}, {-5, -5}, {-6, -6}, {-7, -7}},
};
static const int move_table_knight[8][2] = {
  {1, 2}, {1, -2},
  {-1, 2}, {-1, -2},
  {2, 1}, {2, -1},
  {-2, 1}, {-2, -1},
};
static const int move_table_king[8][2] = {
  {1, 0}, {0, 1}, {-1, 0}, {0, -1}, 
  {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
};

const Move nomove = {{0,0},{0,0}};
/**
 * @brief evaluate the state
 * 
 * @return int 
 */
int State::evaluate(){
  // [TODO] design your own evaluation function

  int evaluation = 0;
  int opposing[] = {1, 0};
  int piecevalue[7] = {2, 20, 100, 60, 70, 180, 1000};
  int centerpos[2][2] = {{2, 2},{3,2}};

  for(int side = 0; side < 2; side++){
    bool occupied[BOARD_H][BOARD_W];
    //check the board.
    for(int i = 0; i < BOARD_H; i++){
      for(int j = 0; j < BOARD_W; j++){
        char curpiece = this->board.board[side][i][j];
        if(curpiece == (char)0) continue; //go to next check if space is empty
        int basevalue = piecevalue[(int)curpiece], iterstart = 0, iterend = 0, pieceeval = 0;
        switch(curpiece){//show range used in iteration table from move table prev defined
          case (char)2:
            iterend = 4;
            break;
          case (char)4:
            iterstart = 4;
            iterend = 8;
            break;
          case (char)3:
          case (char)5:
          case (char)6:
            iterend = 8;
            break;
        }
        //add base value
        pieceeval += basevalue;
        //position check: pieces near the center of the board have extra value.
        for(int ch = 0 ; ch < 2; ch++) if(i==centerpos[ch][0] && j==centerpos[ch][1]) {
          pieceeval += 3;
          if(curpiece == (char)1) pieceeval++; //slightly prefer pawns in the center.
        }
        //pawn specific checks.
        if(curpiece == (char)1){
          int checktable[2][2] = {{-1, -1},{-1, 1}};
          if(side==0)
            pieceeval += (BOARD_H-1)-i; //+1 eval per rank gained
          else
            pieceeval += i-1;
          //pawn exclusive space checks.
          for(int ch = 0; ch < 2; ch++){
            int checkx = j+checktable[ch][1], checky;
            if(side==1)
              checky = i-checktable[ch][0];
            else
              checky = i+checktable[ch][0];
            if(checkx < 0 || checkx >= BOARD_W || checky < 0 || checky >= BOARD_H) continue; //go to next cycle if check is OOB  
            //empty space + unique space check
            if(this->board.board[side][checky][checkx] == (char)0 && this->board.board[opposing[side]][checky][checkx] == (char)0 && occupied[checky][checkx]){
              occupied[checky][checkx] = true;
              pieceeval += 1;
            }
            //opponent piece check
            if(this->board.board[opposing[side]][checky][checkx] != (char)0){
              //king piece check
              if(this->board.board[opposing[side]][checky][checkx] == (char)6){
                if(player == side) pieceeval += piecevalue[6]<<3;
                else pieceeval += piecevalue[6];
              }
              else pieceeval += piecevalue[(int)this->board.board[opposing[side]][checky][checkx]]>>2;
            }
            //friendly piece check
            if(this->board.board[side][checky][checkx] != (char)0 || this->board.board[side][checky][checkx] != (char)6)
              pieceeval += piecevalue[(int)this->board.board[side][checky][checkx]]>>3;
          }
        }
        //knight specific checks.
        else if(curpiece == (char)3){
          for(int ch = iterstart; ch < iterend; ch++){
            int checkx = j+move_table_knight[ch][1], checky = i+move_table_knight[ch][0];
            if(checkx < 0 || checkx >= BOARD_W || checky < 0 || checky >= BOARD_H) continue; //go to next cycle if check is OOB
            //empty space + unique space check
            if(this->board.board[side][checky][checkx] == (char)0 && this->board.board[opposing[side]][checky][checkx] == (char)0 && occupied[checky][checkx]){
              occupied[checky][checkx] = true;
              pieceeval += 1;
            }
            //opponent piece check
            if(this->board.board[opposing[side]][checky][checkx] != (char)0){
              //king piece check
              if(this->board.board[opposing[side]][checky][checkx] == (char)6){
                if(player == side) pieceeval += piecevalue[6]<<3;
                else pieceeval += piecevalue[6];
              }
              else pieceeval += piecevalue[(int)this->board.board[opposing[side]][checky][checkx]]>>2;
            }
            //friendly piece check
            if(this->board.board[side][checky][checkx] != (char)0 || this->board.board[side][checky][checkx] != (char)6)
              pieceeval += piecevalue[(int)this->board.board[side][checky][checkx]]>>3;  
          }
        }
        else if(curpiece == (char)6){ //king piece specific checks
          for(int ch = iterstart; ch < iterend; ch++){
            int checky = i+move_table_king[ch][1], checkx = j+move_table_king[ch][0];
            char friendpiece = this->board.board[side][checky][checkx], enemypiece = this->board.board[opposing[side]][checky][checkx];
            if(friendpiece){
              if(friendpiece == (char)1) pieceeval += 2;
              else pieceeval += 1; //prefer pawns protecting the king over other pieces
            }
            if(enemypiece){
              pieceeval -= piecevalue[6]>>1; //don't have king near other pieces!
            }
          }
        }
        //other piece checks.
        else{
          for(int ch = iterstart; ch < iterend; ch++){
            int cycle = 7;
            if(curpiece == (char)6) cycle = 1; //only check over one distance if king.
            for(int dis = 0; dis < cycle; dis++){
              char pincheck = (char)0;
              int checkx = j+move_table_rook_bishop[ch][dis][1], checky = i+move_table_rook_bishop[ch][dis][0];
              //oob check
              if(checkx < 0 || checkx >= BOARD_W || checky < 0 || checky >= BOARD_H) break;
              //empty space + unique space check
              if(this->board.board[side][checky][checkx] == (char)0 && this->board.board[opposing[side]][checky][checkx] == (char)0 && occupied[checky][checkx]){
                occupied[checky][checkx] = true;
                if(curpiece == (char)6) pieceeval -= 1;
                else pieceeval += 1;
              }
              char ownside = this->board.board[side][checky][checkx], opposingside = this->board.board[opposing[side]][checky][checkx];
              //own piece collision check
              if(ownside != (char)0) {
                if(ownside != (char)6) pieceeval += 2;
                if(curpiece == (char)6) pieceeval += 3; //flat 3 eval increase for every friendly piece surrounding king
                break;
              }
              if(opposingside != (char)0){
                //king piece check
                if(opposingside == (char)6){
                  if(player == side) pieceeval += piecevalue[6]<<3;
                  else pieceeval += piecevalue[6];
                  break;
                }
                else{
                  if(piecevalue[(int)opposingside] <= piecevalue[(int)curpiece])pieceeval += piecevalue[(int)opposingside]>>1;
                  else pieceeval += piecevalue[(int)opposingside]>>3;
                  break;
                }
              }
            }
          }
        }
        //end: if on black then flip sign
        if(side == 1) pieceeval = -pieceeval;
        evaluation += pieceeval;
      }
    }
    int moves = this->legal_actions.size();
    if(player != side){
      State* tempstate = this->next_state(nomove);
      tempstate ->get_legal_actions();
      moves = tempstate->legal_actions.size();
    }
    evaluation += moves*3;
  }
  // flip signs if the state is for the second player.
  if(player == 1) evaluation = -evaluation;

  //return calculated evaluation parameter
  return evaluation;
}


/**
 * @brief return next state after the move
 * 
 * @param move 
 * @return State* 
 */
State* State::next_state(Move move){
  Board next = this->board;
  Point from = move.first, to = move.second;
  
  int8_t moved = next.board[this->player][from.first][from.second];
  //promotion for pawn
  if(moved == 1 && (to.first==BOARD_H-1 || to.first==0)){
    moved = 5;
  }
  if(next.board[1-this->player][to.first][to.second]){
    next.board[1-this->player][to.first][to.second] = 0;
  }
  
  next.board[this->player][from.first][from.second] = 0;
  next.board[this->player][to.first][to.second] = moved;
  
  State* next_state = new State(next, 1-this->player);
  
  if(this->game_state != WIN)
    next_state->get_legal_actions();
  return next_state;
}

/**
 * @brief get all legal actions of now state
 * 
 */
void State::get_legal_actions(){
  // [Optional]
  // This method is not very efficient
  // You can redesign it
  this->game_state = NONE;
  std::vector<Move> all_actions;
  auto self_board = this->board.board[this->player];
  auto oppn_board = this->board.board[1 - this->player];
  
  int now_piece, oppn_piece;
  for(int i=0; i<BOARD_H; i+=1){
    for(int j=0; j<BOARD_W; j+=1){
      if((now_piece=self_board[i][j])){
        // std::cout << this->player << "," << now_piece << ' ';
        switch (now_piece){
          case 1: //pawn
            if(this->player && i<BOARD_H-1){
              //black
              if(!oppn_board[i+1][j] && !self_board[i+1][j])
                all_actions.push_back(Move(Point(i, j), Point(i+1, j)));
              if(j<BOARD_W-1 && (oppn_piece=oppn_board[i+1][j+1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i+1, j+1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
              if(j>0 && (oppn_piece=oppn_board[i+1][j-1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i+1, j-1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
            }else if(!this->player && i>0){
              //white
              if(!oppn_board[i-1][j] && !self_board[i-1][j])
                all_actions.push_back(Move(Point(i, j), Point(i-1, j)));
              if(j<BOARD_W-1 && (oppn_piece=oppn_board[i-1][j+1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i-1, j+1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
              if(j>0 && (oppn_piece=oppn_board[i-1][j-1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i-1, j-1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
            }
            break;
          
          case 2: //rook
          case 4: //bishop
          case 5: //queen
            int st, end;
            switch (now_piece){
              case 2: st=0; end=4; break; //rook
              case 4: st=4; end=8; break; //bishop
              case 5: st=0; end=8; break; //queen
              default: st=0; end=-1;
            }
            for(int part=st; part<end; part+=1){
              auto move_list = move_table_rook_bishop[part];
              for(int k=0; k<std::max(BOARD_H, BOARD_W); k+=1){
                int p[2] = {move_list[k][0] + i, move_list[k][1] + j};
                
                if(p[0]>=BOARD_H || p[0]<0 || p[1]>=BOARD_W || p[1]<0) break;
                now_piece = self_board[p[0]][p[1]];
                if(now_piece) break;
                
                all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));
                
                oppn_piece = oppn_board[p[0]][p[1]];
                if(oppn_piece){
                  if(oppn_piece==6){
                    this->game_state = WIN;
                    this->legal_actions = all_actions;
                    return;
                  }else
                    break;
                };
              }
            }
            break;
          
          case 3: //knight
            for(auto move: move_table_knight){
              int x = move[0] + i;
              int y = move[1] + j;
              
              if(x>=BOARD_H || x<0 || y>=BOARD_W || y<0) continue;
              now_piece = self_board[x][y];
              if(now_piece) continue;
              all_actions.push_back(Move(Point(i, j), Point(x, y)));
              
              oppn_piece = oppn_board[x][y];
              if(oppn_piece==6){
                this->game_state = WIN;
                this->legal_actions = all_actions;
                return;
              }
            }
            break;
          
          case 6: //king
            for(auto move: move_table_king){
              int p[2] = {move[0] + i, move[1] + j};
              
              if(p[0]>=BOARD_H || p[0]<0 || p[1]>=BOARD_W || p[1]<0) continue;
              now_piece = self_board[p[0]][p[1]];
              if(now_piece) continue;
              
              all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));
              
              oppn_piece = oppn_board[p[0]][p[1]];
              if(oppn_piece==6){
                this->game_state = WIN;
                this->legal_actions = all_actions;
                return;
              }
            }
            break;
        }
      }
    }
  }
  std::cout << "\n";
  this->legal_actions = all_actions;
}


const char piece_table[2][7][5] = {
  {" ", "♙", "♖", "♘", "♗", "♕", "♔"},
  {" ", "♟", "♜", "♞", "♝", "♛", "♚"}
};
/**
 * @brief encode the output for command line output
 * 
 * @return std::string 
 */
std::string State::encode_output(){
  std::stringstream ss;
  int now_piece;
  for(int i=0; i<BOARD_H; i+=1){
    for(int j=0; j<BOARD_W; j+=1){
      if((now_piece = this->board.board[0][i][j])){
        ss << std::string(piece_table[0][now_piece]);
      }else if((now_piece = this->board.board[1][i][j])){
        ss << std::string(piece_table[1][now_piece]);
      }else{
        ss << " ";
      }
      ss << " ";
    }
    ss << "\n";
  }
  return ss.str();
}


/**
 * @brief encode the state to the format for player
 * 
 * @return std::string 
 */
std::string State::encode_state(){
  std::stringstream ss;
  ss << this->player;
  ss << "\n";
  for(int pl=0; pl<2; pl+=1){
    for(int i=0; i<BOARD_H; i+=1){
      for(int j=0; j<BOARD_W; j+=1){
        ss << int(this->board.board[pl][i][j]);
        ss << " ";
      }
      ss << "\n";
    }
    ss << "\n";
  }
  return ss.str();
}
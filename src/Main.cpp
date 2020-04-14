#include <Siv3D.hpp>

#define int_main() void Main()

constexpr const int left_board_pos = 70;
constexpr const int top_board_pos = 50;
constexpr const int cell_width = 60;
constexpr const int board_width = cell_width * 8;
constexpr const int stone_size = 25;

constexpr const int BLACK_STONE = 1;
constexpr const int WHITE_STONE = -1;

class OthelloGame{
private:
    const int SEARCH_LV = 5;
    const int FINAL_STAGE_NUM = 47;
    const int64 VALUE_PLACE[64] = {
        150, -10, 10,  5,  5, 10, -10, 150,
        -10,-200,  2,  1,  1,  2,-200, -10,
         10,   2,  0,  1,  1,  0,   2,  10,
          5,   1,  1,  0,  0,  1,   1,   5,
          5,   1,  1,  0,  0,  1,   1,   5,
         10,   2,  0,  1,  1,  0,   2,  10,
        -10,-200,  2,  1,  1,  2,-200, -10,
        150, -10, 10,  5,  5, 10, -10, 150
    };
    const int64 INF = 1e18;
    
    int64 black_board, white_board;
    bool black_turn;
    bool isAIcolorWhite = true;
    
public:
    OthelloGame() : black_board(0x0000001008000000),
                    white_board(0x0000000810000000),
                    black_turn(true) {}
    
    Array<Array<int>> getBorad(){
        Array<Array<int>> a(8, Array<int>(8));
        
        for(int i = 0; i < 64; i++){
            if(black_board & (1LL << i)){
                a[7 - (i / 8)][7 - (i % 8)] = BLACK_STONE;
            }
            if(white_board & (1LL << i)){
                a[7 - (i / 8)][7 - (i % 8)] = WHITE_STONE;
            }
        }
        
        return a;
    }
    
    // posで指定した場所に石を置き、revで指定した場所の石をひっくり返す
    void revStone(const int64 pos, const int64 rev){
        if(black_turn){
            black_board ^= pos | rev;
            white_board ^= rev;
        }
        else{
            white_board ^= pos | rev;
            black_board ^= rev;
        }

        black_turn = !black_turn;
    }
    
    // 一手戻す
    void undo(const int64 pos, const int64 rev){
        black_turn = !black_turn;
        
        if(black_turn){
            black_board ^= pos | rev;
            white_board ^= rev;
        }
        else{
            white_board ^= pos | rev;
            black_board ^= rev;
        }
    }
    
    // posの場所に石を置いたとき、石が返る位置のbitを立てて返す
    int64 canPut(const int64 pos){
        if((black_board | white_board) & pos) return 0; //着手箇所が空白でない場合


        int64 rev = 0, r, mask;

        if(black_turn){     // 黒が打つ場合
            r = 0;
            mask = (pos >> 1) & 0x7f7f7f7f7f7f7f7f; //右方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){ //白石が連続する間
                r |= mask;
                mask = (mask >> 1) & 0x7f7f7f7f7f7f7f7f;
            }
            if(mask & black_board) rev |= r; //黒石があれば返す位置を確定する

            r = 0;
            mask = (pos >> 8) & 0x00ffffffffffffff; //下方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask >> 8) & 0x00ffffffffffffff;
            }
            if(mask & black_board) rev |= r;

            r = 0;
            mask = (pos << 1) & 0xfefefefefefefefe; //左方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask << 1) & 0xfefefefefefefefe;
            }
            if(mask & black_board) rev |= r;

            r = 0;
            mask = (pos << 8) & 0xffffffffffffff00; //上方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask << 8) & 0xffffffffffffff00;
            }
            if(mask & black_board) rev |= r;

            r = 0;
            mask = (pos >> 9) & 0x007f7f7f7f7f7f7f; //右下方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask >> 9) & 0x007f7f7f7f7f7f7f;
            }
            if(mask & black_board) rev |= r;

            r = 0;
            mask = (pos >> 7) & 0x00fefefefefefefe; //左下方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask >> 7) & 0x00fefefefefefefe;
            }
            if(mask & black_board) rev |= r;

            r = 0;
            mask = (pos << 9) & 0xfefefefefefefe00; //左上方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask << 9) & 0xfefefefefefefe00;
            }
            if(mask & black_board) rev |= r;

            r = 0;
            mask = (pos << 7) & 0x7f7f7f7f7f7f7f00; //右上方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask << 7) & 0x7f7f7f7f7f7f7f00;
            }
            if(mask & black_board) rev |= r;
        }
        else{       // 白が打つ場合
            r = 0;
            mask = (pos >> 1) & 0x7f7f7f7f7f7f7f7f; //右方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){ //黒石が連続する間
                r |= mask;
                mask = (mask >> 1) & 0x7f7f7f7f7f7f7f7f;
            }
            if(mask & white_board) rev |= r; //白石があれば返す位置を確定する

            r = 0;
            mask = (pos >> 8) & 0x00ffffffffffffff; //下方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask >> 8) & 0x00ffffffffffffff;
            }
            if(mask & white_board) rev |= r;
            
            r = 0;
            mask = (pos << 1) & 0xfefefefefefefefe; //左方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask << 1) & 0xfefefefefefefefe;
            }
            if(mask & white_board) rev |= r;

            r = 0;
            mask = (pos << 8) & 0xffffffffffffff00; //上方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask << 8) & 0xffffffffffffff00;
            }
            if(mask & white_board) rev |= r;

            r = 0;
            mask = (pos >> 9) & 0x007f7f7f7f7f7f7f; //右下方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask >> 9) & 0x007f7f7f7f7f7f7f;
            }
            if(mask & white_board) rev |= r;

            r = 0;
            mask = (pos >> 7) & 0x00fefefefefefefe; //左下方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask >> 7) & 0x00fefefefefefefe;
            }
            if(mask & white_board) rev |= r;

            r = 0;
            mask = (pos << 9) & 0xfefefefefefefe00; //左上方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask << 9) & 0xfefefefefefefe00;
            }
            if(mask & white_board) rev |= r;

            r = 0;
            mask = (pos << 7) & 0x7f7f7f7f7f7f7f00; //右上方向へ返せるかを調べる
            while (mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask << 7) & 0x7f7f7f7f7f7f7f00;
            }
            if(mask & white_board) rev |= r;
        }

        return rev;
    }
    
    // (y, x) を bit座標に変換
    int64 vec2bit(int y, int x){
        return 1LL << ((8 * (7 - y)) + (7 - x));
    }

    // (y, x) に石を置く、置けた場合はtrueを返す
    bool putStone(const int y, const int x){
        const int64 pos = vec2bit(y, x);
        if(const int64 rev = canPut(pos); rev != 0){
            revStone(pos, rev);
            return true;
        }
        
        return false;
    }
    
    void AI(){
        // 打てないならパス
        bool pass = true;
        for (int64 pos = 0x8000000000000000; pos != 0; pos = (pos >> 1) & 0x7fffffffffffffff) {
            if (canPut(pos)){
                pass = false;
                break;
            }
        }
        if(pass){
            black_turn = !black_turn;
            return;
        }
        
        for(bool again = true; /* 相手がパスならtrue */ again; ){
            again = false;
            
            int stone_sum = countStone(BLACK_STONE) + countStone(WHITE_STONE);
            if(stone_sum >= 64) return;
            int64 pos = negaMax(std::min(SEARCH_LV, 64 - stone_sum), true, -INF, INF);
            int64 rev = canPut(pos);
            revStone(pos, rev);
            
            // 相手が打てないならもう一度AIのターン
            for(int64 pos = 0x8000000000000000; pos != 0; pos = (pos >> 1) & 0x7fffffffffffffff){
                if(canPut(pos)) return;
            }
            black_turn = !black_turn;
            again = true;
        }
    }
    
    int64 negaMax(const int64 depth, const bool is_before_put, const int64 alpha, const int64 beta, bool first_hand = true){
        if (depth == 0) return -boardValue();

        int64 max_value = alpha;
        bool is_put = false;
        int64 best_pos = 0;
        
        for(int64 pos = 0x8000000000000000; pos != 0; pos = (pos >> 1) & 0x7fffffffffffffff){
            if(int64 rev = canPut(pos); rev != 0){
                is_put = true;
                revStone(pos, rev);
                int64 tmp = -negaMax(depth - 1, true, -beta, -max_value, false);
                undo(pos, rev);

                if(tmp >= beta) return tmp;
                if(tmp > max_value){
                    max_value = tmp;
                    best_pos = pos;
                }
            }
        }

        if(is_put) {
            if(first_hand) return best_pos;
            return max_value;
        }
        else if(!is_before_put) return -boardValue();
        else{
            black_turn = !black_turn;
            int64 tmp = -negaMax(depth - 1, false, -beta, -max_value, false);
            black_turn = !black_turn;
            return tmp;
        }
    }
    
    // 盤面の価値を返す
    int64 boardValue(){
        int64 value = 0;
        const int64 VSP = 1, VCP = 40, VFS = 45;    // 評価値の重み
        
        const int stone_count = countStone(BLACK_STONE) + countStone(WHITE_STONE);

        if(stone_count >= FINAL_STAGE_NUM){
            value += countStone(WHITE_STONE) - countStone(BLACK_STONE);
        }
        else{
            value += valueStonePlace() * VSP;
            value += valueCanPut() * VCP;
            value += valueFixedStone() *  VFS;
        }

        if(!isAIcolorWhite) value = -value;    // AIが黒なら反転

        return value;
    }
    
    // 黒(または白)石の数を数える
    int countStone(const int stone){
        return __builtin_popcountll((stone == BLACK_STONE) ? black_board : white_board);
    }
    
    // オセロの石の位置によって価値を計算
    int64 valueStonePlace() {
        int64 value = 0, c = 0;

        if(isAIcolorWhite) {
            if(countStone(WHITE_STONE) >= 1) c = 0;  // 全滅対策
            else if(countStone(BLACK_STONE) == 0) c = -50000;    // 相手を全滅させたら評価値大アップ
            else c = 50000;
        }
        else {
            if(countStone(BLACK_STONE) >= 1) c = 0;
            else if(countStone(WHITE_STONE) == 0) c = 50000;
            else c = -50000;
        }

        int64 b = black_board, w = white_board;
        for(int i = 63; i >= 0; i--){
            value += (b & 1) * VALUE_PLACE[i];
            value -= (w & 1) * VALUE_PLACE[i];

            b >>= 1;
            w >>= 1;
        }

        return -(value + c);
    }

    // 石の置ける位置の数によって価値を計算
    int64 valueCanPut(){
        int64 value = 0;

        for(int64 mov = 0x8000000000000000; mov != 0; mov = (mov >> 1) & 0x7fffffffffffffff){
            if(canPut(mov)) value++;
        }

        if(black_turn) return -value;
        else return value;
    }
    
    // 確定石の数によって価値を計算
    int64 valueFixedStone(){
        int64 bit;
        int64 value = 0;

        if((black_board | white_board) & 0x8000000000000000){
            if(black_board & 0x8000000000000000){
                bit = 0x8000000000000000;
                while(black_board & bit){
                    value += 1;
                    bit = (bit >> 1) & 0x7fffffffffffffff;
                }
                bit = 0x8000000000000000;
                while(black_board & bit){
                    value += 1;
                    bit = (bit >> 8) & 0x00ffffffffffffff;
                }
            }
            else{
                bit = 0x8000000000000000;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit >> 1) & 0x7fffffffffffffff;
                }
                bit = 0x8000000000000000;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit >> 8) & 0x00ffffffffffffff;
                }
            }
        }
        if((black_board | white_board) & 0x0100000000000000){
            if(black_board & 0x0100000000000000){
                bit = 0x0100000000000000;
                while(black_board & bit){
                    value += 1;
                    bit = (bit << 1) & 0xfffffffffffffffe;
                }
                bit = 0x0100000000000000;
                while(black_board & bit){
                    value += 1;
                    bit = (bit >> 8) & 0x00ffffffffffffff;
                }
            }
            else{
                bit = 0x0100000000000000;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit << 1) & 0xfffffffffffffffe;
                }
                bit = 0x0100000000000000;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit >> 8) & 0x00ffffffffffffff;
                }
            }
        }
        if((black_board | white_board) & 0x0000000000000080){
            if(black_board & 0x0000000000000080){
                bit = 0x0000000000000080;
                while(black_board & bit){
                    value += 1;
                    bit = (bit >> 1) & 0x7fffffffffffffff;
                }
                bit = 0x0000000000000080;
                while(black_board & bit){
                    value += 1;
                    bit = (bit << 8) & 0xffffffffffffff00;
                }
            }
            else{
                bit = 0x0000000000000080;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit >> 1) & 0x7fffffffffffffff;
                }
                bit = 0x0000000000000080;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit << 8) & 0xffffffffffffff00;
                }
            }
        }
        if((black_board | white_board) & 0x0000000000000001){
            if(black_board & 0x0000000000000001){
                bit = 0x0000000000000001;
                while(black_board & bit){
                    value += 1;
                    bit = (bit << 1) & 0xfffffffffffffffe;
                }
                bit = 0x0000000000000001;
                while(black_board & bit){
                    value += 1;
                    bit = (bit << 8) & 0xffffffffffffff00;
                }
            }
            else{
                bit = 0x0000000000000001;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit << 1) & 0xfffffffffffffffe;
                }
                bit = 0x0000000000000001;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit << 8) & 0xffffffffffffff00;
                }
            }
        }

        return -value;
    }

};

void showBoard(OthelloGame &othello){
    auto board = othello.getBorad();
    
    // 盤を描画
    Rect(left_board_pos, top_board_pos, board_width, board_width).draw(Palette::Darkgreen);
    for(int i = 0; i < 9; i++){
        // 盤の枠線
        Line(left_board_pos + i * cell_width, top_board_pos, left_board_pos + i * cell_width, top_board_pos + board_width).draw(1, Palette::Black);
        Line(left_board_pos, top_board_pos + i * cell_width, left_board_pos + board_width, top_board_pos + i * cell_width).draw(1, Palette::Black);
    }
    
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            // 黒石を描画
            if(board[i][j] == BLACK_STONE){
                Circle(left_board_pos + cell_width / 2 + j * cell_width, top_board_pos + cell_width / 2 + i * cell_width, stone_size).draw(Palette::Black);
            }
            // 白石を描画
            if(board[i][j] == WHITE_STONE){
                Circle(left_board_pos + cell_width / 2 + j * cell_width, top_board_pos + cell_width / 2 + i * cell_width, stone_size).draw(Palette::White);
            }
        }
    }
}


int_main() {
    Scene::SetBackground(Palette::Pink);
    
    OthelloGame othello;
    
    while (System::Update()) {
        showBoard(othello);
        
        if(MouseL.down()){
            Vec2 cur_pos = Cursor::Pos();
            if(cur_pos.x < left_board_pos || cur_pos.y < top_board_pos) continue;
            int x = (cur_pos.x - left_board_pos) / cell_width;
            int y = (cur_pos.y - top_board_pos) / cell_width;
            if(x < 0 || x >= 8 || y < 0 || y >= 8) continue;
            
            if(othello.putStone(y, x)){
                othello.AI();
            }
        }
    }
}

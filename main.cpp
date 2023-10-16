#include <bits/stdc++.h>
using namespace std;

namespace utility {
    struct timer {
        chrono::system_clock::time_point start;
        // 開始時間を記録
        void CodeStart() {
            start = chrono::system_clock::now();
        }
        // 経過時間 (ms) を返す
        double elapsed() const {
        using namespace std::chrono;
            return (double)duration_cast<milliseconds>(system_clock::now() - start).count();
        }
    } mytm;
}

inline unsigned int rand_int() {
    static unsigned int tx = 123456789, ty=362436069, tz=521288629, tw=88675123;
    unsigned int tt = (tx^(tx<<11));
    tx = ty; ty = tz; tz = tw;
    return ( tw=(tw^(tw>>19))^(tt^(tt>>8)) );
}
#define TIME_LIMIT 1850

//-----------------以下から実装部分-----------------//

map<char,char> ch_r = {{'<','>'},{'>','<'},{'=','='}};

struct State {
    char ch;
    int n, q, d;

    State() : n(0) {}
    explicit State(int _n, int _q, int _d) : n(_n), q(_q), d(_d) {}
    
    inline void relationUpdate(const int e1, const int e2, vector<vector<char>>& r) {
        // ~~~ 個々の関係性の更新 ~~~ //
        if( !q-- ) return;
        cout << 1 << " " << 1 << " " << e1 << " " << e2 << '\n' << flush, cin >> ch;
        r[e1][e2] = ch;
        r[e2][e1] = ch_r[ch];

        // 3段論法の要領で a < b && b < c なら a < c も情報追加 (worshal-froid風)
        for(int k=0; k<n; k++) for(int i=0; i<n; i++) for(int j=0; j<n; j++) {
            if( r[i][k] != '?' && r[i][k] == r[k][j] ) r[i][j] = r[i][k];
            if( r[j][k] != '?' && r[j][k] == r[k][i] ) r[j][i] = r[j][k];
        }
        return;
    }

    inline void relationUpdate(const int i1, const int i2, vector<vector<char>>& r, const vector<vector<int>>& latest) {
        // ~~ group間の関係性の更新 ~~~ //
        if( !q-- ) return;
        cout << latest[i1].size() << " " << latest[i2].size() << " ";
        for(auto &&ele:latest[i1]) cout << ele << " ";
        for(auto &&ele:latest[i2]) cout << ele << " ";
        cout << '\n' << flush, cin >> ch;
        r[i1][i2] = ch;
        r[i2][i1] = ch_r[ch];

        // 3段論法の要領で a < b && b < c なら a < c も情報追加 (worshal-froid風)
        for(int k=0; k<d; k++) for(int i=0; i<d; i++) for(int j=0; j<d; j++) {
            if( r[i][k] != '?' && r[i][k] == r[k][j] ) r[i][j] = r[i][k];
            if( r[j][k] != '?' && r[j][k] == r[k][i] ) r[j][i] = r[j][k];
        }
        return;
    }

    inline void smoothing(const int g1, const int g2, vector<vector<char>>& r_one, vector<vector<char>>& r_group, vector<int>& res, vector<vector<int>>& latest) {
        int i1 = rand_int()%latest[g1].size(), i2 = rand_int()%latest[g2].size();
        int e1 = latest[g1][i1], e2 = latest[g2][i2];

        if( r_one[e1][e2] == '?' ) relationUpdate(e1, e2, r_one);
        if( r_one[e1][e2] == '=' || !q ) return;

        if( r_group[g1][g2] == '?' ) relationUpdate(g1, g2, r_group, latest);
        if( r_group[g1][g2] == '=' || !q ) return;

        // 軽 ⇒ 重 に重い荷物を渡す時は return (大小関係が崩れる為)
        if( r_group[g1][g2] == ch_r[r_one[e1][e2]] ) return;

        swap(latest[g1][i1], latest[g2][i2]);
        cout << latest[g1].size() << " " << latest[g2].size() << " ";
        for(auto &&ele:latest[g1]) cout << ele << " ";
        for(auto &&ele:latest[g2]) cout << ele << " ";
        cout << '\n' << flush, cin >> ch;
        if( !(--q) ) return;

        if( r_group[g1][g2] != ch ) swap(latest[g1][i1], latest[g2][i2]);
        else swap(res[e1],res[e2]);
        return;
    }

    inline void moving(const int g1, int g2, vector<vector<char>>& r_group, vector<int>& res, vector<vector<int>>& latest) {
        if( latest[g1].size() == 1 ) return;
        int i1 = rand_int()%latest[g1].size();
        int e1 = latest[g1][i1];

        if( r_group[g1][g2] == '?' ) relationUpdate(g1, g2, r_group, latest);
        if( r_group[g1][g2] == '=' || !q ) return;

        // 軽 ⇒ 重 に重い荷物を移動する時は return (大小関係が崩れる為)
        if( r_group[g1][g2] == '<' ) return;

        latest[g2].emplace_back(e1);
        latest[g1].erase(latest[g1].begin()+i1);

        cout << latest[g1].size() << " " << latest[g2].size() << " ";
        for(auto &&ele:latest[g1]) cout << ele << " ";
        for(auto &&ele:latest[g2]) cout << ele << " ";
        cout << '\n' << flush, cin >> ch;
        if( !(--q) ) return;

        if( r_group[g1][g2] != ch ) {
            latest[g1].emplace_back(e1);
            latest[g2].pop_back();
        }
        else res[e1] = g2;
        return;
    }
};

struct Solver {
    char ch;
    int n, d, q;
    vector<int> res;

    State state;
    vector<vector<int>> latest;
    vector<vector<char>> relation_one, relation_group;

    Solver(){
        this->input();
        state = State(n, q, d);
        // relation_one[i][j] : -1 (未測定), 0 (i<j), 1 (i==j), 2 (i>j)
        relation_group.assign(d,vector<char>(d,'?'));
        relation_one.assign(n,vector<char>(n,'?'));
        latest.assign(d,vector<int>{});
        res.assign(n,0);
        for(int i=0; i<d; i++) relation_group[i][i] = '=';
        for(int i=0; i<n; i++) relation_one[i][i] = '=';
    }

    void input(){cin >> n >> d >> q;}
    void output(){
        // 余りがあったら浪費
        while( state.q-- ) cout << 1 << " " << 1 << " " << 0 << " " << 1 << '\n' << flush, cin >> ch;
        for(int i=0; i<n; i++) cout << res[i] << " ";
        cout << flush << endl;
        return;
    }

    void solve(){
        utility::mytm.CodeStart();

        // 1. 適当にgroup作成
        for(int i=0; i<n; i++) {
            res[i] = i%d;
            latest[res[i]].emplace_back(i);
        }

        // 2. group間の関係を調整
        while( state.q && utility::mytm.elapsed() <= TIME_LIMIT ) {
            // feature : 完全乱択ではなく、
            //           原始モンテカルロで期待値を求める ⇒ 差が大きい group を候補にして要素swap
            int g1 = rand_int()%d, g2 = rand_int()%d;
            while( g1 == g2 ) g2 = rand_int()%d;

            int query = rand_int()%2;
            if( query ) state.smoothing(g1, g2, relation_one, relation_group, res, latest);
            else state.moving(g1, g2, relation_group, res, latest);
        }
        cerr << "Rest Query Time: " << q << endl;
        return;
    }
};

int main(){
    cin.tie(0);
    ios_base::sync_with_stdio(false);

    Solver solver;
    solver.solve();
    solver.output();
    
    return 0;
}
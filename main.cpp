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
    int n, q, d, cnt;

    State() : n(0) {}
    explicit State(int _n, int _q, int _d) : n(_n), q(_q), d(_d), cnt(0) {}
    
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
        return;
    }

    inline void groupCompare(int ele, vector<int>& rank, vector<vector<char>>& r, vector<vector<int>>& latest) {
        // ※ rank は降順で並んでいるとする
        if( rank.empty() ) {
            rank.emplace_back(ele);
            return;
        }

        int top = 0, under = rank.size();
        while( under-top > 1 ) {
            int mid = (top+under)/2;
            if( !q ) return;
            relationUpdate(ele, rank[mid], r, latest);
            if( r[ele][rank[mid]] == '<' ) top = mid+1;
            else if( r[ele][rank[mid]] == '>' ) under = mid;
            else {
                top = mid;
                break;
            }
        }
        if( top != rank.size() ) {
            if( !q ) return;
            relationUpdate(ele, rank[top], r, latest);
        }
        if( top != rank.size() && r[ele][rank[top]] == '<' ) top++;
        rank.insert(rank.begin() + top, ele);
        for(int i=0; i<rank.size(); i++) {
            if( i < top ) r[ele][rank[i]] = '<', r[rank[i]][ele] = '>';
            if( i > top ) r[ele][rank[i]] = '>', r[rank[i]][ele] = '<';
        }
        return;
    }

    inline void smoothing(const int g1, const int g2, vector<vector<char>>& r_one, vector<vector<char>>& r_group, vector<int>& res, vector<vector<int>>& latest, vector<int>& rank) {
        int i1 = rand_int()%latest[g1].size(), i2 = rand_int()%latest[g2].size();
        int e1 = latest[g1][i1], e2 = latest[g2][i2];

        if( r_one[e1][e2] == '?' ) relationUpdate(e1, e2, r_one);
        if( r_one[e1][e2] == '=' || r_one[e1][e2] == '<' || !q ) return;

        swap(latest[g1][i1], latest[g2][i2]);
        cout << latest[g1].size() << " " << latest[g2].size() << " ";
        for(auto &&ele:latest[g1]) cout << ele << " ";
        for(auto &&ele:latest[g2]) cout << ele << " ";
        cout << '\n' << flush, cin >> ch;
        if( !(--q) ) return;

        if( r_group[g1][g2] != ch ) {
            swap(latest[g1][i1], latest[g2][i2]);
            cnt++;
        }
        else {
            cnt = 0;
            swap(res[e1],res[e2]);
            vector<int> next;
            for(int i=0; i<d; i++) if( rank[i] != g1 && rank[i] != g2 ) next.emplace_back(rank[i]);
            swap(rank, next);
            groupCompare(g1, rank, r_group, latest);
            groupCompare(g2, rank, r_group, latest);
        }
        return;
    }

    inline void moving(const int g1, int g2, vector<vector<char>>& r_group, vector<int>& res, vector<vector<int>>& latest, vector<int>& rank) {
        if( latest[g1].size() == 1 ) return;
        int i1 = rand_int()%latest[g1].size();
        int e1 = latest[g1][i1];

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
            cnt++;
        }
        else {
            cnt = 0;
            res[e1] = g2;
            vector<int> next;
            for(int i=0; i<d; i++) if( rank[i] != g1 && rank[i] != g2 ) next.emplace_back(rank[i]);
            swap(rank, next);
            groupCompare(g1, rank, r_group, latest);
            groupCompare(g2, rank, r_group, latest);
        }
        return;
    }
};

struct Solver {
    char ch;
    int n, d, q;
    vector<int> res, g_rank;

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
        g_rank.assign({});
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
        for(auto ele:g_rank) cerr << ele << " ";
        cerr << endl;
        return;
    }

    void solve(){
        utility::mytm.CodeStart();

        // 1. group作成 & groupの大小管理
        for(int i=0; i<n; i++) {
            res[i] = i%d;
            latest[res[i]].emplace_back(i);
        }
        for(int i=0; i<d; i++) state.groupCompare(i, g_rank, relation_group, latest);

        // 2. group間の関係を調整
        while( state.q && utility::mytm.elapsed() <= TIME_LIMIT ) {
            // feature : 完全乱択ではなく、
            //           原始モンテカルロで期待値を求める ⇒ 差が大きい group を候補にして要素swap
            int g1 = g_rank[0], g2 = g_rank.back();
            if( state.cnt >= 5 ) {
                g1 = rand_int()%d, g2 = g1;
                while( g1 == g2 ) g2 = rand_int()%d;
                for(int i=0; i<d; i++) {
                    if( g_rank[i] == g1 ) break;
                    if( g_rank[i] == g2 ) {
                        swap(g1,g2);
                        break;
                    }
                }
            }

            int query = rand_int()%2;
            if( query ) state.smoothing(g1, g2, relation_one, relation_group, res, latest, g_rank);
            else state.moving(g1, g2, relation_group, res, latest, g_rank);
        }
        cerr << "Rest Query Time: " << state.q << endl;
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
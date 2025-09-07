#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace std;

using namespace __gnu_pbds;
#define Code ios_base::sync_with_stdio(false);
#define By cin.tie(NULL);
#define Solve cout.tie(NULL);

using ll = long long;
using lld = long double;
using ull = unsigned long long;

const lld pi = 3.141592653589793238;
const ll INF = INT_MAX;
const ll mod = 1e9 + 7;

typedef pair<ll, ll> pll;
typedef vector<ll> vll;
typedef vector<pll> vpll;
typedef unordered_map<ll, ll> umll;
typedef map<ll, ll> mll;

#define ff first
#define ss second
#define pb push_back
#define mp make_pair
#define fl(i, n) for(int i = 0; i < n; i++)
#define rl(i, m, n) for(int i = n; i >= m; i--)
#define py cout << "YES\n";
#define pn cout << "NO\n";
#define pm cout << "-1\n";
#define vr(v) v.begin(), v.end()
#define rev(v) v.rbegin(), v.rend()

#ifdef TA77
#define debug(x) cerr << #x << " = " << x << endl;
#else
#define debug(x)
#endif

template<typename T1, typename T2>
istream& operator>>(istream &in, pair<T1, T2> &p) { return (in >> p.first >> p.second); }
template<typename T>
istream& operator>>(istream &in, vector<T> &v) { for (auto &x : v) in >> x; return in; }
template<typename T1, typename T2>
ostream& operator<<(ostream &out, const pair<T1, T2> &p) { return (out << p.first << " " << p.second); }
template<typename T>
ostream& operator<<(ostream &out, const vector<T> &v) { for (auto &x : v) out << x << " "; return out; }

template <typename T> void print(T &&t) { cout << t << "\n"; }
template<typename T> void printvec(vector<T> v) { for (auto x : v) cout << x << " "; cout << "\n"; }
template <typename T>
using ordered_set = tree<T, null_type, less<T>, rb_tree_tag, tree_order_statistics_node_update>;

ll gcd(ll a, ll b) { return b == 0 ? a : gcd(b, a % b); }
ll lcm(ll a, ll b) { return a / gcd(a, b) * b; }
ll powermod(ll x, ll y, ll p) {
    ll res = 1; x %= p;
    while (y > 0) {
        if (y & 1) res = (res * x) % p;
        y >>= 1; x = (x * x) % p;
    }
    return res;
}

bool isPrime(ll n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (ll i = 5; i * i <= n; i += 6)
        if (n % i == 0 || n % (i + 2) == 0) return false;
    return true;
}

void solve() {
    // Your logic here
    ll n,k;
    cin >> n >>k;
    vector<ll>a(n);
    cin >> a;
    map<ll,ll>m;
    fl(i,n){
        m[a[i]]++;
    }
    int mx = 0;
    while(m[mx]>0){
        mx++;
    }
    fl(i,n){
        if(m[a[i]] == 1 && a[i]<mx){
            continue;
        }
        else{
            a[i] = mx;
        }
    }
    k--;
    if(k == 0){
        cout<<accumulate(a.begin(),a.end(),0ll)<<endl;
        return;
    }
    m.clear();
    fl(i,n){
        m[a[i]]++;
    }
    mx = 0;
    while(m[mx]>0){
        mx++;
    }
    fl(i,n){
        if(m[a[i]] == 1 && a[i]<mx){
            continue;
        }
        else{
            a[i] = mx;
        }
    }
    k--;
    if(k == 0){
        cout<<accumulate(a.begin(),a.end(),0ll)<<endl;
        return;
    }
    m.clear();
    fl(i,n){
        m[a[i]]++;
    }
    mx = 0;
    while(m[mx]>0){
        mx++;
    }
    fl(i,n){
        if(m[a[i]] == 1 && a[i]<mx){
            continue;
        }
        else{
            a[i] = mx;
        }
    }
    k--;
    if(k == 0){
        cout<<accumulate(a.begin(),a.end(),0ll)<<endl;
        return;
    }
    m.clear();
    fl(i,n){
        m[a[i]]++;
    }
    ll ans = accumulate(a.begin(),a.end(),0ll);
    ll o = -1;
    fl(i,n){
        if(m[a[i]]>1){
            o = a[i];
            break;
        }
    }
    if(k%2){
        cout<<ans-m[o]<<endl;
        return;
    }
    cout<<ans<<endl;
}
int main() {
    Code By Solve
    int t;
    cin >> t;
    while(t--){
       solve();
    }
    return 0;
}
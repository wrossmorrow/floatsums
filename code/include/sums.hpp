
#include <math.h>

template<typename T> void seqsums( int N , T * x ) noexcept 
{
    for( int n = 1 ; n < N ; n++ ) x[0] += x[n];
}

template<typename T> void pairsums( int N , T * x ) noexcept 
{
    if( N == 1 ) { return; }
    if( N == 2 ) { x[0] += x[1]; return; }
    for( int n = 0 ; n < N/2 ; n++ ) x[n] = x[2*n] + x[2*n+1];
    if( N % 2 == 1 ) { x[N/2-1] += x[N-1]; }
    pairsums(N/2, x);
}

class Summer {
public: 
    virtual char label() noexcept { return '?'; }
    virtual void sum( int N , float  * x ) noexcept {}
    virtual void sum( int N , double * x ) noexcept {}
};

class SequentialSummer : public Summer {
public: 
    virtual char label() noexcept { return 's'; }
    virtual void sum( int N , float  * x ) noexcept { seqsums(N,x); }
    virtual void sum( int N , double * x ) noexcept { seqsums(N,x); }
};

class PairwiseSummer : public Summer {
public: 
    virtual char label() noexcept { return 'p'; }
    virtual void sum( int N , float  * x ) noexcept { pairsums(N,x); }
    virtual void sum( int N , double * x ) noexcept { pairsums(N,x); }
};

template<typename T> long roundT( T x ) noexcept 
{
    return ( x < 0 ? -(long)(-x+0.5) : (long)(x+0.5) );
}

template<typename T> void blocksums( int Nb , int N , T * x , T f , Summer * S ) noexcept 
{
    int B , R , b;
    if( N <= Nb ) { S->sum(N,x); return; }
    R = N % Nb; B = ( N - R ) / Nb;
    for( b = 0 ; b < B ; b++ ) {
        S->sum( Nb , x + Nb*b ); // x[Nb*b] will have bth block sum
        x[b] = (T)( roundT(f*x[Nb*b]) )/f; // truncate error and move
    }
    if( R > 0 ) {
        S->sum( R , x + Nb*B ); // x[Nb*B] will have remainder block sum
        x[B] = (T)( roundT(f*x[Nb*B]) )/f; // truncate error and move
        B++; // for recursion call
    }
    blocksums( Nb , B , x , f , S ); // recurse
}

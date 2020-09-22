
#include <math.h>
#include <float.h>
#include <iostream>

using namespace std;

// overloaded returns to enable call in templated methods
double epsilon( float  m ) { return (double)FLT_EPSILON; }
double epsilon( double m ) { return DBL_EPSILON; }

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

template<typename T> int seqbound( T m ) noexcept 
{
    double u = epsilon(m);

    // N(N-1) u EX < 0.5 ~ N^2 u EX < 0.5
    // return (int)floor( 0.5 * u / sqrt(m) );

    // N^2 - N - 1 / (2 u EX) < 0 
    // N^2 - N - 1 / (2 u EX) = 0 
    //     ==> 1/2 +/- 1/2 sqrt( 1 + 2 / ( u EX ) ) 
    //     ==> N ~ 1/2 [ 1 + sqrt( 1 + 2 / ( u EX ) ) ]
    // Note that ( N^2 - N - 1 / (2 u EX) )(0) < 0
    return (int)floor( ( 1.0 + sqrt( 1.0 + 2.0 / (u*m) ) ) / 2.0 );

    // Note that the result passed back is >= 1
    // if m ~ 1/u, it's (1+sqrt(3))/2 ~ 1.366 and m ~ 1/u is 
    // large in context, at least for doubles
}

template<typename T> int pairbound( T m ) noexcept 
{
    int i;
    double F, D, B, C, S;
    double u = epsilon(m);
    double f = log(2.0) + log(u) + log(m);
    double d = 1.0/log(2.0);

    // N log2 N < 1 / ( 2 u EX )

    B = 1.0 / ( 2.0 * u * m );

    /*
    // log B + log log2 B - log( 2 u EX ) = 0
    // 1/B + 1/log2 B d/dB[log2 B]
    //      = 1/B + 1/log2 B  1/( B log 2 )
    //      = 1/B + ( 1/log 2 ) 1/( B log2 B )
    F = log(B) + log(log2(B)) - f;
    for( i = 0 ; i < 100 ; i++ ) {
        if( abs(F) <= 1.0e-6 ) break;
        D = 1.0 / B + d / ( B * log2(B) );
        S = - F / D;
        std::cout << "  " << B << "  " << S << std::endl;
        C = B + S;
        while( C <= 0.0 ) { 
            S /= 2.0; 
            C = B + S; 
        }
        B = C;
        F = log(B) + log(log2(B)) - f;
    }

    std::cout << B << std::endl;
    */

    f = 1.0 / ( 2.0 * u * m );

    // B log2 B - 1 / ( 2 u EX ) = 0
    // log2 B + B d/dB[log2 B] 
    //      = log2 B + B/( B log 2 )
    //      = log2 B + 1/log 2
    F = B * log2(B) - f;
    for( i = 0 ; i < 100 ; i++ ) {
        if( abs(F) <= 1.0e-6 ) break;
        D = log2(B) + d;
        S = - F / D;
        C = B + S;
        while( C <= 0.0 ) { S /= 2.0; C = B + S; }
        B = C;
        F = B * log2(B) - f;
    }

    return (int)floor(B);
}

class Summer {
public: 
    virtual char label() noexcept { return '?'; }
    virtual void sum( int N , float  * x ) noexcept {}
    virtual void sum( int N , double * x ) noexcept {}
    virtual int bound( float  m ) noexcept { return 0; }
    virtual int bound( double m ) noexcept { return 0; }
};

class SequentialSummer : public Summer {
public: 
    virtual char label() noexcept override { return 's'; }
    virtual void sum( int N , float  * x ) noexcept override { seqsums(N,x); }
    virtual void sum( int N , double * x ) noexcept override { seqsums(N,x); }
    virtual int bound( float  m ) noexcept override { return seqbound(m); }
    virtual int bound( double m ) noexcept override { return seqbound(m); }
};

class PairwiseSummer : public Summer {
public: 
    virtual char label() noexcept override { return 'p'; }
    virtual void sum( int N , float  * x ) noexcept override { pairsums(N,x); }
    virtual void sum( int N , double * x ) noexcept override { pairsums(N,x); }
    virtual int bound( float  m ) noexcept override { return pairbound(m); }
    virtual int bound( double m ) noexcept override { return pairbound(m); }
};

template<typename T> long roundT( T x ) noexcept 
{
    return ( x < 0 ? -(long)(-x+0.5) : (long)(x+0.5) );
}

template<typename T> void blocksums( int Nb , int N , T * x , T f , Summer * S ) noexcept 
{
    int B , R;

    // if N is small enough relative to the block size, just sum
    if( N <= Nb ) { S->sum(N,x); return; }

    // get number of blocks (B) of Nb amounts in N amounts, with remainder (R)
    R = N % Nb; B = ( N - R ) / Nb;

    // sum up each consecutive block of Nb amounts, and remainder
    for( int b = 0 ; b < B ; b++ ) {
        S->sum(Nb, x+Nb*b); // x[Nb*b] will have bth block sum
        x[b] = ((T)roundT(f*x[Nb*b]))/f; // truncate error and move
    }
    if( R > 0 ) {
        S->sum(R, x+Nb*B); // x[Nb*B] will have remainder block sum
        x[B] = ((T)roundT(f*x[Nb*B]))/f; // truncate error and move
        B++; // we need B+1 for recursion call below
    }

    // recurse... 
    // x[0:B] or x[0:B) will have the block sums, rounded to the desired 
    // place, put back into whatever units the data should be in (via f)
    // we may still need to block further, but we have only B or B+1 
    // amounts to sum up now (if B+1 we've already incremented B for this). 
    // because we are recursing into "sums of sums", our underlying amount
    // distribution has changed and we have to therefore modify the bound. 
    // std::cout << "(" << N << "," << Nb << ") -> (" << B << "," << (int)ceil(sqrt((double)Nb)) << ")\n";
    Nb = (int)floor(sqrt((double)Nb));
    if( Nb < 2 ) { 
        // std::cout << "WARNING: degenerate iteration in blocksum. Not recursing further.\n";
        S->sum(B, x);
    } else {
        blocksums( Nb , B , x , f , S ); 
    }

}

bool blocksim( long Nb , long N ) noexcept 
{
    if( N <= Nb ) { return true; }
    int R = N % Nb , B = N/Nb + ( R > 0 ? 1 : 0);
    Nb = (long)floor(sqrt((double)Nb));
    return ( Nb < 2 ? false : blocksim(Nb, B) );
}

void _bisect( const long Nb , long * Nl , long * Nu ) noexcept
{
    if( Nu[0] <= Nl[0] + 1 ) { Nu[0] = Nl[0] + 1; return; }
    long Nm = ( Nu[0] + Nl[0] ) / 2;
    if( blocksim(Nb, Nm) ) { Nl[0] = Nm; } else { Nu[0] = Nm; }
    _bisect(Nb, Nl, Nu);
}

void bisect( const long Nb , long * Nl , long * Nu ) noexcept
{
    if( Nu[0] < Nl[0] ) return bisect(Nb, Nu, Nl);
    while( ! blocksim(Nb, Nl[0]) ) Nl[0] = Nl[0]/2;
    while(  blocksim(Nb, Nu[0])  ) Nu[0] = Nu[0]*2;
    _bisect( Nb , Nl , Nu );
}


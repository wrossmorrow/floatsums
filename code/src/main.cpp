
#include <stdio.h>  
#include <iostream>
#include <unistd.h>  

#include <string>

#include "sums.hpp"
#include "bins.hpp"
#include "results.hpp"

using namespace std;

Results experiment( int N , int T , BinModel * M , Summer * S , bool block , int Bf , int Bd ) // , std::string& fn )
{
    int i , t , s;

    long   * alc , _lc;
    float  * afc , * afd , * fwk , _fc , _fd;
    double * adc , * add , * dwk , _dc , _dd;
    bool fv = true , dv = true;

    if( block ) {

        long Nl , Nu;

        Nl = Bf + 1 , Nu = 10 * Bf;
        bisect( Bf , &Nl , &Nu );
        fv = ( N <= Nl );

        Nl = Bd + 1 , Nu = 10 * Bd;
        bisect( Bd , &Nl , &Nu );
        dv = ( N <= Nl );

    }

    Results R = Results(N, T, block, fv, dv);

    srand(time(0)); 

    alc = ( long   * )malloc( N * sizeof(long)   );
    afc = ( float  * )malloc( N * sizeof(float)  );
    afd = ( float  * )malloc( N * sizeof(float)  );
    adc = ( double * )malloc( N * sizeof(double) );
    add = ( double * )malloc( N * sizeof(double) );
    fwk = ( float  * )malloc( N * sizeof(float)  );
    dwk = ( double * )malloc( N * sizeof(double) );
    
    for( t = 0 ; t < T ; t++ ) {

        // initialize random amounts
        for( i = 0 ; i < N ; i++ ) {
            alc[i] = M->draw();
            afc[i] =  (float)alc[i]; afd[i] = afc[i]/100.0f;
            adc[i] = (double)alc[i]; add[i] = adc[i]/100.0;
        }

        // add all the entries
        _lc = 0; for( i = 0 ; i < N ; i++ ) { _lc += alc[i]; }
        memcpy( fwk , afc , N*sizeof(float)  ); blocksums( Bf , N , fwk ,   1.0f , S ); _fc = fwk[0];
        memcpy( fwk , afd , N*sizeof(float)  ); blocksums( Bf , N , fwk , 100.0f , S ); _fd = fwk[0];
        memcpy( dwk , adc , N*sizeof(double) ); blocksums( Bd , N , dwk ,   1.0  , S ); _dc = dwk[0];
        memcpy( dwk , add , N*sizeof(double) ); blocksums( Bd , N , dwk , 100.0  , S ); _dd = dwk[0];

        R.fcc[0] += ( _lc == lroundf(_fc) ? 1 : 0 );
        R.fdc[0] += ( _lc == lroundf(100.0f*_fd) ? 1 : 0 );
        R.dcc[0] += ( _lc == lround(_dc) ? 1 : 0 );
        R.ddc[0] += ( _lc == lround(100.0*_dd) ? 1 : 0 );

        // subtract all the entries
        _lc = 0; for( i = 0 ; i < N ; i++ ) { _lc -= alc[i]; }
        memcpy( fwk , afc , N*sizeof(float)  ); blocksums( Bf , N , fwk ,   1.0f , S ); _fc = -fwk[0];
        memcpy( fwk , afd , N*sizeof(float)  ); blocksums( Bf , N , fwk , 100.0f , S ); _fd = -fwk[0];
        memcpy( dwk , adc , N*sizeof(double) ); blocksums( Bd , N , dwk ,   1.0  , S ); _dc = -dwk[0];
        memcpy( dwk , add , N*sizeof(double) ); blocksums( Bd , N , dwk , 100.0  , S ); _dd = -dwk[0];

        R.fcc[1] += ( _lc == lroundf(_fc) ? 1 : 0 );
        R.fdc[1] += ( _lc == lroundf(100.0f*_fd) ? 1 : 0 );
        R.dcc[1] += ( _lc == lround(_dc) ? 1 : 0 );
        R.ddc[1] += ( _lc == lround(100.0*_dd) ? 1 : 0 );

        // add/subtract all the entries randomly (impose signs explicitly)
        _lc = 0; 
        for( i = 0 ; i < N ; i++ ) {
            s = sign();
            _lc += s * alc[i];
            afc[i] *= s; afd[i] *= s;
            adc[i] *= s; add[i] *= s;
        }
        memcpy( fwk , afc , N*sizeof(float)  ); blocksums( Bf , N , fwk ,   1.0f , S ); _fc = fwk[0];
        memcpy( fwk , afd , N*sizeof(float)  ); blocksums( Bf , N , fwk , 100.0f , S ); _fd = fwk[0];
        memcpy( dwk , adc , N*sizeof(double) ); blocksums( Bd , N , dwk ,   1.0  , S ); _dc = dwk[0];
        memcpy( dwk , add , N*sizeof(double) ); blocksums( Bd , N , dwk , 100.0  , S ); _dd = dwk[0];

        R.fcc[2] += ( _lc == lroundf(_fc) ? 1 : 0 );
        R.fdc[2] += ( _lc == lroundf(100.0f*_fd) ? 1 : 0 );
        R.dcc[2] += ( _lc == lround(_dc) ? 1 : 0 );
        R.ddc[2] += ( _lc == lround(100.0*_dd) ? 1 : 0 );

    }

    free(alc);
    free(afc);
    free(afd);
    free(adc);
    free(add);
    free(fwk);
    free(dwk);

    return R;

}

void print_help()
{
    std::cout << "floatsums help.\n";
}

int main( int argc , char * argv[] ) 
{

    int opt;
    bool block = false, verbose = false, file = false;
    char method = 's';
    char ofn[256];
    
    int N = 10 , T = 1, Bf = 0, Bd = 0; 
    BinModel * M = nullptr;
    Summer * S = nullptr;
    
    while( ( opt = getopt(argc, argv, ":hpbvm:N:T:f:") ) != -1 ) {
        switch(opt) {
            case 'h': print_help(); exit(0);
            case 'p': method = 'p'; break;
            case 'b': block = true; break;
            case 'v': verbose = true; break;
            case 'N': N = std::stoi( optarg ); break;
            case 'T': T = std::stoi( optarg ); break;
            case 'm': M = new BinModel( optarg ); break;
            case 'f': file = true; strcpy( ofn , optarg ); break;
            case ':': printf("option needs a value\n"); exit(1);  
            case '?': printf("unknown option: %c\n", optopt); exit(1);  
        }  
    }

    if( M == nullptr ) { // asserts a valid number of bins (w/ initialization)
        std::cout << "ERROR: a model file is required, specify with -m option\n";
        exit(1); 
    }
    if( N <= 1 ) { // asserts a valid number accumulation size
        std::cout << "ERROR: number of amounts must be at least 2\n";
        exit(1); 
    }
    if( T <= 0 ) { // asserts a valid number of trials
        std::cout << "ERROR: trials must be at least 1\n";
        exit(1); 
    }

    double m = M->mean();

    // define summer class instance to use in our sums
    if( method == 's' ) {
        S = new SequentialSummer();
    } else {
        S = new PairwiseSummer();
    }

    if( block ) {
        Bf = S->bound( (float)m );
        Bd = S->bound( m );
    } else { 
        Bf = N+1; 
        Bd = N+1;
    } // makes sure we don't block if option -B isn't passed

    // if asked, print out general information about what we'll do
    if( verbose ) {
        std::cout << "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = " << std::endl;
        std::cout << "+ Running T = " << T << " trials with N = " << N << " amounts having mean " << m << "." << std::endl;
        std::cout << "+ "; M->print();
        std::cout << "+ ";
        if( method == 'p' ) std::cout << "Using pairwise summation";
        else std::cout << "Using sequential summation";
        std::cout << "." << std::endl;
        if( block ) {
            long Nl , Nu;
            std::cout << "+ Blocking";
            std::cout << " with bounds";
            std::cout << " float: ";
            if( Bf == 0 ) std::cout << "undefined"; 
            else std::cout << Bf;
            std::cout << " (";
            Nl = Bf + 1; Nu = 10 * Bf; bisect( Bf , &Nl , &Nu ); 
            if( N > Nl ) { std::cout << "in"; } std::cout << "valid";
            std::cout << ") , double: ";
            if( Bd == 0 ) std::cout << "undefined"; 
            else std::cout << Bd;
            std::cout << " (";
            Nl = Bd + 1; Nu = 10 * Bd; bisect( Bd , &Nl , &Nu ); 
            if( N > Nl ) { std::cout << "in"; }
            std::cout << "valid)." << std::endl;
        }
        if( file ) std::cout << "+ Writing results to " << ofn << "." << std::endl;
        std::cout << std::endl;
    }

    // do experiment, assigning the result object
    Results R = experiment(N, T, M, S, block, Bf, Bd);

    if( verbose ) {
        std::cout << "+ Done." << std::endl;
        std::cout << "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = ";
        std::cout << std::endl << std::endl;
    }


    // output
    if( file ) R.write( ofn , S->label() );
    else R.print( S->label() );

    return 0;

}

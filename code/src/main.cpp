
#include <stdio.h>  
#include <unistd.h>  

#include <string>

#include "sums.hpp"
#include "bins.hpp"
#include "results.hpp"

using namespace std;

Results experiment( int B , int N , int T , BinModel * M , Summer * S ) // , std::string& fn )
{
    int i , t , s;

    long   * alc , _lc;
    float  * afc , * afd , * fwk , _fc , _fd;
    double * adc , * add , * dwk , _dc , _dd;

    Results R = Results(B,N,T);

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
        memcpy( fwk , afc , N*sizeof(float)  ); blocksums( B , N , fwk ,   1.0f , S ); _fc = fwk[0];
        memcpy( fwk , afd , N*sizeof(float)  ); blocksums( B , N , fwk , 100.0f , S ); _fd = fwk[0];
        memcpy( dwk , adc , N*sizeof(double) ); blocksums( B , N , dwk ,   1.0  , S ); _dc = dwk[0];
        memcpy( dwk , add , N*sizeof(double) ); blocksums( B , N , dwk , 100.0  , S ); _dd = dwk[0];

        R.fcc[0] += ( _lc == lroundf(_fc) ? 1 : 0 );
        R.fdc[0] += ( _lc == lroundf(100.0f*_fd) ? 1 : 0 );
        R.dcc[0] += ( _lc == lround(_dc) ? 1 : 0 );
        R.ddc[0] += ( _lc == lround(100.0*_dd) ? 1 : 0 );

        // subtract all the entries
        _lc = 0; for( i = 0 ; i < N ; i++ ) { _lc -= alc[i]; }
        memcpy( fwk , afc , N*sizeof(float)  ); blocksums( B , N , fwk ,   1.0f , S ); _fc = -fwk[0];
        memcpy( fwk , afd , N*sizeof(float)  ); blocksums( B , N , fwk , 100.0f , S ); _fd = -fwk[0];
        memcpy( dwk , adc , N*sizeof(double) ); blocksums( B , N , dwk ,   1.0  , S ); _dc = -dwk[0];
        memcpy( dwk , add , N*sizeof(double) ); blocksums( B , N , dwk , 100.0  , S ); _dd = -dwk[0];

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
        memcpy( fwk , afc , N*sizeof(float)  ); blocksums( B , N , fwk ,   1.0f , S ); _fc = fwk[0];
        memcpy( fwk , afd , N*sizeof(float)  ); blocksums( B , N , fwk , 100.0f , S ); _fd = fwk[0];
        memcpy( dwk , adc , N*sizeof(double) ); blocksums( B , N , dwk ,   1.0  , S ); _dc = dwk[0];
        memcpy( dwk , add , N*sizeof(double) ); blocksums( B , N , dwk , 100.0  , S ); _dd = dwk[0];

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

int main( int argc , char * argv[] ) 
{

    int opt;
    bool block = false, verbose = false, file = false;
    char method = 's';
    char ofn[256];
    
    int N = 10 , T = 1, B = 0; 
    BinModel * M = nullptr;
    Summer * S = nullptr;
    
    while( ( opt = getopt(argc, argv, ":pvm:N:T:B:f:") ) != -1 ) {
        switch(opt) {
            case 'p': method = 'p'; break;
            case 'v': verbose = true; break;
            case 'N': N = std::stoi( optarg ); break;
            case 'T': T = std::stoi( optarg ); break;
            case 'B': block = true; B = std::stoi( optarg ); break;
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
    if( block && B <= 1 ) { // asserts valid block size
        std::cout << "ERROR: block size must be at least 2\n";
        exit(1); 
    }

    if( !block ) { B = N+1; } // makes sure we don't block if option -B isn't passed

    // define summer class instance to use in our sums
    if( method == 's' ) {
        S = new SequentialSummer();
    } else {
        S = new PairwiseSummer();
    }

    // if asked, print out general information about what we'll do
    if( verbose ) {
        std::cout << "+ Running T = " << T << " trials with N = " << N << " amounts. ";
        if( method == 'p' ) std::cout << "Using pairwise summation";
        else std::cout << "Using sequential summation";
        if( block ) std::cout << ", with blocking";
        std::cout << ".\n";
        std::cout << "+ "; M->print();
        if( file ) std::cout << "Writing results to " << ofn << ".\n";
    }

    // do experiment, assigning the result object
    Results R = experiment(B, N, T, M, S);

    // output
    if( file ) R.write( ofn , S->label() );
    else R.print( S->label() );

    return 0;

}

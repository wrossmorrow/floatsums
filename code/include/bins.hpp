
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;

double random_u( ) noexcept { return ((double)rand())/((double)RAND_MAX); }

int sign() noexcept { return ( random_u() <= 0.5 ? +1 : -1 ); }

int randint( int a , int b ) noexcept 
{ 
    if( b == a ) return b;
    if( b <  a ) return randint(b,a);
    return (int)( (double)(b-a) * random_u() ) + a;
}

long random_amount_unif( int M ) noexcept { return (long)( rand() % M ); }

// binp ~ a bins - 1 length array of non-decreasing values in [0,1]
// binb ~ a bins + 1 length array of non-decreasing positive values 
// !! WARNING !! - conditions are not checked
long bin_draw( int bins , double * binp , int * binb )
{
    double r = random_u();
    for( int b = 0 ; b < bins-1 ; b++ ) {
        if( r < binp[b] ) return (long)randint(binb[b],binb[b+1]);
    }
    return (long)randint(binb[bins-1],binb[bins]);
}

void read_bins( const char * file , int * bins , double ** binp , int ** binb )
{
    int b;
    std::istringstream iss;

    std::ifstream infile( file );
    std::string line;

    std::getline(infile, line);
    std::istringstream iss1(line);
    if ( !( iss1 >> bins[0] ) ) { 
        std::cout << "error reading number of bins";
        exit(1); 
    }

    binp[0] = (double*)malloc( (bins[0])*sizeof(double) );
    binb[0] = (int*)malloc( (bins[0]+1)*sizeof(int) );

    std::getline(infile, line);
    std::istringstream iss2(line);
    for( b = 0 ; b < bins[0]-1 ; b++ ) {
        if( !( iss2 >> binp[0][b] ) ) { 
            std::cout << "error reading bin probabilities (" << b << ")";
            exit(1); 
        }
        if( binp[0][b] < 0.0 ) { 
            std::cout << "bin probabilities can't be negative";
            exit(1); 
        }
        if( b > 0 ) binp[0][b] += binp[0][b-1];
        if( binp[0][b] > 1.0 ) {
            std::cout << "bin probabilities must sum to one";
            exit(1); 
        }
    }
    binp[0][bins[0]-1] = 1.0;

    std::getline(infile, line);
    std::istringstream iss3(line);
    if( !( iss3 >> binb[0][0] ) ) { 
        std::cout << "error reading bin bounds (0)";
        exit(1); 
    }
    for( b = 1 ; b < bins[0]+1 ; b++ ) {
        if( !( iss3 >> binb[0][b] ) ) { 
            std::cout << "error reading bin bounds (" << b << ")";
            exit(1); 
        }
        if( binb[0][b] < binb[0][b-1] ) {
            std::cout << "bin probabilities must be non-decreasing";
            exit(1); 
        }
    }
}

class BinModel {

public: 

    BinModel( const char * file )
    {

        int b;
        std::istringstream iss;

        std::ifstream infile( file );
        std::string line;

        std::getline(infile, line);
        std::istringstream iss1(line);
        if ( !( iss1 >> this->bins ) ) { 
            std::cout << "error reading number of bins";
            exit(1); 
        }

        this->binp = (double*)malloc( (this->bins)*sizeof(double) );
        this->binb = (int*)malloc( (this->bins+1)*sizeof(int) );

        if( bins > 1 ) {
            std::getline(infile, line);
            std::istringstream iss2(line);
            for( b = 0 ; b < this->bins-1 ; b++ ) {
                if( !( iss2 >> this->binp[b] ) ) { 
                    std::cout << "error reading bin probabilities (" << b << ")";
                    exit(1); 
                }
                if( this->binp[b] < 0.0 ) { 
                    std::cout << "bin probabilities can't be negative";
                    exit(1); 
                }
                if( b > 0 ) this->binp[b] += this->binp[b-1];
                if( this->binp[b] > 1.0 ) {
                    std::cout << "bin probabilities must sum to one";
                    exit(1); 
                }
            }
        }
        this->binp[this->bins-1] = 1.0;

        std::getline(infile, line);
        std::istringstream iss3(line);
        if( !( iss3 >> this->binb[0] ) ) { 
            std::cout << "error reading bin bounds (0)";
            exit(1); 
        }
        for( b = 1 ; b < this->bins+1 ; b++ ) {
            if( !( iss3 >> this->binb[b] ) ) { 
                std::cout << "error reading bin bounds (" << b << ")";
                exit(1); 
            }
            if( this->binb[b] < this->binb[b-1] ) {
                std::cout << "bin probabilities must be non-decreasing";
                exit(1); 
            }
        }
    }

    long draw() noexcept 
    {
        double r = random_u();
        for( int b = 0 ; b < this->bins-1 ; b++ ) {
            if( r < this->binp[b] ) return (long)randint(this->binb[b],this->binb[b+1]);
        }
        return (long)randint(this->binb[this->bins-1],this->binb[this->bins]);
    }

    void print() noexcept 
    {
        std::cout << "Model CDF: (" << this->binb[0] << ",0)";
        for( int b = 1 ; b < this->bins+1 ; b++ ) {
            std::cout << ",(" << this->binb[b] << "," << this->binp[b-1] << ")";
        }
        std::cout << "\n";
    }

private:

    int bins, * binb;
    double * binp;

};

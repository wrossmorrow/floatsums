
#include <fstream>

using namespace std;

class Results {

public: 

    char lbl[3] = {'+','-','?'};
    int fcc[3] = {0} , fdc[3]  = {0}, dcc[3]  = {0}, ddc[3] = {0};
    int B , N , T;

    Results( int B , int N , int T ) : B{B},N{N},T{T} {}

    void write( const char * const fn , const char sl )
    {
        ofstream file;
        file.open( fn , ios::out );
        for( int i = 0 ; i < 3 ; i++ ) {
            file << this->N << ",";
            file << this->T << ",";
            file << this->lbl[i] << ",";
            file << sl << ",";
            file << (this->B < this->N ? 'y' : 'n') << ",";
            file.setf(ios::fixed, ios::floatfield); 
            file.precision(2); 
            file << 100.0*((double)this->fcc[i])/((double)T) << ",";
            file << 100.0*((double)this->fdc[i])/((double)T) << ",";
            file << 100.0*((double)this->dcc[i])/((double)T) << ",";
            file << 100.0*((double)this->ddc[i])/((double)T) << "\n";
        }
        file.close();
    }

    void print( const char sl ) noexcept
    {
        for( int i = 0 ; i < 3 ; i++ ) {
            std::cout << this->N << ",";
            std::cout << this->T << ",";
            std::cout << this->lbl[i] << ",";
            std::cout << sl << ",";
            std::cout << (this->B < this->N ? 'y' : 'n') << ",";
            std::cout.setf(ios::fixed, ios::floatfield); 
            std::cout.precision(2); 
            std::cout.width(6);
            std::cout << 100.0*((double)this->fcc[i])/((double)T) << ",";
            std::cout.width(6);
            std::cout << 100.0*((double)this->fdc[i])/((double)T) << ",";
            std::cout.width(6);
            std::cout << 100.0*((double)this->dcc[i])/((double)T) << ",";
            std::cout.width(6);
            std::cout << 100.0*((double)this->ddc[i])/((double)T) << "\n";
        }
    }

};
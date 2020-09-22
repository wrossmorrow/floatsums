
#include <fstream>

using namespace std;

class Results {

public: 

    char lbl[3] = {'+','-','?'};
    int fcc[3] = {0} , fdc[3]  = {0}, dcc[3]  = {0}, ddc[3] = {0};
    int N , T;
    bool B , F , D;

    Results( int N , int T , bool B , bool F , bool D ) : N{N},T{T},B{B},F{F},D{D} {}

    void write( const char * const fn , const char sl )
    {
        ofstream file;
        file.open( fn , ios::out );
        for( int i = 0 ; i < 3 ; i++ ) {
            file << this->N << ",";
            file << this->T << ",";
            file << this->lbl[i] << ",";
            file << sl << ",";
            file << (this->B ? 'y' : 'n') << ",";
            if( this->B ) {
                file << (this->F ? 'y' : 'n') << ",";
                file << (this->D ? 'y' : 'n') << ",";
            } else {
                file << "-,-,";
            }
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
            std::cout << (this->B ? 'y' : 'n') << ",";
            std::cout << (this->F ? 'y' : 'n') << ",";
            std::cout << (this->D ? 'y' : 'n') << ",";
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
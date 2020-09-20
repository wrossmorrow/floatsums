
int sign(); // a random sign, -1 or +1
double random_u( ); // a standard uniform value
int randint( int a , int b ); // a random integer between a and b (inclusive)

// a uniform random integer amount from 0 to M
long random_amount_unif( int M );

// binp ~ a bins - 1 length array of non-decreasing values in [0,1]
// binb ~ a bins + 1 length array of non-decreasing positive values 
// !! WARNING !! - conditions are not checked
long random_amount_bins( int bins , double * binp , double * binb );


import sys
from math import log2 , ceil , floor , sqrt

def limit( N ):
    K = floor(log2(log2(N)))
    V = 1.0
    for k in range(0,K+1): 
        V = V * ceil( N**( 1/(2**k) ) )
    return V

def blocksim(Nb, N): 
    if N <= Nb : return True;
    R , B = N % Nb , ceil(N/Nb)
    Nb = floor(sqrt(Nb))
    return ( False if Nb < 2 else blocksim(Nb,B) )

def bisect(Nb, Nl, Nu, bl=None, bu=None): 
    if Nu <  Nl: return bisect(Nb, Nu, Nl)
    if Nu <= Nl + 1: return Nl, Nl+1
    if bl is None or not bl: 
        bl = blocksim(Nb, Nl)
        while not bl: 
            Nl = Nl/2
            bl = blocksim(Nb, Nl)
    if bu is None or bu : 
        bu = blocksim(Nb, Nu)
        while bu: 
            Nu = 2*Nu
            bu = blocksim(Nb, Nu)
    # assured bl = True, bu = False
    Nm = floor((Nu+Nl)/2) # may be Nl, Nl+1, 
    bm = blocksim(Nb, Nm)
    if bm: 
        return bisect(Nb, Nm, Nu, bl=bm, bu=Nu)
    else: 
        return bisect(Nb, Nl, Nm, bl=bl, bu=Nm)

if __name__ == "__main__": 
    if len(sys.argv) == 1: 
        sys.exit(1)
    for i in range(1,len(sys.argv)): 
        Nb = int(sys.argv[i])
        bounds = bisect(Nb, Nb+1, 10*Nb)
        print( f"{Nb} :: limit, maybe: {limit(Nb)}" )
        print( f"{Nb} :: limit, bisect: {bounds} {blocksim(Nb, bounds[0])} {blocksim(Nb, bounds[1])}" )


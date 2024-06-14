#include "os.h"


void page_table_update(uint64_t pt,uint64_t vpn, uint64_t ppn){
    uint64_t i=0, curInd,mask = 0x1ff;
    mask <<= (36+12); /*Now mask is 9 1's followed by 36+12 0's*/

    vpn &= 0x1ffffffffffffff; /*Zero out 7 MSB which are sign ext.*/
    
    while(i<5){
        curInd = (vpn & mask) >> (9*(4-i) + 12); /*Select relevant bits for this pt layer*/
        


        mask >>= 9; /*Shift mask to "mask" the next level's bits*/
        i++;
    }
}


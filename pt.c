#include "os.h"

/**
 * According to assignment:
 * Each entry is 64 bits
 * Each pt node is 512=2^9 entries in size (4KB/64bit)
 * This means that each pt level needs to correspond with 9 bits from the vpn
 * This in turn implies that our pt needs to have 5 levels, because vpn is 45 bits.
 **/

#define OFFSET_SIZE 12
#define NINE_BITS 0x1ff

#define FORTYFIVE_BITS 0x1ffffffffffffff
#define ALL_BUT_LSB 0xfffffffffffffffe

void page_table_update(uint64_t pt,uint64_t vpn, uint64_t ppn){
    uint64_t *Vpt = (uint64_t *)phys_to_virt(pt<<OFFSET_SIZE); /*pt is only ppn so I make it a whole address*/
    
    uint64_t i=0, curInd,mask = NINE_BITS;
    mask <<= (45-9); /*Now mask is 9 1's followed by 36 0's*/
    
    while(i<5){
        curInd = (vpn & mask) >> (9*(4-i)); /*Select relevant bits for this pt layer*/
        
        if(i==4){ /*Last pt layer*/
            switch(ppn){
                case NO_MAPPING: /*"Deletion" of allocated frame*/
                    Vpt[curInd] &= ALL_BUT_LSB; /*Mark PTE invalid by zeroing out lsb*/
                    return;
                default: /*Insertion of frame into PT*/
                    Vpt[curInd] = ((ppn)<<OFFSET_SIZE) | 1; /*Insert ppn into right spot and mark it valid*/
                    return;
            }
        }

        /*This checks if the PTE is marked invalid - meaning it's not mapped*/
        if((Vpt[curInd]&1) == 0){
            if(ppn == NO_MAPPING){ /*vpn to delete isn't mapped*/
                return;
            }else{
                Vpt[curInd] = (alloc_page_frame()<<OFFSET_SIZE) | 1; /*Add a pt node and mark it valid*/
            }
        }


        Vpt = (uint64_t *)phys_to_virt(Vpt[curInd] & ALL_BUT_LSB); /*Go a layer down in pt*/
        mask >>= 9; /*Shift mask to "mask" the next level's bits*/
        i++;
    }
}

uint64_t page_table_query(uint64_t pt, uint64_t vpn){
    uint64_t *Vpt = (uint64_t *)phys_to_virt(pt<<OFFSET_SIZE); /*pt is only ppn so I make it a whole address*/

    uint64_t i=0, curInd,mask = NINE_BITS;
    mask <<= (45-9); /*Now mask is 9 1's followed by 36 0's*/
    
    while(i<5){
        curInd = (vpn & mask) >> (9*(4-i)); /*Select relevant bits for this pt layer*/

        if((Vpt[curInd] & 1) == 0){ /*If page is invalid it's not mapped*/
            return NO_MAPPING;
        }

        if(i==4){ /*Last pt layer*/
            return (Vpt[curInd] & FORTYFIVE_BITS) >> OFFSET_SIZE; /*If we got here, frame is valid so no need to check*/
        }

        Vpt = (uint64_t *)phys_to_virt(Vpt[curInd] & ALL_BUT_LSB); /*Go a layer down in pt*/
        mask >>= 9; /*Shift mask to "mask" the next level's bits*/
        i++;
    }

    return NO_MAPPING; /*Will not get here, just to calm down compiler*/
}
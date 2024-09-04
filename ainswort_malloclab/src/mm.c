#include "mm.h"        // prototypes of functions implemented in this file
#include "mm_list.h"   // "mm_list_..."  functions -- to manage explicit free list
#include "mm_block.h"  // "mm_block_..." functions -- to manage blocks on the heap
#include "memlib.h"    // mem_sbrk -- to extend the heap
#include <string.h>    // memcpy -- to copy regions of memory

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) > (y) ? (y) : (x))
//initial extend heap in innit? by how much
//cyber security and data science
//lecture online?
//is my implementation look good so far?? and tips on what to do from ehre
/**
 * Mark a block as free, coalesce with contiguous free blocks on the heap, add
 * the coalesced block to the free list.
 *
 * @param bp address of the block to mark as free
 * @return the address of the coalesced block
 */
static BlockHeader *free_coalesce(BlockHeader *bp) {

    // mark block as free
    int size = mm_block_size(bp);
    mm_block_set_header(bp, size, 0);
    mm_block_set_footer(bp, size, 0);

    // check whether contiguous blocks are allocated
    int prev_alloc = mm_block_allocated(mm_block_prev(bp));
    int next_alloc = mm_block_allocated(mm_block_next(bp));
    BlockHeader *nfp = mm_block_next(bp);
    if (prev_alloc && next_alloc) {
        // TODO: add bp to free list
        mm_list_prepend(bp);
        return bp;

    } else if (prev_alloc && !next_alloc) {
        // TODO: remove next block from free list
        // TODO: add bp to free list
        // TODO: coalesce with next block
        // mm_block_next(BlockHeader *bp)
        mm_list_remove(nfp);
        int s2 = mm_block_size(nfp);
        int s1 = mm_block_size(bp);
        mm_block_set_header(bp, (s1+s2), 0);
        mm_block_set_footer(nfp, (s1+s2), 0);
        mm_list_append(bp);
        return bp;

    } else if (!prev_alloc && next_alloc) {
        // TODO: coalesce with previous block
        int s2 = mm_block_size(mm_block_prev(bp));
        int s1 = mm_block_size(bp);
        mm_block_set_header(mm_block_prev(bp), (s1+s2), 0);
        mm_block_set_footer(bp, (s1+s2), 0);
        return mm_block_prev(bp);

    } else {
        // TODO: remove next block from free list
        // TODO: coalesce with previous and next block
        mm_list_remove(nfp);
        int s2 = mm_block_size(nfp);
        int s1 = mm_block_size(bp);
        int s3 = mm_block_size(mm_block_prev(bp));
        mm_block_set_header(mm_block_prev(bp), (s1+s2+s3), 0);
        mm_block_set_footer(nfp, (s1+s2+s3), 0);
        return mm_block_prev(bp);
    }
}

/**
 * Allocate a free block of `size` byte (multiple of 8) on the heap.
 *
 * @param size number of bytes to allocate (a multiple of 8)
 * @return pointer to the header of the allocated block
 */
static BlockHeader *extend_heap(int size) {

    // bp points to the beginning of the new block
    char *bp = mem_sbrk(size);
    if ((long)bp == -1)
        return NULL;

    // write header over old epilogue, then the footer
    BlockHeader *old_epilogue = (BlockHeader *)bp - 1;
    mm_block_set_header(old_epilogue, size, 0);
    mm_block_set_footer(old_epilogue, size, 0);

    // write new epilogue
    mm_block_set_header(mm_block_next(old_epilogue), 0, 1);

    // merge new block with previous one if possible
    return free_coalesce(old_epilogue);
}

int mm_init(void) {

    // init list of free blocks
    mm_list_init();

    // create empty heap of 4 x 4-byte words
    char *new_region = mem_sbrk(16);
    if ((long)new_region == -1)
        return -1;

    heap_blocks = (BlockHeader *)new_region;
    mm_block_set_header(heap_blocks, 0, 0);      // skip 4 bytes for alignment
    mm_block_set_header(heap_blocks + 1, 8, 1);  // allocate a block of 8 bytes as prologue
    mm_block_set_footer(heap_blocks + 1, 8, 1);
    mm_block_set_header(heap_blocks + 3, 0, 1);  // epilogue (size 0, allocated)
    heap_blocks += 1;                            // point to the prologue header

    // TODO: extend heap with an initial heap size
    extend_heap(512);
    return 0;
}

void mm_free(void *bp) {
    // TODO: move back 4 bytes to find the block header, then free block
    bp = (char *)bp - 4;
    bp = free_coalesce(bp);
    //marek as free and possibly merge, this is pointer to payload, so find block header then free with free coales
}

/**
 * Find a free block with size greater or equal to `size`.
 *
 * @param size minimum size of the free block
 * @return pointer to the header of a free block or `NULL` if free blocks are
 *         all smaller than `size`.
 */
static BlockHeader *find_fit(int size) {
    // TODO: implement
    BlockHeader *nfp = mm_list_headp;
    while(nfp != NULL){
        int s1 = mm_block_size(nfp);
        if(s1 >= size)return nfp;
        nfp = mm_list_next(nfp);
    }
    return NULL;
}

/**
 * Allocate a block of `size` bytes inside the given free block `bp`.
 *
 * @param bp pointer to the header of a free block of at least `size` bytes
 * @param size bytes to assign as an allocated block (multiple of 8)
 * @return pointer to the header of the allocated block
 */
static BlockHeader *place(BlockHeader *bp, int size) {
    // TODO: if current size is greater, use part and add rest to free list
    // TODO: return pointer to header of allocated block
    int s = mm_block_size(bp);
    //set header and footer of this block to correct sizem, make a mew block for restt and append
    if(s > (size+8)){
        mm_block_set_header(bp, size, 1);
        mm_block_set_footer(bp, size, 1);
        BlockHeader *newb = mm_block_next(bp);//next bllock could alredaay be allocated
        s = s-size;
       // if(s % 8 != 0)s = s + (s%8);
        mm_block_set_header(newb, s, 0);
        mm_block_set_footer(newb, s, 0);
        mm_list_prepend(newb);
    }else{
        mm_block_set_header(bp, s, 1);
        mm_block_set_footer(bp, s, 1);
    }
    mm_list_remove(bp);
    return bp;
}
/**k
 * Compute the required block size (including space for header/footer) from the
 * requested payload size.test_place_small_leftover
 *
 * @param payload_size requested payload size
 * @return a block size including header/footer that is a multiple of 8
 */
static int required_block_size(int payload_size) {
    payload_size += 8;                    // add 8 for for header/footer
    return ((payload_size + 7) / 8) * 8;  // round up to multiple of 8
}

void *mm_malloc(size_t size) {
    // ignore spurious requests
    if (size == 0)
        return NULL;

    int required_size = required_block_size(size);
    // TODO: find a free block(find fit) or extend heap(if dont find block)
    // TODO: allocate and return pointer to payload
    //split block inm place
    BlockHeader *nfp = find_fit(required_size);
    if(nfp == NULL){
        extend_heap(required_size);
        nfp = find_fit(required_size);
    }
    nfp = place(nfp,required_size);
    return mm_block_payload_addr(nfp);
}

void *mm_realloc(void *ptr, size_t size) {

    if (ptr == NULL) {
        // equivalent to malloc
        return mm_malloc(size);

    } else if (size == 0) {
        // equivalent to free
        mm_free(ptr);
        return NULL;

    } else {
        size_t s = mm_block_size(ptr-4);
        BlockHeader *nfp = mm_block_next(ptr-4);
        //check prev and one after
        // TODO: reallocate, reusing current block if possible
        // TODO: copy data over new block with memcpy
        // TODO: return pointer to payload of new block
        //extend
        //extend with next being free
        //shrink
        //if enough space add to free list
        //could extend to prev or next
        int req = required_block_size(size);
        if(req<=s){
            return ptr;
        }else{
            int next_alloc = mm_block_allocated(mm_block_next(ptr-4));
            int prev_alloc = mm_block_allocated(mm_block_prev(ptr-4));

            if(!next_alloc){
                //remove alsp
                size_t s2 = mm_block_size(nfp) + s;
                if(req<s2){
                    //remove from free
                    mm_list_remove(nfp);
                    mm_block_set_header(ptr-4, s2, 1);
                    mm_block_set_footer(nfp, s2, 1);
                    return ptr;
                }
            if(!prev_alloc){
                
            }
        //if blocksize>req size extend
        //get required size, if size > connected size ts the extend
        /*
        if(size>(s-16)){//extend, leave new data unint
            //extends
            int next_alloc = mm_block_allocated(mm_block_next(ptr-4));
            int prev_alloc = mm_block_allocated(mm_block_prev(ptr-4));

            if(!next_alloc){
                BlockHeader *nfp = mm_block_next(ptr-4);
                mm_list_remove(nfp);
                int s2 = mm_block_size(nfp);
                int ts = (s+s2) - (size+8);
                if(ts>8){
                    mm_block_set_header(ptr-4, size+8, 1);
                    mm_block_set_footer(ptr-4, size+8, 1);
                    nfp = mm_block_next(ptr-4);
                    mm_block_set_footer(nfp, ts, 0);//may set these to 0
                    mm_block_set_header(nfp, ts, 0);//try prepend
                    mm_list_append(nfp);
                }else{
                    mm_block_set_header(ptr-4, s+s2, 1);
                    mm_block_set_footer(ptr-4, s+s2, 1);
                }
                /*prend one seize and append other
                mm_block_set_header(ptr-4, (s+s2), 1);
                mm_block_set_footer(ptr-4, (s+s2), 1);
                mm_block_set_footer(nfp, (s+s2), 1);//may set these to 0
                mm_block_set_header(nfp, (s+s2), 1);
               // nfp = place(ptr-4,size);
                //set rest of block to free if possible so place or coalese

                //prob wont have to copy because u leave new data unint
                //memcpy(nfp, ptr, MIN(size, (unsigned)mm_block_size(ptr-4) - 8));
                //cpy mem, maybe wont evn have to copy mem
                return ptr;
                */
            }//else if(!prev_alloc){
                /*
                //nfp = place(ptr-4,size);causes seg fault
                //will have to move mem, then possibly realloc to free
               // memmove(nfp, ptr-4, size);
                BlockHeader *nfp = mm_block_prev(ptr-4);
                int s2 = mm_block_size(nfp);
                //memmove(nfp+1, ptr, size);//nfp +1 bc payload??
                 //memmove(mm_block_payload_addr(nfp), ptr, size);
                int ts = (s+s2) - (size+8);
                if(ts>=8){
                    mm_list_remove(nfp);
                   // BlockHeader *newp = mm_block_next(nfp);   
                    mm_block_set_header(nfp, size+8, 1);
                    mm_block_set_footer(nfp, size+8, 1);
                    memmove(mm_block_payload_addr(nfp), ptr, s-8);
                    BlockHeader *newp = mm_block_next(nfp); 
                    mm_block_set_header(newp, ts, 0);
                    mm_block_set_footer(newp, ts, 0);//causes seg faults
                    mm_list_append(newp);
                    //mm_list_append(newp);
                }else if(size<=(s+s2)){
                    mm_list_remove(nfp);
                    mm_block_set_header(nfp, (s+s2), 1);
                    mm_block_set_footer(ptr-4, (s+s2), 1);//causes seg faults
                    memmove(mm_block_payload_addr(nfp), ptr, s-8);
                }else{
                    //realloc with first available block of size
                    void *new_ptr = mm_malloc(size);
                    memcpy(new_ptr, ptr, MIN(size, (unsigned)mm_block_size(ptr-4) - 8));
                    mm_free(ptr);
                    return new_ptr;
                }
                return mm_block_payload_addr(nfp);
                */
                
            
        }//else{//keep og data disregard cutoff
            //may have to use req size
            //if you have less than 16 bytes dont put in free list
            //would u still wanna move back if prev block is not allocatwd
            //free coalese
            /*
            if(s > (size+8)){//do we want the plus 8 here//check next
                //like shrink forward or back
                mm_block_set_header(ptr-4, size+8, 1);
                mm_block_set_footer(ptr-4, size+8, 1);
                //nexy block could be alloced
                BlockHeader *newb = mm_block_next(ptr-4);
                free_coalesce(newb);
                /*
                s = s-size-8;
                mm_block_set_header(newb, s, 0);
                mm_block_set_footer(newb, s, 0);
                //eiher append or coalese here
                mm_list_append(newb);
               // free_coalesce(newb);//free coalse for next bloc
            }
            return ptr;
            */
               
       //}

        // TODO: remove this naive implementation
        void *new_ptr = mm_malloc(size);
        memcpy(new_ptr, ptr, MIN(size, (unsigned)mm_block_size(ptr-4) - 8));
        mm_free(ptr);
        return new_ptr;
       
    }
}
//unused parts of free list 

//wat does it mean or wat is caus mm_realloc did not preserve data from old block
//wat is size in realloc, kust payload size??
//size is just payload
//memmov, is overlap if you extend to prev
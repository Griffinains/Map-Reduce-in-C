#include <mm_list.h>  // prototypes of functions implemented in this file
#include <unistd.h>   // NULL

BlockHeader *mm_list_headp;
BlockHeader *mm_list_tailp;

/**
 * Initializes to an empty list.
 */
void mm_list_init() {
    mm_list_headp = NULL;
    mm_list_tailp = NULL;
}

/**
 * In addition to the block header with size/allocated bit, a free block has
 * pointers to the headers of the previous and next blocks on the free list.
 *
 * Pointers use 4 bytes because this project is compiled with -m32.
 * Check Figure 9.48(b) in the textbook.
 */
typedef struct {
    BlockHeader header;
    BlockHeader *prev_free;
    BlockHeader *next_free;
} FreeBlockHeader;

/**
 * Find the header address of the previous **free** block on the **free list**.
 *
 * @param bp address of a block header (it must be a free block)
 * @return address of the header of the previous free block on the list
 */
BlockHeader *mm_list_prev(BlockHeader *bp) {
    FreeBlockHeader *fp = (FreeBlockHeader *)bp;
    return fp->prev_free;
}

/**
 * Find the header address of the next **free** block on the **free list**.
 *
 * @param bp address of a block header (it must be a free block)
 * @return address of the header of the next free block on the list
 */
BlockHeader *mm_list_next(BlockHeader *bp) {
    FreeBlockHeader *fp = (FreeBlockHeader *)bp;
    return fp->next_free;
}

/**
 * Set the pointer to the previous **free** block.
 *
 * @param bp address of a free block header
 * @param prev address of the header of the previous free block (to be set)
 */
static void mm_list_prev_set(BlockHeader *bp, BlockHeader *prev) {
    FreeBlockHeader *fp = (FreeBlockHeader *)bp;
    fp->prev_free = prev;
}


/**
 * Set the pointer to the next **free** block.
 *
 * @param bp address of a free block header
 * @param next address of the header of the next free block (to be set)
 */
static void mm_list_next_set(BlockHeader *bp, BlockHeader *next) {
    FreeBlockHeader *fp = (FreeBlockHeader *)bp;
    fp->next_free = next;
}
//BlockHeader *mm_list_headp;
//BlockHeader *mm_list_tailp;
/**
 * Add a block at the beginning of the free list.
 *
 * @param bp address of the header of the block to add
 */
void mm_list_prepend(BlockHeader *bp) {
    // TODO: implement
    FreeBlockHeader *fp = (FreeBlockHeader *)bp;
    if(mm_list_headp == NULL){
        fp->next_free = NULL;
        mm_list_tailp = bp;
    }else{
        BlockHeader *nfp = mm_list_headp;
        fp->next_free = nfp;
        mm_list_prev_set(nfp, bp);
    }
        fp->prev_free = NULL;
        mm_list_headp = bp;

}

/**
 * Add a block at the end of the free list.
 *
 * @param bp address of the header of the block to add
 */
void mm_list_append(BlockHeader *bp) {
    // TODO: implement
    FreeBlockHeader *fp = (FreeBlockHeader *)bp;
    if(mm_list_tailp == NULL){
        fp->prev_free = NULL;
        mm_list_headp = bp;
    }else{
        //BlockHeader *nfp = mm_block_next(mm_list_tailp);
        BlockHeader *nfp = mm_list_tailp;
        fp->prev_free = nfp;
        mm_list_next_set(nfp,bp);
    }
    fp->next_free = NULL;
    mm_list_tailp = bp;
}

/**
 * Remove a block from the free list.
 *
 * @param bp address of the header of the block to remove
 */
void mm_list_remove(BlockHeader *bp) {
    // TODO: implement
    if(mm_list_headp != NULL){
        if(mm_list_headp == bp && mm_list_tailp == bp){
           mm_list_headp = NULL;
           mm_list_tailp = NULL;
        }else if(mm_list_headp == bp){
            BlockHeader *nfp = mm_list_next(mm_list_headp);
            mm_list_headp = nfp;
            mm_list_prev_set(nfp, NULL);
        }else if(mm_list_tailp == bp){
            BlockHeader *nfp = mm_list_prev(mm_list_tailp);
            mm_list_tailp = nfp;
            mm_list_next_set(nfp, NULL);
        } else {
            BlockHeader *nfp = mm_list_next(bp);
            BlockHeader *bfp = mm_list_prev(bp);
            mm_list_next_set(bfp, nfp);
            mm_list_prev_set(nfp, bfp);
            mm_list_next_set(bp, NULL);
            mm_list_prev_set(bp, NULL);
        }
    }
}

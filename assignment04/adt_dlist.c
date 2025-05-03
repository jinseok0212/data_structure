#include <stdlib.h> // malloc

#include "adt_dlist.h"

// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr){
    NODE *newnode = malloc(sizeof(NODE));
    if(newnode == NULL) return 0;

   newnode -> dataPtr = dataInPtr;

   if(pPre == NULL){
        newnode -> llink = NULL;
        newnode -> rlink = pList -> head;

    if(pList -> head != NULL){
        pList -> head -> llink = newnode;
    }
    pList -> head = newnode;
   }

   else{
        newnode -> rlink = pPre -> rlink;
        newnode -> llink = pPre;
        pPre -> rlink = newnode;

    if(newnode -> rlink != NULL){
        newnode -> rlink -> llink = newnode;
    }

    if(newnode -> rlink == NULL){
        pList -> rear = newnode;
       }
   }

   pList -> count++;

   return 1;

}

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr){
    *dataOutPtr = pLoc -> dataPtr;

    if(pPre == NULL){
        pList -> head = pLoc -> rlink;
        if(pList -> head != NULL){
            pList -> head -> llink = NULL;
        }
    }
    else{
        pPre -> rlink = pLoc -> rlink;
        
         if(pLoc -> rlink != NULL){
            pLoc -> rlink -> llink = pPre;
        }
        else{
            pList -> rear = pPre;
        }
    }
    free(pLoc);

    pList -> count--;
}

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu){
    NODE *cur = pList -> head;
    NODE *prev = NULL;



    while(cur != NULL){
       int find = pList -> compare(pArgu, cur -> dataPtr);

       if(find == 0){
        *pLoc = cur;
        *pPre = prev;
        return 1; 
       }
       else if(find < 0){
        break;
       }
       else{
        prev = cur;
        cur = cur -> rlink;
       }
    }
    
    *pLoc = cur;
    *pPre = prev;

    return 0;
    
}

////////////////////////////////////////////////////////////////////////////////
// function declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList( int (*compare)(const void *, const void *)){
    LIST *list = malloc(sizeof(LIST));
    if(list == NULL) return NULL;

    list -> count = 0;
    list -> head = NULL;
    list -> rear = NULL;

    list -> compare = compare;

    return list;

}

//  이름 리스트에 할당된 메모리를 해제 (head node, data node)
void destroyList( LIST *pList, void (*callback)(void *)){
    NODE *cur = pList -> head;
    NODE *prev = NULL;

    while(cur != NULL){
        prev = cur;
        callback(prev -> dataPtr);
        free(prev);
        cur = cur -> rlink;
    }

    free(pList);
}

// Inserts data into list
// callback은 이미 리스트에 존재하는 데이터를 발견했을 때 호출하는 함수
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
int addNode( LIST *pList, void *dataInPtr, void (*callback)(const void *)){
    NODE *pLoc = pList -> head;
    NODE *pPre = NULL;
    

    if(_search(pList, &pPre, &pLoc, dataInPtr) == 1){
        callback(pLoc -> dataPtr);
        
        return 2;
    }

    if(_search(pList, &pPre, &pLoc, dataInPtr) == 0){
        _insert(pList, pPre, dataInPtr);
        return 1;
    }

    else{
        return 0;
    }
}

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, void *keyPtr, void **dataOutPtr){
    NODE *pLoc = pList -> head;
    NODE *pPre= NULL;

    int find = _search(pList, &pPre, &pLoc, keyPtr);
    
    if(find == 0){
        return 0;
    }
    else{
        _delete(pList, pPre, pLoc, dataOutPtr);
        return 1;
    }

}

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchNode( LIST *pList, void *pArgu, void **dataOutPtr){
    NODE *pLoc = NULL;
    NODE *pPre = NULL;

    int find = _search(pList, &pPre, &pLoc, pArgu);

    if(find == 1){
        *dataOutPtr = pLoc -> dataPtr;
        return 1;
    }
    else{
        return 0;
    }
}

// returns number of nodes in list
int countList( LIST *pList){
    return pList -> count;
}

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList){
    if(pList -> count == 0){
        return 1;
    }
    else{
        return 0;
    }
}

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const void *)){
    NODE *cur = pList -> head;

    while(cur != NULL){
        callback(cur -> dataPtr);
        cur = cur -> rlink;
    }

}

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const void *)){
    NODE *cur = pList -> rear;

    while(cur != NULL){
        callback(cur -> dataPtr);
        cur = cur -> llink;
    }
}

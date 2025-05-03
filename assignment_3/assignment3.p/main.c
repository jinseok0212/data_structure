#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT            1
#define FORWARD_PRINT    2
#define BACKWARD_PRINT    3
#define SEARCH            4
#define DELETE            5
#define COUNT            6

// User structure type definition
// 단어 구조체
typedef struct {
    char    *word;        // 단어
    int        freq;        // 빈도
} tWord;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
    tWord        *dataPtr;
    struct node    *llink; // backward pointer
    struct node    *rlink; // forward pointer
} NODE;

typedef struct
{
    int        count;
    NODE    *head;
    NODE    *rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return    head node pointer
//             NULL if overflow
LIST *createList(void);

//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList( LIST *pList);
    

// Inserts data into list
// return    0 if overflow
//            1 if successful
//            2 if duplicated key (이미 저장된 단어는 빈도 증가)
int addNode( LIST *pList, tWord *dataInPtr);

// Removes data from list
//    return    0 not found
//            1 deleted
int removeNode( LIST *pList, tWord *keyPtr, tWord **dataOutPtr);

// interface to search function
//    pArgu    key being sought
//    dataOutPtr    contains found data
//    return    1 successful
//            0 not found
int searchNode( LIST *pList, tWord *pArgu, tWord **dataOutPtr);

// returns number of nodes in list
int countList( LIST *pList);

// returns    1 empty
//            0 list has data
int emptyList( LIST *pList);

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const tWord *));

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const tWord *));

// internal insert function
// inserts data into list
// for addNode function
// return    1 if successful
//             0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tWord *dataInPtr);

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tWord **dataOutPtr);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return    1 found
//             0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu);

////////////////////////////////////////////////////////////////////////////////
// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화// return    word structure pointer
// return    할당된 단어 구조체에 대한 pointer
//            NULL if overflow
tWord *createWord( char *word);

//  단어 구조체에 할당된 메모리를 해제
// for destroyList function
void destroyWord( tWord *pNode);

////////////////////////////////////////////////////////////////////////////////
// gets user's input
int get_action(void)
{
    char ch;
    scanf( "%c", &ch);
    ch = toupper( ch);
    switch( ch)
    {
        case 'Q':
            return QUIT;
        case 'P':
            return FORWARD_PRINT;
        case 'B':
            return BACKWARD_PRINT;
        case 'S':
            return SEARCH;
        case 'D':
            return DELETE;
        case 'C':
            return COUNT;
    }
    return 0; // undefined action
}

// compares two words in word structures
// for _search function
// 정렬 기준 : 단어
int compare_by_word( const void *n1, const void *n2)
{
    tWord *p1 = (tWord *)n1;
    tWord *p2 = (tWord *)n2;
    
    return strcmp( p1->word, p2->word);
}

// prints contents of word structure
// for traverseList and traverseListR functions
void print_word(const tWord *dataPtr)
{
    printf( "%s\t%d\n", dataPtr->word, dataPtr->freq);
}

// gets user's input
void input_word(char *word)
{
    fprintf( stderr, "Input a word to find: ");
    fscanf( stdin, "%s", word);
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
    LIST *list;
    
    char word[100];
    tWord *pWord;
    int ret;
    FILE *fp;
    
    if (argc != 2){
        fprintf( stderr, "usage: %s FILE\n", argv[0]);
        return 1;
    }
    
    fp = fopen( argv[1], "rt");
    if (!fp)
    {
        fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
        return 2;
    }
    
    // creates an empty list
    list = createList();
    if (!list)
    {
        printf( "Cannot create list\n");
        return 100;
    }
    
    while(fscanf( fp, "%s", word) != EOF)
    {
        pWord = createWord( word);
        
        // 이미 저장된 단어는 빈도 증가
        ret = addNode( list, pWord);
        
        if (ret == 0 || ret == 2) // failure or duplicated
        {
            destroyWord( pWord);
        }
    }
    
    fclose( fp);
    
    fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
    
    while (1)
    {
        tWord *ptr;
        int action = get_action();
        
        switch( action)
        {
            case QUIT:
                destroyList( list);
                return 0;
            
            case FORWARD_PRINT:
                traverseList( list, print_word);
                break;
            
            case BACKWARD_PRINT:
                traverseListR( list, print_word);
                break;
            
            case SEARCH:
                input_word(word);
                
                pWord = createWord( word);

                if (searchNode( list, pWord, &ptr)) print_word( ptr);
                else fprintf( stdout, "%s not found\n", word);
                
                destroyWord( pWord);
                break;
                
            case DELETE:
                input_word(word);
                
                pWord = createWord( word);

                if (removeNode( list, pWord, &ptr))
                {
                    fprintf( stdout, "%s\t%d deleted\n", ptr->word, ptr->freq);
                    destroyWord( ptr);
                }
                else fprintf( stdout, "%s not found\n", word);
                
                destroyWord( pWord);
                break;
            
            case COUNT:
                fprintf( stdout, "%d\n", countList( list));
                break;
        }
        
        if (action) fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
    }
    return 0;
}



LIST *createList(void){
    LIST *pList = malloc(sizeof(LIST));
    
    pList -> count = 0;
    pList -> head = NULL;
    pList -> rear = NULL;
    
    return pList;
}

//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList( LIST *pList){
    NODE *cur = pList -> head;
    
    while(cur){
        
        NODE *next = cur -> rlink;
        destroyWord(cur -> dataPtr);
        free(cur);
        cur =  next;
    }
    
    free(pList);
    
}
    
// Inserts data into list
// return    0 if overflow
//            1 if successful
//            2 if duplicated key (이미 저장된 단어는 빈도 증가)
int addNode(LIST *pList, tWord *dataInPtr) {
    NODE *pPre = NULL;
    NODE *pLoc = NULL;

    int found = _search(pList, &pPre, &pLoc, dataInPtr);

    if (found) {
        pLoc->dataPtr->freq++;
        
        return 2;
    }

    // 중복 아니면 삽입 시도
    if (!(_insert(pList, pPre, dataInPtr))) {
        return 0;
    }

    pList->count++;
    return 1;
}


// Removes data from list
//    return    0 not found
//            1 deleted
int removeNode( LIST *pList, tWord *keyPtr, tWord **dataOutPtr){
    NODE *pPre = NULL;
    NODE *pLoc = NULL;
    
    int found = _search(pList, &pPre, &pLoc, keyPtr);
    
    if(!found) return 0;
    
    _delete(pList, pPre, pLoc, dataOutPtr);
    pList -> count--;
    
    return 1;
}

// interface to search function
//    pArgu    key being sought
//    dataOutPtr    contains found data
//    return    1 successful
//            0 not found
int searchNode( LIST *pList, tWord *pArgu, tWord **dataOutPtr){
    NODE *pPre = NULL;
    NODE *pLoc = NULL;
    
    int found = _search(pList, &pPre, &pLoc, pArgu);
    
    if(found){
        *dataOutPtr = pLoc -> dataPtr;
    }
    
    return found;
}

// returns number of nodes in list
int countList( LIST *pList){
    
    return pList -> count;
}

// returns    1 empty
//            0 list has data
int emptyList( LIST *pList){
    
    int found = countList(pList);
    
    if(found == 0){
        return 1;
    }
    else{
        return 0;
    }
    
}

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const tWord *)){
    
    NODE *cur = pList -> head;
    
    while(cur){
        callback(cur -> dataPtr);
        cur = cur -> rlink;
    }
}

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const tWord *)){
    
    NODE *cur = pList -> rear;
    
    while(cur){
        callback(cur -> dataPtr);
        cur = cur -> llink;
    }
}

// internal insert function
// inserts data into list
// for addNode function
// return    1 if successful
//             0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tWord *dataInPtr){
    
    NODE *newNode = malloc(sizeof(NODE));
    if(newNode == NULL) return 0;
    
    
    newNode -> dataPtr = dataInPtr;
    newNode -> llink = NULL;
    newNode -> rlink = NULL;
    
    if(pPre == NULL){
        newNode -> rlink = pList -> head;
        
        if(pList -> head != NULL){
            pList -> head -> llink = newNode;
        }
        
        pList -> head = newNode;
        
        if(pList -> rear == NULL){
            pList -> rear = newNode;
        }
        return 1;
    }
    else{
        newNode->rlink = pPre->rlink;
        newNode->llink = pPre;
        
        if(pPre -> rlink != NULL){
            pPre -> rlink -> llink = newNode;
        }
        
        pPre -> rlink =newNode;
        
        if(newNode -> rlink == NULL){
            pList -> rear = newNode;
        }
        return 1;
    }
}

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tWord **dataOutPtr){
    
    *dataOutPtr = pLoc -> dataPtr;
    
    if(pPre == NULL){  // a <-> b <-> c    a 삭제할거임
       
        pList -> head = pLoc -> rlink;  // 갱신했고
        
        if(pList -> head != NULL){
            pList -> head -> llink = NULL;
        }
        else{
            pList -> rear = NULL;
        }
        
        free(pLoc);
    }
    
    else{
        if(pPre != NULL && pLoc -> rlink == NULL){// pLoc가 리스트의 끝
            pPre -> rlink = NULL;
            pList -> rear = pPre;
            
            free(pLoc);
        }
        
        if(pPre != NULL && pLoc -> rlink != NULL){// 중간 단어.  a <-> b <-> c   b를 제거할거임
            
            pPre -> rlink = pLoc -> rlink; // ->
            pLoc -> rlink -> llink = pPre; // <-
            
            free(pLoc);
            
        }
    }
}

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return    1 found
//             0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu){
    
    *pPre = NULL;
    *pLoc = pList -> head;
    
    while(*pLoc != NULL && compare_by_word((*pLoc) -> dataPtr, pArgu) < 0){
        *pPre = *pLoc;
        *pLoc = (*pLoc) -> rlink;
    }
    
    if(*pLoc != NULL && compare_by_word((*pLoc) -> dataPtr, pArgu) == 0){
        return 1;
    }
    
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화// return    word structure pointer
// return    할당된 단어 구조체에 대한 pointer
//            NULL if overflow


tWord *createWord( char *word){
    tWord *pWord = malloc(sizeof(tWord));
    if(pWord == NULL) return NULL;
    
    pWord -> word = strdup(word);
    
    if(pWord -> word == NULL){
        free(pWord);
        return NULL;
    }
    
    pWord -> freq = 1;
    
    return pWord;
    
}

//  단어 구조체에 할당된 메모리를 해제
// for destroyList function
void destroyWord( tWord *pNode){
    if(pNode == NULL) return;
    
    free(pNode -> word);
    free(pNode);
}


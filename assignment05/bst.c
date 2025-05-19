#include <stdlib.h> // malloc
#include <stdio.h>

#include "bst.h"


// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *BST_Create( int (*compare)(const void *, const void *)){
    TREE *newtree = (TREE *)malloc(sizeof(TREE));
    if(newtree == NULL) return NULL;

    newtree -> count = 0;
    newtree -> root = NULL;
    newtree -> compare = compare;

    return newtree;
}

/* Deletes all data in tree and recycles memory
*/
void BST_Destroy( TREE *pTree, void (*callback)(void *)){
	_destroy(pTree -> root, callback);
	pTree -> root = NULL;
	pTree -> count = 0;
}

/* Inserts new data into the tree
	callback은 이미 트리에 존재하는 데이터를 발견했을 때 호출하는 함수
	return	0 overflow
			1 success
			2 if duplicated key
*/
int BST_Insert( TREE *pTree, void *dataInPtr, void (*callback)(void *)){
	NODE *newnode = _makeNode(dataInPtr);
	if(newnode == NULL) return 0;

	if(pTree -> root == NULL){
		pTree -> root = newnode;
		pTree -> count++;
		return 1;
	}

	int result = _insert(pTree -> root, newnode, pTree -> compare, callback);

	if(result == 1){
		pTree -> count++;
		return 1;
	}
	else{
		return 2; 
	}
	
}

/* Deletes a node with keyPtr from the tree
	return	address of data of the node containing the key
			NULL not found
*/
void *BST_Delete( TREE *pTree, void *keyPtr){
	void *dataOutPtr = NULL;
	pTree -> root = _delete(pTree -> root, keyPtr, &dataOutPtr, pTree -> compare);

	if(dataOutPtr != NULL){
		pTree -> count--;
	}
	return dataOutPtr;
}

/* Retrieve tree for the node containing the requested key (keyPtr)
	return	address of data of the node containing the key
			NULL not found
*/
void *BST_Search( TREE *pTree, void *keyPtr){

	if(pTree -> root == NULL) return NULL;

	NODE *found = _search(pTree -> root, keyPtr, pTree -> compare);

	if(found == NULL) return NULL;
	else return found -> dataPtr;
}

/* prints tree using inorder traversal
*/
void BST_Traverse( TREE *pTree, void (*callback)(const void *));

/* prints tree using right-to-left inorder traversal
*/
void BST_TraverseR( TREE *pTree, void (*callback)(const void *));

/* Print tree using right-to-left inorder traversal with level
*/
void printTree( TREE *pTree, void (*callback)(const void *));

/* returns number of nodes in tree
*/
int BST_Count( TREE *pTree);


// internal functions (not mandatory)
// used in BST_Insert
static int _insert( NODE *root, NODE *newPtr, int (*compare)(const void *, const void *), void (*callback)(void *)){
	int cmp = compare(newPtr -> dataPtr, root -> dataPtr);

	if(cmp < 0){
		if(root -> left == NULL){
			root -> left = newPtr;
		}
		else{
			return _insert(root -> left, newPtr, compare, callback);
		}
	}
	else if(cmp > 0){
		if(root -> right == NULL){
			root -> right = newPtr;
		}
		else{
			return _insert(root -> right, newPtr, compare, callback);
		}
	}
	else{
		callback(newPtr -> dataPtr);
		return 2;
	}
	return 1;
}

// used in BST_Insert
static NODE *_makeNode( void *dataInPtr){
	NODE *newnode = (NODE *)malloc(sizeof(NODE));
	if(newnode == NULL) return NULL;

	newnode -> dataPtr = dataInPtr;
	newnode -> left = NULL;
	newnode -> right = NULL;

	return newnode;
}

// used in BST_Destroy
static void _destroy( NODE *root, void (*callback)(void *)){
    if(root == NULL) return;
    _destroy(root -> left, callback);
	_destroy(root -> right, callback);
	callback(root -> dataPtr);
	free(root);
}

// used in BST_Delete
// return 	pointer to root
static NODE *_delete( NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *)){
	if(root == NULL) return NULL;

	int cmp = compare(keyPtr, root -> dataPtr);

	if(cmp < 0){
		root -> left = _delete(root -> left, keyPtr, dataOutPtr, compare);
	}
	else if(cmp > 0){
		root -> right = _delete(root -> right, keyPtr, dataOutPtr, compare);
	}
	else{
		*dataOutPtr = root -> dataPtr;
		if(root -> left == NULL && root -> right == NULL){
			free(root);
			return NULL;
		}
		else if(root -> right == NULL){
			NODE *temp = root -> left;
			free(root);
			return temp;
		}

		else if(root -> left == NULL){
			NODE *temp = root -> right;
			free(root);
			return temp;
		}
		
		else{
			NODE *minright = root -> right;
			while(minright -> left != NULL){
				minright = minright -> left;
			}
			root -> dataPtr = minright -> dataPtr;
			root -> right = _delete(root -> left, minright -> dataPtr, dataOutPtr, compare);
		}
	}
	return root;
}

// used in BST_Search
// Retrieve node containing the requested key
// return	address of the node containing the key
//			NULL not found
static NODE *_search( NODE *root, void *keyPtr, int (*compare)(const void *, const void *)){
	if(root == NULL) return NULL;

	int cmp = compare(keyPtr, root -> dataPtr );

	if(cmp == 0){
		return root;
	}
	else if(cmp < 0){
		return _search(root -> left, keyPtr, compare);
	}
	else{
		return _search(root -> right, keyPtr, compare);
	}
}

// used in BST_Traverse
static void _traverse( NODE *root, void (*callback)(const void *));

// used in BST_TraverseR
static void _traverseR( NODE *root, void (*callback)(const void *));

// used in printTree
static void _inorder_print( NODE *root, int level, void (*callback)(const void *));


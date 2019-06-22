
/************************************************************************ Includes *******************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"

/************************************************************************ Defines ********************************************************************************/

/************************************* Return Codes ***************************/

#define RETURN_CODE_SUCCESS 0
#define RETURN_CODE_MALLOC_ERROR 1
#define RETURN_CODE_MUTEX_INIT_ERROR 2

/************************************************************************ Structs ********************************************************************************/

/************************************* List ***********************************/

struct list {

  struct node* mHead;

  pthread_mutex_t mLock;

};

/************************************* Node ***********************************/

struct node {

  int mValue;

  struct node* mNext;

  pthread_mutex_t mLock;

};

/************************************************************************ Private Methods ************************************************************************/

/************************************* Print Node *****************************/

// Method: print_node
// Input:
// Output:
// Return:
void print_node(node* node)
{
  // DO NOT DELETE
  if(node)
  {
    printf("%d ", node->mValue);
  }
}

/************************************************************************ Public Methods ************************************************************************/

/************************************* Create List ****************************/

// Method: create_list
// Input: None
// Output: None
// Return: pointer to new list
list* create_list()
{

  list *newList = (list*)malloc(sizeof(list));
  if(newList == NULL)
  {
    printf("create_list: malloc error !!!\n");
    exit(RETURN_CODE_MALLOC_ERROR);
  }

  newList->mHead = NULL;

  int returnErrorCode = pthread_mutex_init(&newList->mLock, NULL);
  if(returnErrorCode != RETURN_CODE_SUCCESS)
  {
    printf("create_list: mutex init error !!!\n");
    delete_list(newList);
    exit(RETURN_CODE_MUTEX_INIT_ERROR);
  }

  return newList;
}

/************************************* Delete List ****************************/

// Method: delete_list
// Input: list
// Output: None
// Return: None
void delete_list(list* list)
{

  if(list == NULL)
  {
    printf("delete_list: list NULL error !!!\n");
    return;
  }

  pthread_mutex_lock(&list->mLock);

  while(list->mHead)
  {

    node *currentNode = list->mHead;
    node *nextNode = currentNode->mNext;

    int returnErrorCode = pthread_mutex_destroy(&currentNode->mLock);
    if(returnErrorCode != RETURN_CODE_SUCCESS)
    {
      printf("delete_list: node mutex destroy error !!!\n");
    }

    free(currentNode);

    list->mHead = nextNode;

  }

  pthread_mutex_unlock(&list->mLock);

  int returnErrorCode = pthread_mutex_destroy(&list->mLock);
  if(returnErrorCode != RETURN_CODE_SUCCESS)
  {
    printf("delete_list: list mutex destroy error !!!\n");
  }

  free(list);

}

/************************************* Insert Value ***************************/

// Method: insert_value
// Input: list, value
// Output: None
// Return: None
void insert_value(list* list, int value)
{

  if(list == NULL)
  {
    printf("insert_value: list NULL error !!!\n");
    return;
  }

  // Create new node

  node *newNode = (node *)malloc(sizeof(node));
  if(newNode == NULL)
  {
      printf("insert_value: malloc error !!!\n");
      exit(RETURN_CODE_MALLOC_ERROR);
  }

  newNode->mValue = value;

  int returnErrorCode = pthread_mutex_init(&newNode->mLock, NULL);
  if(returnErrorCode != RETURN_CODE_SUCCESS)
  {
      printf("insert_value: mutex init error !!!\n");
      free(newNode);
      exit(RETURN_CODE_MUTEX_INIT_ERROR);
  }

  // Add new node to list

  node *currentNode;

  pthread_mutex_lock(&list->mLock);

  if(list->mHead == NULL)
  {

      newNode->mNext = NULL;
      list->mHead = newNode;

      pthread_mutex_unlock(&list->mLock);
      return;

  } else if (value < list->mHead->mValue) {

//    pthread_mutex_lock(&list->mHead->mLock);

    newNode->mNext = list->mHead;
    list->mHead = newNode;

//    pthread_mutex_unlock(&list->mHead->mLock);

    pthread_mutex_unlock(&list->mLock);
    return;

  } else {

    currentNode = list->mHead;
    pthread_mutex_lock(&currentNode->mLock);

    while(currentNode->mNext != NULL && currentNode->mNext->mValue < value)
    {
        pthread_mutex_unlock(&currentNode->mLock);

        currentNode = currentNode->mNext;
        pthread_mutex_lock(&currentNode->mLock);
    }

    newNode->mNext = currentNode->mNext;
    currentNode->mNext = newNode;
    pthread_mutex_unlock(&currentNode->mLock);

    pthread_mutex_unlock(&list->mLock);

  }

}

/************************************* Remove Value ***************************/

// Method: remove_value
// Input:
// Output:
// Return:
void remove_value(list* list, int value)
{
  // add code here
}

/************************************* Print List *****************************/

// Method: print_list
// Input: list
// Output: None
// Return: None
void print_list(list* list)
{

  if(list == NULL)
  {
    printf("print_list: list NULL error !!!\n");
    return;
  }

  pthread_mutex_lock(&list->mLock);

  node *currentNode = list->mHead;

  while(currentNode)
  {

    pthread_mutex_lock(&currentNode->mLock);
    
    print_node(currentNode);
    
    pthread_mutex_unlock(&currentNode->mLock);

    currentNode = currentNode->mNext;

  }

  pthread_mutex_unlock(&list->mLock);

  printf("\n"); // DO NOT DELETE

}

/************************************* Count List *****************************/

// Method: count_list
// Input:
// Output:
// Return:
void count_list(list* list, int (*predicate)(int))
{
  int count = 0; // DO NOT DELETE

  // add code here

  printf("%d items were counted\n", count); // DO NOT DELETE
}

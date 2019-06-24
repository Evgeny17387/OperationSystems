
/************************************************************************ Includes *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <limits.h>

#include <pthread.h>

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
    printf("create_list: list malloc error !!!\n");
    exit(RETURN_CODE_MALLOC_ERROR);
  }

  // Create dummy node for convenience

  node *newNode = (node *)malloc(sizeof(node));
  if(newNode == NULL)
  {
      printf("create_list: dummy node malloc error !!!\n");
      free(newList);
      exit(RETURN_CODE_MALLOC_ERROR);
  }

  newNode->mValue = INT_MIN;

  int returnErrorCode = pthread_mutex_init(&newNode->mLock, NULL);
  if(returnErrorCode != RETURN_CODE_SUCCESS)
  {
      printf("create_list: mutex init error !!!\n");
      free(newList);
      free(newNode);
      exit(RETURN_CODE_MUTEX_INIT_ERROR);
  }

  newList->mHead = newNode;

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

  // Lock dummy node
  pthread_mutex_lock(&list->mHead->mLock);

  while(list->mHead->mNext)
  {

    node *currentNode = list->mHead->mNext;

    pthread_mutex_lock(&currentNode->mLock);

    list->mHead->mNext = currentNode->mNext;

    pthread_mutex_unlock(&currentNode->mLock);

    int returnErrorCode = pthread_mutex_destroy(&currentNode->mLock);
    if(returnErrorCode != RETURN_CODE_SUCCESS)
    {
      printf("delete_list: node mutex destroy error !!!\n");
    }

    free(currentNode);

  }

  pthread_mutex_unlock(&list->mHead->mLock);

  // Delete dummy node

  int returnErrorCode = pthread_mutex_destroy(&list->mHead->mLock);
  if(returnErrorCode != RETURN_CODE_SUCCESS)
  {
    printf("delete_list: dummy node mutex destroy error !!!\n");
  }

  free(list->mHead);

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

  node *currentNode = list->mHead;
  pthread_mutex_lock(&currentNode->mLock);

  node *nextNode = currentNode->mNext;

  while(nextNode != NULL)
  {

    pthread_mutex_lock(&nextNode->mLock);

    if (value < nextNode->mValue){

      newNode->mNext = nextNode;
      currentNode->mNext = newNode;

      pthread_mutex_unlock(&currentNode->mLock);
      pthread_mutex_unlock(&nextNode->mLock);

      return;

    }

    pthread_mutex_unlock(&currentNode->mLock);

    currentNode = nextNode;
    nextNode = nextNode->mNext;

  }

  newNode->mNext = nextNode;
  currentNode->mNext = newNode;

  pthread_mutex_unlock(&currentNode->mLock);

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

  node *currentNode = list->mHead;
  pthread_mutex_lock(&currentNode->mLock);

  node *nextNode = currentNode->mNext;

  while(nextNode != NULL)
  {

    pthread_mutex_lock(&nextNode->mLock);

    print_node(nextNode);

    pthread_mutex_unlock(&currentNode->mLock);

    currentNode = nextNode;
    nextNode = nextNode->mNext;

  }

  pthread_mutex_unlock(&currentNode->mLock);

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

/************************************* Stuck Node *****************************/

// Method: stuck_node
// Input:
// Output:
// Return:
void stuck_node(list* list, int value)
{

  node *currentNode = list->mHead->mNext;

  while(currentNode->mValue != value)
  {

    currentNode = currentNode->mNext;

  }

  pthread_mutex_lock(&currentNode->mLock);

}

/*
 * File:   DLinkedList.h
 */

#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include "list/IList.h"

#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
class DLinkedList : public IList<T> {
 public:
  class Node;         // Forward declaration
  class Iterator;     // Forward declaration
  class BWDIterator;  // Forward declaration

 protected:
  Node *head;
  Node *tail;
  int count;
  bool (*itemEqual)(T &lhs, T &rhs);
  void (*deleteUserData)(DLinkedList<T> *);

 public:
  DLinkedList(void (*deleteUserData)(DLinkedList<T> *) = 0,
              bool (*itemEqual)(T &, T &) = 0);
  DLinkedList(const DLinkedList<T> &list);
  DLinkedList<T> &operator=(const DLinkedList<T> &list);
  ~DLinkedList();

  // Inherit from IList: BEGIN
  void add(T e);
  void add(int index, T e);
  T removeAt(int index);
  bool removeItem(T item, void (*removeItemData)(T) = 0);
  bool empty();
  int size();
  void clear();
  T &get(int index);
  int indexOf(T item);
  bool contains(T item);
  string toString(string (*item2str)(T &) = 0);
  // Inherit from IList: END

  void println(string (*item2str)(T &) = 0) {
    cout << toString(item2str) << endl;
  }
  void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0) {
    this->deleteUserData = deleteUserData;
  }

  bool contains(T array[], int size) {
    int idx = 0;
    for (DLinkedList<T>::Iterator it = begin(); it != end(); it++) {
      if (!equals(*it, array[idx++], this->itemEqual)) return false;
    }
    return true;
  }

  Iterator begin() { return Iterator(this, true); }
  Iterator end() { return Iterator(this, false); }

  BWDIterator bbegin() { return BWDIterator(this, true); }
  BWDIterator bend() { return BWDIterator(this, false); }

 protected:
  void copyFrom(const DLinkedList<T> &list);
  void removeInternalData();
  Node *getPreviousNodeOf(int index);

  //! FUNTION STATIC
 public:
  static void free(DLinkedList<T> *list) {
    if (list == nullptr) return;
    typename DLinkedList<T>::Iterator it = list->begin();
    while (it != list->end()) {
      T item = *it;
      ++it;
      delete item;
    }
  }
  

 protected:
  static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &)) {
    if (itemEqual == 0)
      return lhs == rhs;
    else
      return itemEqual(lhs, rhs);
  }

 public:
  class Node {
   public:
    T data;
    Node *next;
    Node *prev;
    friend class DLinkedList<T>;

   public:
    Node(Node *next = 0, Node *prev = 0) {
      this->next = next;
      this->prev = prev;
    }
    Node(T data, Node *next = 0, Node *prev = 0) {
      this->data = data;
      this->next = next;
      this->prev = prev;
    }
  };

 public:
  class Iterator {
   private:
    DLinkedList<T> *pList;
    Node *pNode;

   public:
    Iterator(DLinkedList<T> *pList = 0, bool begin = true) {
      if (begin) {
        if (pList != 0)
          this->pNode = pList->head->next;
        else
          pNode = 0;
      } else {
        if (pList != 0)
          this->pNode = pList->tail;
        else
          pNode = 0;
      }
      this->pList = pList;
    }

    Iterator &operator=(const Iterator &iterator) {
      this->pNode = iterator.pNode;
      this->pList = iterator.pList;
      return *this;
    }
    void remove(void (*removeItemData)(T) = 0) {
      if (!pNode || pNode == pList->head || pNode == pList->tail) return;
      Node *pPrev = pNode->prev;
      pNode->prev->next = pNode->next;
      pNode->next->prev = pNode->prev;
      if (removeItemData != 0) removeItemData(pNode->data);
      delete pNode;
      pNode = pPrev;
      pList->count -= 1;
    }

    T &operator*() { return pNode->data; }
    bool operator!=(const Iterator &iterator) {
      return pNode != iterator.pNode;
    }
    // Prefix ++ overload
    Iterator &operator++() {
      pNode = pNode->next;
      return *this;
    }
    // Postfix ++ overload
    Iterator operator++(int) {
      Iterator iterator = *this;
      ++*this;
      return iterator;
    }
  };

  class BWDIterator {
    // TODO implement
    private:
    DLinkedList<T> *pList;
    Node *pNode;

    public:
    BWDIterator(DLinkedList<T> *pList = 0, bool begin = true) {
      if (begin) {
        if (pList != 0)
          this->pNode = pList->head->next;
        else
          pNode = 0;
      } else {
        if (pList != 0)
          this->pNode = pList->tail;
        else
          pNode = 0;
      }
      this->pList = pList;
    }
    BWDIterator &operator=(const BWDIterator &iterator) {
    this->pNode = iterator.pNode;
    this->pList = iterator.pList;
    return *this;
  }

  void remove(void (*removeItemData)(T)) {
    if (!pNode || pNode == pList->head || pNode == pList->tail) return;
    pNode->prev->next = pNode->next;
    pNode->next->prev = pNode->prev;
    Node *pPrev = pNode->prev;
    if (removeItemData != nullptr) removeItemData(pNode->data);
    delete pNode;
    pNode = pPrev;
    pList->count--;
  }

  T &operator*() { return pNode->data; }
  bool operator!=(const BWDIterator &iterator) { return pNode != iterator.pNode; }
  
  /// Prefix -- overload
  BWDIterator &operator--() {
    pNode = pNode->prev;
    return *this;
  }
  // Postfix -- overload
  BWDIterator operator--(int) {
    BWDIterator iterator = *this;
    --*this;
    return iterator;
   }
  };
};

template <class T>
using List = DLinkedList<T>;

//! ////////////////////////////////////////////////////////////////////
//! //////////////////////     METHOD DEFNITION      ///////////////////
//! ////////////////////////////////////////////////////////////////////
template <class T>
DLinkedList<T>::DLinkedList(void (*deleteUserData)(DLinkedList<T> *),
                            bool (*itemEqual)(T &, T &)) {
  head = new Node();
  tail = new Node();
  head->next = tail;
  tail->prev = head;
  count = 0;
  this->itemEqual = itemEqual;
  this->deleteUserData = deleteUserData;
}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list) {
  head = new Node();
  tail = new Node();
  head->next = tail;
  tail->prev = head;
  count = 0;
  this->itemEqual = list.itemEqual;
  this->deleteUserData = list.deleteUserData;
  copyFrom(list);
}

template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list) {
  removeInternalData();
  copyFrom(list);
  return *this;
}

template <class T>
DLinkedList<T>::~DLinkedList() {
  removeInternalData();
  delete head;
  delete tail;
}

template <class T>
void DLinkedList<T>::add(T e) {
  Node *newNode = new Node(T(e), tail, tail->prev);
  tail->prev->next = newNode;
  tail->prev = newNode;
  count++;
}

template <class T>
void DLinkedList<T>::add(int index, T e) {
    if (index < 0 || index > this->count) throw std::out_of_range("Index is out of range!");
    Node *current = this->head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    Node *newNode = new Node(e, current->next, current);
    current->next->prev = newNode;
    current->next = newNode;
    this->count++;
}

template <class T>
T DLinkedList<T>::removeAt(int index) {
  if (index < 0 || index >= this->count) throw std::out_of_range("Index is out of range!");
    Node *current = this->head->next;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    T removedData = current->data;
    current->prev->next = current->next;
    current->next->prev = current->prev;
    delete current;
    this->count--;
    return removedData;
}

template <class T>
bool DLinkedList<T>::empty() {
  return count == 0;
}

template <class T>
int DLinkedList<T>::size() {
  return count;
}

template <class T>
void DLinkedList<T>::clear() {
  removeInternalData();
  head->next = tail;
  tail->prev = head;
  count = 0;
}

template <class T>
T &DLinkedList<T>::get(int index) {
  if (index < 0 || index >= count) throw out_of_range("Index is out of range!");
  Node *node = getPreviousNodeOf(index)->next;
  return node->data;
}

template <class T>
int DLinkedList<T>::indexOf(T item) {
  Node *current = head->next;
  for (int i = 0; i < count; i++) {
    if (equals(current->data, item, itemEqual)) return i;
    current = current->next;
  }
  return -1;
}

template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T)) {
  Node *current = head->next;
  while (current != tail) {
    if (equals(current->data, item, itemEqual)) {
      current->prev->next = current->next;
      current->next->prev = current->prev;
      if (removeItemData != nullptr) removeItemData(current->data);
      delete current;
      count--;
      return true;
    }
    current = current->next;
  }
  return false;
}

template <class T>
bool DLinkedList<T>::contains(T item) {
  return indexOf(item) != -1;
}

template <class T>
string DLinkedList<T>::toString(string (*item2str)(T &)) {
  stringstream ss;
  ss << "[";
  if (head->next != tail) {
    Node *ptr = head->next;
    while (ptr != tail) {
      if (item2str != 0)
        ss << item2str(ptr->data);
      else
        ss << ptr->data;
      if (ptr->next != tail) ss << ", ";
      ptr = ptr->next;
    }
  }
  ss << "]";
  return ss.str();
}
//! ////////////////////////////////////////////////////////////////////
//! ////////////////////// (private) METHOD DEFNITION //////////////////
//! ////////////////////////////////////////////////////////////////////
template <class T>
void DLinkedList<T>::copyFrom(const DLinkedList<T> &list) {
  /**
   * Copies the contents of another doubly linked list into this list.
   * Initializes the current list to an empty state and then duplicates all data
   * and pointers from the source list. Iterates through the source list and
   * adds each element, preserving the order of the nodes.
   */
  // TODO implement
  clear();
  Node *current = list.head->next;
  while (current != list.tail) {
    add(T(current->data));
    current = current->next;
  }
  this->itemEqual = list.itemEqual;
  this->deleteUserData = list.deleteUserData;
}

template <class T>
void DLinkedList<T>::removeInternalData() {
  /**
   * Clears the internal data of the list by deleting all nodes and user-defined
   * data. If a custom deletion function is provided, it is used to free the
   * user's data stored in the nodes. Traverses and deletes each node between
   * the head and tail to release memory.
   */
  // TODO implement
  if (deleteUserData != nullptr) {
    deleteUserData(this);
  }
   Node *current = head->next;
  while (current != tail) {
    Node *temp = current;
    current = current->next;
    delete temp;
  }
  head->next = tail;
  tail->prev = head;
  count = 0;
}

template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index) {
  /**
   * Returns the node preceding the specified index in the doubly linked list.
   * If the index is in the first half of the list, it traverses from the head;
   * otherwise, it traverses from the tail. Efficiently navigates to the node by
   * choosing the shorter path based on the index's position.
   */
  // TODO implement
  if (index <= count / 2) {
    Node *current = head;
    for (int i = 0; i < index; i++) {
      current = current->next;
    }
    return current;
  } else {
    Node *current = tail;
    for (int i = count; i > index; i--) {
      current = current->prev;
    }
    return current;
  }
}

#endif /* DLINKEDLIST_H */
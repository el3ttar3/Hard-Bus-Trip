//
// Created by PC on 12/11/2023.
//

#include "PriorityQueue.h"
#include "Station.h"
#include "Queue.cpp"
#include "Passenger.h"
#include "Bus.h"
template class PriorityQueue<Passenger*>;
template class PriorityQueue<Bus*>;
template <typename T>
PriorityQueue<T>::~PriorityQueue() {
    Node* temp = head;
    while (temp != nullptr) {
        Node* next = temp->next;
        delete temp;
        temp = next;
    }
}

template <typename T>
void PriorityQueue<T>::enqueuePQ(T item, int priority) {
    Node* n = new Node(item, priority);
    if (isEmpty() || n->priority > head->priority) {
        n->next = head;
        head = n;
        size++;
    }
    else {
        Node* temp = head;
        while (temp->next != nullptr && temp->next->priority >= n->priority) {
            temp = temp->next;
        }
        n->next = temp->next;
        temp->next = n;
        size++;
    }
}

template <typename T>
void PriorityQueue<T>::dequeuePQ() {
    if (isEmpty()) return;
    Node* temp = head;
    head = head->next;
    delete temp;
    size--;
}

template <typename T>
bool PriorityQueue<T>::isEmpty() {
    return (head == nullptr);
}

template <typename T>
T PriorityQueue<T>::peek() {
    if (isEmpty()) {
        // Return a default-constructed value for type T when the queue is empty
        return T();
    }
    return head->value;
}

template <typename T>
void PriorityQueue<T>::display() {
    if (isEmpty()) {
        cout << "Queue is empty" << endl;
        return;
    }
    cout << "Queue elements are: ";
    Node* temp = head;
    while (temp != nullptr) {
        cout << "(" << temp->value << ", " << temp->priority << ") ";
        temp = temp->next;
    }
    cout << endl;
}

template <typename T>
int PriorityQueue<T>::getSize() const {
    return size;
}
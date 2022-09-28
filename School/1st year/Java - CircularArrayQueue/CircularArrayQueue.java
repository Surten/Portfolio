package cz.cvut.fel.pjv;

/**
 * Implementation of the {@link Queue} backed by fixed size array.
 */
public class CircularArrayQueue implements Queue {

    private int enque;
    private int deque;
    private int counter;
    private int capacity;
    private String[] array;

    /**
     * Creates the queue with capacity set to the value of 5.
     */
    public CircularArrayQueue() {
        enque = 0;
        deque = 0;
        counter = 0;
        capacity = 5;
        this.array = new String[capacity];
    }


    /**
     * Creates the queue with given {@code capacity}. The capacity represents maximal number of elements that the
     * queue is able to store.
     * @param capacity of the queue
     */
    public CircularArrayQueue(int capacity) {
        enque = 0;
        deque = 0;
        counter = 0;
        this.capacity = capacity;
        this.array = new String[capacity];
    }

    public int size() {
        return counter;
    }

    public boolean isEmpty() {
        return counter == 0;
    }

    public boolean isFull() {
        return counter == capacity;
    }

    public boolean enqueue(String obj) {
        if (isFull()){
            return false;
        }
        array[enque] = obj;
        counter++;
        enque = (enque + 1) % capacity;
        return true;
    }

    public String dequeue() {
        if (isEmpty()){
            return null;
        }
        String ret = array[deque];
        deque = (deque + 1) % capacity;
        counter--;
        return ret;
    }

    public void printAllElements() {
        for (int i = 0; i < counter; i++) {
            System.out.println(array[(deque + i)% capacity]);
        }
    }
}

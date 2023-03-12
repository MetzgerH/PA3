/*
 *  Separate chaining hashtable
 */

#ifndef __CHAINING_HASH_H
#define __CHAINING_HASH_H

// Standard library includes
#include <iostream>
#include <vector>
#include <list>
#include <stdexcept>

// Custom project includes
#include "Hash.h"

// Namespaces to include
using std::vector;
using std::list;
using std::pair;

using std::cout;
using std::endl;

//
// Separate chaining based hash table - derived from Hash
//
template<typename K, typename V>
class ChainingHash : public Hash<K,V> {
private:
    int load;
    vector<list<pair<K, V>>> table;
public:
    ChainingHash(int n = 11) {
        table = vector<list<pair<K, V>>>(n);
        load = 0;
    }

    ~ChainingHash() {
        this->clear();
    }

    int size() {
        return load;
    }

    V operator[](const K& key) {
        auto it = table[hash(key)].begin();
        for (; it != table[hash(key)].end() && it->first != key;it++);
        return it->second;
    }

    bool insert(const std::pair<K, V>& pair) {
        for (auto it = table[hash(pair.first)].begin(); it != table[hash(pair.first)].end(); it++)
            if (*it == pair)
                return false;
        table[hash(pair.first)].push_back(pair);
        load++;
        this->validate();
        return true;
    }

    void erase(const K& key) {
        auto it = table[hash(key)].begin();
        for (; it != table[hash(key)].end(); it++)
        {
            if (it->first == key)
                break;
        }
        if (it != table[hash(key)].end())
        {
            table[hash(key)].erase(it);
            this->load--;
        }
    }

    void clear() {
        table.clear();
        load = 0;
    }

    int bucket_count() {
        return table.size();
    }

    float load_factor() {
        return (float)this->size() / this->bucket_count();
    }

private:
    void validate()
    {
        if (load_factor() >= 0.75)
            rehash();
    }

    void rehash()
    {
        //int newSize = this->findNextPrime(this->bucket_count() + 1);
        int newSize = this->findNextPrime(this->bucket_count() *2);
        while ((float)this->size() / newSize > 0.75)
        {
            //newSize = this->findNextPrime(newSize + 1);
            newSize = this->findNextPrime(newSize * 2);
        }
        table.resize(newSize);
        for (auto bucketSlot = table.begin(); bucketSlot != table.end(); bucketSlot++)
        {
            if (!bucketSlot->empty())//if there's anything at that bucketslot
            {
                for (auto pairInList = bucketSlot->begin(); pairInList != bucketSlot->end();)
                {
                    if (hash(pairInList->first) == bucketSlot - table.begin())//if its in the right spot already
                    {
                        pairInList++;
                    }
                    else
                    {
                        pair<K, V> pairToReinsert = *pairInList; //create a copy of this pair

                        auto tempIterator = prev(pairInList, 1); //create an iterator that points to the previous item in list
                        bucketSlot->remove(*pairInList);
                        insert(pairToReinsert);
                        pairInList = tempIterator;

                    }
                }
            }
        }
        //vector<list<pair<K, V>>>* oldTable = table;//"moves" the oldtable
        //table = new vector<list<pair<K, V>>>(newSize);//constructs a new table from scratch
        //load = 0; //reset load because we're gonna re-add all the contents
        //for (auto bucketSlot = oldtable.begin(); bucketSlot < oldtable.end(); bucketSlot++) //go through every slot in the old table
        //{
        //    
        //    while (!bucketSlot->empty()) //while this slot in the old table still has stuff in it
        //    {
        //        this->insert(*bucketSlot->begin()); //add the first entry in the old table to the new table
        //        bucketSlot->pop_front(); //remove the first entry in the old table to move onto the next for the next iteration of the while loop
        //    }
        //    
        //}
        //delete oldTable;
    }

    int findNextPrime(int n)
    {
        while (!isPrime(n))
        {
            n++;
        }
        return n;
    }

    int isPrime(int n)
    {
        for (int i = 2; i <= sqrt(n); i++)
        {
            if (n % i == 0)
            {
                return false;
            }
        }

        return true;
    }

    int hash(const K& key) {
        return key % table.size();
    }

};

#endif //__CHAINING_HASH_H

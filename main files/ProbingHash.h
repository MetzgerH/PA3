#ifndef __PROBING_HASH_H
#define __PROBING_HASH_H

#include <vector>
#include <stdexcept>

#include "Hash.h"

using std::vector;
using std::pair;

enum EntryState {
    EMPTY = 0, VALID = 1,DELETED = 2
};

template<typename K, typename V>
class ProbingHash : public Hash<K,V> { // derived from Hash
private:
    // Needs a table and a size.
    // Table should be a vector of std::pairs for lazy deletion
    vector<pair<pair<K, V>, EntryState>> table; //a vector of pairs of the data (which is a pair of key and value) and their state
    int load;
public:
    ProbingHash(int n = 11) {
        table = vector<pair<pair<K, V>, EntryState>>(n, pair<pair<K,V>,EntryState>(pair<K,V>(0, 0), EMPTY));
        load = 0;
    }

    ~ProbingHash() {
        this->clear();
    }

    int size() {
        return load;
    }

    V operator[](const K& key) {
        int insertionSpot = hash(key);
        int spotsChecked = 0;
        while (spotsChecked < this->bucket_count() &&
            (table[insertionSpot].second == DELETED || 
            (table[insertionSpot].second == VALID && table[insertionSpot].first.first != key)))
        {
            spotsChecked++;
            insertionSpot = hash(insertionSpot + 1);
            
        }
        if (table[insertionSpot].second == DELETED)
            return NULL;
        if (table[insertionSpot].first.first == key)
        {
            return table[insertionSpot].first.second;
        }
        return NULL;
    }

    bool insert(const std::pair<K, V>& pair) {
        this->load++;
        this->validate();

        int insertionSpot = hash(pair.first);
        while (table[insertionSpot].second == VALID)
        {
            if (table[insertionSpot].first == pair)
                return false;
            insertionSpot = hash(insertionSpot + 1);
        }
        table[insertionSpot].first = pair;
        table[insertionSpot].second = VALID;
        
        return true;
    }

    void erase(const K& key) {
        int insertionSpot = hash(key);
        int spotsChecked = 0;
        while (spotsChecked < this->bucket_count() &&
            (table[insertionSpot].second == DELETED ||
                (table[insertionSpot].second == VALID && table[insertionSpot].first.first != key))){
            spotsChecked++;//this is just to make sure we don't just go on forever if they try to erase a non-existant pair
            insertionSpot = hash(insertionSpot + 1);
        } 
        
        if (spotsChecked == this->bucket_count())
            return;
        table[insertionSpot].second = DELETED;
        load--;
    }

    void clear() {
        int maintainedSize = table.size();
        table.clear();
        load = 0;

        table.resize(maintainedSize, pair<pair<K, V>, EntryState>(pair<K, V>(0, 0), EMPTY));
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
        //int newSize = this->findNextPrime(this->bucket_count() +1);
        int newSize = this->findNextPrime(this->bucket_count() *2);
        while ((float)this->size() / newSize > 0.75)
        {
            //newSize = this->findNextPrime(newSize + 1);
            newSize = this->findNextPrime(newSize * 2);
        }
        table.resize(newSize, pair<pair<K, V>, EntryState>(pair<K, V>(0, 0), EMPTY));;
        for (auto it = table.begin(); it != table.end(); it++)
        {
            if (it->second == VALID && hash(it->first.first) != it - table.begin())
            {
                it->second = DELETED;
                load--;
                this->insert(it->first);
            }
        }
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
        return key % this->bucket_count();
    }
    
};

#endif //__PROBING_HASH_H

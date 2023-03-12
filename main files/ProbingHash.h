#ifndef __PROBING_HASH_H
#define __PROBING_HASH_H

#include <vector>
#include <stdexcept>

#include "Hash.h"

using std::vector;
using std::pair;

#define MAXLOAD 0.75

enum EntryState {
    EMPTY = 0, VALID = 1,DELETED = 2
};

template<typename K, typename V>
class ProbingHash : public Hash<K,V> { // derived from Hash
private:
    vector<pair<pair<K, V>, EntryState>> table; //a vector of pairs of the data (which is a pair of key and value) and their state
    int load;
public:
    ProbingHash(int n = 11) 
    {
        table = vector<pair<pair<K, V>, EntryState>>(n, pair<pair<K,V>,EntryState>(pair<K,V>(0, 0), EMPTY));
        load = 0;
    }

    ~ProbingHash() 
    {
        this->clear();
    }

    int size() 
    {
        return load;
    }

    V operator[](const K& key) 
    {
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

    bool insert(const std::pair<K, V>& pair) 
    {
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

    void erase(const K& key) 
    {
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

    void clear() 
    {
        int maintainedSize = table.size();
        table.clear();
        load = 0;

        table.resize(maintainedSize, pair<pair<K, V>, EntryState>(pair<K, V>(0, 0), EMPTY));
    }

    int bucket_count() 
    {
        return table.size();
    }

    float load_factor() 
    {
        return (float)this->size() / this->bucket_count();
    }

private:
    void validate()
    {
        if (load_factor() >= MAXLOAD)
            rehash();
    }

    void rehash()
    {
        //int newSize = this->findNextPrime(this->bucket_count() +1); //realized that this is VERY slow when rehashing is frequent, like in the tests
        int newSize = this->findNextPrime(this->bucket_count() *2); //double the size, then find the next prime
        while ((float)this->size() / newSize > MAXLOAD) //make sure that newSize is big enough to get the load under MAXLOAD
        {
            //newSize = this->findNextPrime(newSize + 1);
            newSize = this->findNextPrime(newSize * 2); //double again
        }
        table.resize(newSize, pair<pair<K, V>, EntryState>(pair<K, V>(0, 0), EMPTY));; //make the table bigger to match newSize
        for (auto it = table.begin(); it != table.end(); it++) //iterate through table
        {
            if (it->second == VALID && hash(it->first.first) != it - table.begin()) //if current item's key doesn't match its place in table
            {
                it->second = DELETED; //delete it
                load--; //subtract from load (insert adds from load)
                this->insert(it->first);  //re-insert it where it should be (which may be the same place as it was before)
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

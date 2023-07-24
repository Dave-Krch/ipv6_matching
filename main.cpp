#include <iostream>
#include <bitset>
#include <vector>

const int TREE_DEPTH = 5;
const int CHARACTERS = 2;

struct entry {
    std::vector<bool> address_in_bits;
    int pop;

    entry(const std::vector<bool> & input, int pop) : address_in_bits(input), pop(pop) {

    }
};

struct trie_node {
    trie_node * parent;
    trie_node * children[CHARACTERS];
    size_t pop;

    trie_node() : parent(nullptr), children{}, pop(-1) {
    }

    ~trie_node() {
        for(int i = 0; i < CHARACTERS; i++) {
            if(children[i] != nullptr)
                delete children[i];
        }
    }

};

struct trie {
    trie_node root;

    void insert_entry(const entry & e) {
        trie_node * current_node = &root;

        for(const auto bit: e.address_in_bits) {
            if(current_node->children[bit] == nullptr) {
                current_node->children[bit] = new trie_node();
                current_node->children[bit]->parent = current_node;
                current_node = current_node->children[bit];
            }
            else {
                current_node = current_node->children[bit];
            }
        }

        current_node->pop = e.pop;
    }
};


int main() {
    trie a;
    a.insert_entry(entry({1,1,1}, 1));
    a.insert_entry(entry({0,1,1}, 1));
    a.insert_entry(entry({1,0,1}, 1));
    a.insert_entry(entry({1,1,0}, 1));
    a.insert_entry(entry({0,0,0}, 1));

    return 0;
}

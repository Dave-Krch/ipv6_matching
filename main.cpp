#include <iostream>
#include <bitset>
#include <utility>
#include <vector>
#include <sstream>
#include <fstream>

const int ADDRESS_LENGTH = 5;
const int CHARACTERS = 2;

std::vector<bool> address_to_bits(const std::string & input) {
    std::vector<bool> out;

    size_t pos = input.find('/');

    //address to stringstream
    std::stringstream address_ss(input.substr(0, pos));

    //prefix and written bit counter
    size_t prefix = stoi(input.substr(pos + 1));
    size_t writen_bits = 0;

    char separator = ':';
    std::string token;

    while (std::getline(address_ss, token, separator)) {
        if(token.empty()) {
            while (writen_bits < prefix) {
                out.push_back(false);
                writen_bits++;
            }
            break;
        }

        std::stringstream token_ss(token);
        token_ss << std::hex << token;

        size_t token_decimal;
        token_ss >> token_decimal;

        std::bitset<16> token_binary(token_decimal);

        std::cout << token_binary.to_string() << std::endl;

        for(int i = 15; i >= 0; i-- ) {
            if(writen_bits == prefix)
                break;
            out.push_back(token_binary[i]);
            writen_bits ++;
        }
    }

    for(size_t bit: out) {
        std::cout << bit;
    }

    std::cout << std::endl;

    return out;
}

struct trie_node {
    trie_node * parent;
    trie_node * children[CHARACTERS];
    //set -1 if node doesnt end subnet record
    size_t pop;

    trie_node() : parent(nullptr), children{}, pop(-1) {
    }

    ~trie_node() {
        for(auto & i : children) {
                delete i;
        }
    }

};

struct trie {
    trie_node root;

    void insert_entry(const std::vector<bool> & address_in_bits, size_t pop) {

        trie_node * current_node = &root;

        for(const auto bit: address_in_bits) {
            if(current_node->children[bit] == nullptr) {
                current_node->children[bit] = new trie_node();
                current_node->children[bit]->parent = current_node;
                current_node = current_node->children[bit];
            }
            else {
                current_node = current_node->children[bit];
            }
        }

        current_node->pop = pop;
    }

    void load_data(const char * path) {
        std::ifstream data;
        data.open(path);

        if (!data.is_open())
            std::cout << "Cannot open file" << std::endl;

        std::string address;
        std::string pop;

        while (data >> address >> pop) {
            std::cout << "Inserting: " << address << " " << pop << std::endl;
            insert_entry(address_to_bits(address), stoi(pop));
        }
    }

    bool address_stored(const std::vector<bool> & address_in_bits) {
        trie_node * current_node = &root;

        for(auto bit: address_in_bits) {
            if(current_node->children[bit] == nullptr) {
                std::cout << "Address not stored" << std::endl;
                return false;
            }

            current_node = current_node->children[bit];
        }

        std::cout << "Address stored, pop = " << current_node->pop << std::endl;
        return true;
    }

};

int main() {

    trie a;

    a.load_data("../data");
    a.address_stored(address_to_bits(std::string("2081:49f0:d0b8::/48")));

    return 0;
}

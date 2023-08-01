#include <iostream>
#include <bitset>
#include <utility>
#include <vector>
#include <sstream>
#include <fstream>
#include <assert.h>

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

        //std::cout << token_binary.to_string() << std::endl;

        for(int i = 15; i >= 0; i-- ) {
            if(writen_bits == prefix)
                break;
            out.push_back(token_binary[i]);
            writen_bits ++;
        }
    }

    for(size_t bit: out) {
        //std::cout << bit;
    }

    //std::cout << std::endl;

    return out;
}

struct Node {
    Node * parent;
    Node * children[2];
    //set -1 if node doesnt end subnet record
    size_t pop;

    Node() : parent(nullptr), children{}, pop(-1) {
    }

    ~Node() {
        for(auto & i : children) {
                delete i;
        }
    }

};

struct Trie {
    Node root;

    void InsertEntry(const std::vector<bool> & address_in_bits, size_t pop) {

        Node * current_node = &root;

        for(const auto bit: address_in_bits) {
            if(current_node->children[bit] == nullptr) {
                current_node->children[bit] = new Node();
                current_node->children[bit]->parent = current_node;
                current_node = current_node->children[bit];
            }
            else {
                current_node = current_node->children[bit];
            }
        }

        current_node->pop = pop;
    }

    void LoadData(const char * path) {
        std::ifstream data;
        data.open(path);

        if (!data.is_open())
            std::cout << "Cannot open file" << std::endl;

        std::string address;
        std::string pop;

        while (data >> address >> pop) {
             //std::cout << "Inserting: " << address << " " << pop << std::endl;
            InsertEntry(address_to_bits(address), stoi(pop));
        }
    }

    bool AddressStored(const std::vector<bool> & address_in_bits) {
        Node * current_node = &root;

        for(auto bit: address_in_bits) {
            if(current_node->children[bit] == nullptr) {
                //std::cout << "Address not stored" << std::endl;
                return false;
            }

            current_node = current_node->children[bit];
        }

        //std::cout << "Address stored, pop = " << current_node->pop << std::endl;
        return true;
    }

};

std::pair<size_t, u_char> Route(Trie * data, const std::vector<bool> & address_in_bits) {
    Node * current_node = &data->root;

    size_t traversed_bits = 0;

     //std::cout << "------------------" << std::endl;

    for(auto bit: address_in_bits) {
        if(current_node->children[bit] != nullptr) {
            //std::cout << bit;
            traversed_bits++;
            current_node = current_node->children[bit];
            continue;
        }
        break;
    }

    //std::cout << std::endl << "------------------  " << traversed_bits << " " << current_node->pop << std::endl;

    while ((current_node->pop == -1) && (traversed_bits > 0)) {
        current_node = current_node->parent;
        traversed_bits --;
    }

    if(traversed_bits == 0)
        return {0, 0};

    return {current_node->pop, traversed_bits};
}

int main() {

    Trie a;

    a.LoadData("../data");

    std::pair<size_t, u_char> val;

    val = Route(&a, address_to_bits(std::string("2001:1502::/56")));

    assert((val.first == 1) && (val.second == 16));

    val = Route(&a, address_to_bits(std::string("2000:0001::/32")));

    assert((val.first == 6) && (val.second == 32));

    val = Route(&a, address_to_bits(std::string("aaaa:0001::/32")));

    assert((val.first == 0) && (val.second == 0));

    return 0;
}

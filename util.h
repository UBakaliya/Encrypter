/**
 * @file util.h
 * @author Uvaish Bakaliya
 * @date 2022-12-13
 * @copyright Copyright (c)UB 2022
 */

#pragma once

#include <algorithm>
#include <queue>
#include <unordered_map>

typedef hashmap hashmapF;
typedef unordered_map<int, string> hashmapE;

// *HuffmanNode to store data and also to manipulate in various ways
struct HuffmanNode
{
    int character;
    int count;
    HuffmanNode *zero;
    HuffmanNode *one;
};

// *Use for comparing the value that are going to be store inside of priority
// queue
struct compare
{
    bool operator()(const HuffmanNode *lhs,
                    const HuffmanNode *rhs)
    {
        return lhs->count > rhs->count;
    }
};

// *This method frees the memory allocated for the Huffman tree.
void freeTree(HuffmanNode *node)
{
    if (node == nullptr)
        return;
    freeTree(node->zero);
    freeTree(node->one);
    delete node;
}

// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
void buildFrequencyMap(string filename, bool isFile, hashmapF &map)
{
    char splitC;
    string dataStr;

    // read from the file if true:
    if (isFile)
    {
        ifstream inFile(filename);
        while (inFile.get(splitC))
            dataStr += splitC; // insert char into string
        inFile.close();

        // loop through the string and check charater is within dataStr
        for (const auto &c : dataStr)
        {
            long int occur = 0;
            // count the occurrences and store it into map
            for (long unsigned int i = 0; (i = dataStr.find(c, i)) != string::npos; i++)
                occur++;
            map.put(c, occur);
        }
    }
    // read from the file string if false:
    else
    {
        // loop through the filename string and find the occurrences
        for (unsigned i = 0; i < filename.length(); i++)
            map.put(filename[i], count(filename.begin(), filename.end(), filename[i]));
    }
    // add the last PSEUDO_EOF to map
    map.put(PSEUDO_EOF, 1);
}

// *This function builds an encoding tree from the frequency map.
HuffmanNode *buildEncodingTree(hashmapF &map)
{
    priority_queue<HuffmanNode *, vector<HuffmanNode *>, compare> pq; // to store nodes

    // get all the keys into vector
    vector<int> keys = map.keys();

    // use the keys to get the vales
    for (unsigned i = 0; i < keys.size(); i++)
    {
        // add the node into pq
        HuffmanNode *hmNode = new HuffmanNode;
        hmNode->character = keys[i];
        hmNode->count = map.get(keys[i]);
        hmNode->zero = nullptr;
        hmNode->one = nullptr;
        pq.push(hmNode);
    }
    // loop until there is only one node which is going to be our tree
    while (pq.size() != 1)
    {
        HuffmanNode *root = new HuffmanNode;
        root->character = NOT_A_CHAR;
        root->zero = pq.top();
        pq.pop();
        root->one = pq.top();
        root->count = (root->zero->count + root->one->count);
        pq.pop();
        pq.push(root); // and new node
    }
    return pq.top();
}

// *Recursive helper function for building the encoding map.
void _buildEncodingMap(HuffmanNode *node, hashmapE &encodingMap, string str,
                       HuffmanNode *prev)
{
    // base case:
    if (node == nullptr)
        return;
    // if not the char then added to our encodingMap
    if (node->character != NOT_A_CHAR)
        encodingMap[node->character] = str;
    _buildEncodingMap(node->zero, encodingMap, str + '0', prev);
    _buildEncodingMap(node->one, encodingMap, str + '1', prev);
}

// *This function builds the encoding map from an encoding tree.
hashmapE buildEncodingMap(HuffmanNode *tree)
{
    hashmapE encodingMap;
    _buildEncodingMap(tree, encodingMap, "", tree); // build encoding map!
    return encodingMap;
}

// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
string encode(ifstream &input, hashmapE &encodingMap, ofbitstream &output,
              int &size, bool makeFile)
{
    char searchingC;
    string str;

    // if true then:
    if (makeFile)
    {
        // find the character & store it inside of str
        while (input.get(searchingC))
            str += encodingMap[searchingC];
        str += encodingMap[PSEUDO_EOF]; // add the eof as well
        // write the bits to ouput stream so we can use it later
        for (const auto &c : str)
        {
            if (c == '0')
                output.writeBit(0);
            else
                output.writeBit(1);
        }
        size = str.size();
    }
    return str;
}

// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
string decode(ifbitstream &input, HuffmanNode *encodingTree, ofstream &output)
{
    string codes;
    HuffmanNode *curr = encodingTree; // temporary, to get back to root

    while (!input.eof())
    {
        int bit = input.readBit();
        // if zero then move left:
        if (bit == 0)
            curr = curr->zero;
        // else right:
        else
            curr = curr->one;
        // exit if it reaches the eof char
        if (curr->character == PSEUDO_EOF)
            break;
        // reached leaf node
        if (curr->character != NOT_A_CHAR)
        {
            codes += curr->character;
            output.put(curr->character);
            curr = encodingTree;
        }
    }
    return codes;
}

// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
string compress(string filename)
{
    bool isFile = true;
    ifstream input(filename);
    // check valid to open:
    if (input.fail())
        isFile = false;

    // get and the necessary items we need to endcode the file
    hashmapF freqMap;
    buildFrequencyMap(filename, isFile, freqMap);
    HuffmanNode *encodingTree = buildEncodingTree(freqMap);
    hashmapE encodingMap = buildEncodingMap(encodingTree);

    ofbitstream output(filename + ".huf"); // make the file end if .huf
    output << freqMap;
    int size = 0;
    string codeStr = encode(input, encodingMap, output, size, true); // encode the file!
    output.close();

    return codeStr;
}

// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
string decompress(string filename)
{
    // check if it is a .huf file
    size_t pos = filename.find(".huf");
    if ((int)pos >= 0)
        filename = filename.substr(0, pos);

    pos = filename.find(".");
    string ext = filename.substr(pos, filename.length() - pos);
    filename = filename.substr(0, pos);
    ifbitstream input(filename + ext + ".huf"); // open the file names .huf

    hashmapF frequencyMap;
    input >> frequencyMap; // read the stored frequency into frequencyMap (hashtable)

    HuffmanNode *encodingTree;
    encodingTree = buildEncodingTree(frequencyMap); // build the tree so we can use it for decode()
    ofstream output(filename + "_unc" + ext);       // write the new file with the add _unc
    string decodeStr = decode(input, encodingTree, output);
    output.close();

    return decodeStr;
}

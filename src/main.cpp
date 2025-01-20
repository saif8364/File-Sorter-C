#include <iostream>
#include <string>
#include <filesystem>
#include <ctime>

namespace fs = std::filesystem;
using namespace std;

// File metadata structure
struct FileMetadata {
    string name;
    string extension;
    uintmax_t size;
    string fullPath;  // Store the full path of the file

    FileMetadata(string name, string extension, uintmax_t size, string fullPath)
        : name(name), extension(extension), size(size), fullPath(fullPath) {}
};

// Binary Search Tree Node
struct TreeNode {
    FileMetadata file;
    TreeNode* left;
    TreeNode* right;

    TreeNode(FileMetadata file) : file(file), left(nullptr), right(nullptr) {}
};

// Hash Table Bucket Structure (for manual collision handling)
struct HashTableBucket {
    string extension;
    TreeNode* root;
    HashTableBucket* next;

    HashTableBucket(string ext) : extension(ext), root(nullptr), next(nullptr) {}
};

// Simple Custom Hash Table Implementation
class CustomHashTable {
public:
    CustomHashTable(int size) : tableSize(size) {
        table = new HashTableBucket*[tableSize]();
    }

    ~CustomHashTable() {
        for (int i = 0; i < tableSize; ++i) {
            HashTableBucket* bucket = table[i];
            while (bucket) {
                HashTableBucket* next = bucket->next;
                deleteTree(bucket->root);
                delete bucket;
                bucket = next;
            }
        }
        delete[] table;
    }

    int hash(string extension) {
        int hashValue = 0;
        for (char c : extension) {
            hashValue += c;
        }
        return hashValue % tableSize;
    }

    void insert(string extension, FileMetadata file) {
        int index = hash(extension);
        HashTableBucket* bucket = table[index];

        if (!bucket) {
            table[index] = new HashTableBucket(extension);
            bucket = table[index];
        }

        bucket->root = insertIntoBST(bucket->root, file);
    }

    void display(string extension) {
        int index = hash(extension);
        HashTableBucket* bucket = table[index];

        while (bucket) {
            if (bucket->extension == extension) {
                inorder(bucket->root);
                return;
            }
            bucket = bucket->next;
        }
        cout << "No files with extension " << extension << " found.\n";
    }

    void displayAll() {
        for (int i = 0; i < tableSize; ++i) {
            HashTableBucket* bucket = table[i];
            while (bucket) {
                inorder(bucket->root);
                bucket = bucket->next;
            }
        }
    }

    HashTableBucket** getTable() const { return table; }
    int getTableSize() const { return tableSize; }

private:
    int tableSize;
    HashTableBucket** table;

    TreeNode* insertIntoBST(TreeNode* root, FileMetadata file) {
        if (!root) return new TreeNode(file);

        if (file.name < root->file.name) {
            root->left = insertIntoBST(root->left, file);
        } else {
            root->right = insertIntoBST(root->right, file);
        }

        return root;
    }

    void inorder(TreeNode* root) {
        if (!root) return;
        inorder(root->left);
        cout << "File: " << root->file.name << " | Extension: " << root->file.extension
             << " | Size: " << root->file.size << " bytes\n";
        inorder(root->right);
    }

    void deleteTree(TreeNode* root) {
        if (root) {
            deleteTree(root->left);
            deleteTree(root->right);
            delete root;
        }
    }
};

struct QueueNode {
    fs::path data;
    QueueNode* next;

    QueueNode(fs::path data) : data(data), next(nullptr) {}
};

class CustomQueue {
public:
    CustomQueue() : front(nullptr), rear(nullptr) {}

    ~CustomQueue() {
        while (front) {
            QueueNode* temp = front;
            front = front->next;
            delete temp;
        }
    }

    void enqueue(fs::path path) {
        QueueNode* newNode = new QueueNode(path);
        if (rear) {
            rear->next = newNode;
        } else {
            front = newNode;
        }
        rear = newNode;
    }

    fs::path dequeue() {
        if (!front) return "";

        QueueNode* temp = front;
        fs::path path = front->data;
        front = front->next;
        if (!front) rear = nullptr;

        delete temp;
        return path;
    }

    bool isEmpty() const {
        return front == nullptr;
    }

private:
    QueueNode* front;
    QueueNode* rear;
};

void scanDirectory(const fs::path& directoryPath, CustomHashTable& hashTable) {
    CustomQueue directories;
    directories.enqueue(directoryPath);

    while (!directories.isEmpty()) {
        fs::path currentDir = directories.dequeue();

        if (fs::is_directory(currentDir)) {
            cout << "Directory: " << currentDir.filename().string() << "\n";

            for (const auto& entry : fs::directory_iterator(currentDir)) {
                if (entry.is_directory()) {
                    directories.enqueue(entry.path());
                } else if (entry.is_regular_file()) {
                    string extension = entry.path().extension().string().substr(1);
                    string fileName = entry.path().filename().string();
                    uintmax_t fileSize = entry.file_size();

                    FileMetadata file(fileName, extension, fileSize, entry.path().string());

                    hashTable.insert(extension, file);
                }
            }
        }
    }
}

void moveFilesFromBST(TreeNode* root, const fs::path& destDir) {
    if (!root) return;

    moveFilesFromBST(root->left, destDir);

    try {
        fs::path sourceFile = root->file.fullPath;
        fs::path destinationFile = destDir / root->file.name;

        if (fs::exists(sourceFile)) {
            fs::copy(sourceFile, destinationFile, fs::copy_options::overwrite_existing);
            cout << "Moved: " << root->file.name << " to " << destDir.filename().string() << "\n";
        } else {
            cout << "File not found: " << root->file.name << "\n";
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Error moving file: " << e.what() << "\n";
    }

    moveFilesFromBST(root->right, destDir);
}

void sortAndMoveFilesUsingHashTable(CustomHashTable& hashTable, const string& sortedDataPath) {
    cout << "\nSorting and moving files based on hash table data...\n";

    for (int i = 0; i < hashTable.getTableSize(); ++i) {
        HashTableBucket* bucket = hashTable.getTable()[i];

        while (bucket) {
            fs::path destDir = fs::path(sortedDataPath) / (bucket->extension + "_files");
            if (!fs::exists(destDir)) {
                try {
                    fs::create_directory(destDir);
                } catch (const fs::filesystem_error& e) {
                    cerr << "Error creating directory: " << e.what() << "\n";
                    continue;
                }
            }

            moveFilesFromBST(bucket->root, destDir);

            bucket = bucket->next;
        }
    }

    cout << "Files have been sorted and moved to: " << sortedDataPath << "\n";
}

int main() {
    string rootPath;
    cout << "Enter the path of the root directory to scan: ";
    getline(cin >> ws, rootPath);

    fs::path rootDirPath(rootPath);
    if (!fs::exists(rootDirPath) || !fs::is_directory(rootDirPath)) {
        cout << "Invalid directory path. Exiting..." << endl;
        return 1;
    }

    CustomHashTable hashTable(10);

    scanDirectory(rootDirPath, hashTable);

    cout << "\nAll files organized by extension:\n";
    hashTable.displayAll();

    char sortOption;
    cout << "\nDo you want to sort and move the files to Desktop? (y/n): ";
    cin >> sortOption;

    if (tolower(static_cast<unsigned char>(sortOption)) == 'y') {
        #ifdef _WIN32
            string desktopPath = (fs::path(getenv("USERPROFILE")) / "Desktop").string();
        #else
            string desktopPath = (fs::path(getenv("HOME")) / "Desktop").string();
        #endif
        string sortedDataPath = desktopPath + "/Sorted_Files";

        fs::create_directory(sortedDataPath);

        sortAndMoveFilesUsingHashTable(hashTable, sortedDataPath);
    }

    return 0;
}
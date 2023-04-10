#include <bits/stdc++.h>
using namespace std;

// Structure for the callee function information
struct fInfo {
    string fName;
    vector<int> fArgs;
};


struct element {
    int data;
    element *prev, *next;
};

// structure for the variable information
struct vInfo {
    string vName;
    size_t vSize;
    void *vPtr;
};

vector<pair<fInfo, vector<vInfo>>> PT;

bool memory_is_occupied(const void* ptr, size_t size) {
    const unsigned char *p = static_cast<const unsigned char *>(ptr);
    for (size_t i = 0; i < size; ++i) {
        if (p[i] != 0) return true;
    }
    return false;
}

void *createMem(size_t size) {
    void *mem = malloc(size);
    // error handling
    if (mem == NULL) {
        std::cout << "Error: Memory allocation failed.\n";
        exit(1);
    }

    memset(mem, 0, size);
    return mem;
}

void freeElem(string name);
void printPageTable();

void createList(std::string name, size_t size) {
    auto &entry = PT.back();
    fInfo f = entry.first;
    void *ptr = entry.second.front().vPtr;
    size_t addr_size = entry.second.front().vSize / sizeof(element);

    assert (entry.second.front().vName.empty());

    element *itr = (element *)ptr, *itr_prev = NULL;

    // create the linked list of given size in the allocated memory space
    // and assign only the unoccupied memory blocks using FIRST FIT Strategy
    for (; size > 0 && addr_size > 0; itr++) {
        if (!memory_is_occupied(itr, sizeof(element))) {
            itr->data = 0;
            itr->prev = itr_prev;
            if (itr_prev != NULL) itr_prev->next = itr;
            else {
                // updatePageTable(f, name, size, itr);
                entry.second.push_back({name, size * sizeof(element), itr});
            }
            itr_prev = itr;
            size--;
        }
        addr_size--;
    }

    if (addr_size == 0 && size > 0) {
        std::cout << "Error: Memory allocation failed.\n";
        // assigning appropriate memory size to free
        if (itr_prev != NULL) {
            entry.second.back().vSize -= size * sizeof(element);
            freeElem(name);
        }
        exit(1);
    }
}

void assignVal(std::string name, size_t index, size_t val) {
    
    void *ptr = NULL;
    size_t size = 0;

    for (auto entry = PT.rbegin(); entry != PT.rend(); entry++) {
        for (auto it : entry->second) {
            if (it.vName == name) {
                ptr = it.vPtr;
                size = it.vSize / sizeof(element);
                break;
            }
        }
    }

    if (ptr == NULL) {
        std::cout << "Error: No such variable exists.\n";
        exit(1);
    }

    if (index >= size || index < 0) {
        std::cout << index << ' ' << size << "Error: Index out of bounds.\n";
        exit(1);
    }

    element *itr = (element *)ptr;
    for (int i = 0; i < index; i++) {
        itr = itr->next;
    }
    itr->data = val;
}

int getVal(std::string name, size_t index) {
    void *ptr = NULL;
    size_t size = 0;

    for (auto entry = PT.rbegin(); entry != PT.rend(); entry++) {
        for (auto it : entry->second) {
            if (it.vName == name) {
                ptr = it.vPtr;
                size = it.vSize / sizeof(element);
                break;
            }
        }
    }

    if (ptr == NULL) {
        std::cout << "Error: No such variable exists.\n";
        exit(1);
    }

    if (index >= size || index < 0) {
        std::cout << "Error: Index out of bounds.\n";
        exit(1);
    }

    element *itr = (element *)ptr;
    for (int i = 0; i < index; i++) {
        itr = itr->next;
    }
    return itr->data;
}

void freeElem(std::string name = "") {
    if (name.empty()) {
        // remove the last entry from the page table 
        // (i.e the current function address space)
        auto entry = PT.back();
        fInfo f = entry.first;
        void *ptr = entry.second.front().vPtr;

        free(ptr);
        PT.pop_back();
    }
    else {
        auto &entry = PT.back();
        fInfo f = entry.first;
        void *ptr = NULL;
        size_t size = 0;

        for (int i = 0; i < entry.second.size(); i++) {
            auto it = entry.second[i];
            if (it.vName == name) {
                ptr = it.vPtr;
                size = it.vSize / sizeof(element);
                // delete the entry from the page table
                // delete by the array index
                entry.second.erase(entry.second.begin() + i);
                break;
            }
        }

        if (ptr == NULL) {
            std::cout << "Error: No such variable exists.\n";
            exit(1);
        }

        element *itr = (element *)ptr;
        while (size > 1) {
            itr = itr->next;
            memset(itr->prev, 0, sizeof(element));
            size--;
        }
        if (size) memset(itr, 0, sizeof(element));
    }
}

void updatePageTable(fInfo f, size_t size, void *ptr) {
    // update the local Page Table entry
    PT.push_back(make_pair(f, vector<vInfo> {{"", size, ptr}}));
}

void printPageTable () {
    for (auto entry : PT) {
        fInfo f = entry.first;
        std::cout << f.fName << ": ";
        for (int it : f.fArgs) {
            std::cout << it << " ";
        }
        std::cout << "\n";
        for (auto it : entry.second) {
            std::cout << it.vName << " | " << it.vSize << " | " << it.vPtr << "\n";
        }
    }
}

void merge(string name, int l, int m, int r) {
    element *itr = (element *)createMem((r - l + 1) * sizeof(element));
    updatePageTable({__FUNCTION__, {l, m, r}}, (r - l + 1) * sizeof(element), itr);

    createList ("tmp", r - l + 1);
    int p1 = l, p2 = m + 1, p3 = 0;
    while (p1 <= m && p2 <= r) {
        int v1 = getVal(name, p1);
        int v2 = getVal(name, p2);
        if (v1 < v2) {
            assignVal("tmp", p3++, v1);
            p1++;
        }
        else {
            assignVal("tmp", p3++, v2);
            p2++;
        }
    }
    while (p1 <= m) {
        assignVal("tmp", p3++, getVal(name, p1++));
    }
    while (p2 <= r) {
        assignVal("tmp", p3++, getVal(name, p2++));
    }
    for (int i = l; i <= r; i++) {
        assignVal(name, i, getVal("tmp", i - l));
    }

    freeElem("tmp");
    freeElem();
}

void mergesort(string name, int l, int r) {
    if(l >= r) return;
    int m = l + (r - l) / 2;
    mergesort(name, l, m);
    mergesort(name, m + 1, r);
    merge(name, l, m, r);
}

void bubblesort(string name, int l, int r) {
    for (int i = l; i <= r; i++) {
        for (int j = r - 1; j > l; j--) {
            if (getVal(name, j) < getVal(name, j - 1)) {
                int temp = getVal(name, j);
                assignVal(name, j, getVal(name, j - 1));
                assignVal(name, j - 1, temp);
            }
        }
    }
}

int main() {
    updatePageTable({"global", {}}, 0, NULL);


    element *itr = (element *)createMem(50000 * sizeof(element));
    updatePageTable({__FUNCTION__, {}}, 50000 * sizeof(element), itr);
    
    createList("A", 50000);

    for (int i = 0; i < 50000; i++) {
        assignVal("A", i, rand() % 100000);
    }

    for (int i = 0; i < 15; i++) {
        std::cout << getVal("A", i) << " ";
    }

    std::cout << std::endl;

    

    mergesort("A", 0, 14);

    // bubblesort("A", 0, 14);

    for (int i = 0; i < 15; i++) {
        std::cout << getVal("A", i) << " ";
    }

    freeElem();

}
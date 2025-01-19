    #include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128
# define ZERO 0

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str)
{
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src)
{
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner)
{
    if (!owner->pokedexRoot)
    {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");
    VisitNodeFunc visit = printPokemonNode;

    switch (choice)
    {
    case 1:
        BFSGeneric(owner->pokedexRoot,visit);
        break;
    case 2:
        preOrderGeneric(owner->pokedexRoot,visit);
        break;
    case 3:
        inOrderGeneric(owner->pokedexRoot,visit);
        break;
    case 4:
        postOrderGeneric(owner->pokedexRoot,visit);
        break;
    case 5:
        displayAlphabetical(owner->pokedexRoot);
        break;
    default:
        printf("Invalid choice.\n");
    }
}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    if(!ownerHead) {
        printf("No existing Pokedexes.\n");
        return;
    }
    int pokeDex;
    OwnerNode *cur = ownerHead;
    OwnerNode *temp = ownerHead;
    // list owners
    printf("\nExisting Pokedexes:\n");
    printOwners();
    pokeDex = readIntSafe("Choose a Pokedex by number: ");
    for (int i = 1; i < pokeDex; i++)
        temp = temp->next;
    cur = temp;
    printf("\nEntering %s's Pokedex...\n", cur->ownerName);

    int subChoice;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", cur->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice)
        {
        case 1:
            addPokemon(cur);
            break;
        case 2:
            displayMenu(cur);
            break;
        case 3:
            freePokemon(cur);
            break;
        case 4:
            pokemonFight(cur);
            break;
        case 5:
            evolvePokemon(cur);
            break;
        case 6:
            printf("Back to Main Menu.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice)
        {
        case 1:
            openPokedexMenu();
            break;
        case 2:
            enterExistingPokedexMenu();
            break;
        case 3:
            deletePokedex();
            break;
        case 4:
            mergePokedexMenu();
            break;
        case 5:
            sortOwners();
            break;
        case 6:
            printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main()
{
    mainMenu();
    //freeAllOwners();
    return 0;
}
void openPokedexMenu() {
    char *name;
    int starterPokemine;
    printf("Your name:");
    name = getDynamicInput();
    printf("Choose Starter:\n"
        "1. Bulbasaur\n"
        "2. Charmander\n"
        "3. Squirtle\n");
    starterPokemine = readIntSafe("Your choice: ");
    PokemonNode *newPokemon;
    // Use the enum values directly to select the correct Pokémon
    switch (starterPokemine) {
        case 1:
            newPokemon = createPokemonNode("Bulbasaur");
        break;
        case 2:
            newPokemon = createPokemonNode("Charmander");
        break;
        case 3:
            newPokemon = createPokemonNode("Squirtle");
        break;
        default:
            printf("Invalid selection!\n");
        return;
    }
    OwnerNode *newOwner = createOwner(name,newPokemon);
    if(!newOwner) {
        printf("Memory allocation failed.\n");
        free(newPokemon);
        return;
    }
    linkOwnerInCircularList(newOwner);
    printf("New Pokedex created for %s with starter %s.\n",name,newPokemon->data->name);
}
PokemonNode *createPokemonNode(const char* name){
    // Loop through the pokedex array to find the matching name
    for (int i = 0; i < sizeof(pokedex) / sizeof(PokemonData); i++) {
        if (strcmp(pokedex[i].name, name) == 0) {
            PokemonNode* new_node = (PokemonNode*)malloc(sizeof(PokemonNode));
            new_node->data = (PokemonData*)malloc(sizeof(PokemonData));
            // Need to duplicate the name string
            new_node->data->name = myStrdup(pokedex[i].name); // Add this line
            new_node->data->id = pokedex[i].id;
            new_node->data->TYPE = pokedex[i].TYPE;
            new_node->data->hp = pokedex[i].hp;
            new_node->data->attack = pokedex[i].attack;
            new_node->data->CAN_EVOLVE = pokedex[i].CAN_EVOLVE;
            new_node->left = NULL;
            new_node->right = NULL;
            return new_node;
        }
    }
}
OwnerNode *createOwner(char *ownerName, PokemonNode *starter) {
    OwnerNode *node = (OwnerNode *)malloc(sizeof(OwnerNode));
    node->ownerName = ownerName;
    node->pokedexRoot = starter;
    node->next = NULL;
    node->prev = NULL;
    return node;
}
void linkOwnerInCircularList(OwnerNode *newOwner) {
    // Check if the linked list is empty-then the new owner is the head
    if(!ownerHead) {
        ownerHead = newOwner;
        //Circule linked list
        newOwner->next = newOwner;
        newOwner->prev = newOwner;
    }// Adding a new owner to the last in linked list
    else {
        OwnerNode *temp = ownerHead->prev;
        temp->next = newOwner;
        newOwner->prev = temp;
        newOwner->next = ownerHead;
        ownerHead->prev = newOwner;
    }
}
void addPokemon(OwnerNode *owner) {
    int pokemonId = readIntSafe("Enter ID to add: ");
    PokemonNode *newPokemon = createPokemonNode(pokedex[pokemonId-1].name);
    // Store the original root
    PokemonNode *originalRoot = owner->pokedexRoot;
    // Perform the insertion
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newPokemon);
    // Check if the Pokemon was actually added by comparing roots
    if (owner->pokedexRoot == originalRoot && originalRoot != NULL) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", newPokemon->data->id);
        // Free the unused new Pokemon node
        freePokemonNode(newPokemon);
    } else {
        printf("Pokemon %s (ID %d) added\n", newPokemon->data->name, newPokemon->data->id);
    }
}
PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode) {
    if (!root) {
        return newNode;
    }
    if(newNode->data->id < root->data->id) {
        root->left = insertPokemonNode(root->left, newNode);
    } else if(newNode->data->id > root->data->id) {
        root->right = insertPokemonNode(root->right, newNode);
    } else if(newNode->data->id == root->data->id) {
        return root;  // Pokemon already exists
    }
    return root;
}
void printOwners() {
        int counter = 1;
        OwnerNode *temp = ownerHead;
        do {
         printf("%d. %s\n",counter,temp->ownerName);
            temp = temp->next;
            counter++;
        }while (temp != ownerHead);
}
int sizeOfBinTree(PokemonNode *root) {
    if (root == NULL)
        return 0;
    return 1 + sizeOfBinTree(root->left) + sizeOfBinTree(root->right);
}
void BFSGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL)
        return;
    int size = sizeOfBinTree(root);
    PokemonNode **temp =(PokemonNode**)malloc(sizeof(PokemonNode*) * size);
    int front =0;
    int rear =0;
    temp[rear++] = root;

    while (front < rear) {
        PokemonNode *currentNode = temp[front++];
        visit(currentNode);
        if (currentNode->left != NULL )
            temp[rear++] = currentNode->left;

        if (currentNode->right != NULL )
            temp[rear++] = currentNode->right;
    }
}
void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL)
        return;
    visit(root);
    preOrderGeneric(root->left, visit);
    preOrderGeneric(root->right, visit);
}
void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL)
        return;
    inOrderGeneric(root->left, visit);
    visit(root);
    inOrderGeneric(root->right, visit);
}
void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL)
        return;
    postOrderGeneric(root->left, visit);
    postOrderGeneric(root->right, visit);
    visit(root);
}
void initNodeArray(NodeArray *na, int capacity) {
    na->capacity = capacity;
    na->size = 0;
    na->nodes = (PokemonNode**)malloc(sizeof(PokemonNode*) * capacity);
}
void addNode(NodeArray *na, PokemonNode *node) {
    na->nodes[na->size++] = node;
}// A recursive function to add all the nodes to the array na
void collectAll(PokemonNode *root, NodeArray *na) {
    if (root == NULL)
        return;
    addNode(na,root);
    collectAll(root->left,na);
    collectAll(root->right,na);
}
void displayAlphabetical(PokemonNode *root) {
    NodeArray *na = (NodeArray*)malloc(sizeof(NodeArray*));
    initNodeArray(na,sizeOfBinTree(root));
    collectAll(root,na);
    //A bubbleSort for Alphabetical
    for(int i = 0; i < na->capacity - 1; i++) {
        for(int j = 0; j < na->capacity - i - 1; j++) {
            if(strcmp(na->nodes[j]->data->name,na->nodes[j+1]->data->name) > 0) {
                //Swap pointers
                PokemonNode *temp = na->nodes[j];
                na->nodes[j] = na->nodes[j+1];
                na->nodes[j+1] = temp;
            }
        }
    }
    for(int i = 0; i < na->capacity; i++)
        printPokemonNode(na->nodes[i]);
}
void freePokemon(OwnerNode *owner) {
    int id = readIntSafe("Enter Pokemon ID to release: ");
    int removed = removePokemonByID(&owner->pokedexRoot,id);
    if(removed == -1) {
        printf("No Pokemon with ID %d found.",id);
    }else if(removed == 0) {
        printf("No Pokemon to release.\n");
    }else
        printf("Removing Pokemon %s (ID%d)\n",pokedex[id-1].name,id);
}
int removePokemonByID(PokemonNode **node, int id) {
    // if the tree is empty.
    if (node == NULL || *node == NULL)
        return 0;
    PokemonNode *root = *node;
    // go left
    if(root->data->id > id) {
        int result = removePokemonByID(&root->left, id);
        return result == 1 ? 1 : -1;
        // go right
    }else if(root->data->id < id) {
        int result = removePokemonByID(&root->right, id);
        return result == 1 ? 1 : -1;
        //has one child
    }else {
        PokemonNode *temp = NULL;
        if(!root->left) {
            temp = root->right;
            free(root);
            *node = temp;
            return 1;
        }else if(!root->right) {
            temp = root->left;
            free(root);
            *node = temp;
            return 1;
        }
            // Case 2: Node with two children
            // Find the in-order successor (smallest node in the right subtree)
            PokemonNode *successor = root->right;
            PokemonNode *successorParent = root;
            while (successor->left != NULL) {
                successorParent = successor;
                successor = successor->left;
            }
            // Replace root's data with successor's data
            root->data = successor->data;

            // Remove the successor node from the tree
            if (successorParent->left == successor) {
                successorParent->left = successor->right;
            } else {
                successorParent->right = successor->right;
            }
            free(successor); // Free the successor node
            return 1;
    }
    return -1;
}
void pokemonFight(OwnerNode *owner) {
    int firstId = readIntSafe("Enter ID of the first Pokemon: ");
    PokemonNode *firstPokemon = searchPokemon(owner->pokedexRoot,firstId);
    int secondId = readIntSafe("Enter ID of the second Pokemon: ");
    PokemonNode *secondPokemon = searchPokemon(owner->pokedexRoot,secondId);
    if(firstPokemon == NULL || secondPokemon == NULL) {
        printf("One or both Pokemon IDs not found.\n");
        return;
    }
    double a = (firstPokemon->data->attack*1.5)+(firstPokemon->data->hp*1.2);
    double b = (secondPokemon->data->attack*1.5)+(secondPokemon->data->hp*1.2);
    printf("Pokemon 1: %s (Score = %.2f)\n"
        "Pokemon 2: %s (Score = %.2f)\n",firstPokemon->data->name,a,secondPokemon->data->name,b);
    if(a > b) {
        printf("%s wins!\n",firstPokemon->data->name);
    }else if(a < b) {
        printf("%s wins!\n",secondPokemon->data->name);
    }else
        printf("It's a tie!\n");
}
PokemonNode *searchPokemon(PokemonNode *root, int id) {
    if (!root)
        return NULL;
    if (root->data->id > id) {
        return searchPokemon(root->left,id);
    }else if (root->data->id < id) {
        return searchPokemon(root->right,id);
    }
    return root;
}
void evolvePokemon(OwnerNode *owner) {
    int id = readIntSafe("Enter ID of Pokemon to evolve: ");
    PokemonNode *pokemon = searchPokemon(owner->pokedexRoot, id);
    if (pokemon == NULL) {
        printf("Pokemon with ID %d not found.\n", id);
        return;
    }
    if (!pokemon->data->CAN_EVOLVE) {
        printf("%s (ID %d) cannot evolve.\n", pokemon->data->name, id);
        return;
    }
    PokemonNode *evolvedVersion = searchPokemon(owner->pokedexRoot, id + 1);

    if (evolvedVersion != NULL) {
        printf("Evolution ID %d (%s) already in the Pokedex. Releasing %s (ID %d).\n",
               evolvedVersion->data->id, evolvedVersion->data->name,
               pokemon->data->name, id);
        removePokemonByID(&owner->pokedexRoot, id);
        return;
    }
    // Evolve the Pokemon
    printf("Pokemon evolved from %s (ID %d) ", pokemon->data->name, id);
    PokemonNode *newPokemon = createPokemonNode(pokedex[id].name);
    printf("to %s (ID %d).\n", newPokemon->data->name, newPokemon->data->id);
    // Remove the old Pokemon
    removePokemonByID(&owner->pokedexRoot, id);
    // Insert the evolved Pokemon into the BST
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newPokemon);
}
void deletePokedex() {
    int ownerId = readIntSafe("Choose a Pokedex to delete by number: ");
    if (!ownerHead) {
        printf("No existing Pokedexes to delete.");
        return;
    }
    // Ensure that the ownerId is valid
    OwnerNode *current = ownerHead;
    int count = 1; // to track the number of elements in the list
    while (current->next != ownerHead) {
        current = current->next;
        count++;
    }
    // Check if the input ownerId is within bounds
    if (ownerId < 1 || ownerId > count) {
        printf("Invalid Pokedex number.\n");
        return;
    }
    // Reset current to head again and traverse to the target ownerId
    current = ownerHead;
    for (int i = 1; i < ownerId; i++) {
        current = current->next;
    }
    removeOwnerFromCircularList(current);
}
void removeOwnerFromCircularList(OwnerNode *target) {
    if (!target) {
        printf("Invalid target node.\n");
        return;
    }
    // Case 1: Only one node in the list
    if (target->next == target) {
        ownerHead = NULL;
        freeOwnerNode(target);
        printf("Last Pokedex deleted.\n");
        return;
    }
    // Case 2: More than one node
    // Update the links
    target->prev->next = target->next;
    target->next->prev = target->prev;
    // If we're removing the head, update it
    if (target == ownerHead) {
        ownerHead = target->next;
    }
    // Store the name for the message
    char *ownerName = myStrdup(target->ownerName);
    // Free the node and its contents
    freeOwnerNode(target);
    printf("Pokedex for %s deleted.\n", ownerName);
    free(ownerName);
}
void freeOwnerNode(OwnerNode *owner) {
    if (!owner) {
        return;
    }
    // Free the Pokemon tree first
    if (owner->pokedexRoot) {
        freePokemonTree(owner->pokedexRoot);
        owner->pokedexRoot = NULL;
    }
    // Free the owner's name
    if (owner->ownerName) {
        free(owner->ownerName);
        owner->ownerName = NULL;
    }
    // Clear the links
    owner->next = NULL;
    owner->prev = NULL;
    // Finally free the owner node itself
    free(owner);
}
void freePokemonNode(PokemonNode *node) {
    if (node == NULL) {
        return;
    }
    if (node->data != NULL) {
        free(node->data->name);
        free(node->data);
    }
    free(node);
}

void freePokemonTree(PokemonNode *root) {
    if (root == NULL) {
        return;
    }
    freePokemonTree(root->left);
    freePokemonTree(root->right);
    freePokemonNode(root);
}
// Function to perform BFS and merge pokedexes
void mergePokeDex(OwnerNode *ownerA, OwnerNode *ownerB) {
    if (!ownerB->pokedexRoot) return;
    // Create a queue for BFS
    PokemonNode **queue = malloc(sizeOfBinTree(ownerB->pokedexRoot) * sizeof(PokemonNode*));  // Adjust size as needed
    int front = 0, rear = 0;
    // Start BFS from root of ownerB's pokedex
    queue[rear++] = ownerB->pokedexRoot;
    while (front < rear) {
        PokemonNode *current = queue[front++];
        // Create a new pokemon node and insert it into ownerA's pokedex
        PokemonNode *newPokemon = createPokemonNode(current->data->name);
        ownerA->pokedexRoot = insertPokemonNode(ownerA->pokedexRoot, newPokemon);
        // Add left and right children to queue if they exist
        if (current->left) {
            queue[rear++] = current->left;
        }
        if (current->right) {
            queue[rear++] = current->right;
        }
    }
    free(queue);
}
void mergePokedexMenu() {
    if(ownerHead->next == ownerHead) {
        printf("Not enough owners to merge.\n");
        return;
    }
    printf("Enter name of first owner: ");
    char *firstOwner = getDynamicInput();
    printf("Enter name of second owner: ");
    char *secondOwner = getDynamicInput();
    OwnerNode *OwnerA = findOwnerByName(firstOwner);
    OwnerNode *OwnerB = findOwnerByName(secondOwner);
    if (OwnerA == NULL || OwnerB == NULL) {
        printf("One or both owners not found.\n");
        free(firstOwner);
        free(secondOwner);
        return;
    }
    printf("Merging %s and %s...\n",firstOwner,secondOwner);
    mergePokeDex(OwnerA,OwnerB);
    printf("Merge completed.\n");
    removeOwnerFromCircularList(OwnerB);
    printf("Owner'%s' has been removed after metging.\n",secondOwner);
    free(firstOwner);
    free(secondOwner);
}
OwnerNode *findOwnerByName(const char *name) {
    OwnerNode *current = ownerHead;
    while (current->next != ownerHead) {
        if(strcmp(current->ownerName, name) == 0)
            return current;
        current = current->next;
    }
    if(strcmp(current->ownerName,name) == 0)
        return current;
    else
        return NULL;
}
void sortOwners() {
    if (ownerHead->next == ownerHead || ownerHead == NULL) {
        printf("0 or 1 owners only => no need to sort.\n");
    }
    int swapped = 0;
    OwnerNode *current ;
    OwnerNode *last = ownerHead->prev;
    do {
        swapped = 0;          // Reset swap flag for this pass
        current = ownerHead;  // Start from the head
        // Inner while: moves through adjacent pairs of nodes
        while (current != last) {
            // Compare current with next node
            if (strcmp(current->ownerName, current->next->ownerName) > 0) {
                // Swap their data
                swapOwnerData(current, current->next);
                swapped = 1;  // Mark that we did a swap
            }
            current = current->next;  // Move to next pair
        }
        last = current->prev;  // Shrink the unsorted portion
    } while (swapped);  // Outer loop: continue if any swaps happened
    printf("Owners sorted by name.\n");
}
void swapOwnerData(OwnerNode *a, OwnerNode *b) {
    char *tempName = a->ownerName;
    a->ownerName = b->ownerName;
    b->ownerName = tempName;

    PokemonNode *tempPokedex = a->pokedexRoot;
    a->pokedexRoot = b->pokedexRoot;
    b->pokedexRoot = tempPokedex;
}
void printOwnersCircular() {
    OwnerNode *current = ownerHead;
    printf("Enter direction (F or B):");
    char *direction  = getDynamicInput();
    int numPrints = readIntSafe("How many prints? ");
    if(*direction == 'f' || *direction == 'F') {
        for(int i = 1; i <= numPrints; i++) {
            printf("[%d]%s\n",i,current->ownerName);
            current = current->next;
        }
    }else if(*direction == 'b' || *direction == 'B') {
        for(int i = 1; i <= numPrints; i++) {
            printf("[%d]%s\n",i,current->ownerName);
            current = current->prev;
        }
    }
}

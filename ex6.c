    #include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128
# define ZERO 0
# define ONE 1
# define MAX_SIZE 20

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
            releasePokemon(cur);
            break;
        case 4:
            if(cur->pokedexRoot == NULL) {
                printf("Pokedex is empty.\n");
                break;
            }
            pokemonFight(cur);
            break;
        case 5:
            if(cur->pokedexRoot == NULL) {
                printf("Cannot evolve. Pokedex empty.\n");
                break;
            }
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
            if(ownerHead == NULL) {
                printf("No existing Pokedexes.\n");
                break;
            }
            enterExistingPokedexMenu();
            break;
        case 3:
            if(ownerHead == NULL) {
                printf("No existing Pokedexes to delete.\n");
                break;
            }
            printf("\n=== Delete a Pokedex ===\n");
            deletePokedex();
            break;
        case 4:
            if(ownerHead == NULL) {
                printf("Not enough owners to merge.\n");
                break;
            }
            printf("\n=== Merge Pokedexes ===\n");
            mergePokedexMenu();
            break;
        case 5:
            sortOwners();
            break;
        case 6:
            if(ownerHead == NULL) {
                printf("No owners.\n");
                break;
            }
            printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            freeAllOwners();
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main()
{
    mainMenu();
    freeAllOwners();
    return 0;
}
void openPokedexMenu() {
    int starterPokemine;
    printf("Your name:");
    char *name = getDynamicInput();
    if(findOwnerByName(name) != NULL) {
        printf(" Owner '%s' already exists. "
               "Not creating a new Pokedex.\n",name);
        free(name);
        return;
    }
    printf(" Choose Starter:\n"
        "1. Bulbasaur\n"
        "2. Charmander\n"
        "3. Squirtle\n");
    starterPokemine = readIntSafe("Your choice: ");
    PokemonNode *newPokemon;
    // Use the enum values directly to select the correct Pokémon
    switch (starterPokemine) {
        case 1: {
            newPokemon = createPokemonNode("Bulbasaur");
            free(name);
            break;
        }
        case 2: {
            newPokemon = createPokemonNode("Charmander");
            free(name);
            break;
        }
        case 3: {
            newPokemon = createPokemonNode("Squirtle");
            free(name);
            break;
        }
        default: {
            printf("Invalid selection!\n");
            free(name);
            return;
        }
    }
    OwnerNode *newOwner = createOwner(name,newPokemon);
    if(!newOwner) {
        printf("Memory allocation failed.\n");
        freeOwnerNode(newOwner);
        return;
    }
    linkOwnerInCircularList(newOwner);
    printf("New Pokedex created for %s with starter %s.\n",name,newPokemon->data->name);
    free(name);
}
PokemonNode *createPokemonNode(const char* name){
    PokemonNode* new_node = (PokemonNode*)malloc(sizeof(PokemonNode));
    if (!new_node) {
        printf("Memory allocation failed.\n");;
        return NULL;
    }
    // Loop through the pokedex array to find the matching name
    for (size_t i = 0; i < sizeof(pokedex) / sizeof(PokemonData); i++) {
        if (strcmp(pokedex[i].name, name) == 0) {
            new_node->data = (PokemonData*)malloc(sizeof(PokemonData));
            if (!new_node->data) {
                printf("Memory allocation failed.\n");
                freePokemonNode(new_node);
                return NULL;
            }
            // Need to duplicate the name string
            new_node->data->name = myStrdup(pokedex[i].name);
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
    freePokemonNode(new_node);
    return NULL;
}
OwnerNode *createOwner(char *ownerName, PokemonNode *starter) {
    OwnerNode *node = (OwnerNode *)malloc(sizeof(OwnerNode));
    if(node == NULL) {
        printf("Memory allocation failed.\n");
        freeOwnerNode(node);
        return NULL;
    }
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
//--------Adding Pokemon to the tree--------
void addPokemon(OwnerNode *owner) {
    int pokemonId = readIntSafe("Enter ID to add: ");
    if (pokemonId < 1 || pokemonId > 151) {
        printf("Invalid ID.\n");
        return;
    }// Checking if pokemon is already in pokedex
    if (searchPokemon(owner->pokedexRoot, pokemonId)) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", pokemonId);
        return;
    }//create new pokemon node in pokedex for new pokemon
    PokemonNode *newPokemon = createPokemonNode(pokedex[pokemonId-1].name);
    if(newPokemon == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    // Insert new pokemon into pokedex root
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newPokemon);
    printf("Pokemon %s (ID %d) added.\n", newPokemon->data->name, newPokemon->data->id);
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
//--------------- All the display methods ---------------
void BFSGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL)
        return;
    int size = sizeOfBinTree(root);
    PokemonNode **temp =(PokemonNode**)malloc(sizeof(PokemonNode*) * size);
    if(temp == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
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
    free(temp);
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
void initNodeArray(NodeArray *na, int cap) {
    na->nodes=(PokemonNode**)malloc(cap*sizeof(PokemonNode*));
    if(na->nodes==NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    na->size=0;
    na->capacity=cap;
}
void addNode(NodeArray *na, PokemonNode *node) {
    if(!na || !node) {
        printf("Null node provided.\n");
        return;
    }
    // If dynamic allocation trminate to limit, add more place
    if(na->size==na->capacity) {
        na->capacity*=2;
        PokemonNode** temp=(PokemonNode**)malloc(na->capacity*sizeof(PokemonNode*));
        if(temp==NULL) {
            printf("Memory allocation failed.\n");
            return;
        }
        // Copy existing nodes to new array
        for (int i = 0; i < na->size; i++) {
            temp[i] = na->nodes[i];
        }
        free(na->nodes); // Free old memory
        na->nodes = temp; // initialize pointer to temp
    }
    // Add new node to place size+1 in array
    na->nodes[na->size++]=node;
}// A recursive function to add all the nodes to the array na
void collectAll(PokemonNode *root, NodeArray *na) {
    if (root == NULL)
        return;
    addNode(na,root);
    collectAll(root->left,na);
    collectAll(root->right,na);
}
int compareByNameNode(const void *a, const void *b) {
    PokemonNode *nodeA = *(PokemonNode **)a;
    PokemonNode *nodeB = *(PokemonNode **)b;
    return strcmp(nodeA->data->name, nodeB->data->name);
}
void displayAlphabetical(PokemonNode *root) {
    if(root==NULL) {
        return;
    }
    NodeArray na;

    initNodeArray(&na,sizeOfBinTree(root));
    collectAll(root,&na);
    //A bubbleSort for Alphabetical
    // Sort by name
    qsort(na.nodes,na.size,sizeof(PokemonNode*),compareByNameNode);
    // Display sorted nodes
    for(int i=0;i<na.size;i++) {
        printPokemonNode(na.nodes[i]);
    }
    free(na.nodes); // Free allocated memory
}
// ------------ removing pokemon from the tree --------------
PokemonNode *removeNodeBST(PokemonNode *root, int id) {
    if(root==NULL) {
        return NULL;
    }
    // Search for the node
    if(id<root->data->id) {
        root->left = removeNodeBST(root->left, id);
        return root;
    }
    else if(id>root->data->id) {
        root->right = removeNodeBST(root->right, id);
        return root;
    }
    // Node found - handle deletion cases
    // Case 1: no children
    if(root->left==NULL && root->right==NULL) {
        freePokemonNode(root);
        return NULL;
    }

    // Case 2: one child
    if(root->left==NULL) {
        PokemonNode *temp = root->right;
        freePokemonNode(root);
        return temp;
    }
    if(root->right==NULL) {
        PokemonNode *temp = root->left;
        freePokemonNode(root);
        return temp;
    }
    // Case 3: Two children
    PokemonNode *temp = root->right;
    // Search minimum root in left to be the successor
    while(temp->left != NULL) {
        temp = temp->left;
    }
    // Copy successor data
    root->data->id = temp->data->id;
    // Free  old name before copying new name
    free(root->data->name);
    root->data->name = myStrdup(temp->data->name);
    root->data->TYPE = temp->data->TYPE;
    root->data->hp = temp->data->hp;
    root->data->attack = temp->data->attack;
    root->data->CAN_EVOLVE = temp->data->CAN_EVOLVE;
    // Delete successor from his original location
    root->right = removeNodeBST(root->right, temp->data->id);
    return root;
}

// Search BFS and remove pokemon by ID in BST
PokemonNode *removePokemonByID(PokemonNode *root, int id) {
    // Search for the node using BFS
    PokemonNode *nodeToRemove = searchPokemon(root, id);

    if (nodeToRemove == NULL) {
        // If the node with the given ID does not exist
        printf("Pokemon with ID %d not found.\n", id);
        return root;
    }
    // If the node exists, remove it using the removeNodeBST function
    printf(" Removing Pokemon %s (ID %d).\n",nodeToRemove->data->name, id);
    root = removeNodeBST(root, id);
    return root;
}


void releasePokemon(OwnerNode *owner) {
    // if pokedex of onwer is empty - there is nothing to remove
    if (owner->pokedexRoot == NULL) {
        printf(" No Pokemon to release.\n");
        return;
    }
    // ID to release
    int choice = readIntSafe(" Enter Pokemon ID to release:");

    PokemonNode *found= searchPokemon(owner->pokedexRoot, choice);
    if (found == NULL) {
        printf(" No Pokemon with ID %d found.\n", choice);
        return;
    }
    // Remove pokemon from pokedex by id
    owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, choice);
}

//----------- pokemon fight ------------
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
//--------- search for a pokemon the regular way-------------
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
//------------ evolve the pokemon-------------
void evolvePokemon(OwnerNode *owner) {
    if(owner->pokedexRoot==NULL) {
        printf(" Cannot evolve. Pokedex empty.\n");
        return;
    }
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
    // If evolve version is already in pokedex
    if (evolvedVersion != NULL) {
        printf("Evolution ID %d (%s) already in the Pokedex. Releasing %s (ID %d).\n",
               evolvedVersion->data->id, evolvedVersion->data->name,
               pokemon->data->name, id);
        owner->pokedexRoot= removePokemonByID(owner->pokedexRoot, id);
        return;
    }
    // Evolve the Pokemon
    printf("Pokemon evolved from %s (ID %d) ", pokemon->data->name, id);
    PokemonNode *newPokemon = createPokemonNode(pokedex[id].name);
    if(newPokemon==NULL) {
        printf("Evolution ID falied - memory allocation error.\n");
        return;
    }
    printf("to %s (ID %d).\n", newPokemon->data->name, newPokemon->data->id);
    // Remove the old Pokemon
    owner->pokedexRoot= removePokemonByID(owner->pokedexRoot, id);
    // Insert the evolved Pokemon into the BST
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newPokemon);
}
void deletePokedex() {
    printOwners();
    int ownerId = readIntSafe("Choose a Pokedex to delete by number: ");
    if (!ownerHead) {
        printf("No existing Pokedexes to delete.\n");
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
    printf("Deleting %s's entire Pokedex...\n",current->ownerName);
    removeOwnerFromCircularList(current);
    printf("Pokedex deleted.\n");
}
void removeOwnerFromCircularList(OwnerNode *target) {
    if(target==NULL) {
        printf("Invalid target node.\n");
        return;
    }
    // If only one owner exists
    if (target->next == target) {
        ownerHead = NULL;
        freeOwnerNode(target);
        return;
    }
    // If deleting head of list with multiple owners
    if (target == ownerHead) {
        ownerHead = target->next;
        // Update links
        target->prev->next = target->next;
        target->next->prev = target->prev;
        freeOwnerNode(target);
    }
    // Deleting any other node
    else {
        // Update links
        target->prev->next = target->next;
        target->next->prev = target->prev;
        freeOwnerNode(target);
    }
}
//---------- all the free functions ---------------
void freePokemonNode(PokemonNode *node) {
    if (node == NULL)
        return;
    if(node->data) {
        if(node->data->name)
            free(node->data->name); // Release pokimon's name
        free(node->data); // Release pokimon's data
    }
    free(node); // Release node himself
}

void freePokemonTree(PokemonNode *root) {
    if (root == NULL)
        return;
    freePokemonTree(root->left);  // Release left tree
    freePokemonTree(root->right); // Release right tree
    freePokemonNode(root);        // Release root
}

void freeOwnerNode(OwnerNode *owner) {
    if (owner == NULL)
        return;
    free(owner->ownerName);               // Release owner's name
    freePokemonTree(owner->pokedexRoot);  // release pokedex root
    free(owner);                          // Release onwer
}

void freeAllOwners() {
    if(ownerHead==NULL)
        return;
    OwnerNode *current = ownerHead; // Set current to head of link list
    OwnerNode *next = NULL;
    do {
        next = current->next;  // Saving next node
        freeOwnerNode(current);          // Release current node
        current = next;                  // Cuntinue to next node
    }while(current!=ownerHead && current!=NULL);
    ownerHead=NULL;
}
//-------------- Function to perform BFS and merge pokedexes -----------
void mergePokeDex(OwnerNode *ownerA, OwnerNode *ownerB) {
    if (!ownerB->pokedexRoot) return;
    // Create a queue for BFS
    PokemonNode **queue = malloc(sizeOfBinTree(ownerB->pokedexRoot) * sizeof(PokemonNode*));
    if(queue == NULL) {
        printf("Memory allocation error.\n");
        free(queue);
        return;
    }
    int front = 0, rear = 0;
    // Start BFS from root of ownerB's pokedex
    queue[rear++] = ownerB->pokedexRoot;
    while (front < rear) {
        PokemonNode *current = queue[front++];
        // Create a new pokemon node and insert it into ownerA's pokedex
        PokemonNode *newPokemon = createPokemonNode(current->data->name);
        if (newPokemon == NULL) {
            printf("Memory allocation failed for new Pokemon.\n");
            continue;
        }
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
    // Validation check -cannot merge pokedex with itself
    if(strcmp(firstOwner, secondOwner)==0) {
        printf("Cannot merge Pokedexs with the same name.\n");
        free(firstOwner);
        free(secondOwner);
        return;
    }// Finding two requested names in link list
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
    printf("Owner '%s' has been removed after merging.\n",secondOwner);
    free(firstOwner);
    free(secondOwner);
}
OwnerNode *findOwnerByName(const char *name) {
    if (!ownerHead) return NULL;  // Empty list
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
//--------------- Sorting Owners --------------
void sortOwners() {
    if (ownerHead == NULL ||ownerHead->next == ownerHead ) {
        printf("0 or 1 owners only => no need to sort.\n");
        return;
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
    // Swap pointers of names
    char *tempName= a->ownerName;
    a->ownerName= b->ownerName;
    b->ownerName= tempName;
    // Swap pointers to pokedex root
    PokemonNode *tempPokedexRoot= a->pokedexRoot;
    a->pokedexRoot= b->pokedexRoot;
    b->pokedexRoot= tempPokedexRoot;
}
//--------Printing Owners in a Circle---------
void printOwnersCircular(void) {
    if(ownerHead==NULL) {
        printf(" No owners.\n");
        return;
    }
    printf(" Enter direction (F or B): ");
    char *direction=getDynamicInput();
    while(strcmp(direction,"F")!=0 && strcmp(direction,"B")!=0 && strcmp(direction,"f")!=0
        && strcmp(direction,"b")!=0) {
        printf("Invalid direction, must by F or B.\n");
        free(direction);
        printf(" Enter direction (F or B): ");
        direction=getDynamicInput();
        }
    int numberOfPrints=readIntSafe("How many prints? ");
    OwnerNode *current=ownerHead;
    if (strcmp(direction,"F")==0 || strcmp(direction,"f")==0) {
        for(int i=1; i<=numberOfPrints; i++) {
            printf("[%d] %s\n",i, current->ownerName);
            current=current->next;
        }
    }
    else if(strcmp(direction,"B")==0 || strcmp(direction,"b")==0) {
        for(int i=1; i<=numberOfPrints; i++) {
            printf("[%d] %s\n",i, current->ownerName);
            current=current->prev;
        }
    }
    free(direction);
}

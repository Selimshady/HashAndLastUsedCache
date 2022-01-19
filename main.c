#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 100 // Tmp Struct Array Size
#define NSIZE 20 // City , person name surname size.

typedef struct data{ // To keep the data cache and disk.
    char id[6];
    char name[NSIZE];
    char surname[NSIZE];
    char year[5];
    char city[NSIZE];
    struct data* next; // will be NULL for DISK.
}DATA;

typedef struct hashNode{ // Hash nodes
    char id[6];
    int turn;
    int deleted; // 1 deleted 0 not deleted
}HN;

typedef struct hashTable{ // HashTable
    int size;
    HN** node;
}HT;

typedef struct cacheHead{ //  CacheHead
    int cacheSize; //Total size
    DATA* next;
}CH;

int readFileIntoStruct(DATA**); // Read sample.txt file to struct.
int findPrimeNum(float); // Find the min prime num.
int searchOnHashTable(int,char*,HT*); // Look for the id in the hash. If there is return turn, if not turn 0.
int hornersMethod(char*); //Horner's method returns key for id.
int doubleHash(char*,int,HT*); //Hash method.
int insertCache(CH*,char*,DATA*); // Insert new values into head.
void updateCache(int,CH*); // If id is there, update cache.
void updateHash(int,HT*,CH*); // If id is in hash, update hash.
void afterBroughtUpdateHash(CH*,HT*); // After adding new id to hash, update hash.
void afterBroughtUpdateCache(char*,CH*,DATA*); // Add into cache, and update it.
void addToHash(int,char*,HT*); // Add new id to hash.
void printHashTable(HT*); // Print Hash.
void printCache(CH*); // Print Cache
void printInfo(int,CH*); // Print info from cache.

int main(int argc, char **argv)
{
    int diskSize,i;
    int key,result;
   
    
	DATA* disk;
    diskSize = readFileIntoStruct(&disk); // To keep all infos in hdd.
    
    CH* cacheHead = (CH*)malloc(sizeof(CH)); //Initialize cache.
    printf("Cache uzunlugunu giriniz: ");
    scanf("%d",&cacheHead->cacheSize);
    cacheHead->next = NULL;

    
    HT* hashTable = (HT*)malloc(sizeof(HT)); //Initialize hash.
    hashTable->size = findPrimeNum((float)cacheHead->cacheSize*5/3); // Find prime num.
    hashTable->node = (HN**)malloc(sizeof(HN*)*hashTable->size);
    for(i=0;i<hashTable->size;i++)
        hashTable->node[i] = NULL;
        

    for(i=0;i<diskSize;i++) {
        key = hornersMethod(disk[i].id); // Find key for id.
        result = searchOnHashTable(key,disk[i].id,hashTable); // If 0 It is not in hash, if another it is in hash and it is its place on cache.
        printf("\n\n------------------------------------------------%d.step------------------------------------------------------\n\n",i);
        if(result) {
            printf("%s is found %d.place in cache.\n\n",disk[i].id,result);
            printInfo(result,cacheHead); // print info from cache.
            updateHash(result,hashTable,cacheHead); // Update only turns.
            updateCache(result,cacheHead); // Bring the node to head.
        }
        else  {
            printf("%s is not found in cache.\n\n",disk[i].id);
            addToHash(key,disk[i].id,hashTable); // Find true place and , add new values/node to hash. make its turn 1.
            afterBroughtUpdateHash(cacheHead,hashTable); // Increment all in cache on hash and free last if cache is longer than size.
            afterBroughtUpdateCache(disk[i].id,cacheHead,disk); 
            //printInfo(1,cacheHead); // print info from cache after adding it into cache.
        }
        printf("Hash ve cache guncellendi.");
        printHashTable(hashTable);
        //printCache(cacheHead);
    }
    
    free(disk);
    free(cacheHead);
    for(i=0;i<hashTable->size;i++)
        free(hashTable->node[i]);
    free(hashTable);
	return 0;
}

void printHashTable(HT* hashTable) {
    int i=0;
    printf("\n\n");
    printf("%6s | %6s | %6s | %6s\n","NO","ID","TURN","DELETED");
    for(i=0;i<hashTable->size;i++) {
        printf("%6d |",i);
        if(hashTable->node[i] != NULL) {
            printf(" %6s | %6d | %6d\n",hashTable->node[i]->id,hashTable->node[i]->turn,hashTable->node[i]->deleted);
        }
        else
            printf("----------NULL----------\n");
    }
    printf("\n");
}

/*void printCache(CH* cache) {
    DATA* tmpNode = cache->next;
    while(tmpNode) {
        printf("%s %s %s %s %s\t->\t",tmpNode->id,tmpNode->name,tmpNode->surname,tmpNode->year,tmpNode->city);
        tmpNode = tmpNode->next;
    }
    free(tmpNode);
}*/

void afterBroughtUpdateHash(CH* cache,HT* hashTable) {
    int key,j;
    int hash1,hash2,result;
    DATA* prevNode;
    DATA* tmpNode = cache->next;
    
    while(tmpNode) { // Update the id that is in cache on hash.
        key = hornersMethod(tmpNode->id);
        j=0;
        hash1 = key%hashTable->size;
        hash2 = j*(1 + key%(hashTable->size-1));
        result = (hash1 + hash2)%hashTable->size;
        while(strcmp(hashTable->node[result]->id,tmpNode->id)) {
            j++;
            hash2= j*(1 + key%(hashTable->size-1));
            result = (hash1+ hash2)%hashTable->size;
        }
        if(++(hashTable->node[result]->turn) == cache->cacheSize+1) { // Increment it 1 and if it is bigger than size delete it.
            printf("Cache dolu oldugu icin son eleman: %s cache'den cikarildi.\n\n",hashTable->node[result]->id);
            hashTable->node[result]->deleted=1;
            hashTable->node[result]->turn = 0;
            free(tmpNode); // Cache is limited to size.
            if(cache->cacheSize == 1) //If size == 1 , there is no prevNode
                cache->next= NULL;
            else
                prevNode->next = NULL; // Last will always show NULL.
            return;
        }
        prevNode = tmpNode;
        tmpNode = tmpNode->next;
    }
    free(tmpNode);
}
void afterBroughtUpdateCache(char* id,CH* cache,DATA* disk) { // Create new node to insert cache.
    int i=0;
    DATA* tmpNode = (DATA*)malloc(sizeof(DATA));
    while(strcmp(disk[i].id,id)) // Find infos in disk.
        i++;
    strcpy(tmpNode->id,id);
    strcpy(tmpNode->city,disk[i].city);
    strcpy(tmpNode->name,disk[i].name);
    strcpy(tmpNode->surname,disk[i].surname);
    strcpy(tmpNode->year,disk[i].year);
    tmpNode->next = NULL; // 
    
    tmpNode->next = cache->next;
    cache->next = tmpNode;
}

void addToHash(int key,char* id,HT* hashTable) {
    int result,i=0;
    int hash1 = key%hashTable->size; // no need to calculate key again. so pass it as parameter.
    int hash2 = i*(1 + key%(hashTable->size-1));
    result = (hash1 + hash2)%hashTable->size;
    while(hashTable->node[result] != NULL && hashTable->node[result]->deleted == 0) { // Find the true place for new node. check if it is deleted or null.
        i++;
        hash2 = i*(1 + key%(hashTable->size-1));
        result = (hash1 + hash2)%hashTable->size;
    }
    if(hashTable->node[result] == NULL) 
        hashTable->node[result] = (HN*)malloc(sizeof(HN)); // If null create node.
    strcpy(hashTable->node[result]->id,id); // pass the id
    hashTable->node[result]->turn = 1; // make turn 1
    hashTable->node[result]->deleted = 0; // if deleted change it to not deleted.
}

int searchOnHashTable(int key,char* id,HT* hashTable) {
    int result,i=0,tmp;
    int hash1 = key%hashTable->size;
    int hash2 = i*(1 + key%(hashTable->size-1));
    result = (hash1 + hash2)%hashTable->size;
    
    while(i<hashTable->size && hashTable->node[result] != NULL && strcmp(hashTable->node[result]->id,id)) { // Check if full, check if null, check if it's in the hash.
        i++;
        hash2 = i*(1 + key%(hashTable->size-1));    
        result = (hash1 + hash2)%hashTable->size;
    }
    if(i == hashTable->size || hashTable->node[result] == NULL) // If null or full, return it's not in the hash.
        return 0;
    if(hashTable->node[result]->deleted == 1) // If id is in the hash, check if it is deleted or not.
        return 0;
    else {
        tmp = hashTable->node[result]->turn;
        hashTable->node[result]->turn = 1; // Make turn 1.
        return tmp;
    }
}

void updateHash(int turn,HT* hashTable,CH* cache) {
    int i=0,key,j;
    int hash1,hash2,result;
    
    DATA* tmpNode = cache->next; // update all nodes in cache on hash until turn of last recently used node.
    for(i=1;i<turn;i++) { //Checks if it is before last recently used node.
        
        key = hornersMethod(tmpNode->id);
        j=0;
        hash1 = key%hashTable->size;
        hash2 = j*(1 + key%(hashTable->size-1));
        result = (hash1 + hash2)%hashTable->size;   
    
        while(strcmp(hashTable->node[result]->id,tmpNode->id)) { // Find the id on hash. We use same method to find as to add into hash so that we could find it faster.
            j++;
            hash2 = j*(1 + key%(hashTable->size-1));
            result = (hash1 + hash2)%hashTable->size;
        }
        hashTable->node[result]->turn++; // When it is same, add turn 1. There will be no overflow because there is no new value.
        tmpNode = tmpNode->next;
    }
}

void updateCache(int turn,CH* cache) {
    int i=0;
    DATA* tmpNode;
    DATA* prevNode;
    if(turn !=1) { // If it is already at the head, no need for change.
        prevNode = cache->next;
        tmpNode = prevNode->next;
        for(i=2;i<turn;i++) { //go to the right place.
            prevNode = tmpNode;
            tmpNode = tmpNode->next;
        }
        prevNode->next = tmpNode->next; // link prev and next
        tmpNode->next = cache->next; // take head as your next
        cache->next = tmpNode; // be head.
    }
}

void printInfo(int turn,CH* cache) {
    int i = 0;
    DATA* tmpNode = cache->next;
    for(i=1;i<turn;i++) { // Access to true node in cache.
        tmpNode = tmpNode->next;
    }
    printf("The info: %s %s %s %s %s\n\n",tmpNode->id,tmpNode->name,tmpNode->surname,tmpNode->year,tmpNode->city);
}

int hornersMethod(char* id) {
    int key = 0;
    int i;
    int coEf = 1;
    
    for(i=0;i<5;i++) {
        key += coEf * (id[4-i] - '0' + 1);
        coEf *=31;
    }
    return key;
}

int findPrimeNum(float x) {
    int i,flag = 1;
    int y =x;
    if(x>y)
        y++;
    if(!(y%2))
        y++;
    do{
        i=3;
        while((i<y/2) && (y%i))
            i++;
        if(i >= y/2)
            flag = 0;
        else
            y+=2;
    }while(flag);
    
    return y;
}

int readFileIntoStruct(DATA **ptrStruct) {
    
    FILE *fp; // To keep file pointer
    char *line = (char*)malloc(sizeof(char)*SIZE);//To get the line and values//Allocating memory
    DATA tmp[SIZE];
    int size = 0,i;
    DATA *tmpStruct; // Temporary struct
 
    if((fp = fopen("test.txt","r")) == NULL) {//Opening the file to read
        printf("FILE ACCSESS ERROR!");
        exit(-1);//If there is a problem, exit the program
    } 
    
    while(fgets(line,SIZE,fp) != NULL) {//Read the line
        sscanf(line, "%s %s %s %s %s" ,tmp[size].id,tmp[size].name,tmp[size].surname,tmp[size].year,tmp[size].city);
        tmp[size].next = NULL; // Data shouldn't point anything in Disk.
        size++; //Counts the lines
    }

    tmpStruct = (DATA*)malloc(sizeof(DATA)*size); // Allocating the real struct
    
    for(i=0;i<size;i++) {
        tmpStruct[i] = tmp[i];
    }
    
    *ptrStruct = tmpStruct; // To pass address to main struct
    
    fclose(fp); //close the file
    free(line); // free the line address
    return size; //Return the length
}
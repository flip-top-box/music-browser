#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *genre;
    char *artist;
    char *title;
    char *trackid;
    short popularity;
    bool shown;
} Song;

typedef struct SongArrayList {
    Song *songs;         
    size_t size;         
    size_t capacity;     
} SongArrayList;

typedef struct {
    char *genre;         
    int count;
} GenreInfo;

typedef struct LinkListNode {
   GenreInfo *data;
   struct LinkListNode * next;
} LinkListNode ;

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//FUNCTIONS
bool readInSongs(SongArrayList *songList);
void printSongList(SongArrayList * list, int index);
void generateUniqueGenres(LinkListNode ** ptrUniqueGenresLL, SongArrayList songList);
void printNavBar(LinkListNode * ptrUniqueGenresLL);
bool isInt(const char *str);
void printSongsByGenre(const SongArrayList *songList, const LinkListNode *uniqueGenresLL, int index, int songIndex);
int getGenreCount(const LinkListNode *node, int index);
void printGenreFirst(LinkListNode * headUniqueGenreLL);
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

struct SongArrayList* createArrayList(int initialCapacity) {
    SongArrayList* list = (struct SongArrayList*)malloc(sizeof(struct SongArrayList));

    if (list == NULL) {
        printf("Memory allocation for ArrayList failed.\n");
        return NULL;
    }

    list->songs = (Song *)malloc(initialCapacity * sizeof(Song));

    if (list->songs == NULL) {
        printf("Memory allocation for int array failed.\n");
        free(list);
        return NULL;
    }

    list->size = 0;
    list->capacity = initialCapacity;

    return list;
}

void freeSongArrayList(SongArrayList *songList) {
    if (songList == NULL) {
        return; 
    }

    for (size_t i = 0; i < songList->size; i++) {
        free(songList->songs[i].genre);
        free(songList->songs[i].artist);
        free(songList->songs[i].title);
        free(songList->songs[i].trackid);
    }

    free(songList->songs);
    songList->size = 0;
    songList->capacity = 0;
}

void freeLinkList(LinkListNode *head) {
    while (head != NULL) {
        LinkListNode *temp = head;
        head = head->next;
        free(temp->data->genre);
        free(temp->data);
        free(temp);
    }
}

// *************************************************************************************
// ****************************   MAIN MAIN MAIN   *************************************
// *************************************************************************************

int main() {
    char usrStr[1];
    int startIndex = 1;

    SongArrayList *songList;
    songList = createArrayList(10);
    //READ IN AND PRINT SONGLIST
    readInSongs(songList);



    //GENERATE THE GENRES
    LinkListNode *headUniqueGenreLL = NULL;
    generateUniqueGenres(&headUniqueGenreLL, *songList);
    printGenreFirst(headUniqueGenreLL);
    printf("Listing songs (%ld):\n", songList->size+1);
    printSongList(songList, startIndex);
    printNavBar(headUniqueGenreLL);


    //GET USER INPUT
    scanf("%s", usrStr);


    // Need browsing and navigation loop
    while (usrStr[0] != 'q') {
    if (isInt(usrStr) == 0) {
        char c = usrStr[0];


        switch (c)
        {
        case 'n':
            startIndex += 10;
            printSongList(songList, startIndex);
            break;

        case 'p':
            if (startIndex == 1){
                printSongList(songList, startIndex);
            } else {
                startIndex -= 10;
                printSongList(songList, startIndex);
            }
            break;

        case 's':
            startIndex = 1;
            printSongList(songList, startIndex);
            break;

        case 'e':
            startIndex = (songList->size + 1) - 10;
            printSongList(songList, startIndex);
            break;

        case 'r':
            startIndex = 1;
            printSongList(songList, startIndex);
        default:
            printf("Invalid command.");
            break;
        }
    } 
    else {
        int choice = usrStr[0] - '0';
        int startIndex2 = 1;
        int returnTo = 0;

        printf("Adjusting filtering\n");
        printSongsByGenre(songList, headUniqueGenreLL, choice, startIndex2);
        printNavBar(headUniqueGenreLL);
        scanf("%s", usrStr);

        char c2 = usrStr[0];



        while (c2 != 'q' && returnTo != 1) {
            if (isdigit(usrStr[0])) {
                break;
            }
            switch (c2)
            {
            case 'n':
                startIndex2 += 10;
                printSongsByGenre(songList, headUniqueGenreLL, choice, startIndex2);
                break;

            case 'p':
                startIndex2 -= 10;
                printSongsByGenre(songList, headUniqueGenreLL, choice, startIndex2);
                break;

            case 's':
                startIndex2 = 1;
                printSongsByGenre(songList, headUniqueGenreLL, choice, startIndex2);
                break;

            case 'e':
                startIndex2 = (getGenreCount(headUniqueGenreLL, choice) + 1) - 10;
                printSongsByGenre(songList, headUniqueGenreLL, choice, startIndex2);
                break;

            case 'r':
                if (startIndex2 == 1) {
                    returnTo = 1;
                    printSongList(songList, 1);
                    break;
                }
                else {
                    choice = 0;
                    startIndex2 = 1;
                    printSongsByGenre(songList, headUniqueGenreLL, choice, startIndex2);
                    break;
                }
                break;


            default:
                printf("Invalid Entry\n");
                break;
            }

            if (c2 != 'q' && returnTo != 1) {
                if (isdigit(usrStr[0])) {
                    break;
                } else {
                    printNavBar(headUniqueGenreLL);
                    scanf("%s", usrStr);
                    c2 = usrStr[0];
                    }
            }

        }
    } 
    if (usrStr[0] != 'q'){
        if (!isdigit(usrStr[0])) {
        printNavBar(headUniqueGenreLL);
        scanf("%s", usrStr);
        }
    }

    } 

    freeLinkList(headUniqueGenreLL);
    freeSongArrayList(songList);


    return 0;
}
//#################################################
//***END MAIN***END MAIN***END MAIN***END MAIN****# 
//#################################################

bool readInSongs(SongArrayList *list) {

    FILE *file = fopen("songs2.5k.txt", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return false;
    }

    char line[256];
    char *saveptr;


    while (fgets(line, sizeof(line), file) != NULL) {

        if (strncmp(line, "genre", 5) == 0) {
            continue;
        }

        char *genre = strdup(strtok_r(line, "\t", &saveptr));
        char *artist = strdup(strtok_r(NULL, "\t", &saveptr));
        char *title = strdup(strtok_r(NULL, "\t", &saveptr));
        char *trackid = strdup(strtok_r(NULL, "\t", &saveptr));
        short popularity = atoi(strtok_r(NULL, "\t", &saveptr));
        bool shown = false;

        Song newSong = {genre, artist, title, trackid, popularity, shown};

        if (list->size == list->capacity) {
            list->capacity *= 2;
            list->songs = realloc(list->songs, list->capacity * sizeof(Song));
        }

        list->songs[list->size++] = newSong;
    }

    fclose(file);
    return true;
}


void printSongList(SongArrayList *list, int index) {

    if (list == NULL || index < 0 || 
    index > list->size) {
        printf("Invalid print index\n");
        return;
    }

    int i = index;
    while (list->size >= index && (i < (index + 10))) {
        printf("\t%d.	%s	-	%s	-	%s	\n", i, list->songs[i-1].genre,
        list->songs[i-1].artist, list->songs[i-1].title);

        i++;
    }
}

void generateUniqueGenres(LinkListNode ** ptrUniqueGenresLL, SongArrayList songList) {
    *ptrUniqueGenresLL = NULL;

    for (size_t i = 0; i < songList.size; i++){
        LinkListNode * current = *ptrUniqueGenresLL;

        while (current != NULL) {
            if (strcmp(current->data->genre, songList.songs[i].genre) == 0) {
                current->data->count++;
                break;
            }
            current = current->next;
        }

        if (current == NULL) {
            LinkListNode *newNode = malloc(sizeof(LinkListNode));
            if (newNode == NULL) {
                printf("newNode memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }

            GenreInfo *newGenreInfo = malloc(sizeof(GenreInfo));
            if (newGenreInfo == NULL) {
                printf("newGenreInfo memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }

            newGenreInfo->genre = strdup(songList.songs[i].genre);
            newGenreInfo->count = 1;

            newNode->data = newGenreInfo;
            newNode->next = *ptrUniqueGenresLL;
            *ptrUniqueGenresLL = newNode;
        }
    }
}

void printGenreFirst(LinkListNode * headUniqueGenreLL) {
    printf("Genre Totals:\n");

    if (headUniqueGenreLL == NULL) {
        return;  // Empty list
    }

    // Create a stack to store elements
    LinkListNode *stack = NULL;

    // Push elements onto the stack
    LinkListNode *current = headUniqueGenreLL;
    while (current != NULL) {
        LinkListNode *temp = malloc(sizeof(LinkListNode));
        if (temp == NULL) {
            // Handle memory allocation error
            exit(EXIT_FAILURE);
        }

        temp->data = current->data;
        temp->next = stack;
        stack = temp;

        current = current->next;
    }

    while (stack != NULL) {
        printf("    %s  %d\n", stack->data->genre, stack->data->count);
        stack = stack->next;
    }

    printf("------------------------------------\n");
}

void printNavBar(LinkListNode * headUniqueGenreLL) {
    printf("-------------------------------------------------------------------------------------------------------\n");
    printf("Navaigation:    ");
    printf("n for next, p for previous, and q for quit, s for start, e for end, r for reset filter.\n");
    printf("Filters:    ");
    int i = 0;

    if (headUniqueGenreLL == NULL) {
        return;  // Empty list
    }

    // Create a stack to store elements
    LinkListNode *stack = NULL;

    // Push elements onto the stack
    LinkListNode *current = headUniqueGenreLL;
    while (current != NULL) {
        LinkListNode *temp = malloc(sizeof(LinkListNode));
        if (temp == NULL) {
            // Handle memory allocation error
            exit(EXIT_FAILURE);
        }

        temp->data = current->data;
        temp->next = stack;
        stack = temp;

        current = current->next;
    }


    while (stack != NULL) {
        if (i == 0) {
             printf("[%d] %s (%d)", i,
            stack->data->genre, stack->data->count);

        } else {
            printf("    -   [%d] %s (%d)", i,
            stack->data->genre, stack->data->count);
        }

        //LinkListNode *temp = stack;
        stack = stack->next;
        //free(temp->data->genre);
        //free(temp->data);
        //free(temp);
        i++;
    }
    printf("\n");
}

bool isInt(const char *str) {
    char *endptr;
    strtol(str, &endptr, 10);
    return *endptr == '\0';
}


void printSongsByGenre(const SongArrayList *songList, const LinkListNode *uniqueGenresLL, int genreIndex, int songIndex) {
    if (songList == NULL || uniqueGenresLL == NULL) {
        printf("Invalid input.\n");
        return;
    }

    // Create a stack to store elements
    LinkListNode *stack = NULL;

    // Push elements onto the stack
    const LinkListNode *current = uniqueGenresLL;
    while (current != NULL) {
        LinkListNode *temp = malloc(sizeof(LinkListNode));
        if (temp == NULL) {
            // Handle memory allocation error
            exit(EXIT_FAILURE);
        }

        temp->data = current->data;
        temp->next = stack;
        stack = temp;

        current = current->next;
    }

    // Find the genre in the linked list based on the index
    const LinkListNode *currentGenre = stack;
    for (int i = 0; i < genreIndex && currentGenre != NULL; i++) {
        currentGenre = currentGenre->next;
        if (genreIndex == 0) {
            currentGenre = currentGenre;
            break;
        }
    }

    // Print songs from the specified genre
    if (currentGenre != NULL) {
        

        int count = 0;
        int endIndex = songIndex + 10;


        for (int j = 0; j < songList->size; j++) {
            if (strcmp(songList->songs[j].genre, currentGenre->data->genre) == 0) {
                count++;
                if (count >= endIndex) {
                    break;
                }

                if (count >= songIndex){
                printf("\t%d.	%s	-	%s	-	%s	\n", count, songList->songs[j].genre,
                songList->songs[j].artist, songList->songs[j].title);
                }

            }
        }

    } else {
        printf("Genre not found in the linked list.\n");
    }
}

int getGenreCount(const LinkListNode *node, int index) {
    if (node == NULL || node->data == NULL) {
        printf("Invalid input.\n");
        return 0;
    }

      LinkListNode *stack = NULL;

  // Push elements onto the stack
  const LinkListNode *current = node;
  while (current != NULL) {
      LinkListNode *temp = malloc(sizeof(LinkListNode));
      if (temp == NULL) {
          // Handle memory allocation error
          exit(EXIT_FAILURE);
      }

      temp->data = current->data;
      temp->next = stack;
      stack = temp;

      current = current->next;
  }

  // Find the genre in the linked list based on the index
  const LinkListNode *currentGenre = stack;
  
    for (int i = 0; i < index; i++) {
      if (index == 0) {
        currentGenre = currentGenre;
        break;
      } else {
        currentGenre = currentGenre->next;
      }
    }

    return currentGenre->data->count;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a struct for movie
typedef struct movie {
    char *title; // Pointer to dynamically allocated movie title
    int year; // Year of release
    char languages[5][20]; // Max 5 languages, each 20 chars max
    float rating; // Movie rating
    struct movie *next; // Pointer to next movie in linked list
} movie;

// Function to create a new movie node
movie* createMovie(char *line) {
    // Allocate memory for a new movie node
    movie *newMovie = malloc(sizeof(movie));
    if (!newMovie) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    // Tokenize the line using ',' delimiter
    char *token = strtok(line, ",");
    newMovie->title = strdup(token); // Duplicate the title string
    
    token = strtok(NULL, ",");
    newMovie->year = atoi(token); // Convert year string to integer
    
    token = strtok(NULL, ",");
    
    // Extract languages from [Language1;Language2] format
    char langStr[100];
    strcpy(langStr, token + 1); // Remove leading [
    langStr[strlen(langStr) - 1] = '\0'; // Remove trailing ]
    
    int langCount = 0;
    char *langToken = strtok(langStr, ";");
    while (langToken != NULL && langCount < 5) {
        strcpy(newMovie->languages[langCount++], langToken); // Store each language
        langToken = strtok(NULL, ";");
    }
    
    token = strtok(NULL, ",");
    newMovie->rating = strtof(token, NULL); // Convert rating string to float
    
    newMovie->next = NULL; // Initialize next pointer to NULL
    return newMovie;
}

// Function to process file and store movies in a linked list
movie* processMovieFile(char* filePath) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        printf("Could not open file %s\n", filePath);
        exit(1);
    }
    
    char *line = NULL;
    size_t len = 0;
    size_t read;
    movie *head = NULL, *tail = NULL;
    
    // Skip header line
    getline(&line, &len, file);
    
    int movieCount = 0;
    while ((read = getline(&line, &len, file)) != -1) {
        movie *newMovie = createMovie(line);
        movieCount++;
        
        if (head == NULL) {
            head = newMovie;
            tail = newMovie;
        } else {
            tail->next = newMovie;
            tail = newMovie;
        }
    }
    
    free(line);
    fclose(file);
    
    printf("Processed file %s and parsed data for %d movies\n", filePath, movieCount);
    return head;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <movies.csv>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    movie *movies = processMovieFile(argv[1]);
    return EXIT_SUCCESS;
}

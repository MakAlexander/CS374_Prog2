/*
 My program reads a CSV file containing movie data, processes the data into a linked list of movie structures,
 and allows the user to interact with the data through a menu.
 The user can:
 1. View movies released in a specific year.
 2. See the highest-rated movie for each year.
 3. Find movies available in a specific language.
 4. Exit the program.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a struct for movie
struct movie {
    char *title;
    int year;
    char languages[5][20];
    float rating;
    struct movie *next;
};

// Function to tokenize data from movie csv
struct movie* createMovie(char *line) {
    struct movie *newMovie = malloc(sizeof(struct movie));
    if (!newMovie) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    char *token;
    char *csv_ptr;  //pointer for CSV tokenization

    // Tokenize Title
    token = strtok_r(line, ",", &csv_ptr);
    if (!token) {
        free(newMovie);
        return NULL;
    }
    newMovie->title = strdup(token);

    // Tokenize Year
    token = strtok_r(NULL, ",", &csv_ptr);
    if (!token) {
        free(newMovie->title);
        free(newMovie);
        return NULL;
    }
    newMovie->year = atoi(token);

    // Tokenize Languages
    token = strtok_r(NULL, ",", &csv_ptr);
    if (!token) {
        free(newMovie->title);
        free(newMovie);
        return NULL;
    }
        
    if (token[0] == '[') token++;       //Remove surrounding brackets if needed:
    size_t lenToken = strlen(token);
    if (token[lenToken - 1] == ']') {
        token[lenToken - 1] = '\0';
    }

        //Tokenize languages using a separate token.
    char *languages = strdup(token);
    int langCount = 0;
    char *lang_ptr;         //context pointer for languages tokenization
    char *langToken = strtok_r(languages, ";", &lang_ptr);

    while (langToken != NULL && langCount < 5) {
        strncpy(newMovie->languages[langCount], langToken, sizeof(newMovie->languages[langCount]) - 1);
        newMovie->languages[langCount][sizeof(newMovie->languages[langCount]) - 1] = '\0';
        langCount++;
        langToken = strtok_r(NULL, ";", &lang_ptr);
    }
    free(languages);

    // Tokenize Rating
    token = strtok_r(NULL, ",", &csv_ptr);
    if (!token || strlen(token) == 0) {
        printf("Warning: Missing rating for movie %s. Setting rating to 0.0\n", newMovie->title);
        newMovie->rating = 0.0;
    } else {
        while (*token == ' ') token++;         //Skip any spaces.
        newMovie->rating = strtof(token, NULL);
    }

    newMovie->next = NULL;
    return newMovie;
}


// Function to process file and store movies in a linked list
struct movie* processMovieFile(char* filePath, int *movieCount) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        printf("Could not open file %s\n", filePath);
        exit(1);
    }
    
    char *line = NULL;
    size_t len = 0;
    struct movie *head = NULL, *tail = NULL;
    
    if (getline(&line, &len, file) == -1) {
        printf("Error: Empty file or read failure \n");
        fclose(file);
        return NULL;
    }
    
    *movieCount = 0;
    while (getline(&line, &len, file) != -1) {
       struct movie *newMovie = createMovie(line);
        if (!newMovie) continue;
        (*movieCount)++;
        
        if (!head) {
            head = newMovie;
            tail = newMovie;
        } else {
            tail->next = newMovie;
            tail = newMovie;
        }
    }
    
    free(line);
    fclose(file);
    
    printf("Processed file %s and parsed data for %d movies \n", filePath, *movieCount);
    return head;
}

void showMoviesByYear(struct movie *head, int year) {
    int found = 0;
    while (head) {
        if (head->year == year) {
            printf("%s\n", head->title);
            found = 1;
        }
        head = head->next;
    }
    if (!found) printf("No data about movies released in the year %d \n", year);
}

void showHighestRatedMovies(struct movie *head) {
    struct movie *bestMovies[3000] = {NULL};
    while (head) {
        int idx = head->year - 1900;
        if (!bestMovies[idx] || head->rating > bestMovies[idx]->rating) {
            bestMovies[idx] = head;
        }
        head = head->next;
    }
    for (int i = 0; i < 3000; i++) {
        if (bestMovies[i]) {
            printf("%d %.1f %s\n", bestMovies[i]->year, bestMovies[i]->rating, bestMovies[i]->title);
        }
    }
}

void showMoviesByLanguage(struct movie *head, char *language) {
    int found = 0;
    while (head) {
        for (int i = 0; i < 5; i++) {
            if (strcmp(head->languages[i], language) == 0) {
                printf("%d %s\n", head->year, head->title);
                found = 1;
                break;
            }
        }
        head = head->next;
    }
    if (!found) printf("No data about movies released in %s \n", language);
}

void showMenu() {
    printf("\n1. Show movies released in the specified year \n");
    printf("2. Show highest rated movie for each year \n");
    printf("3. Show the title and year of release of all movies in a specific language \n");
    printf("4. Exit from the program \n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <movies_sample_1.csv> \n", argv[0]);
        return EXIT_FAILURE;
    }
    
    int movieCount;
    struct movie *movies = processMovieFile(argv[1], &movieCount);
    if (!movies) {
        printf("Error: No movies were processed. Exiting.\n");
        return EXIT_FAILURE;
    }
    
    int choice;
    do {
        showMenu();
        printf("\nEnter a choice from 1 to 4: ");
        scanf("%d", &choice);
        
        if (choice == 1) {
            int year;
            printf("Enter the year for which you want to see movies: ");
            scanf("%d", &year);
            showMoviesByYear(movies, year);
        } else if (choice == 2) {
            showHighestRatedMovies(movies);
        } else if (choice == 3) {
            char language[20];
            printf("Enter the language for which you want to see movies: ");
            scanf("%s", language);
            showMoviesByLanguage(movies, language);
        } else if (choice != 4) {
            printf("You entered an incorrect choice. Try again.\n");
        }
    } while (choice != 4);
    
    return EXIT_SUCCESS;
}
/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2014
 *
 * Copyright (C) 2014-2025 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/code/commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_LEN 64
#define MAX_GUESSES 10

// --- Helper: Random number in range [min, max] ---
static int rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

// --- Game: Dice Roll ---
static void play_dice(int rounds) {
    printf("=== Dice Roll Game ===\n");
    for (int i = 0; i < rounds; i++) {
        int roll = rand_range(1, 6);
        printf("Roll %d: You rolled a %d!\n", i + 1, roll);
    }
}

// --- Game: Guess the Number ---
static void play_guess_number(int rounds) {
    printf("=== Guess the Number ===\n");
    for (int r = 0; r < rounds; r++) {
        int target = rand_range(1, 100);
        int guess = 0, attempts = 0;
        printf("Round %d: Guess the number (1-100):\n", r + 1);
        while (attempts < MAX_GUESSES) {
            printf("Your guess: ");
            if (scanf("%d", &guess) != 1) {
                while (getchar() != '\n'); // flush invalid input
                continue;
            }
            attempts++;
            if (guess < target) {
                printf("Too low!\n");
            } else if (guess > target) {
                printf("Too high!\n");
            } else {
                printf("Correct! You guessed in %d attempts.\n", attempts);
                break;
            }
        }
        if (guess != target) {
            printf("Out of attempts! The number was %d.\n", target);
        }
    }
}

// --- Game: Guess the Word ---
static void play_guess_word(int rounds) {
    const char* words[] = {"apple", "banana", "cherry", "dragon", "elephant"};
    int word_count = sizeof(words) / sizeof(words[0]);

    printf("=== Guess the Word ===\n");
    for (int r = 0; r < rounds; r++) {
        const char* target = words[rand_range(0, word_count - 1)];
        char guess[MAX_WORD_LEN];
        int attempts = 0;

        printf("Round %d: Guess the word:\n", r + 1);
        while (attempts < MAX_GUESSES) {
            printf("Your guess: ");
            if (scanf("%63s", guess) != 1) continue;
            attempts++;
            if (strcmp(guess, target) == 0) {
                printf("Correct! You guessed the word in %d attempts.\n", attempts);
                break;
            } else {
                printf("Wrong! Try again.\n");
            }
        }
        if (strcmp(guess, target) != 0) {
            printf("Out of attempts! The word was '%s'.\n", target);
        }
    }
}

// --- Game: Paper-Rock-Scissors ---
static void play_rps(int rounds) {
    const char* choices[] = {"rock", "paper", "scissors"};
    int choice_count = sizeof(choices) / sizeof(choices[0]);

    printf("=== Paper-Rock-Scissors ===\n");
    for (int r = 0; r < rounds; r++) {
        char player[16];
        int valid = 0;
        while (!valid) {
            printf("Round %d: Enter rock, paper, or scissors: ", r + 1);
            if (scanf("%15s", player) != 1) continue;

            for (int i = 0; i < choice_count; i++) {
                if (strcmp(player, choices[i]) == 0) {
                    valid = 1;
                    break;
                }
            }
        }

        int ai = rand_range(0, choice_count - 1);
        printf("AI chose: %s\n", choices[ai]);

        if (strcmp(player, choices[ai]) == 0) {
            printf("It's a tie!\n");
        } else if ((strcmp(player, "rock") == 0 && strcmp(choices[ai], "scissors") == 0) ||
                   (strcmp(player, "paper") == 0 && strcmp(choices[ai], "rock") == 0) ||
                   (strcmp(player, "scissors") == 0 && strcmp(choices[ai], "paper") == 0)) {
            printf("You win!\n");
        } else {
            printf("You lose!\n");
        }
    }
}

// --- Main Shark Play Command ---
int shark_play(const char* game_name, const char* difficulty, int rounds, int verbose) {
    srand((unsigned int)time(NULL));

    if (!game_name || rounds <= 0) {
        fprintf(stderr, "Invalid game or rounds.\n");
        return 1;
    }

    if (verbose) {
        printf("Starting game '%s' for %d rounds, difficulty: %s\n",
               game_name, rounds, difficulty ? difficulty : "normal");
    }

    if (strcmp(game_name, "dice") == 0) {
        play_dice(rounds);
    } else if (strcmp(game_name, "guess_number") == 0) {
        play_guess_number(rounds);
    } else if (strcmp(game_name, "guess_word") == 0) {
        play_guess_word(rounds);
    } else if (strcmp(game_name, "rps") == 0) {
        play_rps(rounds);
    } else {
        fprintf(stderr, "Unknown game: %s\n", game_name);
        return 1;
    }

    return 0;
}

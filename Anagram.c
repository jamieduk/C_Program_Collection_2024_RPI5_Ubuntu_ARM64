#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define ASCII_SIZE 256
//  gcc -o anagram anagram.c

int checkAnagram(char *str1, char *str2);
void removeNewline(char *str);

int main()
{
    char str1[100], str2[100];
    
    printf("Function: whether two given strings are anagrams:\n");
    printf("Example: pears and spare, stone and tones:\n");
    
    printf("Input the first string: ");
    if (!fgets(str1, sizeof str1, stdin)) {
        fprintf(stderr, "Error reading first string.\n");
        return 1;
    }
    printf("Input the second string: ");
    if (!fgets(str2, sizeof str2, stdin)) {
        fprintf(stderr, "Error reading second string.\n");
        return 1;
    }

    // Remove newline characters
    removeNewline(str1);
    removeNewline(str2);

    if (checkAnagram(str1, str2) == 1) {
        printf("%s and %s are anagrams.\n\n", str1, str2);
    } 
    else 
    {
        printf("%s and %s are not anagrams.\n\n", str1, str2);
    }
    return 0;
}

// Function to remove the newline character from a string
void removeNewline(char *str)
{
    size_t len=strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1]='\0';
    }
}

// Function to check whether two passed strings are anagrams or not
int checkAnagram(char *str1, char *str2)
{
    int str1ChrCtr[ASCII_SIZE]={0}, str2ChrCtr[ASCII_SIZE]={0};
    
    // Check the length of equality of Two Strings
    if (strlen(str1) != strlen(str2)) {
        return 0;
    }

    // Count frequency of characters in str1 
    for (int ctr=0; str1[ctr] != '\0'; ctr++) {
        str1ChrCtr[(unsigned char)str1[ctr]]++;
    }

    // Count frequency of characters in str2 
    for (int ctr=0; str2[ctr] != '\0'; ctr++) {
        str2ChrCtr[(unsigned char)str2[ctr]]++;
    }

    // Compare character counts of both strings 
    for (int ctr=0; ctr < ASCII_SIZE; ctr++) {
        if (str1ChrCtr[ctr] != str2ChrCtr[ctr]) {
            return 0;
        }
    }
    return 1;
}


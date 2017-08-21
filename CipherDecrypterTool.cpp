/*
** @author Shayan Memari
** @description Decrypts a file of ciphers using Caesar,Modified Caesar,Columnar Transposition and Diagonal Transposition.
** @version 2.0 06/08/2017
*/

//#include "stdafx.h"		// FOR VS
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <cstring>
#include "windows.h"

using namespace std;

//	Filenames
#define CIPHER_FILE "input.txt"
#define DIC_FILE "dict_en.txt"
#define KEYWORDS_FILE "words.txt"
#define PLAINTEXT_FILE "output.txt"

//	Function prototypes
int checkFileIsOpen(ifstream&, string);
int checkCaesarCipher(const string, vector<string>&, ofstream&);
int checkModifiedCaesarCipher(const string, vector<string>&, vector<string>&, ofstream&);
int checkColumnarCipher(const string, vector<string>&, ofstream&);
int checkDiagonalTransposition(const string, vector<string>&, ofstream&);
string removeSpaces(string);
string findAndReplaceChars(string, char, char);
int checkDictionary(const string, vector<string>&);
int divisionInteger(int, int);
string removeDuplicateChars(string);
char getNextLetterOfAlphabet(const char[], const char[]);

/* 
** Comparison function to sort the dictionary words vector by length 
** 
** @return the longer word
*/
struct compare{
	bool operator()(const string& first, const string& second){
		return first.size() > second.size();
	}
};


/* 
** The main function: entry point for the program
*/
int main(){
	ifstream cipherFile, keyWordsFile, dictionaryFile;
	ofstream plaintextFile;
	string cipherLine;
	string word;
	vector<string> dicWords;
	vector<string> keyWords;
	compare comp;
	typedef void* HANDLE;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 2);
	cout << "=========================================================" << endl;
	cout << "==========Welcome to Decryption Hacking Tool ============" << endl;
	cout << "=========================================================\n" << endl;
	SetConsoleTextAttribute(hConsole, 15);

	//	Open files
	dictionaryFile.open(DIC_FILE);
	cipherFile.open(CIPHER_FILE);
	keyWordsFile.open(KEYWORDS_FILE);
	plaintextFile.open(PLAINTEXT_FILE, ios::out);

	if (checkFileIsOpen(cipherFile, CIPHER_FILE) == 0){
		return 0;
	}

	if (checkFileIsOpen(keyWordsFile, KEYWORDS_FILE) == 0){
		return 0;
	}

	if (checkFileIsOpen(dictionaryFile, DIC_FILE) == 0){
		return 0;
	}

	cout << "\n========================================================\n" << endl;

	//	Store dictionary words in vector
	while (getline(dictionaryFile, word)){
		dicWords.push_back(word);
	}

	// Sort dictionary words by highest length.
	sort(dicWords.begin(), dicWords.end(), comp);

	//	Store keywords in vector
	while (getline(keyWordsFile, word)){
		keyWords.push_back(word);
	}

	cout << "Decryption Process starting..." << endl;

	unsigned int countLines = 1;
	while (getline(cipherFile, cipherLine)){
		cout << "Line: " << countLines << endl;
		countLines++;
		string cipherLineWithoutSpaces = removeSpaces(cipherLine);

		if (checkCaesarCipher(cipherLineWithoutSpaces, dicWords, plaintextFile) != 1){
			//if (checkColumnarCipher(cipherLineWithoutSpaces, dicWords, plaintextFile) != 1)  //Decrypt using Columnar Cipher
			//{
				               if (checkModifiedCaesarCipher(cipherLineWithoutSpaces, dicWords, keyWords, plaintextFile) != 1)
				               {
				                   //cout << "no matches found in modified caesar." << endl;
				               }
			//}
		}

	}

	cout << "Decryption Process finished...\nPress Control+C to quit...";
	cipherFile.close();
	keyWordsFile.close();
	dictionaryFile.close();
	plaintextFile.close();

	getchar();
	return 0;
}



/**
 * For a given ciphertext, check every possible caesar combination and check whether a word/phrase is decrypted
 * 
 * @param string cipher : a line of cipher text
 * @param vector dic : dictionary words
 * @param ofstream f : file to output results into
 * @return integer : return 1 if found a match for word/phrase else return 0
 */
int checkCaesarCipher(string cipher, vector<string>& dic, ofstream& f){
	string text;

	//	Test each possible Caesar key
	for (int key = -10; key < 26; key++){ 
		text = cipher;

		//	Generate the output text with the given key
		for (unsigned int j = 0; j < text.length(); j++){
			char asma = (text[j] - 'a');
			int csma = (asma - key + 26) % 26;
			char esma = csma + 'a';
			text[j] = esma;
		}

		//Check if the decrypted text is an english phrase
		if (checkDictionary(text, dic) == 1){
			cout << "+++++ Found C +++++" << endl;

			//Write to the output file
			transform(text.begin(), text.end(), text.begin(), ::toupper);

			f << text << "," << "C," << key << endl;
			return 1;
		}
	}
	return 0;
}


/**
 * Decrypt a cipher using Columnar transposition technique if possible.
 * 
 * @param String cipher : ciphertext to decrypt
 * @param vector dic : dictionary words
 * @param ofstream f : file output
 * @return integer : return 1 if cipher was decrypted successfully else return 0
 */
int checkColumnarCipher(string cipher, vector<string>& dic, ofstream& f){
	//string text = cipher;

	for (unsigned int i = 2; i < (cipher.length() - 1); i++){ //Column width can be 2 to (CipherLength-1)
		unsigned int counter = 0;
		string plainText;
		unsigned int rows = divisionInteger(cipher.length(), i);

		unsigned int remainder = cipher.length() % i;

		char textBlock[20][20];
		for (unsigned int column = 0; column < i; column++){
			if (remainder >= (column + 1)){
				for (unsigned int row = 0; row<rows; row++){	//Place char inside the block
					if (isalpha(cipher[counter])){
						textBlock[row][column] = cipher[counter];
						counter++;
					}
				}
			}
			else {
				for (unsigned int row = 0; row<rows - 1; row++){	//Place char inside the block
					if (isalpha(cipher[counter])){
						textBlock[row][column] = cipher[counter];
						counter++;
					}
				}
			}
		}

		for (unsigned int a = 0; a < rows; a++){
			for (unsigned int b = 0; b < i; b++){
				if (isalpha(textBlock[a][b])){
					plainText += textBlock[a][b];
				}
			}
		}

		if (checkDictionary(plainText, dic) == 1){
			//Append to the output file
			cout << "found T\n";
			transform(plainText.begin(), plainText.end(), plainText.begin(), ::toupper);
			for (unsigned int g = 5; g<plainText.length(); g += 6){
				plainText.insert(g, 1, ' ');
			}
			f << plainText << "," << "T," << i << endl;
			return 1;
		}
	}
	return 0;
}

/**
 * Check the ciphertext is decryptable using modified caesar cipher and the list of common keywords.
 * @param string cipher : ciphertext
 * @param vector dic : dictionary words
 * @param vector keywords : keywords
 * @param ofstream f : file output
 * @return Integer : return 1 if found a match or else 0
 */
int checkModifiedCaesarCipher(string cipher, vector<string>& dic, vector<string>& keywords, ofstream& f){
	const char correctAlphabet[] = { 
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
		 'u', 'v', 'w', 'x', 'y', 'z'
	};

	string text;

	for (unsigned int t = 0; t < keywords.size(); t++){  
		string theKeyword = removeDuplicateChars(keywords[t]);
		char modifiedAlphabet[26] = { 0 };
		char modifiedAlphabetShifted[26] = { 0 };

		//	TODO new function for this
		//	Modify the alphabet
		for (unsigned int letter = 0; letter < 26; letter++){
			//	Insert keyword at the start of alphabet
			if (letter < theKeyword.length()){
				modifiedAlphabet[letter] = theKeyword[letter];
			} else {	//	Insert the remaining letters of alphabet in order
				modifiedAlphabet[letter] = getNextLetterOfAlphabet(modifiedAlphabet, correctAlphabet);
			}
		}

			if (cipher == "dgyzrkcoeznkdo"){
				cout << "modified alphabet: ";
				for (unsigned int b = 0; b<sizeof(modifiedAlphabet); b++){
					cout << modifiedAlphabet[b];
				}
				cout << endl;
			}

		// Test each possible key
		for (int i = 1; i < 26; i++){
			text = cipher;

			// Shift alphabet by key
			for (unsigned int xletter=0; xletter<26; xletter++){
				modifiedAlphabetShifted[xletter] = modifiedAlphabet[(xletter + i + 26) % 26];
			}

			// Substitute the characters in the text
			for (unsigned int j = 0; j < text.length(); j++){
				char * charPointer;
				charPointer = strchr(modifiedAlphabetShifted, text[j]);    	// get pointer to the location of char inside modified Alphabet

				int loc = (charPointer - modifiedAlphabetShifted + 1);

				text[j] = correctAlphabet[loc - 1];
				loc = 0;											
			}

			//Check if the decrypted text is an english phrase
			if (checkDictionary(text, dic) == 1){
				cout << "+++++ Found M +++++" << endl;
				transform(text.begin(), text.end(), text.begin(), ::toupper);
				f << text << ", " << "M, " << keywords[t] << ", " << i << endl;
				return 1;

			}
		}//for loop keys
	}//for loop keywords
	return 0;
}


/**
 * find next available character in the modified alphabet 
 * @param char[] alpha_modified :
 * @param car[] alpha_normal : 
 * @return char
 */
char getNextLetterOfAlphabet(const char alph_modified[], const char alpha_normal[]){
	unsigned int i = 0;

	if (strchr(alph_modified, alpha_normal[i]) == NULL){	//	did not found the char
		return alpha_normal[i];
	}

	while (strchr(alph_modified, alpha_normal[i]) != NULL){	//	found char inside
		i++;
		if (strchr(alph_modified, alpha_normal[i]) == NULL){
			//cout << "returned char: " << alpha_normal[i] << endl;
			return alpha_normal[i];
		}
	}
}


/**
 * Simple integer division method 
 * 
 * @param Integer num : number to divide
 * @param Integer denominator : denominator
 * @return Integer : return result of division, used in columnar cipher
 */
int divisionInteger(int num, int deno){
	return num / deno
		+ (((num < 0) ^ (deno > 0)) && (num%deno));
}

/* 
** Remove duplicate letters in a keyword (Used in modified cipher method)
**
** @param string str : the keyword to manipulate
** @return string modified keyword with duplicates removed
*/
string removeDuplicateChars(string str){
	string newKeyword;

  short count[256] = {0};
  copy_if(str.begin(), str.end(), back_inserter(newKeyword),
      [&](unsigned char c) { return count[c]++ == 0; });
			
	return newKeyword;
}

/**
 * Replace a character in a string with another character
 * 
 * @param string stringToModify : string to replace a character in
 * @param char ch1 : the character to find and modify
 * @param char ch2 : the character to replace the found character with
 * @return string : the modified string 
 */
string findAndReplaceChars(string stringToModify, char ch1, char ch2){
	for (unsigned int i = 0; i < stringToModify.length(); ++i){
		if (stringToModify[i] == ch1)
			stringToModify[i] = ch2;
	}
	return stringToModify;
}

/**
 * Remove all spaces from the input string and return it
 * 
 * @param string input : string to remove spaces from
 * @return string : modified input without the spaces.
 */
string removeSpaces(string input){
	input.erase(remove(input.begin(), input.end(), ' '), input.end());
	return input;
}


/**
 * Check if a file stream is opened successfully or not.
 * 
 * @param ifstream file : file to investigate
 * @param string fileName : name of the file
 * @return integer : return 1 if file is open else return 0
 */
int checkFileIsOpen(ifstream& file, string fileName){
	typedef void* HANDLE;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (file.is_open()){
		SetConsoleTextAttribute(hConsole, 2);
		cout << fileName << " was opened successfully..." << endl;
		SetConsoleTextAttribute(hConsole, 15);
		return 1;
	} 
	else {
		SetConsoleTextAttribute(hConsole, 228);
		cout << fileName << " failed to open..." << endl;
		SetConsoleTextAttribute(hConsole, 15);
		return 0;
	}
}

/**
 * Looks up the dictionary and finds matched words in the plaintext and then deletes it.
 * 
 * @param string possiblePlaintext : the text to check
 * @param vector dic : english dictionary words 
 * @return integer : return 1 if the text is considered to be an english word/phrase otherwise return 0
 */
int checkDictionary(string possiblePlaintext, vector<string>& dic){
	string theText = possiblePlaintext;

	for (unsigned int i = 0; i < dic.size(); i++){
		size_t found = theText.find(dic[i]);

		if (found == 0){
			theText.erase(found, dic[i].length());
			i = 0;
		}

		if (theText.empty()){    
			return 1;
		}
	}
	return 0;
}

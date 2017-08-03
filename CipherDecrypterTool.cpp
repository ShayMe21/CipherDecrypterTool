/*
** Author: Shayan Memari
** Program: Decrypts a file with ciphers using 4 different methods.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <cstring>


using namespace std;
// Filenames
#define CIPHER_FILE "input.txt"
#define DIC_FILE "dict_en.txt"
#define KEYWORDS_FILE "words.txt"
#define PLAINTEXT_FILE "output.txt"

// Function prototypes
int checkFileIsOpen(ifstream&, string);
int checkCaesarCipher(const string, vector<string>&, ofstream&);
int checkModifiedCaesarCipher(const string, vector<string>&, vector<string>&, ofstream&);
int checkColumnarCipher(const string, vector<string>&, ofstream&);
int checkDiagonalTransposition(const string, vector<string>&, ofstream&);
string removeSpaces(string);
string replaceChar(string, char, char);
int checkDictionary(const string, vector<string>&);
int divisionInteger(int, int);
string removeDups(string);
char nextAvailableChar(const char[], const char[]);

//Comparison function to sort the dic words vector by length
struct compare
{
	bool operator()(const string& first, const string& second)
	{
		return first.size() > second.size();
	}
};

int main()
{
	cout << "======================================================" << endl;
	cout << "==========Welcome to Decryption Hack Tool ============" << endl;
	cout << "======================================================\n" << endl;

	// Input and Output file streams
	ifstream cipherFile, keyWordsFile, dictionaryFile;
	ofstream plaintextFile;

	dictionaryFile.open(DIC_FILE);
	cipherFile.open(CIPHER_FILE);
	keyWordsFile.open(KEYWORDS_FILE);
	plaintextFile.open(PLAINTEXT_FILE, ios::out);

	//Check Files opened successfully.
	checkFileIsOpen(cipherFile, CIPHER_FILE);
	checkFileIsOpen(keyWordsFile, KEYWORDS_FILE);
	checkFileIsOpen(dictionaryFile, DIC_FILE);
	//checkFileIsOpen(plaintextFile, PLAINTEXT_FILE);
	cout << "\n======================================================\n" << endl;

	// Read ciphertext line by line
	string cipherLine;
	string word;
	vector<string> dic_words;
	vector<string> key_words;

	//Store dictionary words in vector
	while (getline(dictionaryFile, word))
	{
		dic_words.push_back(word);
	}

	// Sort dictionary words by highest length.
	compare comp;
	sort(dic_words.begin(), dic_words.end(), comp);

	//Store keywords in vector
	while (getline(keyWordsFile, word))
	{
		key_words.push_back(word);
	}

	// Decipher the texts
	cout << "Decryption Process started...\nWriting to file: " << PLAINTEXT_FILE << endl;

	while (getline(cipherFile, cipherLine))    //For each line of Ciphertext
	{
		string line = removeSpaces(cipherLine);    //Remove spaces between characters
												   //cout << line << endl;
		if (checkCaesarCipher(line, dic_words, plaintextFile) != 1)    //Decrypt using Caesar Cipher
		{
			if (checkColumnarCipher(line, dic_words, plaintextFile) != 1)  //Decrypt using Columnar Cipher
			{
				//                if (checkModifiedCaesarCipher(line, dic_words, key_words, plaintextFile) != 1)  //Decrypt using modified Caesar Cipher.
				//                {
				//                    cout << "no matches found in modified caesar." << endl;
				//                }
			}
		}

	}

	cout << "Decryption Process finished...\n";
	cipherFile.close();
	keyWordsFile.close();
	dictionaryFile.close();
	plaintextFile.close();

	// To keep the window open
	getchar();

}

string removeDups(string str)
{
	int pos = 0;
	while (str[pos])
	{
		size_t pos_found = str.find(str[pos], pos + 1);
		if (pos_found != string::npos)// found a dup
		{
			//cout << "duplicate found: " << str[pos_found] << endl;

			str.erase(remove(str.begin() + pos + 1, str.end(), str[pos_found]), str.end());  //remove the rest of the the dup char
																							 //cout << str << endl;
		}

		pos++;
	}
	return str;
}


string replaceChar(string str, char ch1, char ch2)
{
	for (unsigned int i = 0; i < str.length(); ++i)
	{
		if (str[i] == ch1)
			str[i] = ch2;
	}

	return str;
}

// Function to remove whitespaces from a string.
string removeSpaces(string input)
{
	input.erase(remove(input.begin(), input.end(), ' '), input.end());
	return input;
}

// Function to check if the file is opened successfully or not.
int checkFileIsOpen(ifstream& f, string s)
{
	if (f.is_open())
	{
		cout << s << " was opened successfully." << endl;
		return 1;
	}
	else
	{
		cout << s << " could not be opened." << endl;
		return 0;
	}

}


int checkDictionary(string ptext, vector<string>& dic)
{
	string theText = ptext;
	//vector<string> matchedWords;
	//compare comp;

	for (unsigned int i = 0; i < dic.size(); i++)
	{
		size_t found = theText.find(dic[i]);

		if (found == 0) //Word found is the first word in the phrase
		{
			theText.erase(found, dic[i].length());     //delete the word from the theText
			i = 0;
		}
		if (theText.empty())    //When phrase is empty its considered a proper English expression
		{
			return 1;
		}
	}

	return 0;
}
//Function to check for a solution to the ciphertext using standard Ceasar method.
int checkCaesarCipher(string cipher, vector<string>& dic, ofstream& f)
{
	string text = cipher;
	for (int i = -10; i < 26; i++) //Test each possible key
	{
		text = cipher;
		for (unsigned int j = 0; j < text.length(); j++)
		{

			char asma = (text[j] - 'a');
			int csma = (asma - i + 26) % 26;
			char esma = csma + 'a';
			text[j] = esma;
		}

		//Check if the decrypted text is an english phrase
		if (checkDictionary(text, dic) == 1)
		{
			cout << "found C\n";
			//Write to the output file
			transform(text.begin(), text.end(), text.begin(), ::toupper);
			for (int g = 5; g<text.length(); g += 6) {
				text.insert(g, 1, ' ');
			}
			f << text << "," << "C," << i << endl;
			return 1;

		}
	}
	return 0;

}

int divisionInteger(int num, int deno)
{
	return num / deno
		+ (((num < 0) ^ (deno > 0)) && (num%deno));
}

int checkColumnarCipher(string cipher, vector<string>& dic, ofstream& f)
{
	//string text = cipher;

	for (unsigned int i = 2; i < (cipher.length() - 1); i++)//Column width can be 2 to (CipherLength-1)
	{
		unsigned int counter = 0;
		string plainText;
		unsigned int rows = divisionInteger(cipher.length(), i);

		unsigned int remainder = cipher.length() % i;

		char textBlock[20][20];
		for (unsigned int column = 0; column < i; column++)
		{
			if (remainder >= (column + 1))
			{
				for (unsigned int row = 0; row<rows; row++)//Place char inside the block
				{
					if (isalpha(cipher[counter]))
					{
						textBlock[row][column] = cipher[counter];
						counter++;
					}
				}
			}
			else
			{
				for (unsigned int row = 0; row<rows - 1; row++)//Place char inside the block
				{
					if (isalpha(cipher[counter]))
					{
						textBlock[row][column] = cipher[counter];
						counter++;
					}
				}
			}

		}

		for (unsigned int a = 0; a < rows; a++)
		{
			for (unsigned int b = 0; b < i; b++)
			{
				if (isalpha(textBlock[a][b]))
				{
					plainText += textBlock[a][b];
				}
			}

		}

		if (checkDictionary(plainText, dic) == 1)
		{
			//Append to the output file
			cout << "found T\n";
			transform(plainText.begin(), plainText.end(), plainText.begin(), ::toupper);
			for (int g = 5; g<plainText.length(); g += 6) {
				plainText.insert(g, 1, ' ');
			}
			f << plainText << "," << "T," << i << endl;
			return 1;
		}

	}
	return 0;
}


int checkModifiedCaesarCipher(string cipher, vector<string>& dic, vector<string>& keywords, ofstream& f)
{
	char alphabetReal[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
	};
	string text;

	for (unsigned int t = 0; t < keywords.size(); t++)  //For each possible keyword used, generate the modified alphabet
	{
		string myKeyword = removeDups(keywords[t]);    //remove duplicated chars from keyword
		char alphabet[26] = { 0 };

		// Modify the alphabet
		for (unsigned int r = 0; r < 26; r++)
		{
			if (r < myKeyword.length())
			{
				alphabet[r] = myKeyword[r];     //insert the keyword first
			}
			else
			{
				alphabet[r] = nextAvailableChar(alphabet, alphabetReal);    //insert the next letter in the alphabet
			}

		}
		cout << "alphabet: \n";
		for (unsigned int b = 0; b<sizeof(alphabet); b++) {
			cout << alphabet[b];
		}
		cout << endl;


		// Test each possible key
		for (int i = -10; i < 26; i++)
		{
			text = cipher;
			// Substitute the characters in the text
			for (unsigned int j = 0; j < text.length(); j++)
			{
				char * charPointer;
				charPointer = strchr(alphabet, text[j]);    // get pointer to the location of char
				int loc = (charPointer - alphabet);         // get its location in our alphabet
				text[j] = alphabet[(loc + 26 - i) % 26];    // replace the char (shift by i)
			}

			cout << "Cipher=" << cipher << ", text=" << text << ", key=" << i << ", keyword=" << keywords[t] << endl;


			//Check if the decrypted text is an english phrase
			if (checkDictionary(text, dic) == 1)
			{
				//Append to the output file
				transform(text.begin(), text.end(), text.begin(), ::toupper);
				f << text << ", " << "M, " << keywords[t] << ", " << i << endl;
				return 1;

			}
		}//for loop keys
	}//for loop keywords
	return 0;
}


char nextAvailableChar(const char alph_modified[], const char alpha_normal[])
{
	unsigned int i = 0;

	if (strchr(alph_modified, alpha_normal[i]) == NULL)//not found the char
	{
		//cout << "returned char: " << alpha_normal[i] << endl;
		return alpha_normal[i];
	}

	while (strchr(alph_modified, alpha_normal[i]) != NULL)//found char inside
	{
		i++;
		if (strchr(alph_modified, alpha_normal[i]) == NULL)
		{
			//cout << "returned char: " << alpha_normal[i] << endl;
			return alpha_normal[i];
		}
	}

}


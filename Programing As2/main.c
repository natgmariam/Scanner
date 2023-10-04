/* 
Name: Natnael Gebremariam
NSHE ID: 5002150850
Class: CS326-1002 
Assignment: C program that breaks down a JSON file into char, to imitate a scanner 
          and parser
Description: The program reads in the file character by character and stors them as 
              tokens based on a globbal variable that its compared to. Ones its done,
              it goes through a error check and matches the the char to token names 
              then prints out line number, token id, and token value. In addition to 
              handling edge cases like not even braces and incorrect format
*/
#include <stdbool.h> //lib 
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <ctype.h>

//struct to determine token type, line number, and array to load the tokens
struct Token {
  int kind;
  int lineNo; 
  char lexeme[100]; 
};
//global variables 
const int EOT = 0; // end of file
const int STRING = 1; // sequence of chars
const int IDENT = 2; // identifier
const int COMMA = 3; // ’,’
const int COLON = 4; // ’:’
const int LBRACE = 5; // ’{’
const int RBRACE = 6; // ’}’
const int LBRACK = 7; // ’[’
const int RBRACK = 8; // ’]’
const int CONST = 9; // 'true', 'false', 'null', 'numbers'

//function to get the next token, input is lookahead char. output is a token
struct Token getNextToken(FILE* jasonInput)
{
  //creater an object 
  struct Token token;
  // move on to the next character / see what comes next 
  int lookahead; 
  //set the line being read from the file, the  // this cant workindex to token line 
  static int lineNo = 1; 
  //make an index variable 
  int index; 
  
  while ((lookahead = fgetc(jasonInput)) != EOF)
  { 
      //set index to zero to initialize for each token 
      index = 0; 
      switch(lookahead)
        {
          case ' ': 
          case '\n': 
          case '\t':
            //just consume whitespace till next char is hit unless \n 
            //inc to the next line only after end line and use that to denote lineNo 
            if(lookahead == '\n') {lineNo++;}
            continue; 
          case ',': 
            //set token kind, set line number, and add to lexeme
            //then return the token
            token.kind = COMMA;     //store the token 
            token.lineNo= lineNo;   //record the line number
            token.lexeme[index++] = ',';   //record the token into lexeme and inc index
            token.lexeme[index] = '\0';//after token has been recorded, null terminate string
            return token;                  //preventing dublicates or additional char 
          case ':':
            token.kind = COLON; 
            token.lineNo= lineNo; 
            token.lexeme[index++] = ':'; 
            token.lexeme[index] = '\0'; 
            return token;  
          case '{': 
          
            token.kind = LBRACE; 
            token.lineNo= lineNo; 
            token.lexeme[index++] = '{'; 
            token.lexeme[index] = '\0'; 
            return token;  
          case '}': 
            token.kind = RBRACE;
            token.lineNo= lineNo; 
            token.lexeme[index++] = '}'; 
            token.lexeme[index] = '\0'; 
            return token;  
          case '[': 
            token.kind = LBRACK; 
            token.lineNo= lineNo; 
            token.lexeme[index++] = '['; 
            token.lexeme[index] = '\0'; 
            return token;  
          case ']': 
            token.kind = RBRACK; 
            token.lineNo= lineNo; 
            token.lexeme[index++] = ']'; 
            token.lexeme[index] = '\0'; 
            return token;
          //case for to differentiating the String and Identifier
          case '\"':
            token.kind = STRING; 
            token.lineNo = lineNo; 
            //loop until the all string have been added
            while((lookahead = fgetc(jasonInput)) != '\"' && lookahead != EOF)
              {
                token.lexeme[index++] = lookahead; 
              }
               token.lexeme[index] = '\0';
            //if its the end of file and is not a proper string/" " then return error and EOT token
            if(lookahead == EOF)
            {
              printf("\nSomething went wrong! Unterminated String in line %d\n", token.lineNo);
              token.kind = EOT;
              token.lineNo = lineNo-1; //dec line since its an error 
              while(index >= 0)
              {token.lexeme[index] = '0';
               index--; 
              }
              return token;
            }
            //add the Identifier, using the : to denote when it ends 
            //compares the char and add it to Ident if it comes before : and ends if other char found
            //use ungetc to push back : if its identfier to account it for colon case. 
            lookahead = fgetc(jasonInput);
            if(lookahead == ':'){
              token.kind = IDENT;
              ungetc(lookahead, jasonInput);
              return token;
            }

            if(lookahead == ' '){
              lookahead = fgetc(jasonInput);
              if(lookahead == ':'){
                token.kind = IDENT;
                ungetc(lookahead, jasonInput);
                return token;
              }
            }   
            ungetc(lookahead, jasonInput);
            return token; 
          default:
              //all other chars not considered part of the global are saved as const
              token.kind = CONST; 
              token.lexeme[index++] = lookahead;
              while((lookahead = fgetc(jasonInput)) != EOF && 
                lookahead != ' '&&
                lookahead != '\n' &&
                lookahead != '\t' &&
                lookahead != ',' &&
                lookahead != ':' &&
                lookahead != '{' &&
                lookahead != '}' &&
                lookahead != '[' &&
                lookahead != ']' &&
                lookahead != '\"') 
              {
                  token.lexeme[index++]=lookahead; 
              }
              token.lexeme[index] = '\0'; 
              if(lookahead != EOF)
                ungetc(lookahead, jasonInput); 
              return token; 
            }
    }
    token.kind = EOT; 
    token.lineNo = lineNo-1; 
    token.lexeme[0] = '\0';
    return token; 
}
//checks to see there is an even match in the braces and brackets 
bool checkMatch(struct Token arrayToken[], char *tokenNames[], int i)
{
  int lbraceCount = 0; //counters of braces and brackets
  int rbraceCount = 0; 
  int lbrackCount = 0; 
  int rbrackCount = 0; 
  //loop through the array and check if there is an even number of L & R braces and brackets 
  for(int k = 0; i != k; k++)
    {
      if(arrayToken[k].kind == LBRACE)
      {
        lbraceCount++; 
      }
      if(arrayToken[k].kind == RBRACE)  
      {
        rbraceCount++;
      }
      if(arrayToken[k].kind == LBRACK)
      {
        lbrackCount++;
      }
      if(arrayToken[k].kind == RBRACK)
      {  
        rbrackCount++; 
      }
      //if one has more then the other, its a format isue so indicate false
      if(lbrackCount < rbrackCount)
      {
          return false; 
     
      }
    }
  return true; 
}
int main(int argc, char *argv[])
{
  struct Token nextToken; 
  struct Token arrayToken[50]; 
  //read the jason input as a file and try the scanner 
  //does not work at the moment so dont worry about 
  FILE *jasonInput = fopen(argv[1], "r"); // opens from the command line and sets it to read
  //FILE jasonInput = stdin; 
  //given token names to print 
  char *tokenNames[10] = {
    "EOF", "STRING", "IDENT", "COMMA", "COLON",
    "LBRACE", "RBRACE", "LBRACK", "RBRACK", "CONST",
  }; 
  //print scanning
  printf("\nScanning the file...");
  //call the scanner to get the token char by char 
  nextToken = getNextToken(jasonInput); 
  int i = 0;                         //index 
  while(nextToken.kind + 1 != EOT)  //+1 one to record the eof/read up to and EOF 
    {  
      arrayToken[i] = nextToken;    //assigne the token to a token Array
      i++;                          //inc index 
      nextToken = getNextToken(jasonInput);   //call next token, char from input
      if(nextToken.kind == EOT)      //you run into a eof|eot, call next token and break
      {
        arrayToken[i] = nextToken;
        break; 
      }
    }
     //close all opened files
    fclose(jasonInput); 
  //flag to check if inputs has propper structure such as brace and brackets
  bool flag=checkMatch(arrayToken, tokenNames, i);
  //if it doesnt, print where and show
  if(!flag)                                         
  {
      printf("\nScanning done!\n\n");
      printf("List of tokens:\n"); 
      for(int k = 0; k != i+1; k++)
      {
        printf("<@%d,<%d,%s>=\"%s\"> \n",arrayToken[k].lineNo, arrayToken[k].kind, 
        tokenNames[arrayToken[k].kind], arrayToken[k].lexeme); 
      }
      printf("\nParsing file...");
      printf("\nSomething went wrong! Expecting '}' but found ']' in line %d", arrayToken[i-2].lineNo); 
      return 0;         //exit program
  }

  bool errorHandle = true;     //for second error, grammer error
  
  if(errorHandle)             //always true untill an error found 
  {
    printf("\nScanning done!\n\n"); 
    printf("List of tokens:\n");
    for(int k = 0; k != i+1; k++)      //go through array print
    {
      if(arrayToken[k].lexeme[0] == '0')  //chech if an error occurd 
      {
        //printf("Could not parsfile."); 
        errorHandle = false;              //change error handling to false to match output
        break; 
      }
       if(errorHandle == true)          //if no error print out parsed values
       {
        printf("<@%d,<%d,%s>=\"%s\"> \n",arrayToken[k].lineNo, arrayToken[k].kind, 
       tokenNames[arrayToken[k].kind], arrayToken[k].lexeme);
       }
    }
    if(errorHandle)      //if no error program should be done
    {
    printf("\nParsing file...");
    printf("\nParsing done!"); 
    }
    
  }  
  
  if(!errorHandle)      //if an error occurd, print only till error and show message
  {  
    printf("\nCould not parse file.");
    exit(0); 
  }
  
  return 0; 
}


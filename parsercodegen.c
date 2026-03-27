// Oluwadamilola Ogboja
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

//TA said to make ARRAY_SIZE a large number like 5000
#define ARRAY_SIZE 5000
#define MAXLEN 11
#define MAXNUMLEN 5
#define MAX_SYMBOL_TABLE_SIZE 500

//hw2 Functions
int identnum_architect(char pas[], int locale, int LexLocale);
int NumChecker(char holder[]);
void lexeme_printer();

//hw3 functions
void CONSTDECLARATION();
int VARDECLARATION();
void BLOCK();
void EXPRESSION();
void STATEMENT();
void CONDITION();
void FACTOR();
void TERM();
void PROGRAM();
int SYMBOLTABLECHECK(char string[]);
int getnexttoken(int num);
void tableDump();
void AssemblyDump();
void elfDump();

//Declaration of Token Types
typedef enum {
oddsym = 1, identsym, numbersym, plussym, minussym,
multsym, slashsym, fisym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, /*callsym delete late,*/ constsym=28, varsym, /*procsym delete late,*/ writesym=31,
readsym , /*elsesym delete late*/} token_type;

//Declaration of Token Struct
struct lexeme
{
  int value;
  char name[11];
  char ident[11];
  int number;
  int errcode;
  int cond; //1=identifier or 2=number
};

//Declaration of Symbol Table Struct
typedef struct {
  int kind;      // const = 1, var = 2, proc = 3
  char name[10]; // name up to 11 chars
  int val;       // number (ASCII value)
  int level;     // L level
  int addr;      // M address
  int mark;      // to indicate unavailable or deleted
} symbol;

//SYM Variables
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
symbol code[MAX_SYMBOL_TABLE_SIZE];
int symtotal = 0;    // amount of symbols
int codelength = -1; // array num -1 to be at the the last populated index
int codeIndex = 0;   // current code index
int currentToken = 0;
int recentnum = 0;

//HW2 variables
char pas[ARRAY_SIZE];
struct lexeme tokenlist[ARRAY_SIZE];
int total=0;


void elfDump() {
  FILE *ifp = fopen("elf.txt", "w");
  for (int i = 0; i < codeIndex; i++)
    fprintf(ifp, "%d %d %d\n", code[i].val, code[i].level, code[i].addr);
  fclose(ifp);
}

void tableDump(){

  printf("\nSymbol Table:\n");
  printf("Kind | Name        | Value | Level | Address | Mark\n");
  printf("---------------------------------------------------\n");
  for(int i = 0; i < symtotal; i++){
    symbol_table[i].mark = 1;
    printf("%4d | %11s | %5d | %5d | %5d | %5d\n", symbol_table[i].kind, symbol_table[i].name, symbol_table[i].val, symbol_table[i].level, symbol_table[i].addr, symbol_table[i].mark);
  }
  printf("\n");

}

void AssemblyDump() {
  for (int i = 0; i < currentToken; i++) {
    switch (code[i].val) {
    case 1:
      strcpy(code[i].name, "LIT");
      break;

    case 2:
      strcpy(code[i].name, "OPR");
      switch (code[i].addr) {
      case 0:
        strcpy(code[i].name, "RTN");
        break;

      case 1:
        strcpy(code[i].name, "ADD");
        break;

      case 2:
        strcpy(code[i].name, "SUB");
        break;

      case 3:
        strcpy(code[i].name, "MUL");
        break;

      case 4:
        strcpy(code[i].name, "DIV");
        break;

      case 5:
        strcpy(code[i].name, "EQL");
        break;

      case 6:
        strcpy(code[i].name, "NEQ");
        break;

      case 7:
        strcpy(code[i].name, "LSS");
        break;

      case 8:
        strcpy(code[i].name, "LEQ");
        break;

      case 9:
        strcpy(code[i].name, "GTR");
        break;

      case 10:
        strcpy(code[i].name, "GEQ");
        break;
      }
      break;

    case 3:
      strcpy(code[i].name, "LOD");
      break;

    case 4:
      strcpy(code[i].name, "STO");
      break;

    case 5:
      strcpy(code[i].name, "CAL");
      break;

    case 6:
      strcpy(code[i].name, "INC");
      break;

    case 7:
      strcpy(code[i].name, "JMP");
      break;

    case 8:
      strcpy(code[i].name, "JPC");
      break;

    case 9:
      strcpy(code[i].name, "SYS");
      break;
    }
  }
  printf("Assembly Code:\n\n");
  printf("Line\tOP\tL\tM\n");
  for (int i = 0; i < codeIndex; i++) {
    printf("%d\t%s\t%d\t%d\n", i, code[i].name, code[i].level, code[i].addr);
  }
}


void emit(int opPrint, int lPrint, int mPrint) {
  code[codeIndex].val = opPrint;
  code[codeIndex].level = lPrint;
  code[codeIndex].addr = mPrint;
  codeIndex++;
  codelength++;
}

int SYMBOLTABLECHECK(char string[]) {
  // linear search through symbol table looking at name
  // return index if found, -1 if not
  int i = 0;

  while (i < symtotal) {
    if (strcmp(symbol_table[i].name, string) == 0) {
      return i;
    }
    i++;
  }
  return -1;
}





void PROGRAM() {
  emit(7, 0, 13);
  BLOCK();
  if (tokenlist[currentToken].value != periodsym) {

    printf("error: program must end with period\n"); // error
    exit(1);
  }
  emit(9, 0, 3);
  AssemblyDump();
  tableDump();
  elfDump();
}

void BLOCK() {
  CONSTDECLARATION();
  int numVars = VARDECLARATION();
  emit(6, 0, 3 + numVars);
  STATEMENT();
}

void CONSTDECLARATION() {
  if (tokenlist[currentToken].value == constsym) {
    do {
      currentToken += 1;
      if (tokenlist[currentToken].value != identsym) {

        printf("error: const, var, and read keywords must be followed by identifier\n"); // error
        exit(1);
      }
      if (SYMBOLTABLECHECK(tokenlist[currentToken].ident) != -1) {

        printf("error: symbol name has already been declared\n"); // error
        exit(1);
      }
      char temp[10];
      strcpy(temp, tokenlist[currentToken].ident); // save ident name
      currentToken += 1;
      if (tokenlist[currentToken].value != eqsym) {

        printf("Error: constants must be assigned with =\n"); // error
        exit(1);
      }
      currentToken += 1;
      if (tokenlist[currentToken].value != numbersym) {

        printf("error: constants must be assigned an integer value\n"); // error
        exit(1);
      }
      // add to symbol table (kind 1, saved name, number, 0, 0)
      symbol_table[symtotal].kind = 1;
      strcpy(symbol_table[symtotal].name, temp);
      symbol_table[symtotal].val = tokenlist[currentToken].number;
      symbol_table[symtotal].level = 0;
      symbol_table[symtotal].addr = 0;
      symtotal++;
      currentToken += 1;

    } while (tokenlist[currentToken].value == commasym);

    if (tokenlist[currentToken].value != semicolonsym) {
      printf("error: constant and variable declarations must be followed by a semicolon\n"); // error token recovery needed

    }
    currentToken += 1;
  }
}

int VARDECLARATION() {
  // – returns number of variables
  int numVars = 0;
  if (tokenlist[currentToken].value == varsym) {
    do {
      numVars++;
      currentToken += 1;
      if (tokenlist[currentToken].value != identsym) {

        printf("error: const, var, and read keywords must be followed by identifier\n"); // error
        exit(1);
      }
      if (SYMBOLTABLECHECK(tokenlist[currentToken].ident) != -1) {

        printf("error: symbol name has already been declared\n"); // error
        exit(1);
      }
      // add to symbol table (kind 2, ident, 0, 0, var# + 2)
      symbol_table[symtotal].kind = 2;
      strcpy(symbol_table[symtotal].name, tokenlist[currentToken].ident);
      symbol_table[symtotal].val = 0;
      symbol_table[symtotal].level = 0;
      symbol_table[symtotal].addr = numVars + 2;
      symtotal++;
      currentToken += 1;
    } while (tokenlist[currentToken].value == commasym);

    if (tokenlist[currentToken].value != semicolonsym) {
      printf("error: constant and variable declarations must be followed by a semicolon\n"); // error
      // token recovery needed
    }

    currentToken += 1;
  }
  return numVars;
}

void STATEMENT() { // fix organization and do-while loops and return statements

  if (tokenlist[currentToken].value == identsym) {

    int symIdx = SYMBOLTABLECHECK(tokenlist[currentToken].ident);
    if (symIdx == -1) {

      printf("error: undeclared identifier\n"); // error
      exit(1);
    }
    if (symbol_table[symIdx].kind != 2) {

      printf("error: only variable values may be altered\n"); // error
      exit(1);
    }
    currentToken += 1;
    if (tokenlist[currentToken].value != becomessym) {

      printf("error: assignment statements must use :=\n"); // error
      exit(1);
    }
    currentToken += 1;
    EXPRESSION();
    emit(4, 0, symbol_table[symIdx].addr); // STO (M = symbol_table[symIdx].addr)

    return;
  }

  if (tokenlist[currentToken].value == beginsym) {
    do {
      currentToken += 1;
      STATEMENT();
    } while (tokenlist[currentToken].value == semicolonsym);

    if (tokenlist[currentToken].value != endsym) {

      printf("error: begin must be followed by end\n"); // errorr
      exit(1);
    }
    currentToken += 1;
    return;
  }

  if (tokenlist[currentToken].value == ifsym) {
    currentToken += 1;
    CONDITION();
    int jpcIdx = codelength;
    emit(8, 0, (jpcIdx*3)+10);    // JPC
    if (tokenlist[currentToken].value != thensym) {

      printf("error: if must be followed by then\n"); // error
      exit(1);
    }
    currentToken += 1;
    STATEMENT();
    if (tokenlist[currentToken].value != fisym)
    {
      printf("error: then must be followed by fi\n"); // error
      exit(1);
    }
    currentToken += 1;
    code[jpcIdx].addr = codelength;
    return;
  }



  if (tokenlist[currentToken].value == whilesym) {
    currentToken += 1;
    int loopIdx = codelength;
    CONDITION(/*currentToken*/);
    if (tokenlist[currentToken].value != dosym) {

      printf("error: while must be followed by do\n");
      exit(1);
    }
    currentToken += 1;
    int jpcIdx = codelength;
    emit(8, 0, (jpcIdx*3)+10); // JPC
    STATEMENT();
    emit(7, 0, (loopIdx*3)+10); // JMP(M = loopIdx)
    code[jpcIdx].addr = codelength;
    return;
  }
  if (tokenlist[currentToken].value == readsym) {
    currentToken += 1;
    if (tokenlist[currentToken].value != identsym) {

      printf("error: const, var, and read keywords must be followed by identifier\n"); // error
      exit(1);
    }
    int symIdx = SYMBOLTABLECHECK(tokenlist[currentToken].ident);
    if (symIdx == -1) {

      printf("error: undeclared identifier\n"); // error
      exit(1);
    }
    if (symbol_table[symIdx].kind != 2) {

      printf("error: token not a number\n"); // error
      exit(1);
    }
    currentToken += 1;
    emit(9, 0, 2); // READ
    emit(4, symbol_table[symIdx].level,
         symbol_table[symIdx].addr); //  STO(M = symbol_table[symIdx].addr)
    return;
  }
  if (tokenlist[currentToken].value == writesym) {
    currentToken += 1;
    EXPRESSION(/*currentToken*/);
    emit(9, 0, 1); // WRITE
    return;
  }
}

void CONDITION() {
  if (tokenlist[currentToken].value == oddsym) {
    currentToken += 1;
    EXPRESSION();
    emit(2, 0, 11); // ODD
  } else {
    EXPRESSION();
    if (tokenlist[currentToken].value == eqsym) {
      currentToken += 1;
      EXPRESSION();
      emit(2, 0, 5); //  EQL
    } else if (tokenlist[currentToken].value == neqsym) {
      currentToken += 1;
      EXPRESSION();
      emit(2, 0, 6); // NEQ
    } else if (tokenlist[currentToken].value == lessym) {
      currentToken += 1;
      EXPRESSION();
      emit(2, 0, 7); // LSS
    } else if (tokenlist[currentToken].value == leqsym) {
      currentToken += 1;
      EXPRESSION();
      emit(2, 0, 8); // LEQ
    } else if (tokenlist[currentToken].value == gtrsym) {
      currentToken += 1;
      EXPRESSION();
      emit(2, 0, 9); // GTR
    } else if (tokenlist[currentToken].value == geqsym) {
      currentToken += 1;
      EXPRESSION();
      emit(2, 0, 10); // GEQ
    } else {

      printf("error: condition must contain comparison operator\n"); //error
      exit(1);
    }
  }
}

void EXPRESSION() {
  TERM();
  while (tokenlist[currentToken].value == plussym ||
         tokenlist[currentToken].value == minussym) {
    if (tokenlist[currentToken].value == plussym) {
      currentToken += 1;
      TERM();
      emit(2, 0, 1); // ADD
    } else {
      currentToken += 1;
      TERM();
      emit(2, 0, 2); // SUB
    }
  }
}

void TERM() {
  FACTOR();
  while (tokenlist[currentToken].value == multsym ||
         tokenlist[currentToken].value == slashsym) {
    if (tokenlist[currentToken].value == multsym) {
      currentToken += 1;
      FACTOR();
      emit(2, 0, 3); // MUL
    } else {
      currentToken += 1;
      FACTOR();
      emit(2, 0, 4); // DIV
    }
  }
}

void FACTOR() {
  int symIdx;
  symIdx = SYMBOLTABLECHECK(tokenlist[currentToken].ident);
  if (tokenlist[currentToken].value == identsym) {

    symIdx = SYMBOLTABLECHECK(tokenlist[currentToken].ident);
    if (symIdx == -1) {

      printf("error: undeclared identifier\n"); // error
      exit(1);
    }
    if (symbol_table[symIdx].kind == 1) {
      emit(1, 0,
           symbol_table[symIdx].val); // LIT (M = symbol_table[symIdx].Value)
    } else {
      emit(3, symbol_table[symIdx].level,
           symbol_table[symIdx].addr); // LOD (M = symbol_table[symIdx].addr)
    }
    currentToken += 1;
  } else if (tokenlist[currentToken].value == numbersym) {
    emit(1, 0, symbol_table[symIdx].val); // LIT
    currentToken += 1;
  } else if (tokenlist[currentToken].value == lparentsym) {
    currentToken += 1;
    EXPRESSION();
    if (tokenlist[currentToken].value != rparentsym) {

      printf("error: right parenthesis must follow left parenthesis\n"); // error
      exit(1);
    }
    currentToken += 1;
  } else {

    printf("error: arithmetic equations must contain operands, parentheses, numbers, or symbols\n"); // error
    exit(1);
  }
}

int getnexttoken(int num) {
  if (recentnum == num) {
    num = recentnum + 1;
    recentnum = num;
  } else {
    num += 1;
    recentnum = num;
  }

  return num;
}


//hw2 starts here
// Function to check whether a full string is a number
int NumChecker(char holder[]) {

  for (int n = 0; holder[n] != '\0'; n++) {
    if (isdigit(holder[n]) == 0){
            //printf("%s", holder);
      return 0;
    }
  }
  return 1;
}

//function to build the words and numbers that go into the table.
int identnum_architect(char pas[], int locale, int LexLocale) {
  //given the first letter/number of a valid token type, it will build the word/number letter by letter and build the tokenlist array and find any errors
  char string[ARRAY_SIZE] ={0};
  int num = 0;
  bool valid = true;


  while (valid == true) {
    string[num] = pas[locale];
    if (isspace(pas[locale + 1]) != 0 || (!isalpha(pas[locale + 1]) && !isdigit(pas[locale + 1]))) {
        valid = false;
    } else {

        num += 1;
        locale += 1;
    }
  }
  if (NumChecker(string) == 1) {
    //checks if the ENTIRE string is a number
    if (num >= MAXNUMLEN){
      //checks if string exceeds maximum number length
      printf("%s ", string);
      printf("Error : Numbers cannot exceed 5 digits\n");
      exit(1);
    }
    else {
      strcpy(tokenlist[LexLocale].name, (string));
      tokenlist[LexLocale].number = atoi(string);
      tokenlist[LexLocale].value = numbersym;
      tokenlist[LexLocale].cond = 2;
      //printf("%d\t 3\n", tokenlist[LexLocale].number);
      total++;
    }
  } else {
    if (isdigit(string[0])){
      //checks if the first character is a number
      char string2[ARRAY_SIZE] ={0};
      string2[0] = string[0];
      int u=1;
      while(isdigit(string[u]))
        {
          string2[u] = string[u];
          u++;
        }

      if(u>=MAXNUMLEN)
      {
        printf("%s ", string2);
        printf("Error : Numbers cannot exceed 5 digits\n");
        exit(1);
      }
      else{
        strcpy(tokenlist[LexLocale].name, (string2));
        tokenlist[LexLocale].number = atoi(string2);
        tokenlist[LexLocale].value = numbersym;
        tokenlist[LexLocale].cond = 2;
        //printf("%d\t 3\n", tokenlist[LexLocale].number);
        total++;
        LexLocale+=1;
      }

      string[num + 1] = '\0';
      int u2=0;
      while(string[u]!= '\0')
        {
          string[u2] = string[u];
          u++;
          u2++;
        }
      if(u2>=MAXLEN)
      {
        printf("%s ", string);
        printf("Error : Identifier names cannot exceed 11 characters\n");
        exit(1);
      }
      else{
      string[u2] = '\0';
      strcpy(tokenlist[LexLocale].ident, (string));
      tokenlist[LexLocale].value = identsym;
      tokenlist[LexLocale].cond = 1;
      //printf("%s\t 2\n", string);
      total++;
      }
    }
    else if (num >= MAXLEN) {
      //checks if string exceeds maximum length
      printf("%s ", string);
      printf("Error : Identifier names cannot exceed 11 characters\n");
      exit(1);
    } else {
      string[num + 1] = '\0';
      strcpy(tokenlist[LexLocale].ident, (string));
      tokenlist[LexLocale].value = identsym;
      tokenlist[LexLocale].cond = 1;
      //printf("%s\t 2\n", string);
      total++;
    }
  }
  return num;
}

//function to print the lexeme table
void lexeme_printer() {
  printf("\n");
  for (int x = 0; x < total; x++) {
    if (tokenlist[x].cond == 1){
      printf("%d %s ", tokenlist[x].value, tokenlist[x].ident);
    }
    else if (tokenlist[x].cond == 2){
      printf("%d %d ", tokenlist[x].value, tokenlist[x].number);
    }
    else{
      printf("%d ", tokenlist[x].value);
    }
    }
  printf("\n");
}


int main(int argc, char* argv [])
{
  //open input file
  FILE* input = fopen(argv[1], "r");

  bool CommentCheck = false;

    // Tests if the file exists
  if (input == NULL)
  {
     printf("File unable to be opened\n");
     return 0;
  }

  //read input file
  //printf("Source Program:\n");
  char v;
  int next=0;
  while(fscanf(input, "%c", &v) != EOF)
  {
    //printf ("%c", v);
    pas[next] = v;
    next += 1;
  }

  pas[next] = '\0'; //for error safety
  fclose(input);

  //"%-12.*s %d\n"  12 characters

  printf("\n\n");
  //printf("Lexeme Table:\n\n");
  //printf("lexeme   token type\n");



  for (int i = 0; i < ARRAY_SIZE; i++) {
    tokenlist[total].cond = 0;

    switch (pas[i]) {
    case '\0':
      i = ARRAY_SIZE;
      break;

    // slashsym 7, comments
    case '/':
      if (pas[i + 1] == '*') {
        CommentCheck = true;
        i += 1;
        while (CommentCheck == true) {
          i += 1;
          if (pas[i] == '*')
            if (pas[i + 1] == '/') {
              CommentCheck = false;
              i += 1;
            }
        }
      } else {
        tokenlist[total].value = slashsym;
        strcpy(tokenlist[total].name, "/");
        //printf("/\t 7\n");
        total++;
      }
      break;

    // eqsym 9
    case '=':
      tokenlist[total].value = eqsym;
      strcpy(tokenlist[total].name, "=");
      //printf("=\t 9\n");
      total++;
      break;

    // neqsym 10, leqsym 12, lessym 11
    case '<':
      if (pas[i + 1] == '>') {
        tokenlist[total].value = neqsym;
        strcpy(tokenlist[total].name, "<>");
        i += 1;
        //printf("<>\t 10\n");
      } else if (pas[i + 1] == '=') {
        tokenlist[total].value = leqsym;
        strcpy(tokenlist[total].name, "<=");
        i += 1;
        //printf("<=\t 12\n");
      } else {
        tokenlist[total].value = lessym;
        strcpy(tokenlist[total].name, "<");
        //printf("<\t 11\n");
      }
      total++;
      break;

    // geqsym 14, gtrsym 13
    case '>':
      if (pas[i + 1] == '=') {
        tokenlist[total].value = geqsym;
        strcpy(tokenlist[total].name, ">=");
        i += 1;
        //printf(">=\t 14\n");
      } else {
        tokenlist[total].value = gtrsym;
        strcpy(tokenlist[total].name, ">");
        //printf(">\t 13\n");
      }
      total++;
      break;

    // plussym 4
    case '+':
      tokenlist[total].value = plussym;
      strcpy(tokenlist[total].name, "+");
      //printf("+\t 4\n");
      total++;
      break;

    // minussym 5
    case '-':
      tokenlist[total].value = minussym;
      strcpy(tokenlist[total].name, "-");
      //printf("-\t 5\n");
      total++;
      break;

    // multsym 6
    case '*':
      tokenlist[total].value = multsym;
      strcpy(tokenlist[total].name, "*");
      //printf("*\t 6\n");
      total++;
      break;

    // lparentsym 15
    case '(':
      tokenlist[total].value = lparentsym;
      strcpy(tokenlist[total].name, "(");
      //printf("(\t 15\n");
      total++;
      break;

    // rparentsym 16
    case ')':
      tokenlist[total].value = rparentsym;
      strcpy(tokenlist[total].name, ")");
      //printf(")\t 16\n");
      total++;
      break;

    // commasym 17
    case ',':
      tokenlist[total].value = commasym;
      strcpy(tokenlist[total].name, ",");
      //printf(",\t 17\n");
      total++;
      break;

    // semicolonsym 18
    case ';':
      tokenlist[total].value = semicolonsym;
      strcpy(tokenlist[total].name, ";");
      //printf(";\t 18\n");
      total++;
      break;

    // periodsym 19
    case '.':
      tokenlist[total].value = periodsym;
      strcpy(tokenlist[total].name, ".");
      //printf(".\t 19\n");
      total++;
      break;

    // becomesym 20
    case ':':
      if (pas[i + 1] == '=') {
        tokenlist[total].value = becomessym;
        strcpy(tokenlist[total].name, ":=");
        i += 1;
        //printf(":=\t 20\n");
        total++;
      } else {
        printf(": ");
        printf("Error : Invalid Symbol\n");
        exit(1);
      }
      break;
      // xorsym 8
     /* case 'x':
      if (pas[i + 1] == 'o')
        if (pas[i + 2] == 'r') {
          if (isspace(pas[i + 3]) == 1 || isalpha(pas[i + 3]) == 0) {
            tokenlist[total].value = xorsym;
            strcpy(tokenlist[total].name, "xor");
            i += 2;
            total++;
            printf("xor\t 8\n");
          } else
            i += identnum_architect(pas, i, total);
        } else
          i += identnum_architect(pas, i, total);
      else
        i += identnum_architect(pas, i, total);
      break;
      */

    // beginsym 21
    case 'b':
      if (pas[i + 1] == 'e')
        if (pas[i + 2] == 'g')
          if (pas[i + 3] == 'i')
            if (pas[i + 4] == 'n') {
              if (isspace(pas[i + 5]) == 1 || isalpha(pas[i + 5]) == 0) {
                tokenlist[total].value = beginsym;
                strcpy(tokenlist[total].name, "begin");
                i += 4;
                total++;
                //printf("begin\t 21\n");
              }

              else
                i += identnum_architect(pas, i, total);
            } else
              i += identnum_architect(pas, i, total);
          else
            i += identnum_architect(pas, i, total);
        else
          i += identnum_architect(pas, i, total);
      else
        i += identnum_architect(pas, i, total);
      break;

      // endsym 22, elsesym 33
    case 'e':
      if (pas[i + 1] == 'n')
        if (pas[i + 2] == 'd') {
          if (isspace(pas[i + 3]) == 1 || isalpha(pas[i + 3]) == 0) {
            tokenlist[total].value = endsym;
            strcpy(tokenlist[total].name, "end");
            i += 2;
            total++;
            //printf("end\t 22\n");
          } else
            i += identnum_architect(pas, i, total);
        } else {
          i += identnum_architect(pas, i, total);
        }
        /*
      else if (pas[i + 1] == 'l')
        if (pas[i + 2] == 's')
          if (pas[i + 3] == 'e')
            if (isspace(pas[i + 4]) == 1 || isalpha(pas[i + 4]) == 0) {
              tokenlist[total].value = elsesym;
              strcpy(tokenlist[total].name, "else");
              i += 3;
              total++;
              //printf("else\t 33\n");
            } else
              i += identnum_architect(pas, i, total);
          else
            i += identnum_architect(pas, i, total);
        else
          i += identnum_architect(pas, i, total);
      else
        i += identnum_architect(pas, i, total);
      */
      break;

    // ifsym 23
    case 'i':
      if (pas[i + 1] == 'f')
        if (isspace(pas[i + 2]) == 1 || isalpha(pas[i + 2]) == 0) {
          tokenlist[total].value = ifsym;
          strcpy(tokenlist[total].name, "if");
          i += 1;
          //printf("if\t 23\n");
          total++;
        } else
          i += identnum_architect(pas, i, total);
      else
        i += identnum_architect(pas, i, total);
      break;
      //fisym 8
      case 'f':
        if (pas[i + 1] == 'i')
          if (isspace(pas[i + 2]) == 1 || isalpha(pas[i + 2]) == 0) {
            tokenlist[total].value = fisym;
            strcpy(tokenlist[total].name, "fi");
            i += 1;
            //printf("fi\t 8\n");
            total++;
          } else
            i += identnum_architect(pas, i, total);
        else
          i += identnum_architect(pas, i, total);
        break;

      // thensym 24
    case 't':
      if (pas[i + 1] == 'h')
        if (pas[i + 2] == 'e')
          if (pas[i + 3] == 'n')
            if (isspace(pas[i + 4]) == 1 || isalpha(pas[i + 4]) == 0) {
              tokenlist[total].value = thensym;
              strcpy(tokenlist[total].name, "then");
              i += 3;
              //printf("then\t 24\n");
              total++;
            } else
              i += identnum_architect(pas, i, total);
          else
            i += identnum_architect(pas, i, total);
        else
          i += identnum_architect(pas, i, total);
      else
        i += identnum_architect(pas, i, total);
      break;

      // whilesym 25, writesym 31
    case 'w':
      if (pas[i + 1] == 'h')
        if (pas[i + 2] == 'i')
          if (pas[i + 3] == 'l')
            if (pas[i + 4] == 'e')
              if (isspace(pas[i + 5]) == 1 || isalpha(pas[i + 5]) == 0) {
                tokenlist[total].value = whilesym;
                strcpy(tokenlist[total].name, "while");
                i += 4;
                total++;
                //printf("while\t 25\n");
              } else
                i += identnum_architect(pas, i, total);
            else
              i += identnum_architect(pas, i, total);
          else
            i += identnum_architect(pas, i, total);
        else
          i += identnum_architect(pas, i, total);

      else if (pas[i + 1] == 'r')
        if (pas[i + 2] == 'i')
          if (pas[i + 3] == 't')
            if (pas[i + 4] == 'e')
              if (isspace(pas[i + 5]) == 1 || isalpha(pas[i + 5]) == 0) {
                tokenlist[total].value = writesym;
                strcpy(tokenlist[total].name, "write");
                i += 4;
                total++;
                //printf("write\t 31\n");
              } else
                i += identnum_architect(pas, i, total);
            else
              i += identnum_architect(pas, i, total);
          else
            i += identnum_architect(pas, i, total);
        else
          i += identnum_architect(pas, i, total);
      else
        i += identnum_architect(pas, i, total);
      break;
      // dosym 26
    case 'd':
      if (pas[i + 1] == 'o')
        if (isspace(pas[i + 2]) == 1 || isalpha(pas[i + 2]) == 0) {
          tokenlist[total].value = dosym;
          strcpy(tokenlist[total].name, "do");
          i += 1;
          //printf("do\t 26\n");
          total++;
        } else
          i += identnum_architect(pas, i, total);
      else
        i += identnum_architect(pas, i, total);
      break;

      // oddsym 1
    case 'o':
      if (pas[i + 1] == 'd')
        if (pas[i + 2] == 'd')
          if (isspace(pas[i + 3]) == 1 || isalpha(pas[i + 3]) == 0) {
            tokenlist[total].value = oddsym;
            strcpy(tokenlist[total].name, "odd");
            i += 2;
            //printf("odd\t 1\n");
            total++;
          } else
            i += identnum_architect(pas, i, total);
        else
          i += identnum_architect(pas, i, total);
      else
        i += identnum_architect(pas, i, total);
      break;

      // callsym 27, constsym 28
    case 'c':
      /*
      if (pas[i + 1] == 'a')
        if (pas[i + 2] == 'l')
          if (pas[i + 3] == 'l')
            if (isspace(pas[i + 4]) == 1 || isalpha(pas[i + 4]) == 0) {
              tokenlist[total].value = callsym;
              strcpy(tokenlist[total].name, "call");
              i += 3;
              //printf("call\t 27\n");
              total++;
            } else
              i += identnum_architect(pas, i, total);
          else
            i += identnum_architect(pas, i, total);
        else
          i += identnum_architect(pas, i, total);
        */

      /* else */ if (pas[i + 1] == 'o')
        if (pas[i + 2] == 'n')
          if (pas[i + 3] == 's')
            if (pas[i + 4] == 't') {
              if (isspace(pas[i + 5]) == 1 || isalpha(pas[i + 5]) == 0) {
                tokenlist[total].value = constsym;
                strcpy(tokenlist[total].name, "const");
                i += 4;
                //printf("const\t 28\n");
                total++;
              } else
                i += identnum_architect(pas, i, total);
            } else
              i += identnum_architect(pas, i, total);
          else
            i += identnum_architect(pas, i, total);
        else
          i += identnum_architect(pas, i, total);
      else
        i += identnum_architect(pas, i, total);
      break;

    // varsym 29
    case 'v':
      if (pas[i + 1] == 'a')
        if (pas[i + 2] == 'r') {
          if (isspace(pas[i + 3]) == 1 || isalpha(pas[i + 3]) == 0) {
            tokenlist[total].value = varsym;
            strcpy(tokenlist[total].name, "var");
            i += 2;
            //printf("var\t 29\n");
            total++;
          } else
            i += identnum_architect(pas, i, total);
        } else
          i += identnum_architect(pas, i, total);
      else
        i += identnum_architect(pas, i, total);
      break;

      // procsym 30
      /*
    case 'p':
      if (pas[i + 1] == 'r') {

        if (pas[i + 2] == 'o') {

          if (pas[i + 3] == 'c') {

            if (pas[i + 4] == 'e') {

              if (pas[i + 5] == 'd') {

                if (pas[i + 6] == 'u') {

                  if (pas[i + 7] == 'r') {

                    if (pas[i + 8] == 'e') {

                      if (isspace(pas[i + 9]) == 1 ||
                          isalpha(pas[i + 9]) == 0) {
                        tokenlist[total].value = procsym;
                        strcpy(tokenlist[total].name, "procedure");
                        i += 8;
                        total++;
                       // printf("procedure\t 30\n");

                      } else {
                        i += identnum_architect(pas, i, total);
                      }
                    } else {
                      i += identnum_architect(pas, i, total);
                    }
                  }

                  else {
                    i += identnum_architect(pas, i, total);
                  }
                } else {
                  i += identnum_architect(pas, i, total);
                }
              } else {
                i += identnum_architect(pas, i, total);
              }
            } else {
              i += identnum_architect(pas, i, total);
            }
          } else {
            i += identnum_architect(pas, i, total);
          }
        } else {
          i += identnum_architect(pas, i, total);
        }
      } else {
        i += identnum_architect(pas, i, total);
      }

      break;
      */

      // readsym 32
    case 'r':
      if (pas[i + 1] == 'e')
        if (pas[i + 2] == 'a')
          if (pas[i + 3] == 'd')
            if (isspace(pas[i + 4]) == 1 || isalpha(pas[i + 4]) == 0) {
              tokenlist[total].value = readsym;
              strcpy(tokenlist[total].name, "read");
              i += 3;
             // printf("read\t 32\n");
              total++;
            } else
              i += identnum_architect(pas, i, total);
          else
            i += identnum_architect(pas, i, total);
        else
          i += identnum_architect(pas, i, total);
      else
        i += identnum_architect(pas, i, total);
      break;

    default:
      // for any other letter
      if (isalpha(pas[i]) != 0) {
        i += identnum_architect(pas, i, total);
      }

      // for any other number
      else if (pas[i] >= '0' && pas[i] <= '9') {
        i += identnum_architect(pas, i, total);
      }

      // for any other invalid symbol
      else {
        if (isspace(pas[i]) == 0){
          printf("%c ", pas[i]);
          printf("Error : Invalid Symbol\n");
          exit(1);
        }
      }
      break;
    }
  }



 // printf("\nToken List:\n");
  //lexeme_printer(); //prints token list

  PROGRAM(); //start of HW3
  return 0;
}

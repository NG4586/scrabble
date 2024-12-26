// Nathaniel Graves
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <cstdlib>
#include <ctime>
using std::ifstream;
using std::getline;
using std::string;
using std::toupper;
using std::stoi;
using std::vector;
using std::set;
using std::cin;
using std::cout;
using std::endl;

#define SIZE 15

const string VALS[11] = {" ", "₁", "₂", "₃", "₄", "₅", "₆", "₇", "₈", "₉", "₀"};

const int MODS[15][15] = {4, 0, 0, 1, 0, 0, 0, 4, 0, 0, 0, 1, 0, 0, 4,
                          0, 3, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 3, 0,
                          0, 0, 3, 0, 0, 0, 1, 0, 1, 0, 0, 0, 3, 0, 0,
                          1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 1,
                          0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0,
                          0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0,
                          0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0,
                          4, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 4,
                          0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0,
                          0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0,
                          0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0,
                          1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 1,
                          0, 0, 3, 0, 0, 0, 1, 0, 1, 0, 0, 0, 3, 0, 0,
                          0, 3, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 3, 0,
                          4, 0, 0, 1, 0, 0, 0, 4, 0, 0, 0, 1, 0, 0, 4};

const int BAG[27] = {9, 2, 2, 4, 12, 2, 3, 2, 9,
                     1, 1, 4, 2, 6, 8, 2, 1, 6,
                     4, 6, 4, 2, 2, 1, 2, 1, 2};

const int POINTS[27] = {1, 3, 3, 2, 1, 4, 2, 4, 1,
                        8, 5, 1, 3, 1, 1, 3, 10, 1,
                        1, 1, 1, 4, 4, 8, 4, 10, 0};

const string CODES[6] = {" is not a word", "Out of bounds",
                         "Blocked by existing tiles", "Required letters not present",
                         "No new tiles placed", "No connecting tile or start position"};


struct tile
{
  char symbol;
  int value;
};

struct square
{
  bool empty;
  tile letter;
  int modifier;
};

struct player
{
  vector<tile> letters;
  int points;
};


tile makeTile(char letter, int points)
{
  tile newTile;
  newTile.symbol = letter;
  newTile.value = points;
  return newTile;
}


void addTile(vector<tile>* source, vector<tile>* target)
{
  int index = rand() % (source -> size());
  target -> push_back(source -> operator[](index));
  source -> erase((source -> begin()) + index);
}


set<string> loadWords(string filename)
{
  ifstream source(filename);
  set<string> words;
  string word;
  while (getline(source, word))
    {
      words.insert(word);
    }
  return words;
}


void rowShift(int* row, int* col, int amount)
{
  *row += amount;
}


void colShift(int* row, int* col, int amount)
{
  *col += amount;
}


int playerMove(square board[SIZE][SIZE], player moving, set<int> blanks,
               set<string>* words, string word, int row, int col, char direction)
{
  if (!(words -> count(word)))
    {
      return -6;
    }
  if ((row < 0) || (row >= SIZE) || (col < 0) || (col >= SIZE))
    {
      return -5;
    }
  void (*shift)(int*, int*, int);
  void (*antishift)(int*, int*, int);
  if (direction == 'r')
    {
      if (col + (word.size() - 1) >= SIZE)
        {
          return -5;
        }
      if ((col - 1 >= 0 && !board[row][col - 1].empty) ||
          (col + word.size() < SIZE && !board[row][col + word.size()].empty))
        {
          return -4;
        }
      shift = colShift;
      antishift = rowShift;
    }
  else
    {
      if (row + (word.size() - 1) >= SIZE)
        {
          return -5;
        }
      if ((row - 1 >= 0 && !board[row - 1][col].empty) ||
          (row + word.size() < SIZE && !board[row + word.size()][col].empty))
        {
          return -4;
        }
      shift = rowShift;
      antishift = colShift;
    }
  int r = row;
  int c = col;
  bool addition = false;
  bool connection = false;
  set<int> assigned;
  int base = 0;
  int multiplier = 1;
  string crossWord;
  int crossPoints;
  int bonusPoints = 0;
  for (int index = 0; index < word.size(); index ++)
    {
      if (board[r][c].empty)
        {
          addition = true;
          if (r == SIZE / 2 && c == r)
            {
              connection = true;
            }
          if (!blanks.count(index))
            {
              for (int n = 0; n < (moving.letters).size(); n ++)
                {
                  if (!assigned.count(n) && (moving.letters)[n].symbol == word[index])
                    {
                      base += (moving.letters)[n].value;
                      if (board[r][c].modifier == 1 || board[r][c].modifier == 2)
                        {
                          base += (moving.letters)[n].value * board[r][c].modifier;
                        }
                      assigned.insert(n);
                      break;
                    }
                  if (n == (moving.letters).size() - 1)
                    {
                      return -3;
                    }
                }
            }
          if (board[r][c].modifier == 3 || board[r][c].modifier == 4)
            {
              multiplier *= board[r][c].modifier - 1;
            }
          if (direction == 'r' && (r > 0 && !board[r - 1][c].empty ||
                                   r < SIZE - 1 && !board[r + 1][c].empty) ||
              direction == 'd' && (c > 0 && !board[r][c - 1].empty ||
                                   c < SIZE - 1 && !board[r][c + 1].empty))
            {
              crossWord = "";
              crossPoints = 0;
              do
                {
                  antishift(&r, &c, -1);
                }
              while (r >= 0 && c >= 0 && !board[r][c].empty);
              antishift(&r, &c, 1);
              while (!board[r][c].empty)
                {
                  crossWord += (board[r][c].letter).symbol;
                  crossPoints += (board[r][c].letter).value;
                  antishift(&r, &c, 1);
                }
              crossWord += word[index];
              if (!blanks.count(index))
                {
                  crossPoints += POINTS[word[index] - 'A'];
                  if (board[r][c].modifier == 1 || board[r][c].modifier == 2)
                    {
                      crossPoints += POINTS[word[index] - 'A'] * board[r][c].modifier;
                    }
                }
              antishift(&r, &c, 1);
              while (r < SIZE && c < SIZE && !board[r][c].empty)
                {
                  crossWord += (board[r][c].letter).symbol;
                  crossPoints += (board[r][c].letter).value;
                  antishift(&r, &c, 1);
                }
              do
                {
                  antishift(&r, &c, -1);
                }
              while (!board[r][c].empty);
              if (!(words -> count(crossWord)))
                {
                  return -6;
                }
              if (board[r][c].modifier == 3 || board[r][c].modifier == 4)
                {
                  crossPoints *= board[r][c].modifier - 1;
                }
              bonusPoints += crossPoints;
              connection = true;
            }
        }
      else
        {
          if ((board[r][c].letter).symbol != word[index])
            {
              return -4;
            }
          connection = true;
          base += (board[r][c].letter).value;
        }
      shift(&r, &c, 1);
    }
  if (!addition)
    {
      return -2;
    }
  if (!connection)
    {
      return -1;
    }
  if (assigned.size() + blanks.size() == 7)
    {
      bonusPoints += 50;
    }
  return base * multiplier + bonusPoints;
}

void implement(square (*board)[SIZE][SIZE], player* moving, set<int> blanks,
               string word, int row, int col, char direction)
{
  for (int index = 0; index < word.size(); index ++)
    {
      if (blanks.count(index))
        {
          for (int n = 0; n < (moving -> letters).size(); n ++)
            {
              if ((moving -> letters)[n].symbol == ' ')
                {
                  (moving -> letters)[n].symbol = word[index];
                  break;
                }
            }
        }
    }
  set<int> marked;
  for (int index = 0; index < word.size(); index ++)
    {
      if ((*board)[row][col].empty)
        {
          for (int n = 0; n < (moving -> letters).size(); n ++)
            {
              if (!marked.count(index) && (moving -> letters)[n].symbol == word[index])
                {
                  (*board)[row][col].letter = (moving -> letters)[n];
                  (*board)[row][col].modifier = 0;
                  (*board)[row][col].empty = false;
                  (moving -> letters).erase((moving -> letters).begin() + n);
                  break;
                }
            }
        }
      if (direction == 'r')
        {
          col ++;
        }
      else
        {
          row ++;
        }
    }
}


string crossFinder(square board[SIZE][SIZE], set<string>* words,
                   string word, int row, int col, char direction)
{
  void (*shift)(int*, int*, int);
  void (*antishift)(int*, int*, int);
  if (direction == 'r')
    {
      shift = colShift;
      antishift = rowShift;
    }
  else
    {
      shift = rowShift;
      antishift = colShift;
    }
  int r = row;
  int c = col;
  string crossWord;
  for (int index = 0; index < word.size(); index ++)
    {
      if (board[r][c].empty &&
          (direction == 'r' && (r > 0 && !board[r - 1][c].empty ||
                                r < SIZE - 1 && !board[r + 1][c].empty) ||
           direction == 'd' && (c > 0 && !board[r][c - 1].empty ||
                                c < SIZE - 1 && !board[r][c + 1].empty)))
        {
          crossWord = "";
          do
            {
              antishift(&r, &c, -1);
            }
          while (r >= 0 && c >= 0 && !board[r][c].empty);
          antishift(&r, &c, 1);
          while (!board[r][c].empty)
            {
              crossWord += (board[r][c].letter).symbol;
              antishift(&r, &c, 1);
            }
          crossWord += word[index];
          antishift(&r, &c, 1);
          while (r < SIZE && c < SIZE && !board[r][c].empty)
            {
              crossWord += (board[r][c].letter).symbol;
              antishift(&r, &c, 1);
            }
          do
            {
              antishift(&r, &c, -1);
            }
          while (!board[r][c].empty);
          if (!(words -> count(crossWord)))
            {
              return crossWord;
            }
        }
      shift(&r, &c, 1);
    }
  return "error";
}


int safeStoi(string input)
{
  bool valid = false;
  while (!valid)
    {
      for (int n = 0; n < input.size(); n ++)
        {
          if ((input[n] < '0') || (input[n] > '9'))
            {
              cout << "Numerical input required" << endl;
              cin >> input;
              break;
            }
          if (n == input.size() - 1)
            {
              valid = true;
            }
        }
    }
  return stoi(input);
}


bool humanTurn(square (*board)[SIZE][SIZE], set<string>* words,
               vector<tile>* pool, player* human)
{
  int score = -1;
  string input;
  string word;
  int row;
  int col;
  char direction;
  set<int> blanks;
  do
    {
      cin >> input;
      if (input[0] == '*')
        {
          if (input.substr(1) == "shuffle")
            {
              vector<tile> newOrder;
              int nextTile;
              while (!(human -> letters).empty())
                {
                  nextTile = rand() % (human -> letters).size();
                  newOrder.push_back((human -> letters)[nextTile]);
                  (human -> letters).erase((human -> letters).begin() + nextTile);
                }
              (human -> letters) = newOrder;
              cout << endl << "  ";
              for (int n = 0; n < (human -> letters).size(); n ++)
                {
                  cout << "┌───┐";
                }
              cout << endl << "  ";
              for (int n = 0; n < (human -> letters).size(); n ++)
                {
                  cout << "│ " << (human -> letters)[n].symbol;
                  cout << VALS[(human -> letters)[n].value] << "│";
                }
              cout << endl << "  ";
              for (int n = 0; n < (human -> letters).size(); n ++)
                {
                  cout << "└───┘";
                }
              cout << endl << endl;
              continue;
            }
          if (input.substr(1) == "exchange")
            {
              cin >> input;
              set<int> replace;
              vector<tile> newLetters;
              for (int n = 0; n < input.size(); n ++)
                {
                  pool -> push_back((human -> letters)[input[n] - '0']);
                  replace.insert(input[n] - '0');
                }
              for (int n = 0; n < (human -> letters).size(); n ++)
                {
                  if (!replace.count(n))
                    {
                      newLetters.push_back((human -> letters)[n]);
                    }
                }
              (human -> letters) = newLetters;
              for (int n = 0; n < input.size(); n ++)
                {
                  addTile(pool, &(human -> letters));
                }
              return false;
            }
          if (input.substr(1) == "pass")
            {
              return false;
            }
          cout << "Invalid command" << endl;
          continue;
        }
      for (int n = 0; n < input.size(); n ++)
        {
          input[n] = toupper(input[n]);
        }
      word = input;
      cin >> input;
      row = safeStoi(input.substr(1)) - 1;
      col = input[0] - 'a';
      cin >> input;
      direction = input[0];
      blanks.clear();
      for (int n = 0; n < (human -> letters).size(); n ++)
        {
          if ((human -> letters)[n].symbol == ' ')
            {
              cout << "Position blank #" << blanks.size() + 1 << " (0 = not using)" << endl;
              cin >> input;
              int index = safeStoi(input);
              while (index > word.size())
                {
                  cout << word << " is length " << word.size() << endl;
                  cin >> input;
                  index = safeStoi(input);
                }
              if (index > 0)
                {
                  blanks.insert(index - 1);
                }
            }
        }
      score = playerMove(*board, *human, blanks, words, word, row, col, direction);
      if (score < 0)
        {
          if (score == -6)
            {
              if (!(words -> count(word)))
                {
                  cout << word;
                }
              else
                {
                  cout << crossFinder(*board, words, word, row, col, direction);
                }
              cout << CODES[0] << endl;
            }
          else
            {
              cout << CODES[score + 6] << endl;
            }
        }
    }
  while (score < 0);
  implement(board, human, blanks, word, row, col, direction);
  (human -> points) += score;
  return true;
}


vector<string> combinations(vector<tile> letters, string sequence, int len)
{
  vector<string> sol;
  if (len == 0)
    {
      sol.push_back(sequence);
      return sol;
    }
  vector<tile> remaining;
  vector<string> subSol;
  char numCode;
  for (int n = 0; n < letters.size(); n ++)
    {
      for (int m = 0; m < letters.size(); m ++)
        {
          if (m != n)
            {
              remaining.push_back(letters[m]);
            }
        }
      if (letters[n].symbol == ' ')
        {
          /*for (char c = 'A'; c <= 'Z'; c ++)
            {
              numCode = '0' + n;
              subSol = combinations(remaining, sequence + numCode + c, len - 1);
            }*/
          subSol = combinations(remaining, sequence + 'S', len - 1);
        }
      else
        {
          subSol = combinations(remaining, sequence + letters[n].symbol, len - 1);
        }
      for (int m = 0; m < subSol.size(); m ++)
        {
          sol.push_back(subSol[m]);
        }
      remaining.clear();
    }
  return sol;
}


vector<string> connections(square (*board)[SIZE][SIZE], int row, int col,
                           int len, char direction, vector<tile> letters)
{
  void (*shift)(int*, int*, int);
  if (direction == 'r')
    {
      shift = colShift;
    }
  else
    {
      shift = rowShift;
    }
  int r = row;
  int c = col;
  int lenTest = len;
  bool connection = false;
  while (r < SIZE && c < SIZE)
    {
      if ((*board)[r][c].empty)
        {
          lenTest --;
          if (r == SIZE / 2 && c == r)
            {
              connection = true;
            }
          if ((r > 0 && !(*board)[r - 1][c].empty) ||
              (r < SIZE - 1 && !(*board)[r + 1][c].empty) ||
              (c > 0 && !(*board)[r][c - 1].empty) ||
              (c < SIZE - 1 && !(*board)[r][c + 1].empty))
            {
              connection = true;
            }
        }
      else
        {
          connection = true;
        }
      shift(&r, &c, 1);
    }
  vector<string> tests;
  if (connection && lenTest <= 0)
    {
      vector<string> placements = combinations(letters, "", len);
      for (int n = 0; n < placements.size(); n ++)
        {
          tests.push_back("");
          r = row;
          c = col;
          lenTest = 0;
          while (r < SIZE && c < SIZE)
            {
              if ((*board)[r][c].empty)
                {
                  if (lenTest >= placements[n].size())
                    {
                      break;
                    }
                  tests.back() += placements[n][lenTest];
                  if (placements[n][lenTest] < 'A') // TODO: test
                    {
                      lenTest ++;
                      tests.back() += placements[n][lenTest];
                    }
                  lenTest ++;
                }
              else
                {
                  tests.back() += ((*board)[r][c].letter).symbol;
                }
              shift(&r, &c, 1);
            }
        }
    }
  return tests;
}


string computerTurn(square (*board)[SIZE][SIZE], set<string>* words, player* computer)
{
  string bestWord;
  int bestRow;
  int bestCol;
  char bestDirection;
  set<int> bestBlanks;
  int maxPoints = -1;
  vector<string> testWords;
  int testPoints;
  set<int> blanks;
  string word;
  for (int len = 1; len <= (computer -> letters).size(); len ++)
    {
      for (int r = 0; r < SIZE; r ++)
        {
          for (int c = 0; c < SIZE - (len - 1); c ++)
            {
              testWords = connections(board, r, c, len, 'r', (computer -> letters));
              for (int n = 0; n < testWords.size(); n ++)
                {
                  word = "";
                  for (int m = 0; m < testWords[n].size(); m ++)
                    {
                      if (testWords[n][m] < 'A') // TODO: test
                        {
                          blanks.insert(m - '0');
                        }
                      else
                        {
                          word += testWords[n][m];
                        }
                    }
                  testPoints = playerMove(*board, *computer, blanks, words, word, r, c, 'r');
                  if (testPoints > maxPoints)
                    {
                      bestWord = word;
                      bestRow = r;
                      bestCol = c;
                      bestDirection = 'r';
                      bestBlanks = blanks;
                      maxPoints = testPoints;
                    }
                  blanks.clear();
                }
            }
        }
      for (int r = 0; r < SIZE - (len - 1); r ++)
        {
          for (int c = 0; c < SIZE; c ++)
            {
              testWords = connections(board, r, c, len, 'd', (computer -> letters));
              for (int n = 0; n < testWords.size(); n ++)
                {
                  word = "";
                  for (int m = 0; m < testWords[n].size(); m ++)
                    {
                      if (testWords[n][m] < 'A')
                        {
                          blanks.insert(m - '0');
                        }
                      else
                        {
                          word += testWords[n][m];
                        }
                    }
                  testPoints = playerMove(*board, *computer, blanks, words, word, r, c, 'd');
                  if (testPoints > maxPoints)
                    {
                      bestWord = word;
                      bestRow = r;
                      bestCol = c;
                      bestDirection = 'd';
                      bestBlanks = blanks;
                      maxPoints = testPoints;
                    }
                  blanks.clear();
                }
            }
        }
    }
  if (maxPoints >= 0)
    {
      implement(board, computer, bestBlanks, bestWord, bestRow, bestCol, bestDirection);
      (computer -> points) += maxPoints;
      return bestWord;
    }
  return "";
}


string code(square info)
{
  if (info.empty)
    {
      switch (info.modifier)
        {
          case 1:
            return "\033[36m2ₓ\033[0m";
          case 2:
            return "\033[34m3ₓ\033[0m";
          case 3:
            return "\033[35m2ₓ\033[0m";
          case 4:
            return "\033[31m3ₓ\033[0m";
          default:
            return "  ";
        }
    }
  return (info.letter).symbol + VALS[(info.letter).value];
}


void output(square board[SIZE][SIZE], vector<player> states)
{
  cout << endl << " ";
  for (int n = 0; n < SIZE; n ++)
    {
      cout << "   " << (char) ('a' + n);
    }
  cout << endl << "  ┌";
  for (int n = 0; n < SIZE - 1; n ++)
    {
      cout << "───┬";
    }
  cout << "───┐" << endl;
  for (int n = 0; n < SIZE - 1; n ++)
    {
      if ((n + 1) < 10)
        {
          cout << " ";
        }
      cout << (n + 1) << "│";
      for (int m = 0; m < SIZE; m ++)
        {
          cout << " " + code(board[n][m]) + "│";
        }
      cout << endl << "  ├";
      for (int m = 0; m < SIZE - 1; m ++)
        {
          cout << "───┼";
        }
      cout << "───┤" << endl;
    }
  if (SIZE < 10)
    {
      cout << " ";
    }
  cout << SIZE << "│";
  for (int n = 0; n < SIZE; n ++)
    {
      cout << " " + code(board[SIZE - 1][n]) + "│";
    }
  cout << endl << "  └";
  for (int n = 0; n < SIZE - 1; n ++)
    {
      cout << "───┴";
    }
  cout << "───┘" << endl << endl;
  cout << "  You:  " << states[0].points << endl;
  for (int n = 1; n < states.size(); n ++)
    {
      cout << "  CPU" << (n + 1) << ": " << states[n].points << endl;
    }
  cout << endl << "  ";
  for (int n = 0; n < (states[0].letters).size(); n ++)
    {
      cout << "┌───┐";
    }
  cout << endl << "  ";
  for (int n = 0; n < (states[0].letters).size(); n ++)
    {
      cout << "│ " << (states[0].letters)[n].symbol;
      cout << VALS[(states[0].letters)[n].value] << "│";
    }
  cout << endl << "  ";
  for (int n = 0; n < (states[0].letters).size(); n ++)
    {
      cout << "└───┘";
    }
  cout << endl << endl;
}


int main(int argc, char** argv)
{
  srand(time(NULL));
  if (argc < 2)
    {
      cout << "Specify number of players (2-4)." << endl;
      return 1;
    }
  if (argc < 3)
    {
      cout << "Specify dictionary source file." << endl;
      return 1;
    }
  square board[SIZE][SIZE];
  for (int n = 0; n < SIZE; n ++)
    {
      for (int m = 0; m < SIZE; m ++)
        {
          board[n][m].empty = true;
          board[n][m].modifier = MODS[n][m];
        }
    }
  vector<tile> pool;
  for (int n = 0; n < 26; n ++)
    {
      for (int m = 0; m < BAG[n]; m ++)
        {
          pool.push_back(makeTile('A' + n, POINTS[n]));
        }
    }
  for (int n = 0; n < BAG[26]; n ++)
    {
      pool.push_back(makeTile(' ', POINTS[26]));
    }
  int numPlayers = safeStoi(argv[1]);
  vector<player> gameState;
  player newPlayer;
  for (int n = 0; n < numPlayers; n ++)
    {
      newPlayer.letters = vector<tile>();
      newPlayer.points = 0;
      for (int m = 0; m < 7; m ++)
        {
          addTile(&pool, &(newPlayer.letters));
        }
      gameState.push_back(newPlayer);
    }
  set<string> words = loadWords(argv[2]);
  string computerWord;
  set<int> passed;
  while (passed.size() < gameState.size())
    {
      output(board, gameState);
      if (!humanTurn(&board, &words, &pool, &(gameState[0])))
        {
          passed.insert(0);
        }
      else
        {
          while (pool.size() > 0 && (gameState[0].letters).size() < 7)
            {
              addTile(&pool, &(gameState[0].letters));
            }
          if (passed.count(0))
            {
              passed.erase(0);
            }
        }
      cout << endl;
      for (int n = 1; n < gameState.size(); n ++)
        {
          computerWord = computerTurn(&board, &words, &(gameState[n]));
          if (computerWord == "")
            {
              cout << "CPU" << (n + 1) << " passes" << endl;
              passed.insert(n);
            }
          else
            {
              cout << "CPU" << (n + 1) << " plays " << computerWord << endl;
              while (pool.size() > 0 && (gameState[n].letters).size() < 7)
                {
                  addTile(&pool, &(gameState[n].letters));
                }
              if (passed.count(n))
                {
                  passed.erase(n);
                }
            }
        }
    }
  cout << endl << "All players have passed." << endl;
  cout << "Final score:" << endl;
  cout << "  You:  " << gameState[0].points << endl;
  int highScore = 0;
  for (int n = 1; n < gameState.size(); n ++)
    {
      cout << "  CPU" << (n + 1) << ": " << gameState[n].points << endl;
      if (gameState[n].points > gameState[highScore].points)
        {
          highScore = n;
        }
    }
  cout << endl;
  if (highScore == 0)
    {
      cout << "You win!" << endl;
    }
  else
    {
      cout << "CPU" << (highScore + 1) << " wins." << endl;
    }
  return 0;
}

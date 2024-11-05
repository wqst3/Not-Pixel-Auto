#include <iostream>

#include <chrono>
#include <thread>
#include <iomanip>

#include <windows.h>

#include <fstream>
#include <filesystem>

#include <vector>
#include <string>
#include <tchar.h>


const int TextSpeed = 10;


namespace {
	struct Vector {
		int X;
		int Y;
	};
	
	struct WindowCoordinates {
		int X;
		int Y;
		
		WindowCoordinates(int x, int y, HWND& hwnd) {
			RECT rect;
			GetWindowRect(hwnd, &rect);
			
			X = rect.left + x;
			Y = rect.top + y;
		}
	};
}


void printb(std::string& str,
			unsigned int charNum,
			unsigned int speed,
			unsigned int repeats = 1,
			const char* escapeCodeStart = "",
			const char* escapeCodeEnd = "");

void hideConsoleCursor();
void onConsoleCursor();

void clearConsole();

Vector getConsoleSize();
std::vector<std::string> getTgPaths();
int getOption();

void createTgPaths(std::vector<std::string>&);

void readFile(std::vector<std::string>&, std::ifstream&);
void writeFile(std::vector<std::string>&, std::ofstream&);
bool realFile(std::string&);

void outputPoweredBy();
void outputMenu(std::vector<std::string>&);
void outputSeparator(Vector consoleSize);
void outputSeparator2(Vector consoleSize);
void outputSocialMedia();
void outputPaths(std::vector<std::string>&);
void outputButtons(Vector consoleSize);

void menuLogic(std::vector<std::string>&);

void scriptLogic(std::vector<std::string>&);
void openNotPixel(std::string&);
bool waitOpenNotPixel(HWND&);
void openFrenTemplate(HWND&);
void moveSlider(WindowCoordinates, WindowCoordinates);
void cleanTemplate(HWND&);
COLORREF getColor(HWND&, int x, int y);

void colorChange(HWND&, COLORREF);

void getHwnd(HWND&);

bool pixelIsColor(HDC, WindowCoordinates, COLORREF);

inline void sleep(unsigned int);
void mouseMoveAndClick(WindowCoordinates);
void mouseMoveAndClickR(HWND& hwnd, int x, int y);


int main() {
	hideConsoleCursor();
	
	outputPoweredBy();
	sleep(3 * 1000);
	
	std::vector<std::string> tgPaths;
	
	menuLogic(tgPaths);
	
	onConsoleCursor();
	
	return 1;
}

void printb(std::string& str, unsigned int charNum, unsigned int speed, unsigned int repeats, const char* escapeCodeStart, const char* escapeCodeEnd) {
	/**
	 * Use the ANSI escape code(escapeCodeStart), "" if undefined.
	 * Outputs str repeats times with charNum of characters in front of char
	 * and speed time between them.
	 * Use the ANSI escape code(escapeCodeEnd), "" if undefined.
	 *
	 * Undefined behavior when charNum is large.
	 * Do not use escape-sequences in str.
	 */
	 std::cout << escapeCodeStart;
	 
	 for (int i = 0; i < repeats; ++i) {
		 for (const char& c : str) {
			 for (int j = charNum; j > 0; --j) {
				 std::cout << static_cast<unsigned char>(c + j);
				 std::cout << '\b';
				 
				 sleep(speed);
			 }
			 if (charNum == 0 && repeats == 1) sleep(speed);
			 std::cout << c;
		 }
		 if (repeats > 1) sleep(speed);
	 }	 
	 std::cout << escapeCodeEnd;
}


void hideConsoleCursor() {
	/**
	 * Hides the console cursor.
	 */
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO structCursorInfo;
	GetConsoleCursorInfo(handle,&structCursorInfo);
	structCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo( handle, &structCursorInfo );
}

void onConsoleCursor() {
	/**
	 * Turn on the console cursor.
	 */
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO structCursorInfo;
	GetConsoleCursorInfo(handle,&structCursorInfo);
	structCursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo( handle, &structCursorInfo );
}


void clearConsole() {
	/**
	 * Clear the console and position the cursor
	 * on the top left.
	 */
	std::cout << "\x1B[2J\x1B[H";
}


Vector getConsoleSize() {
	/**
	 * Returns the size of rows and colomns
	 * in struct Vector.
	 */
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	
	Vector consoleSize;
	
	GetConsoleScreenBufferInfo(handle, &csbi);
	consoleSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	consoleSize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	
	return consoleSize;
}

std::vector<std::string> getTgPaths() {
	/**
	 * Requests paths if there is no data.txt.
	 * Reads paths from data.txt if there is one.
	 * Returns std::vector<std::string> of paths.
	 */
	 std::ifstream file("data.txt");
	 
	 std::vector<std::string> tgPaths;
	 
	 if (file.is_open()) { 
		readFile(tgPaths, file);
		file.close();
	 }
	 else {
		createTgPaths(tgPaths);
		
		if (!tgPaths.empty()) {
			std::ofstream newFile("data.txt");
			writeFile(tgPaths, newFile);
			newFile.close();
			
		}
		Vector consoleSize = getConsoleSize();
		outputSeparator2(consoleSize);
	 }
	 return tgPaths;
}

int getOption() {
	/**
	 * Outputs "Type option: ".
	 * Takes input and returns it
	 */
	std::string typeOption = "Type option: ";
	printb(typeOption, 0, TextSpeed);
	
	onConsoleCursor();
	
	unsigned int option;
	std::cin >> option;
	
	hideConsoleCursor();
	
	return option;
}


void createTgPaths(std::vector<std::string>& tgPaths) {
	/**
	 * Requests the telegram path until the user enters 0
	 * and appends it to tgPaths.
	 */
	std::string tgPath;
		 
	while (true) {
		std::string addPath = "Add the path to the telegram(0 to end): ";
		printb(addPath, 0, TextSpeed, 1, "\e[1;93m", "\e[0m");
		 
		onConsoleCursor();
			
		std::getline(std::cin, tgPath);
		
		hideConsoleCursor();
		
		if (tgPath == "0") { break; }
		
		if (realFile(tgPath)) {
			std::string notRealFile = "Cool!";
			printb(notRealFile, 0, TextSpeed, 1, "\e[1;32m", "\e[0m");
			std::cout << std::endl;
			
			tgPaths.push_back(tgPath);
		}
		else {
			std::string notRealFile = "No such path exists";
			printb(notRealFile, 0, TextSpeed, 1, "\e[1;31m", "\e[0m");
			std::cout << std::endl;
		}
	}
}


void readFile(std::vector<std::string>& strings, std::ifstream& file) {
	/**
	 * Read lines from file and push to strings.
	 */
	std::string line;
		
	while (std::getline(file, line)) {
		strings.push_back(line);
	}
}

void writeFile(std::vector<std::string>& strings, std::ofstream& file) {
	/**
	 * Write lines from strings to file.
	 */
	for (std::string& line : strings) {
		file << line << std::endl;
	}
}

bool realFile(std::string& path) {
	/**
	 * Returns true if the file at this path exists
	 * and false if it does not.
	 */
	return (std::filesystem::exists(path) && std::filesystem::is_regular_file(path));
}


void outputPoweredBy() {
	/**
	 * Clears the console and
	 * outputs "powered by wqst3" to the center.
	 *
	 * P.S. a bit inspired by GHOST SPECTRE <3
	 */
	clearConsole();
	
	std::string startStr = "p o w e r e d   b y   ";
	std::string wqst3Str = "w q s t 3";
	
	Vector consoleSize = getConsoleSize();
	int skip = (consoleSize.X - (startStr.length() + wqst3Str.length())) / 2;
	
	for (int i = 0; i < skip; ++i) {
		std::cout << ' ';
	}
	
	printb(startStr, 4, 45, 1, "\e[1;36m", "\e[0m");
	printb(wqst3Str, 4, 45, 1, "\e[1;45m", "\e[0m");
}

void outputMenu(std::vector<std::string>& tgPaths) {
	/**
	 * Returns the cursor to the beginning
	 * of the console and outputs menu.
	 *
	 * P.S. a bit inspired by GHOST SPECTRE <3
	 */
	Vector consoleSize = getConsoleSize();
	
	for (int i = 0; i < consoleSize.X; ++i) std::cout << '\b';
	
	outputSeparator(consoleSize);
	outputSocialMedia();
	outputSeparator2(consoleSize);
	if (tgPaths.empty()) {
		tgPaths = getTgPaths();
	}
	outputPaths(tgPaths);
	outputSeparator2(consoleSize);
	outputButtons(consoleSize);
}

void outputSeparator(Vector consoleSize) {
	/**
	 * Outputs "=" * consoleSize.X.
	 */
	std::string separator(consoleSize.X / 13, '=');
	printb(separator, 0, TextSpeed, 13);
	
	std::cout << std::endl;
}

void outputSeparator2(Vector consoleSize) {
	/**
	 * Outputs "-" * (consoleSize.X / 2).
	 */
	std::string separator((consoleSize.X / 13) / 2, '-');
	printb(separator, 0, TextSpeed, 13);
	
	std::cout << std::endl;
}

void outputSocialMedia() {
	/**
	 * Outputs my telegram channel and github.
	 */
	std::string myReff = "MY REFFERAL LINK: ";
	std::string reffLink = "t.me/notpixel/app?startapp=f988254241_s578208";
	
	std::string myTelegram = "MY TELEGRAM: ";
	std::string telegramLink = "t.me/wqst3here";
	
	std::string myGithub = "MY GITHUB: ";
	std::string githubLink = "github.com/wqst3";
	
	printb(myReff, 0, TextSpeed, 1, "\e[1;35m", "\e[0m");
	printb(reffLink, 0, TextSpeed, 1, "\e[1;45m", "\e[0m");
	std::cout << std::endl;
	
	printb(myTelegram, 0, TextSpeed, 1, "\e[1;94m", "\e[0m");
	printb(telegramLink, 0, TextSpeed, 1, "\e[1;104m", "\e[0m");
	std::cout << std::endl;
	
	printb(myGithub, 0, TextSpeed, 1, "\e[1;90m", "\e[0m");
	printb(githubLink, 0, TextSpeed, 1, "\e[1;100m", "\e[0m");
	std::cout << std::endl;
}

void outputPaths(std::vector<std::string>& tgPaths) {
	/**
	 * Outputs paths to telegram.
	 */
	if (tgPaths.empty()) {
		std::string addPaths = "Add paths to the telegrams";
		printb(addPaths, 0, TextSpeed, 1, "\e[1;31m", "\e[0m");
		std::cout << std::endl;
	}
	else {
		std::string yourPaths = "Your paths to the telegrams: ";
		printb(yourPaths, 0, TextSpeed, 1, "\e[1;93m", "\e[0m");
		std::cout << std::endl;
	}
	for (std::string& path : tgPaths) {
		std::cout << '\t';
		printb(path, 0, TextSpeed, 1, "\e[1;32m", "\e[0m");
		std::cout << std::endl;
	}
}

void outputButtons(Vector consoleSize) {
	/**
	 * Outputs buttons.
	 */
	std::string button1 = "[1] | Run the script";
	printb(button1, 0, TextSpeed, 1, "\e[1;34m", "\e[0m");
	std::cout << std::endl;
	
	std::string button2 = "[2] | Change paths to telegrams";
	printb(button2, 0, TextSpeed, 1, "\e[1;34m", "\e[0m");
	std::cout << "\n\n";
	
	std::string button0 = "[0] | Exit";
	printb(button0, 0, TextSpeed, 1, "\e[1;33m", "\e[0m");
	std::cout << std::endl;
	
	outputSeparator2(consoleSize);
	std::cout << std::endl;
}


void menuLogic(std::vector<std::string>& tgPaths) {
	/**
	 * Performs menu logic.
	 */
	 while (true) {
		outputMenu(tgPaths);
		switch ( getOption() )
		{
			case 0:
				return;

			case 1:
				scriptLogic(tgPaths);
				
				break;

			case 2:
				std::filesystem::remove("data.txt");
				tgPaths.clear();
				
				clearConsole();
				break;

			default:
				std::cin.clear();
				std::cin.ignore(32767,'\n');
				
				clearConsole();
				break;

		}
	}
}


void scriptLogic(std::vector<std::string>& tgPaths) {
	/**
	 * Performs script logic.
	 */
	clearConsole();
	
	while (true) {
		for (std::string& path : tgPaths) {
			std::string pathTmp = path + ": ";
			printb(pathTmp, 0, TextSpeed, 1, "\e[1;36m", "\e[0m");
			
			HWND hwnd;
			
			int i = 0;
			do
			{
			openNotPixel(path);
			
			sleep(6 * 1000);
			
			getHwnd(hwnd);
			
			++i;
			}
			while (waitOpenNotPixel(hwnd) && i < 3);
			if (i == 3) {
				std::string notPixelNotOpened = "Notpixel not opened";
				printb(notPixelNotOpened, 0, TextSpeed, 1, "\e[1;31m", "\e[0m");
				std::cout << std::endl;
			
				continue;
				}
			
			std::string notPixelOpened = "Notpixel opened";
			printb(notPixelOpened, 0, TextSpeed, 1, "\e[1;32m", "\e[0m");
			std::cout << std::endl;
			sleep(10 * 1000);
			
			openFrenTemplate(hwnd);
			sleep(10 * 1000);
			
			int currentX = 68;
			int currentY = 188;
			
			COLORREF needColor, currentColor, realColor;
			
			mouseMoveAndClick(WindowCoordinates(currentX, currentY, hwnd));
			sleep(5 * 1000);
			
			while (!pixelIsColor(GetDC(NULL), WindowCoordinates(38, 38, hwnd), RGB(134, 138, 144))) { ///< until the power runs out
				moveSlider(WindowCoordinates(45, 260, hwnd), WindowCoordinates(45, 235, hwnd)); ///< clean template
				sleep(200);
				
				needColor = getColor(hwnd, currentX, currentY);
				
				currentColor = getColor(hwnd, 55, 565);
				
				if (needColor != currentColor) {
					colorChange(hwnd, needColor);
				}
				
				moveSlider(WindowCoordinates(45, 350, hwnd), WindowCoordinates(45, 370, hwnd)); ///< real template
				sleep(200);
				
				realColor = getColor(hwnd, currentX, currentY);
				
				if (realColor != needColor) {
					mouseMoveAndClick(WindowCoordinates(currentX, currentY, hwnd));
					sleep(200);
					mouseMoveAndClick(WindowCoordinates(200, 600, hwnd)); ///< paint
					
					auto now = std::chrono::system_clock::now();
					std::time_t time_now = std::chrono::system_clock::to_time_t(now);
					
					std::tm now_tm;
					
					localtime_s(&now_tm, &time_now);
					
					std::ostringstream oss;
					oss << std::put_time(&now_tm, "%H:%M:%S");
					
					std::string date = oss.str() + ": ";
					printb(date, 0, TextSpeed, 1);
					
					std::string plus12 = "+12 PX";
					printb(plus12, 0, TextSpeed, 1, "\e[1;32m", "\e[0m");
					std::cout << std::endl;
					
					sleep(200);
				}
				
				if (((currentX - 80) / 4) > 30) {
					currentY += 4;
					currentX = 80;
				}
				else {
					currentX += 4;
				}
			}
			
			printb(pathTmp, 0, TextSpeed, 1, "\e[1;36m", "\e[0m");
			std::string energyOver = "Energy over";
			printb(energyOver, 0, TextSpeed, 1, "\e[1;31m", "\e[0m");
			std::cout << std::endl;
			
			SendMessage(hwnd, WM_CLOSE, 0, 0);
		}
		std::string timeout = "Timeout";
		printb(timeout, 0, TextSpeed, 1, "\e[1;33m", "\e[0m");
		std::cout << std::endl;
		
		sleep(60 * 60 * 1000);
	}
}

void openNotPixel(std::string& path) {
	/**
	 * Runs notpixel through path.
	 */
	std::string start = "start \"\" \"";
	std::string end = "\" -- \"tg://resolve?domain=notpixel&startapp\"";
			
	system((start + path + end).c_str());
}

bool waitOpenNotPixel(HWND& hwnd) {
	/**
	 * Waiting for notpixel to open and return false.
	 * If not opened close TelegramDesktop and return true.
	 */
	while (!pixelIsColor(GetDC(NULL), WindowCoordinates(342, 87, hwnd), RGB(71, 65, 46)))
	{
		if (pixelIsColor(GetDC(NULL), WindowCoordinates(160, 360, hwnd), RGB(32, 42, 56)) ||
			pixelIsColor(GetDC(NULL), WindowCoordinates(300, 200, hwnd), RGB(23, 33, 43)))
		{
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			
			return true;
		}
		sleep(500);
	}
	return false;
}

void openFrenTemplate(HWND& hwnd) {
	/**
	 * Opens the fren template and waits for it to open.
	 */
	
	mouseMoveAndClick(WindowCoordinates(45, 200, hwnd)); ///< fren template

	while (!pixelIsColor(GetDC(NULL), WindowCoordinates(200, 140, hwnd), RGB(32, 42, 56))) { ///< until fren template loads
		sleep(200);
	}
}

void moveSlider(WindowCoordinates coords1, WindowCoordinates coords2) {
	/**
	 * Moves the mouse to x1, y1,
	 * clamps the mouse, moves it to x2, y2,
	 * releases it
	 */
	SetCursorPos(coords1.X, coords2.Y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	SetCursorPos(coords2.X, coords2.Y);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

COLORREF getColor(HWND& hwnd, int currentX, int currentY) {
	/**
	 * Returns the color of the pixel.
	 */
	
	WindowCoordinates currentCoords(currentX, currentY, hwnd);
	
	return GetPixel(GetDC(NULL), currentCoords.X, currentCoords.Y);
}


void colorChange(HWND& hwnd, COLORREF needColor) {
	/**
	 * Changes the brush color.
	 */
	
	mouseMoveAndClick(WindowCoordinates(55, 555, hwnd));
	sleep(5 * 1000);
	
	int currentColorX = 55;
	int currentColorY = 490;
	
	int i = 0;
	while (!pixelIsColor(GetDC(NULL), WindowCoordinates(currentColorX, currentColorY, hwnd), needColor) && i < 30) {
		if (((currentColorX - 55) / 33) == 9) {
			currentColorY += 33;
			currentColorX = 55;
		}
		else {
			currentColorX += 33;
		}
		++i;
	}
	mouseMoveAndClick(WindowCoordinates(currentColorX, currentColorY, hwnd));
	sleep(200);
	
	mouseMoveAndClick(WindowCoordinates(55, 430, hwnd));
	sleep(2 * 1000);
}


void getHwnd(HWND& hwnd) {
	/**
	 * Assigns TelegramDesktop's HWND to hwnd.
	 */
	const char* windowTitle = "TelegramDesktop";
	hwnd = FindWindow(NULL, windowTitle);
}


bool pixelIsColor(HDC hdc, WindowCoordinates pixel, COLORREF color) {
	/**
	 * Compares color1 and color2.
	 */
	COLORREF pixelColor = GetPixel(hdc, pixel.X, pixel.Y);

	return (pixelColor == color);
}


void sleep(unsigned int milliseconds) {
	/**
	 * Sleeps milliseconds of time.
	 */
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void mouseMoveAndClick(WindowCoordinates coords) {
	/**
	 * Moves the cursor to x and y and clicks.
	 */
	SetCursorPos(coords.X, coords.Y);

	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

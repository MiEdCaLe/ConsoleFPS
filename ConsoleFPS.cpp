#include <Windows.h>
#include <iostream>
#include <chrono>

int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerX = 8.0f;	// Player X pos.
float fPlayerY = 8.0f;	// Player Y pos.
float fPlayerA = 0.0f;	// Player angle.

int nMapHeight = 16;
int nMapWidth = 16;

float fFOV = 3.14159 / 4.0;
float fDepth = 16.0f;

int main()
{
	// Create Screen Buffer
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight]; //1 Dimensional Array 

	// Create handle to console
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL , CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	std::wstring map;	//Define UNICODE string

	// We store the map in this string
	map += L"################";
	map += L"#..............#";
	map += L"#....#######...#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..#...........#";
	map += L"#..#...........#";
	map += L"#..#...........#";
	map += L"#..#...........#";
	map += L"#..#...........#";
	map += L"#..#...........#";
	map += L"#..#...........#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";
	
	// set time point 1  to current time and define time point 2
	std::chrono::time_point<std::chrono::system_clock> tp1 = std::chrono::system_clock::now();
	std::chrono::time_point<std::chrono::system_clock> tp2;

	while (1) {
		
		// Clock that measures the time that every frame takes
		// Set time point 2 to current time.
		tp2 = std::chrono::system_clock::now();

		// Calculate the elapsed time between tp2 and tp1
		std::chrono::duration<float> elapsedTime = tp2 - tp1;

		// update the old time point.
		tp1 = tp2;

		float fElapsedTime = elapsedTime.count();

		// Controls
		// Handle CCW Rotation
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerA -= (1.5) * fElapsedTime;

		// Handle Clockwise Rotation
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerA += (1.5) * fElapsedTime;

		// Handle forward movement
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
			fPlayerX += cosf(fPlayerA) * 5.0f * fElapsedTime;
			fPlayerY += sinf(fPlayerA) * 5.0f * fElapsedTime;
		}
			
		// Handle backward movement
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			fPlayerX -= cosf(fPlayerA) * 5.0f * fElapsedTime;
			fPlayerY -= sinf(fPlayerA) * 5.0f * fElapsedTime;
		}
	
		for (int x{ 0 }; x < nScreenWidth; x++) {

			// For each column, calculate the projected ray angle into world space
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			float fDistanceToWall = 0;
			bool bHitWall{ false };

			// We decompose the RayAngle unit vector into it's respective X and Y components. 
			float fEyeX = cosf(fRayAngle);
			float fEyeY = sinf(fRayAngle);

			while (!bHitWall && fDistanceToWall < fDepth) {
				
				// Increment probing length

				fDistanceToWall += 0.1f;
				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				// Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight) {
					bHitWall = true;
					fDistanceToWall = fDepth;
				}
				else {
					// Ray is inbounds so test to see if the ray cell is a wall block
					// Test Y coordinate * 16 to access the different containers in the map (because the map is a 1D array instead of 2D)
					if (map[nTestY * nMapWidth + nTestX] == '#') {
						bHitWall = true;
					}
				}
			}

			// Calculate distance to ceiling and floor
			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			short nShade = ' ';

			if (fDistanceToWall <= fDepth / 4.0f) nShade = 0x2588; // Very Close
			else if (fDistanceToWall < fDepth / 3.0f) nShade = 0x2593;
			else if (fDistanceToWall < fDepth /2.0f) nShade = 0x2592;
			else if (fDistanceToWall < fDepth) nShade = 0x2591;
			else nShade = ' '; // Too Far Away

			for (int y = 0; y < nScreenHeight; y++) {
				// Shade ceiling
				if (y <= nCeiling) {
					screen[y * nScreenWidth + x] = ' ';
				}
				// Shade Walls
				else if(y > nCeiling && y <= nFloor) {
					screen[y * nScreenWidth + x] = nShade;
				// Shade Floors
				} else {
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25) nShade = '#';
					else if (b < 0.5) nShade = 'x';
					else if (b < 0.75) nShade = '.';
					else if (b < 0.9) nShade = '-';
					else nShade = ' ';
					screen[y * nScreenWidth + x] = nShade;
				}
			}
		}

		screen[nScreenWidth * nScreenHeight - 1] = '\0';	// Null terminate the last character in the array
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

	}


	return 0;

}

#include <Windows.h>
#include <iostream>

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
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight]; //2 Dimensional Array 

	// Create handle to console
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL , CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	std::wstring map;	//Define UNICODE string

	// We store the map in this string
	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";

	while (1) {

		for (int x{ 0 }; x < nScreenWidth; x++) {

			// For each column, calculate the projected ray angle into world space
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			float fDistanceToWall = 0;
			bool bHitWall{ false };

			// We decompose the RayAngle unit vector into it's respective X and Y components. 
			float fEyeX = sinf(fRayAngle);
			float fEyeY = cosf(fRayAngle);

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


			for (int y = 0; y < nScreenHeight; y++) {
				if (y < nCeiling) {
					screen[y * nScreenWidth + x] = ' ';
				}
				else if(y > nCeiling && y <= nFloor) {
					screen[y * nScreenWidth + x] = '#';
				} else {
					screen[y * nScreenWidth + x] = ' ';
				}
			}
		}

		screen[nScreenWidth * nScreenHeight - 1] = '\0';	// Null terminate the last character in the array
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

	}


	return 0;

}

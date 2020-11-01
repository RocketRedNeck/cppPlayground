// Traders.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// C-style I/O because I like it that way
#include <stdio.h>

// C++ standard random number generators and function binding
#include <iostream>
#include <cstdint>
#include <random>
#include <functional>

//CSI n A	CUU	Cursor Up	Moves the cursor n(default 1) cells in the given direction.If the cursor is already at the edge of the screen, this has no effect.
//CSI n B	CUD	Cursor Down
//CSI n C	CUF	Cursor Forward
//CSI n D	CUB	Cursor Back
//CSI n E	CNL	Cursor Next Line	Moves cursor to beginning of the line n(default 1) lines down. (not ANSI.SYS)
//CSI n F	CPL	Cursor Previous Line	Moves cursor to beginning of the line n(default 1) lines up. (not ANSI.SYS)
//CSI n G	CHA	Cursor Horizontal Absolute	Moves the cursor to column n(default 1). (not ANSI.SYS)
//CSI n; m H	CUP	Cursor Position	Moves the cursor to row n, column m.The values are 1 - based, and default to 1 (top left corner) if omitted.A sequence such as CSI; 5H is a synonym for CSI 1; 5H as well as CSI 17; H is the same as CSI 17H and CSI 17; 1H
//CSI n J	ED	Erase in Display	Clears part of the screen.If n is 0 (or missing), clear from cursor to end of screen.If n is 1, clear from cursor to beginning of the screen.If n is 2, clear entire screen(and moves cursor to upper left on DOS ANSI.SYS).If n is 3, clear entire screenand delete all lines saved in the scrollback buffer(this feature was added for xtermand is supported by other terminal applications).
//CSI n K	EL	Erase in Line	Erases part of the line.If n is 0 (or missing), clear from cursor to the end of the line.If n is 1, clear from cursor to beginning of the line.If n is 2, clear entire line.Cursor position does not change.
//CSI n S	SU	Scroll Up	Scroll whole page up by n(default 1) lines.New lines are added at the bottom. (not ANSI.SYS)
//CSI n T	SD	Scroll Down	Scroll whole page down by n(default 1) lines.New lines are added at the top. (not ANSI.SYS)
//CSI n; m f	HVP	Horizontal Vertical Position	Same as CUP
//CSI n m	SGR	Select Graphic Rendition	Sets the appearance of the following characters, see SGR parameters below.
//CSI 5i		AUX Port On	Enable aux serial port usually for local serial printer
//CSI 4i		AUX Port Off	Disable aux serial port usually for local serial printer
//
//CSI 6n	DSR	Device Status Report	Reports the cursor position(CPR) to the application as(as though typed at the keyboard) ESC[n; mR, where n is the rowand m is the column.)
//CSI s	SCP	Save Cursor Position	Saves the cursor position / state.
//CSI u	RCP	Restore Cursor Position	Restores the cursor position / state.

// Only defining the ANSI.SYS compliant escape codes for cursor/display control
#define ESC(n) "\033["#n
#define ESC_UP(n) ESC(n)"A"
#define ESC_DOWN(n) ESC(n)"B"
#define ESC_FORWARD(n) ESC(n)"C"
#define ESC_BACK(n) ESC(n)"D"
#define ESC_POS(n,m) ESC(n)";"#m"H"
#define ESC_ERASE_DISPLAY_(n)			ESC(n)"J"
#define ESC_ERASE_DISPLAY_TO_END		ESC_ERASE_DISPLAY_(0)
#define ESC_ERASE_DISPLAY_TO_BEG		ESC_ERASE_DISPLAY_(1)
#define ESC_ERASE_DISPLAY				ESC_ERASE_DISPLAY_(2)
#define ESC_ERASE_DISPLAY_AND_BUFFER	ESC_ERASE_DISPLAY_(3)
#define ESC_ERASE_LINE_(n)				ESC(n)"K"
#define ESC_ERASE_LINE_TO_END		    ESC_ERASE_LINE_(0)
#define ESC_ERASE_LINE_TO_BEG			ESC_ERASE_LINE_(1)
#define ESC_ERASE_LINE					ESC_ERASE_LINE_(2)




std::default_random_engine generator;		// Change as needed (e.g., can connect to hardware engine with is true random)

std::uniform_int_distribution<int> coinDistribution(0, 1);
auto coin = std::bind(coinDistribution, generator);

const uint64_t MAX_FLIPS = 10000000;
uint64_t count[2] = {}; // To demonstrate  random number generator for fairness

const uint64_t MAX_TRADERS = 1000;
const uint64_t MAX_TRADES  = 1000000000;
std::uniform_int_distribution<uint64_t> tradeDistribution((uint64_t)0, MAX_TRADERS);
auto selectTrader = std::bind<uint64_t>(tradeDistribution, generator);

const uint64_t SEED_MONEY = 1000;
uint64_t trader[MAX_TRADERS] = {};
struct Stats
{
	uint64_t wins;
	uint64_t losses;
};
Stats stats[MAX_TRADERS] = {};

const uint64_t MAX_BINS = 35;
const uint64_t LARGEST_BIN = static_cast<uint64_t>(3.5 * SEED_MONEY);
const uint64_t BIN_SIZE = LARGEST_BIN / MAX_BINS;
uint64_t hist[MAX_BINS] = {};

uint64_t winners = 0;
uint64_t losers = 0;
uint64_t disenfranchised = 0;

uint64_t scale = 8;

const bool TAX_ENABLED = false;
const uint64_t TAX_BOUNDARY = 10000;


#define DIM(x) (sizeof(x)/sizeof(x[0]))

void demoCoinFairness(void)
{
	printf("Demonstrating coin fairness...\n");

	for (uint64_t i = 0; i < MAX_FLIPS; ++i)
	{
		coin() ? ++count[1] : ++count[0];
		if ((i % (MAX_FLIPS / 100)) == 0)
		{
			printf("\r%3llu %% complete : n = %8llu : count[0] = %8llu : count[1] = %8llu", i*100/MAX_FLIPS, i, count[0], count[1]);
		}
	}

	printf("\r100 %% complete : n = %8llu : count[0] = %8llu : count[1] = %8llu\n", MAX_FLIPS, count[0], count[1]);

	printf("\n");
}

void executeIncomeModel(void)
{
	// Create an influx of wealth (i.e., a growth in GDP-like value)

}

void executeTaxModel(void)
{
	// Collect graduated tax for any above median
	// Distribute inverse proportionally to any below median to bring
	// traders to minimum standard of living
}

int main()
{
	// Uncomment if needed to convince someone
	// demoCoinFairness();

	// Start every trader with the same balance
	for (uint64_t i = 0; i < DIM(trader); ++i)
	{
		trader[i] = SEED_MONEY;
	}

	// Run billions of trading opportunities
	for (uint64_t i = 0; i < MAX_TRADES; ++i)
	{
		// If taxation and redistribute model is active then
		// run it now
		if (TAX_ENABLED)
		{
			if ((i % TAX_BOUNDARY) == 0)
			{
				executeIncomeModel();

				// The taxman collects and redistributes
				executeTaxModel();
			}

		}
		// Randomly select two traders
		// Once a trader is disenfrachised there is no play
		auto a = selectTrader();
		while (trader[a] == 0)
		{
			a = selectTrader();
		}
		auto b = selectTrader();
		while ((a == b) || (trader[b] == 0))
		{
			b = selectTrader();
		}

		if (coin())
		{
			++trader[a];
			--trader[b];
			stats[a].wins++;
			stats[b].losses++;
		}
		else
		{
			--trader[a];
			++trader[b];
			stats[b].wins++;
			stats[a].losses++;
		}

		if ((i % (MAX_TRADES / 100000)) == 0)
		{
			// Bin up the results so far
			std::memset(hist, 0, sizeof(hist));
			winners = 0;
			losers = 0;
			disenfranchised = 0;

			for (uint64_t j = 0; j < DIM(trader); ++j)
			{
				uint64_t bin = static_cast<uint64_t>((double)trader[j] / (double)LARGEST_BIN * MAX_BINS);
				if (trader[j] > SEED_MONEY)
				{
					++winners;
				}
				else if (trader[j] < SEED_MONEY)
				{
					++losers;
				}
				if (trader[j] == 0)
				{
					++disenfranchised;
				}
				if (bin < MAX_BINS)
				{
					hist[bin]++;
				}
				else
				{
					hist[DIM(hist) - 1]++;
				}
			}

			// Display histogram
			printf(ESC_POS(1,1));//Top left corner
			printf(ESC_ERASE_LINE_TO_END);
			printf("%.3f %% of %llu trades complete\n", (double)(i * 100) / (double)MAX_TRADES, MAX_TRADES);

			uint64_t maxHist = 0;
			for (int64_t j = (MAX_BINS-1); j >= 0; j--)
			{
				printf(ESC_ERASE_LINE_TO_END);
				printf("\r%9llu : %9llu : %s\n", (uint64_t)j * BIN_SIZE, hist[j], std::string(static_cast<unsigned int>(hist[j] / scale), '*').c_str());
				if (hist[j] > maxHist)
				{
					maxHist = hist[j];
				}
			}
			scale = 8 * maxHist / 500;
			printf(ESC_ERASE_LINE_TO_END);
			printf("\r* = %llu\n", scale);
			printf(ESC_ERASE_LINE_TO_END);
			printf("\rWinners = %4llu Even = %4llu Losers = %4llu Disenfranchised = %4llu\n", winners, MAX_TRADERS - winners - losers, losers, disenfranchised);
		}
	}

	return 0;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

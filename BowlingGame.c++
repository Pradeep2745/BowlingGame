#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

class Frame
{
public:
    int firstRoll = 0;
    int secondRoll = 0;
    int thirdRoll = 0; // For 10th frame bonus rolls
    bool isStrike = false;
    bool isSpare = false;

    Frame() {}
    int totalPins() const
    {
        return firstRoll + secondRoll + thirdRoll;
    }
};

class Player
{
private:
    string name;
    vector<Frame> frames;

public:
    Player(const string& playerName) : name(playerName)
    {
        frames.resize(10);
    }

    string getName() const
    {
        return name;
    }

    Frame& getFrame(int gameNumber)
    {
        if (gameNumber < 0 || gameNumber >= 10)
        {
            throw out_of_range("Frame index out of range");
        }
        return frames[gameNumber];
    }

    const vector<Frame>& getFrames() const
    {
        return frames;
    }

    int getScoreForFrame(int frameIndex)
    {
        if (frameIndex < 0 || frameIndex >= 10)
        {
            throw out_of_range("Frame index out of range");
        }

        int score = 0;
        Frame& frame = frames[frameIndex];

        if (frame.isStrike)
        {
            score = 10 + strikeBonus(frameIndex);
        }
        else if (frame.isSpare)
        {
            score = 10 + spareBonus(frameIndex);
        }
        else
        {
            score = frame.firstRoll + frame.secondRoll;
        }
        return score;
    }

    int calculateScore()
    {
        int totalScore = 0;
        for (int gameNumber = 0; gameNumber < 10; gameNumber++)
        {
            totalScore = totalScore + getScoreForFrame(gameNumber);
        }
        return totalScore;
    }

private:
    int strikeBonus(int frameIndex)
    {
        if (frameIndex >= 9)
        {
            return frames[frameIndex].secondRoll + frames[frameIndex].thirdRoll;
        }

        int bonus = 0;
        Frame& nextFrame = frames[frameIndex + 1];

        if (nextFrame.isStrike)
        {
            bonus = 10;
            if (frameIndex + 1 == 9)
            {
                bonus = bonus + nextFrame.secondRoll;
            }
            else
            {
                bonus = bonus + frames[frameIndex + 2].firstRoll;
            }
        }
        else
        {
            bonus = nextFrame.firstRoll + nextFrame.secondRoll;
        }
        return bonus;
    }

    int spareBonus(int frameIndex)
    {
        if (frameIndex >= 9)
        {
            return frames[frameIndex].thirdRoll;
        }
        return frames[frameIndex + 1].firstRoll;
    }
};

class BowlingGame
{
private:
    Player player;

public:
    BowlingGame(const string& playerName) : player(playerName)
    {
    }

    Player& getPlayer()
    {
        return player;
    }
};

// Validate rolls and throw if invalid
void addRollsToFrame(Player& player, int gameNumber, int firstRoll, int secondRoll, int thirdRoll = 0)
{
    if (gameNumber < 0 || gameNumber >= 10)
    {
        throw out_of_range("Frame index out of range");
    }

    if (firstRoll < 0 || firstRoll > 10)
    {
        throw invalid_argument("First roll must be between 0 and 10");
    }

    if (secondRoll < 0 || secondRoll > 10)
    {
        throw invalid_argument("Second roll must be between 0 and 10");
    }

    if (thirdRoll < 0 || thirdRoll > 10)
    {
        throw invalid_argument("Third roll must be between 0 and 10");
    }

    // For frames 0-8, total pins in two rolls cannot exceed 10
    if (gameNumber < 9 && (firstRoll + secondRoll > 10))
    {
        throw invalid_argument("Total pins in a frame cannot exceed 10 (except 10th frame)");
    }

    // For 10th frame (index 9), more flexible rules due to bonus rolls - skipping detailed validation here for simplicity

    Frame& frame = player.getFrame(gameNumber);
    frame.firstRoll = firstRoll;
    frame.secondRoll = secondRoll;
    frame.thirdRoll = thirdRoll;
    frame.isStrike = (firstRoll == 10);
    frame.isSpare = (!frame.isStrike && (firstRoll + secondRoll == 10));
}

void assertEqual(int actual, int expected, const string& testName)
{
    if (actual != expected)
    {
        throw runtime_error(testName + " failed: expected " + to_string(expected) + ", got " + to_string(actual));
    }
    else
    {
        cout << testName << " passed.\n";
    }
}

int main()
{
    try
    {
        cout << "Test cases...\n";

        // Test 1: Simple score with no strikes or spares
        {
            Player player("TestPlayer");
            for (int gameNumber = 0; gameNumber < 10; ++gameNumber)
            {
                addRollsToFrame(player, gameNumber, 3, 4);
            }
            int expected = 70;
            int actual = player.calculateScore();
            assertEqual(actual, expected, "testSimpleScore");
        }

        // Test 2: Spare calculation
        {
            Player player("SparePlayer");
            addRollsToFrame(player, 0, 4, 6);
            addRollsToFrame(player, 1, 3, 5);
            for (int gameNumber = 2; gameNumber < 10; ++gameNumber)
            {
                addRollsToFrame(player, gameNumber, 0, 0);
            }
            int expected = 21;
            int actual = player.calculateScore();
            assertEqual(actual, expected, "testSpareScore");
        }

        // Test 3: Strike calculation
        {
            Player player("StrikePlayer");
            addRollsToFrame(player, 0, 10, 0);
            addRollsToFrame(player, 1, 3, 5);
            for (int gameNumber = 2; gameNumber < 10; ++gameNumber)
            {
                addRollsToFrame(player, gameNumber, 0, 0);
            }
            int expected = 26;
            int actual = player.calculateScore();
            assertEqual(actual, expected, "testStrikeScore");
        }

        // Test 4: Final frame strike with bonus rolls
        {
            Player player("FinalFramePlayer");
            for (int gameNumber = 0; gameNumber < 9; ++gameNumber)
            {
                addRollsToFrame(player, gameNumber, 0, 0);
            }
            addRollsToFrame(player, 9, 10, 10, 10);
            int expected = 30;
            int actual = player.calculateScore();
            assertEqual(actual, expected, "testFinalFrameStrike");
        }

        // Test 5: Final frame spare with bonus roll
        {
            Player player("FinalSparePlayer");
            for (int gameNumber = 0; gameNumber < 9; ++gameNumber)
            {
                addRollsToFrame(player, gameNumber, 0, 0);
            }
            addRollsToFrame(player, 9, 4, 6, 7);
            int expected = 17;
            int actual = player.calculateScore();
            assertEqual(actual, expected, "testFinalFrameSpare");
        }

        // Example of a failure test: invalid frame index
        try
        {
            Player p("InvalidPlayer");
            addRollsToFrame(p, 10, 3, 4); // Invalid frame index: 10
            cout << "Error: Invalid frame index test failed to throw.\n";
        }
        catch (const exception& e)
        {
            cout << "Invalid frame index test passed (caught exception): " << e.what() << "\n";
        }

        cout << "All tests passed successfully.\n";
    }
    catch (const exception& e)
    {
        cerr << "Test failed with exception: " << e.what() << endl;
    }
    catch (...)
    {
        cerr << "Test failed with unknown exception." << endl;
    }

    return 0;
}


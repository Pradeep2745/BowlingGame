#include <iostream>
#include <vector>
#include <string>
#include <cassert>

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
        return frames[gameNumber];
    }

    const vector<Frame>& getFrames() const
    {
        return frames;
    }

    int getScoreForFrame(int frameIndex)
    {
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
        // Bonus is next two rolls after strike
        if (frameIndex >= 9) // 10th frame strike bonus rolls handled in thirdRoll and secondRoll
        {
            return frames[frameIndex].secondRoll + frames[frameIndex].thirdRoll;
        }

        int bonus = 0;
        Frame& nextFrame = frames[frameIndex + 1];

        if (nextFrame.isStrike)
        {
            bonus = 10;
            if (frameIndex + 1 == 9) // 10th frame
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

// Helper for tests to manually set frame rolls and flags
void addRollsToFrame(Player& player, int gameNumber, int firstRoll, int secondRoll, int thirdRoll = 0)
{
    Frame& frame = player.getFrame(gameNumber);
    frame.firstRoll = firstRoll;
    frame.secondRoll = secondRoll;
    frame.thirdRoll = thirdRoll;
    frame.isStrike = (firstRoll == 10);
    frame.isSpare = (!frame.isStrike && (firstRoll + secondRoll == 10));
}

int main()
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
        assert(actual == expected);
        cout << "testSimpleScore passed.\n";
    }

    // Test 2: Spare calculation
    {
        Player player("SparePlayer");
        addRollsToFrame(player, 0, 4, 6);  // spare
        addRollsToFrame(player, 1, 3, 5);
        for (int gameNumber = 2; gameNumber < 10; ++gameNumber)
        {
            addRollsToFrame(player, gameNumber, 0, 0);
        }
        int expected = 21;
        int actual = player.calculateScore();
        assert(actual == expected);
        cout << "testSpareScore passed.\n";
    }

    // Test 3: Strike calculation
    {
        Player player("StrikePlayer");
        addRollsToFrame(player, 0, 10, 0);  // strike
        addRollsToFrame(player, 1, 3, 5);
        for (int gameNumber = 2; gameNumber < 10; ++gameNumber)
        {
            addRollsToFrame(player, gameNumber, 0, 0);
        }
        int expected = 26;
        int actual = player.calculateScore();
        assert(actual == expected);
        cout << "testStrikeScore passed.\n";
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
        assert(actual == expected);
        cout << "testFinalFrameStrike passed.\n";
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
        assert(actual == expected);
        cout << "testFinalFrameSpare passed.\n";
    }

    cout << "All tests passed successfully.\n";

    return 0;
}



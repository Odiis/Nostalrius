#include "Player.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Formulas.h"

using namespace MaNGOS::Honor;

HonorRankInfo MaNGOS::Honor::CalculateRankInfo(HonorRankInfo prk)
{

    if (prk.rank != 0)
    {
        int8 rank = prk.positive ? prk.rank - NEGATIVE_HONOR_RANK_COUNT - 1 : prk.rank - NEGATIVE_HONOR_RANK_COUNT;
        prk.maxRP = (rank) * 5000.00f;
        if (prk.maxRP < 0) // in negative rank case
            prk.maxRP *= -1;
        prk.minRP = prk.maxRP > 5000.0f ? prk.maxRP  - 5000.00f : 2000.00f;

        if (prk.rank == 5)
        {
            prk.maxRP = 2000.0f;
            prk.minRP = 0.0f;
        }
        prk.visualRank = prk.rank > NEGATIVE_HONOR_RANK_COUNT ? prk.rank - NEGATIVE_HONOR_RANK_COUNT : prk.rank * -1;
    }
    else
        InitRankInfo(prk);

    return prk;
}


float MaNGOS::Honor::MaximumRpAtLevel(uint32 level)
{
    if (level <= 29)
        return 6500;
    if (level >= 30 && level <= 35)
        return 7150 + 975 * (level - 30);
    if (level >= 36 && level <= 39)
        return 12025 + 1300 * (level - 35);
    if (level >= 40 && level <= 43)
        return 17225 + 1625 * (level - 39);
    if (level >= 44 && level <= 52)
        return 23725 + 2275 * (level - 43);
    if (level >= 53 && level <= 60)
        return 44200 + 2600 * (level - 52);
    return 65000;
}

void MaNGOS::Honor::InitRankInfo(HonorRankInfo &prk)
{
    prk.positive = true;
    prk.rank = 0;
    prk.visualRank = 0;
    prk.maxRP = 2000.00f;
    prk.minRP = 0.00f;
}

HonorRankInfo MaNGOS::Honor::CalculateHonorRank(float honor_points)
{
    HonorRankInfo prk;
    InitRankInfo(prk);

    // rank none
    if (honor_points == 0)
        return prk;

    prk.positive = honor_points > 0;
    if (!prk.positive)
        honor_points *= -1;

    uint8 rCount = prk.positive ? POSITIVE_HONOR_RANK_COUNT - 2 : NEGATIVE_HONOR_RANK_COUNT;
    uint8 firstRank = prk.positive ? NEGATIVE_HONOR_RANK_COUNT + 1 : 1;

    if (honor_points < 2000.00f)
        prk.rank = prk.positive ? firstRank : NEGATIVE_HONOR_RANK_COUNT;
    else
    {
        if (honor_points > (rCount - 1) * 5000.00f)
            prk.rank = prk.positive ? HONOR_RANK_COUNT - 1 : firstRank;
        else
        {
            prk.rank = uint32(honor_points / 5000.00f) + firstRank;
            prk.rank = (prk.positive ? prk.rank  + 1 : NEGATIVE_HONOR_RANK_COUNT - prk.rank);
        }
    }

    prk = CalculateRankInfo(prk);

    return prk;
}

HonorScores MaNGOS::Honor::GenerateScores(HonorStandingList& standingList)
{
    HonorScores sc;

    // initialize the breakpoint values
    // [-PROGRESSIVE]
    // 1.11- values (source: http://www.wowwiki.com/Honor_system_%28pre-2.0_formulas%29)
    sc.BRK[13] = 0.002f;
    sc.BRK[12] = 0.007f;
    sc.BRK[11] = 0.017f;
    sc.BRK[10] = 0.032f;
    sc.BRK[ 9] = 0.057f;
    sc.BRK[ 8] = 0.097f;
    sc.BRK[ 7] = 0.156f;
    sc.BRK[ 6] = 0.225f;
    sc.BRK[ 5] = 0.324f;
    sc.BRK[ 4] = 0.433f;
    sc.BRK[ 3] = 0.553f;
    sc.BRK[ 2] = 0.687f;
    sc.BRK[ 1] = 0.835f;
    sc.BRK[ 0] = 1.000f;
    /*
        // 1.12+
        sc.BRK[13] = 0.003f;
        sc.BRK[12] = 0.008f;
        sc.BRK[11] = 0.020f;
        sc.BRK[10] = 0.035f;
        sc.BRK[ 9] = 0.060f;
        sc.BRK[ 8] = 0.100f;
        sc.BRK[ 7] = 0.159f;
        sc.BRK[ 6] = 0.228f;
        sc.BRK[ 5] = 0.327f;
        sc.BRK[ 4] = 0.477f;
        sc.BRK[ 3] = 0.587f;
        sc.BRK[ 2] = 0.715f;
        sc.BRK[ 1] = 0.858f;
        sc.BRK[ 0] = 1.000f;
    */

    uint32 rankListSize = 0;
    // you need to reach CONFIG_UINT32_MIN_HONOR_KILLS to be added in standing list
    for (HonorStandingList::const_iterator it = standingList.begin(); it != standingList.end(); ++it)
        if (it->honorKills >= sWorld.getConfig(CONFIG_UINT32_MIN_HONOR_KILLS))
            ++rankListSize;
    // get the WS scores at the top of each break point
    for (uint8 group = 0; group < 14; group++)
        sc.BRK[group] = floor((sc.BRK[group] * rankListSize) + 0.5f);

    // initialize RP array
    // set the low point
    sc.FY[ 0] = 0;

    // the Y values for each breakpoint are fixed
    sc.FY[ 1] = 400;
    for (uint8 i = 2; i <= 13; i++)
        sc.FY[i] = (i - 1) * 1000;

    // and finally
    sc.FY[14] = 13000;   // ... gets 13000 RP

    // the X values for each breakpoint are found from the CP scores
    // of the players around that point in the WS scores
    HonorStanding *tempSt;
    float honor;

    // initialize CP array
    sc.FX[ 0] = 0;

    for (uint8 i = 1; i <= 13; i++)
    {
        // On essaye de faire une moyenne à chaque pallier entre 2 joueurs tangeants.
        honor = 0.0f;
        tempSt = sObjectMgr.GetHonorStandingByPosition(standingList, sc.BRK[i]);
        if (tempSt)
        {
            honor += tempSt->honorPoints;
            tempSt = sObjectMgr.GetHonorStandingByPosition(standingList, sc.BRK[i] + 1);
            if (tempSt)
            {
                honor += tempSt->honorPoints;
                sc.FX[i] = honor / 2.0f;
            }
            else
                sc.FX[i] = honor;
        }
        else
        {
            // Il n'y a personne !!
            // Or il faut obligatoirement des donnees.
            // On extrapole avec les VH du meilleur joueur.
            // C'est de la grosse bidouille, mais on ne peut pas faire autrement.
            tempSt = sObjectMgr.GetHonorStandingByPosition(standingList, 1);
            // Il y a au moins un joueur dans le classement quand meme !
            if (tempSt)
                sc.FX[i] = tempSt->honorPoints;
            else
                sc.FX[i] = 0.0f; // On remplit des donnees mais y'a aucun joueur :/
        }
    }

    // set the high point if FX full filled before
    sc.FX[14] = sc.BRK[13] ? standingList.begin()->honorPoints : 0;   // top scorer

    return sc;
}

float MaNGOS::Honor::CalculateRpEarning(float CP, HonorScores sc)
{
    // search the function for the two points that bound the given CP
    uint8 i = 0;
    while (i < 14 && sc.BRK[i] > 0 && sc.FX[i] < CP)
        i++;

    // we now have i such that FX[i] > CP >= FX[i-1]
    // so interpolate
    return (sc.FY[i] - sc.FY[i - 1]) * (CP - sc.FX[i - 1]) / (sc.FX[i] - sc.FX[i - 1]) + sc.FY[i - 1];
}

float MaNGOS::Honor::HonorableKillPoints(Player *killer, Player *victim, uint32 groupsize)
{
    if (!killer || !victim || !groupsize)
        return 0.0;

    uint32 today = sWorld.GetDateToday();

    int total_kills  = killer->CalculateTotalKills(victim, today, today);
    //int k_rank       = killer->CalculateHonorRank( killer->GetTotalHonor() );
    uint32 v_rank    = victim->GetHonorRankInfo().visualRank;
    uint32 k_level   = killer->getLevel();
    int v_level      = victim->getLevel();
    //float diff_honor = (victim->GetRankPoints() /(killer->GetRankPoints()+1))+1;
    // Penalty due to level diff
    float diff_level = MaNGOS::XP::BaseGainLevelFactor(killer->getLevel(), victim->getLevel());
    // same unit killing penalty
    double f = total_kills >= 10 ? 0 : 1 - double(total_kills) / 10;
    // level related coef
    double ff;

    if (k_level >= 60)
        ff = 1;
    else if ((k_level <= 59) && (k_level >= 50))
        ff = 0.9545;
    else if ((k_level <= 49) && (k_level >= 40))
        ff = 0.5707;
    else if ((k_level <= 39) && (k_level >= 30))
        ff = 0.3434;
    else if ((k_level <= 29) && (k_level >= 20))
        ff = 0.2070;
    else if (k_level <= 19)
        ff = 0.1212;
    else
        ff = 0.1212; // Not sure

    //int honor_points = int(((float)(f * 0.25)*(float)((k_level+(v_rank*5+1))*(1+0.05*diff_honor)*diff_level)));
    //return (honor_points <= 400 ? honor_points : 400) / groupsize;
    double honor_points = 0;
    int leveldiff = (k_level - v_level);
    // [-PROGRESSIVE] Honor gain per victim rank changed in 1.8
    // Values from http://www.wowwiki.com/Honor_system_(pre-2.0_formulas)
    const float expFactor = 157.38086f; // post 1.8 value is 188.3
    return ceil(ff * f * (expFactor * exp(0.05331 * v_rank)) * diff_level / groupsize);
}

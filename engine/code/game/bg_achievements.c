#include "bg_achievements.h"

const bgAchievementTierDef_t bg_distanceAchievementTiers[] = {
        { 10.0, "Sunday Driver", "Cover a relaxed 10 km overall." },
        { 50.0, "Daily Commuter", "Cruise through a combined 50 km." },
        { 150.0, "Road Tripper", "Accumulate 150 km behind the wheel." },
        { 300.0, "Night Rider", "Push past 300 km of distance." },
        { 600.0, "Highway Hero", "Rack up 600 km on the odometer." },
        { 1500.0, "Endurance Ace", "Stay in the race for 1,500 km total." },
        { 5000.0, "Globetrotter", "Log an epic 5,000 km journey." },
        { 10000.0, "Legend of Asphalt", "Master the asphalt for 10,000 km." }
};

const bgAchievementTierDef_t bg_killAchievementTiers[] = {
        { 10.0, "Spark Starter", "Score 10 takedowns." },
        { 25.0, "Arc Blazer", "Deliver 25 total kills." },
        { 50.0, "Demolition Driver", "Knock rivals out 50 times." },
        { 100.0, "Pit Boss", "Dominate the arena with 100 kills." },
        { 250.0, "Arena Menace", "Send 250 opponents packing." },
        { 500.0, "Road Reaper", "Leave 500 wrecks behind." },
        { 1000.0, "Overdrive Executioner", "Achieve 1,000 eliminations." },
        { 2500.0, "Apocalypse Engine", "Crush 2,500 opponents." }
};

const bgAchievementTierDef_t bg_winAchievementTiers[] = {
        { 1.0, "Checkered Debut", "Win your very first race." },
        { 3.0, "Podium Regular", "Collect 3 total victories." },
        { 5.0, "Sprint Specialist", "Take home 5 wins." },
        { 10.0, "Championship Hopeful", "Secure 10 race wins." },
        { 20.0, "Series Star", "Reach 20 gold finishes." },
        { 30.0, "Circuit Royalty", "Earn 30 overall wins." },
        { 40.0, "Dynasty Driver", "Stack up 40 victories." },
        { 50.0, "Hall of Fame", "Celebrate 50 race wins." }
};

const bgAchievementTierDef_t bg_flagCaptureAchievementTiers[] = {
        { 1.0, "Flag Rookie", "Capture your first flag." },
        { 5.0, "Fast Courier", "Deliver 5 flags to base." },
        { 10.0, "Relay Racer", "Bank 10 successful captures." },
        { 25.0, "Siege Runner", "Snatch 25 flags." },
        { 50.0, "Banner Bandit", "Swipe 50 flags." },
        { 75.0, "Frontline Phantom", "Steal 75 flags unnoticed." },
        { 100.0, "Flagship", "Secure 100 captures." },
        { 150.0, "Mythic Messenger", "Run home 150 flags." }
};

const bgAchievementTierDef_t bg_flagAssistAchievementTiers[] = {
        { 1.0, "Helping Hand", "Assist with 1 flag capture." },
        { 5.0, "Wingman", "Support 5 flag scores." },
        { 10.0, "Shield Mate", "Help with 10 captures." },
        { 25.0, "Escort Elite", "Escort 25 flags safely." },
        { 50.0, "Guardian Angel", "Guide 50 flags home." },
        { 75.0, "Formation Leader", "Add 75 total assists." },
        { 100.0, "Tactical Anchor", "Reach 100 flag assists." },
        { 150.0, "Legendary Support", "Record 150 flag assists." }
};

const bgAchievementTierDef_t bg_fuelAchievementTiers[] = {
        { 10.0, "Fuel Sipper", "Burn through 10 L of fuel." },
        { 50.0, "Tank Tipper", "Spend 50 L on the throttle." },
        { 100.0, "Octane Addict", "Consume 100 L overall." },
        { 250.0, "Combustion Captain", "Use 250 L chasing speed." },
        { 500.0, "Turbo Baron", "Torch 500 L of fuel." },
        { 1000.0, "Inferno Investor", "Pour 1,000 L into momentum." },
        { 2500.0, "Petrol Pharaoh", "Spend 2,500 L keeping pace." },
        { 5000.0, "Galaxy Guzzler", "Atomize 5,000 L in total." }
};

const bgAchievementTierDef_t bg_accuracyAchievementTiers[] = {
        { 1.0, "Sharpshooter", "Finish a match with 75% accuracy." },
        { 5.0, "Deadeye", "Post 75% accuracy in 5 matches." },
        { 10.0, "Pinpoint", "Hit the 75% mark in 10 matches." },
        { 20.0, "Bullseye", "Hold 75% accuracy across 20 matches." },
        { 35.0, "Laser Focus", "Reach 75% accuracy in 35 matches." },
        { 50.0, "Hawkeye", "Log 50 high-accuracy matches." },
        { 75.0, "True Aim", "Strike 75% accuracy in 75 matches." },
        { 100.0, "Mark V Master", "Break 100 matches with 75% accuracy." }
};

const bgAchievementTierDef_t bg_excellentAchievementTiers[] = {
        { 1.0, "Quick Combo", "Earn a double kill medal." },
        { 5.0, "Chain Reaction", "Score 5 Excellent medals." },
        { 10.0, "Volley Driver", "Rack up 10 Excellent medals." },
        { 20.0, "Momentum Maker", "Stack 20 double kills." },
        { 35.0, "Blitz Conductor", "Collect 35 Excellent medals." },
        { 50.0, "Overdrive Duellist", "Secure 50 Excellent medals." },
        { 75.0, "Carnage Maestro", "Reach 75 Excellent medals." },
        { 100.0, "Legendary Combo", "Achieve 100 Excellent medals." }
};

const bgAchievementTierDef_t bg_impressiveAchievementTiers[] = {
        { 1.0, "Rail Pair", "Land 2 rail hits in a row." },
        { 5.0, "Polished Aim", "Earn 5 Impressive medals." },
        { 10.0, "Steel Focus", "Earn 10 Impressive medals." },
        { 20.0, "Ion Sight", "Collect 20 Impressive medals." },
        { 35.0, "Rail Virtuoso", "Collect 35 Impressive medals." },
        { 50.0, "Beam Savant", "Earn 50 Impressive medals." },
        { 75.0, "Arc Maestro", "Secure 75 Impressive medals." },
        { 100.0, "Impeccable", "Reach 100 Impressive medals." }
};

const bgAchievementTierDef_t bg_perfectAchievementTiers[] = {
        { 1.0, "Unscathed", "Finish a match without dying." },
        { 3.0, "Untouched", "Win 3 matches without dying." },
        { 5.0, "Flawless", "Win 5 matches without dying." },
        { 10.0, "Immaculate", "Finish 10 matches death-free." },
        { 20.0, "Ghost Driver", "Complete 20 matches without a death." },
        { 35.0, "Untouchable", "Survive 35 matches without falling." },
        { 50.0, "Perfect Storm", "End 50 matches without dying." },
        { 75.0, "Mythic Survivor", "Reach 75 perfect finishes." }
};

const char *const bg_achievementMedalLockedPaths[BG_ACHIEVEMENT_ICON_COUNT] = {
    "menu/achievements/medal_driven_locked",
    "menu/achievements/medal_kills_locked",
    "menu/achievements/medal_wins_locked",
    "menu/achievements/medal_flags_locked",
    "menu/achievements/medal_assists_locked",
    "menu/achievements/medal_fuel_locked",
    "menu/achievements/medal_accuracy_locked",
    "menu/achievements/medal_excellent_locked",
    "menu/achievements/medal_impressive_locked",
    "menu/achievements/medal_perfect_locked"
};

const char *const bg_achievementMedalUnlockedPaths[BG_ACHIEVEMENT_ICON_COUNT] = {
    "menu/achievements/medal_driven_unlocked",
    "menu/achievements/medal_kills_unlocked",
    "menu/achievements/medal_wins_unlocked",
    "menu/achievements/medal_flags_unlocked",
    "menu/achievements/medal_assists_unlocked",
    "menu/achievements/medal_fuel_unlocked",
    "menu/achievements/medal_accuracy_unlocked",
    "menu/achievements/medal_excellent_unlocked",
    "menu/achievements/medal_impressive_unlocked",
    "menu/achievements/medal_perfect_unlocked"
};

const bgAchievementCategoryDef_t bg_achievementCategories[BG_ACHIEVEMENT_CATEGORY_COUNT] = {
    { "Distance Driven", bg_distanceAchievementTiers, ARRAY_LEN( bg_distanceAchievementTiers ), BG_ACHIEVEMENT_ICON_DRIVEN },
    { "Kills", bg_killAchievementTiers, ARRAY_LEN( bg_killAchievementTiers ), BG_ACHIEVEMENT_ICON_KILLS },
    { "Races Won", bg_winAchievementTiers, ARRAY_LEN( bg_winAchievementTiers ), BG_ACHIEVEMENT_ICON_WINS },
    { "Flags Captured", bg_flagCaptureAchievementTiers, ARRAY_LEN( bg_flagCaptureAchievementTiers ), BG_ACHIEVEMENT_ICON_FLAGS },
    { "Flag Assists", bg_flagAssistAchievementTiers, ARRAY_LEN( bg_flagAssistAchievementTiers ), BG_ACHIEVEMENT_ICON_FLAG_ASSISTS },
    { "Fuel Consumed", bg_fuelAchievementTiers, ARRAY_LEN( bg_fuelAchievementTiers ), BG_ACHIEVEMENT_ICON_FUEL },
    { "Accuracy", bg_accuracyAchievementTiers, ARRAY_LEN( bg_accuracyAchievementTiers ), BG_ACHIEVEMENT_ICON_ACCURACY },
    { "Excellent", bg_excellentAchievementTiers, ARRAY_LEN( bg_excellentAchievementTiers ), BG_ACHIEVEMENT_ICON_EXCELLENT },
    { "Impressive", bg_impressiveAchievementTiers, ARRAY_LEN( bg_impressiveAchievementTiers ), BG_ACHIEVEMENT_ICON_IMPRESSIVE },
    { "Perfect", bg_perfectAchievementTiers, ARRAY_LEN( bg_perfectAchievementTiers ), BG_ACHIEVEMENT_ICON_PERFECT }
};

int BG_AchievementCategoryCount( void ) {
    return BG_ACHIEVEMENT_CATEGORY_COUNT;
}

const bgAchievementCategoryDef_t *BG_AchievementGetCategory( int index ) {
    if ( index < 0 || index >= BG_ACHIEVEMENT_CATEGORY_COUNT ) {
        return NULL;
    }

    return &bg_achievementCategories[index];
}

int BG_AchievementTierCount( int categoryIndex ) {
    const bgAchievementCategoryDef_t *category = BG_AchievementGetCategory( categoryIndex );

    return category ? category->tierCount : 0;
}

const bgAchievementTierDef_t *BG_AchievementGetTier( int categoryIndex, int tierIndex ) {
    const bgAchievementCategoryDef_t *category = BG_AchievementGetCategory( categoryIndex );

    if ( !category || tierIndex < 0 || tierIndex >= category->tierCount ) {
        return NULL;
    }

    return &category->tiers[tierIndex];
}

int BG_AchievementUnlockedTiers( const bgAchievementCategoryDef_t *category, double progress ) {
    int unlocked = 0;
    int i;

    if ( !category ) {
        return 0;
    }

    for ( i = 0; i < category->tierCount; ++i ) {
        if ( progress >= category->tiers[i].threshold ) {
            unlocked = i + 1;
        }
    }

    return unlocked;
}

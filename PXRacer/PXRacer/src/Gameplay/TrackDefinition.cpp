#include "TrackDefinition.h"
#include <algorithm>
#include <iterator>  

// Cache static pentru circuite
static std::vector<TrackDefinition> s_trackCache;
static bool s_initialized = false;

void initializeTracksIfNeeded() {
    if (!s_initialized) {
        s_trackCache.push_back(TrackLibrary::createTestTrack());
        s_trackCache.push_back(TrackLibrary::createMonacoGP());
        s_trackCache.push_back(TrackLibrary::createSpaFrancorchamps());
        s_trackCache.push_back(TrackLibrary::createMonzaGP());
        s_trackCache.push_back(TrackLibrary::createSilverstoneGP());
        s_trackCache.push_back(TrackLibrary::createNurburgringGP());
        s_initialized = true;
    }
}

// ✅ TEST OVAL - NASCAR style banking (PERFECT CLOSED LOOP!)
TrackDefinition TrackLibrary::createTestTrack() {
    TrackDefinition track("test_oval", "Test Oval", "Virtual", 2.5f, 3, TrackDifficulty::Easy);
    track.description = "Classic NASCAR-style oval with banking";
    track.year = 2024;

    // Front Straight
    track.sections.push_back(TrackSection(SectionType::Straight, 60, 0.0f, "Front Straight"));
    
    // Turn 1-2 cu BANKING (deal ușor pentru simulare banking)
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 50, 7.0f, "Turn 1-2"));
    
    // Back Straight CU DEAL pentru test! ✅
    track.sections.push_back(TrackSection(SectionType::Hill, 40, 3.0f, "Back Hill"));
    track.sections.push_back(TrackSection(SectionType::Straight, 20, 0.0f, "Back Straight"));
    
    // Turn 3-4
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 50, 7.0f, "Turn 3-4"));

    return track;
}

    // ✅ MONACO - Curbe strânse pe străzi înguste
TrackDefinition TrackLibrary::createMonacoGP() {
    TrackDefinition track("monaco_gp", "Circuit de Monaco", "Monaco", 3.337f, 78, TrackDifficulty::Expert);
    track.description = "The jewel in F1's crown - narrow streets and glamour";
    track.year = 1929;

    track.sections.push_back(TrackSection(SectionType::Straight, 30, 0.0f, "Start/Finish"));
    track.sections.push_back(TrackSection(SectionType::RightHairpin, 25, 9.0f, "Sainte Devote"));  // ✅ Hairpin strâns
    track.sections.push_back(TrackSection(SectionType::Straight, 35, 0.0f, "Beau Rivage"));
    track.sections.push_back(TrackSection(SectionType::RightCurve, 20, 6.5f, "Massenet"));         // ✅ Curbă medie
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 15, 5.5f, "Casino"));            // ✅ Curbă rapidă
    track.sections.push_back(TrackSection(SectionType::RightHairpin, 22, 9.5f, "Grand Hotel"));    // ✅ Hairpin foarte strâns
    track.sections.push_back(TrackSection(SectionType::Straight, 40, 0.0f, "Tunnel"));
    track.sections.push_back(TrackSection(SectionType::Chicane, 25, 7.0f, "Nouvelle Chicane"));    // ✅ Chicane tehnică
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 20, 5.0f, "Tabac"));             // ✅ Curbă largă
    track.sections.push_back(TrackSection(SectionType::RightCurve, 18, 6.0f, "Swimming Pool"));    // ✅ Complexă
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 15, 8.0f, "Rascasse"));          // ✅ Strânsă
    track.sections.push_back(TrackSection(SectionType::RightCurve, 20, 6.5f, "Anthony Noghes"));   // ✅ Medie

    return track;
}

// ✅ SPA - Mix de curbe rapide și tehnice
TrackDefinition TrackLibrary::createSpaFrancorchamps() {
    TrackDefinition track("spa_francorchamps", "Spa-Francorchamps", "Belgium", 7.004f, 44, TrackDifficulty::Hard);
    track.description = "The most challenging circuit - Eau Rouge awaits";
    track.year = 1925;

    track.sections.push_back(TrackSection(SectionType::Straight, 40, 0.0f, "Start/Finish"));
    track.sections.push_back(TrackSection(SectionType::RightHairpin, 20, 9.0f, "La Source"));      // ✅ Hairpin strâns
    track.sections.push_back(TrackSection(SectionType::Hill, 35, 5.0f, "Eau Rouge"));              // ✅ Deal iconic + left
    track.sections.push_back(TrackSection(SectionType::Straight, 50, 0.0f, "Kemmel"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 25, 7.5f, "Les Combes"));        // ✅ Combinație complexă
    track.sections.push_back(TrackSection(SectionType::RightCurve, 20, 6.0f, "Bruxelles"));        // ✅ Downhill
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 30, 5.5f, "Pouhon"));            // ✅ Rapidă double-apex
    track.sections.push_back(TrackSection(SectionType::Chicane, 25, 6.5f, "Chicane"));             // ✅ Fagnes chicane
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 30, 4.5f, "Blanchimont"));       // ✅ Foarte rapidă!
    track.sections.push_back(TrackSection(SectionType::Chicane, 22, 7.5f, "Bus Stop"));            // ✅ Strânsă

    return track;
}

// ✅ MONZA - Temple of Speed cu curbe rapide
TrackDefinition TrackLibrary::createMonzaGP() {
    TrackDefinition track("monza_gp", "Autodromo di Monza", "Italy", 5.793f, 53, TrackDifficulty::Medium);
    track.description = "Temple of Speed - pure throttle action";
    track.year = 1922;

    track.sections.push_back(TrackSection(SectionType::Straight, 60, 0.0f, "Main Straight"));
    track.sections.push_back(TrackSection(SectionType::Chicane, 20, 7.5f, "Variante del Rettifilo")); // ✅ Prima chicane
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 35, 4.0f, "Curva Grande"));         // ✅ Foarte rapidă
    track.sections.push_back(TrackSection(SectionType::Chicane, 18, 6.5f, "Variante della Roggia"));  // ✅ A doua chicane
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 25, 5.5f, "Lesmo 1"));              // ✅ Medie-rapidă
    track.sections.push_back(TrackSection(SectionType::RightCurve, 22, 5.5f, "Lesmo 2"));             // ✅ Medie-rapidă
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 30, 6.5f, "Ascari"));               // ✅ Complexă chicane
    track.sections.push_back(TrackSection(SectionType::RightCurve, 35, 5.0f, "Parabolica"));          // ✅ Long sweeper

    return track;
}

// ✅ SILVERSTONE - Flowing high-speed
TrackDefinition TrackLibrary::createSilverstoneGP() {
    TrackDefinition track("silverstone_gp", "Silverstone Circuit", "United Kingdom", 5.891f, 52, TrackDifficulty::Medium);
    track.description = "Home of British motorsport";
    track.year = 1948;

    track.sections.push_back(TrackSection(SectionType::Straight, 45, 0.0f, "Start/Finish"));
    track.sections.push_back(TrackSection(SectionType::RightCurve, 22, 5.0f, "Abbey"));             // ✅ Rapidă
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 30, 4.5f, "Farm Curve"));         // ✅ Very fast
    track.sections.push_back(TrackSection(SectionType::Straight, 40, 0.0f, "Wellington"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 25, 6.0f, "Brooklands"));         // ✅ Medie
    track.sections.push_back(TrackSection(SectionType::RightCurve, 22, 6.5f, "Luffield"));          // ✅ Tehnică
    track.sections.push_back(TrackSection(SectionType::Straight, 35, 0.0f, "Hangar Straight"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 30, 5.5f, "Stowe"));              // ✅ Rapidă
    track.sections.push_back(TrackSection(SectionType::RightCurve, 25, 5.0f, "Vale"));              // ✅ Rapidă
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 22, 6.0f, "Club"));               // ✅ Finală tehnică

    return track;
}

// ✅ NÜRBURGRING GP - Technical modern layout
TrackDefinition TrackLibrary::createNurburgringGP() {
    TrackDefinition track("nurburgring_gp", "Nürburgring GP", "Germany", 5.148f, 60, TrackDifficulty::Hard);
    track.description = "Technical challenge in the Eifel mountains";
    track.year = 1984;

    track.sections.push_back(TrackSection(SectionType::Straight, 40, 0.0f, "Start/Finish"));
    track.sections.push_back(TrackSection(SectionType::RightCurve, 20, 6.5f, "Turn 1"));            // ✅ Tehnică
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 25, 5.0f, "Turn 2-3"));           // ✅ Combinație rapidă
    track.sections.push_back(TrackSection(SectionType::Straight, 35, 0.0f, "Back Straight"));
    track.sections.push_back(TrackSection(SectionType::RightHairpin, 25, 8.5f, "Veedol Chicane"));  // ✅ Chicane strânsă
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 30, 4.5f, "Schumacher S"));       // ✅ Rapidă S
    track.sections.push_back(TrackSection(SectionType::RightCurve, 22, 5.5f, "Turn 10"));           // ✅ Medie
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 28, 6.0f, "Turn 13"));            // ✅ Finală tehnică

    return track;
}

std::vector<TrackDefinition> TrackLibrary::getAllTracks() {
    initializeTracksIfNeeded();
    return s_trackCache;
}

std::vector<TrackDefinition> TrackLibrary::getTracksByDifficulty(TrackDifficulty diff) {
    initializeTracksIfNeeded();
    std::vector<TrackDefinition> filtered;

    std::copy_if(s_trackCache.begin(), s_trackCache.end(),
        std::back_inserter(filtered),
        [diff](const TrackDefinition& track) {
            return track.difficulty == diff;
        });

    return filtered;
}

TrackDefinition* TrackLibrary::getTrackById(const std::string& id) {
    initializeTracksIfNeeded();

    auto it = std::find_if(s_trackCache.begin(), s_trackCache.end(),
        [&id](const TrackDefinition& track) {
            return track.id == id;
        });

    return (it != s_trackCache.end()) ? &(*it) : nullptr;
}
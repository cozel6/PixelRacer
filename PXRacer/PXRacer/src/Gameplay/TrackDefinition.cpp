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

// ✅ TEST - Circuitul simplu pentru testare și învățare
TrackDefinition TrackLibrary::createTestTrack() {
    TrackDefinition track("test", "Test", "Virtual", 2.5f, 3, TrackDifficulty::Easy);
    track.description = "Simple oval track for testing and practice";
    track.year = 2024;

    // Front Straight
    track.sections.push_back(TrackSection(SectionType::Straight, 60, 0.0f, "Front Straight"));
    
    // Turn 1-2 cu BANKING
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 50, 7.0f, "Turn 1-2"));
    
    // Back Straight cu deal pentru test
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
    track.sections.push_back(TrackSection(SectionType::RightHairpin, 25, 9.0f, "Sainte Devote"));
    track.sections.push_back(TrackSection(SectionType::Straight, 35, 0.0f, "Beau Rivage"));
    track.sections.push_back(TrackSection(SectionType::RightCurve, 20, 6.5f, "Massenet"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 15, 5.5f, "Casino"));
    track.sections.push_back(TrackSection(SectionType::RightHairpin, 22, 9.5f, "Grand Hotel"));
    track.sections.push_back(TrackSection(SectionType::Straight, 40, 0.0f, "Tunnel"));
    track.sections.push_back(TrackSection(SectionType::Chicane, 25, 7.0f, "Nouvelle Chicane"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 20, 5.0f, "Tabac"));
    track.sections.push_back(TrackSection(SectionType::RightCurve, 18, 6.0f, "Swimming Pool"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 15, 8.0f, "Rascasse"));
    track.sections.push_back(TrackSection(SectionType::RightCurve, 20, 6.5f, "Anthony Noghes"));

    return track;
}

// ✅ SPA - Mix de curbe rapide și tehnice
TrackDefinition TrackLibrary::createSpaFrancorchamps() {
    TrackDefinition track("spa_francorchamps", "Spa-Francorchamps", "Belgium", 7.004f, 44, TrackDifficulty::Hard);
    track.description = "The most challenging circuit - Eau Rouge awaits";
    track.year = 1925;

    track.sections.push_back(TrackSection(SectionType::Straight, 40, 0.0f, "Start/Finish"));
    track.sections.push_back(TrackSection(SectionType::RightHairpin, 20, 9.0f, "La Source"));
    track.sections.push_back(TrackSection(SectionType::Hill, 35, 5.0f, "Eau Rouge"));
    track.sections.push_back(TrackSection(SectionType::Straight, 50, 0.0f, "Kemmel"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 25, 7.5f, "Les Combes"));
    track.sections.push_back(TrackSection(SectionType::RightCurve, 20, 6.0f, "Bruxelles"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 30, 5.5f, "Pouhon"));
    track.sections.push_back(TrackSection(SectionType::Chicane, 25, 6.5f, "Fagnes"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 30, 4.5f, "Blanchimont"));
    track.sections.push_back(TrackSection(SectionType::Chicane, 22, 7.5f, "Bus Stop"));

    return track;
}

// ✅ MONZA - Temple of Speed - Layout REAL cu chicane devreme
TrackDefinition TrackLibrary::createMonzaGP() {
    TrackDefinition track("monza_gp", "Autodromo di Monza", "Italy", 5.793f, 53, TrackDifficulty::Medium);
    track.description = "Temple of Speed - heavy braking chicanes and long straights";
    track.year = 1922;

    // Start/Finish - scurt înainte de prima chicane
    track.sections.push_back(TrackSection(SectionType::Straight, 25, 0.0f, "Pit Straight"));
    
    // Prima chicane - FOARTE DEVREME după start (Variante del Rettifilo / Turn 1-2)
    track.sections.push_back(TrackSection(SectionType::Chicane, 22, 8.0f, "Variante del Rettifilo"));
    
    // Curva Grande - curbă lungă spre DREAPTA, foarte rapidă
    track.sections.push_back(TrackSection(SectionType::RightCurve, 45, 3.5f, "Curva Grande"));
    
    // A doua chicane (Variante della Roggia / Turn 4-5)
    track.sections.push_back(TrackSection(SectionType::Chicane, 20, 7.5f, "Variante della Roggia"));
    
    // Lesmo 1 - curbă spre stânga, MAI DESCHISĂ
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 25, 5.0f, "Lesmo 1"));
    
    // Scurtă dreaptă între Lesmo-uri
    track.sections.push_back(TrackSection(SectionType::Straight, 15, 0.0f, "Lesmo Straight"));
    
    // Lesmo 2 - curbă spre stânga, MAI STRÂNSĂ
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 22, 6.5f, "Lesmo 2"));
    
    // Dreaptă spre Ascari
    track.sections.push_back(TrackSection(SectionType::Straight, 30, 0.0f, "Serraglio Straight"));
    
    // Variante Ascari - chicane MAI LARGĂ (intensitate mai mică)
    track.sections.push_back(TrackSection(SectionType::Chicane, 28, 5.5f, "Variante Ascari"));
    
    // Dreaptă lungă spre Parabolica
    track.sections.push_back(TrackSection(SectionType::Straight, 40, 0.0f, "Back Straight"));
    
    // Parabolica (Curva Alboreto) - curbă lungă spre dreapta, accelerare spre finish
    track.sections.push_back(TrackSection(SectionType::RightCurve, 40, 4.5f, "Curva Parabolica"));

    return track;
}

// ✅ SILVERSTONE - Flowing high-speed
TrackDefinition TrackLibrary::createSilverstoneGP() {
    TrackDefinition track("silverstone_gp", "Silverstone Circuit", "United Kingdom", 5.891f, 52, TrackDifficulty::Medium);
    track.description = "Home of British motorsport - fast and flowing";
    track.year = 1948;

    track.sections.push_back(TrackSection(SectionType::Straight, 45, 0.0f, "Start/Finish"));
    track.sections.push_back(TrackSection(SectionType::RightCurve, 22, 5.0f, "Abbey"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 30, 4.5f, "Farm Curve"));
    track.sections.push_back(TrackSection(SectionType::Straight, 40, 0.0f, "Wellington Straight"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 25, 6.0f, "Brooklands"));
    track.sections.push_back(TrackSection(SectionType::RightCurve, 22, 6.5f, "Luffield"));
    track.sections.push_back(TrackSection(SectionType::Straight, 35, 0.0f, "Hangar Straight"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 30, 5.5f, "Stowe"));
    track.sections.push_back(TrackSection(SectionType::RightCurve, 25, 5.0f, "Vale"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 22, 6.0f, "Club"));

    return track;
}

// ✅ NÜRBURGRING GP - Technical modern layout
TrackDefinition TrackLibrary::createNurburgringGP() {
    TrackDefinition track("nurburgring_gp", "Nurburgring GP", "Germany", 5.148f, 60, TrackDifficulty::Hard);
    track.description = "Technical challenge in the Eifel mountains";
    track.year = 1984;

    track.sections.push_back(TrackSection(SectionType::Straight, 40, 0.0f, "Start/Finish"));
    track.sections.push_back(TrackSection(SectionType::RightCurve, 20, 6.5f, "Turn 1"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 25, 5.0f, "Turn 2-3"));
    track.sections.push_back(TrackSection(SectionType::Straight, 35, 0.0f, "Back Straight"));
    track.sections.push_back(TrackSection(SectionType::RightHairpin, 25, 8.5f, "Veedol Chicane"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 30, 4.5f, "Schumacher S"));
    track.sections.push_back(TrackSection(SectionType::RightCurve, 22, 5.5f, "Turn 10"));
    track.sections.push_back(TrackSection(SectionType::LeftCurve, 28, 6.0f, "Turn 13"));

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
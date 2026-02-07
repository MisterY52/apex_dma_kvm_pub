#ifndef VISUALS_H
#define VISUALS_H

struct visuals {
    bool box;         // Whether to draw a box around players
    bool name;        // Whether to display player names
    bool line;        // Whether to draw lines to players
    bool distance;    // Whether to display the distance to players
    bool healthbar;   // Whether to display health bars
    bool shieldbar;   // Whether to display shield bars
    bool skeleton;    // Whether to display skeletons
    // Add other members as needed
    // For example, if you have other visual features like level display, you can add:
    // bool player_level; // Whether to display player levels
};

extern visuals v;

#endif // VISUALS_H
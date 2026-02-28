#pragma once

#define NOMINMAX                   
#define _CRT_SECURE_NO_WARNINGS     

// Undefine if already defined 
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <cstdio> // Added for snprintf

#include "Utils.h"
using namespace std;

class SFMLVisualizer {
private:
    static const int WINDOW_WIDTH = 1200;
    static const int WINDOW_HEIGHT = 800;

    // Helper to convert lat/lon to screen coordinates
    static sf::Vector2f latLonToScreen(double lat, double lon,
        double minLat, double maxLat,
        double minLon, double maxLon) {
        float x = ((lon - minLon) / (maxLon - minLon)) * (WINDOW_WIDTH - 200) + 100;
        float y = WINDOW_HEIGHT - 100 - ((lat - minLat) / (maxLat - minLat)) * (WINDOW_HEIGHT - 200);
        return sf::Vector2f(x, y);
    }

    // Draw text on screen
    static void drawText(sf::RenderWindow& window, const string& text,
        float x, float y, int size, sf::Color color) {
        sf::Font font;
        // Try to load a default system font
        if (!font.loadFromFile("arial.ttf")) {
            // If arial.ttf not found, use default rendering
            return;
        }

        sf::Text sfText(text, font, size);
        sfText.setPosition(x, y);
        sfText.setFillColor(color);
        window.draw(sfText);
    }

public:

    // CITY MAP VISUALIZATION
    static void visualizeCityMap(const string locations[],
        const double lats[],
        const double lons[],
        const int types[],
        int count) {
        if (count == 0) {
            cout << "No locations to visualize!" << endl;
            return;
        }

        sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Islamabad City Map");

        // Find bounds
        double minLat = lats[0], maxLat = lats[0];
        double minLon = lons[0], maxLon = lons[0];

        for (int i = 1; i < count; i++) {
            if (lats[i] < minLat) minLat = lats[i];
            if (lats[i] > maxLat) maxLat = lats[i];
            if (lons[i] < minLon) minLon = lons[i];
            if (lons[i] > maxLon) maxLon = lons[i];
        }

        // Add padding
        double latPadding = (maxLat - minLat) * 0.1;
        double lonPadding = (maxLon - minLon) * 0.1;
        minLat -= latPadding; maxLat += latPadding;
        minLon -= lonPadding; maxLon += lonPadding;

        // Font
        sf::Font font;
        bool fontLoaded = font.loadFromFile("arial.ttf");

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(sf::Color(240, 248, 255)); 

            // Draw title
            if (fontLoaded) {
                sf::Text title("Islamabad City Map", font, 30);
                title.setPosition(10, 10);
                title.setFillColor(sf::Color::Black);
                window.draw(title);
            }

            // Draw legend
            if (fontLoaded) {
                string legendItems[] = { "Bus Stop", "Hospital", "School", "Mall", "Facility" };
                sf::Color legendColors[] = {
                    sf::Color::Blue, sf::Color::Red, sf::Color::Green,
                    sf::Color::Magenta, sf::Color::Cyan
                };

                for (int i = 0; i < 5; i++) {
                    sf::CircleShape legendCircle(8);
                    legendCircle.setPosition(WINDOW_WIDTH - 150, 60 + i * 30);
                    legendCircle.setFillColor(legendColors[i]);
                    window.draw(legendCircle);

                    sf::Text legendText(legendItems[i], font, 14);
                    legendText.setPosition(WINDOW_WIDTH - 130, 55 + i * 30);
                    legendText.setFillColor(sf::Color::Black);
                    window.draw(legendText);
                }
            }

            // Draw locations
            for (int i = 0; i < count; i++) {
                sf::Vector2f pos = latLonToScreen(lats[i], lons[i],
                    minLat, maxLat, minLon, maxLon);

                // Determine color based on type
                sf::Color color;
                float radius = 8.0f;

                switch (types[i]) {
                case 0: color = sf::Color::Blue; break;                   // Bus Stop
                case 1: color = sf::Color::Red; radius = 12.0f; break;    // Hospital
                case 2: color = sf::Color(255, 100, 0); break;            // Pharmacy
                case 3: color = sf::Color::Green; radius = 10.0f; break;  // School
                case 4: color = sf::Color::Magenta; radius = 10.0f; break; // Mall
                case 5: color = sf::Color::Cyan; break;                   // Facility
                default: color = sf::Color(128, 128, 128); break;
                }

                // Draw shape
                sf::CircleShape shape(radius);
                shape.setPosition(pos.x - radius, pos.y - radius);
                shape.setFillColor(color);
                shape.setOutlineColor(sf::Color::Black);
                shape.setOutlineThickness(1);
                window.draw(shape);

                // Draw name
                if (fontLoaded && !locations[i].empty()) {
                    sf::Text nameText(locations[i], font, 10);
                    nameText.setPosition(pos.x + radius + 2, pos.y - 5);
                    nameText.setFillColor(sf::Color::Black);
                    window.draw(nameText);
                }
            }

            // Instructions
            if (fontLoaded) {
                sf::Text instructions("Press ESC or close window to exit", font, 12);
                instructions.setPosition(10, WINDOW_HEIGHT - 30);
                instructions.setFillColor(sf::Color(100, 100, 100));
                window.draw(instructions);
            }

            window.display();
        }
    }

    // GRAPH NETWORK VISUALIZATION
    static void visualizeGraphNetwork(const string nodes[],
        const double lats[],
        const double lons[],
        int nodeCount,
        const bool adjacencyMatrix[][50],
        const double weights[][50]) {
        if (nodeCount == 0) {
            cout << "No nodes to visualize!" << endl;
            return;
        }

        sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
            "Network Graph Visualization");

        // Find bounds
        double minLat = lats[0], maxLat = lats[0];
        double minLon = lons[0], maxLon = lons[0];

        for (int i = 1; i < nodeCount; i++) {
            if (lats[i] < minLat) minLat = lats[i];
            if (lats[i] > maxLat) maxLat = lats[i];
            if (lons[i] < minLon) minLon = lons[i];
            if (lons[i] > maxLon) maxLon = lons[i];
        }

        double latPadding = (maxLat - minLat) * 0.1;
        double lonPadding = (maxLon - minLon) * 0.1;
        minLat -= latPadding; maxLat += latPadding;
        minLon -= lonPadding; maxLon += lonPadding;

        sf::Font font;
        bool fontLoaded = font.loadFromFile("arial.ttf");

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(sf::Color::White);

            // Draw title
            if (fontLoaded) {
                sf::Text title("Graph Network - Roads & Connections", font, 24);
                title.setPosition(10, 10);
                title.setFillColor(sf::Color::Black);
                window.draw(title);
            }

            // Draw edges first (so they appear behind nodes)
            for (int i = 0; i < nodeCount; i++) {
                sf::Vector2f pos1 = latLonToScreen(lats[i], lons[i],
                    minLat, maxLat, minLon, maxLon);

                for (int j = i + 1; j < nodeCount; j++) {
                    if (adjacencyMatrix[i][j]) {
                        sf::Vector2f pos2 = latLonToScreen(lats[j], lons[j],
                            minLat, maxLat, minLon, maxLon);

                        // Draw line
                        sf::Vertex line[] = {
                            sf::Vertex(pos1, sf::Color(100, 100, 100)),
                            sf::Vertex(pos2, sf::Color(100, 100, 100))
                        };
                        window.draw(line, 2, sf::Lines);

                        // Draw weight
                        if (fontLoaded) {
                            float midX = (pos1.x + pos2.x) / 2;
                            float midY = (pos1.y + pos2.y) / 2;

                            char weightStr[20];
                            snprintf(weightStr, sizeof(weightStr), "%.4f", weights[i][j]);
                            sf::Text weightText(weightStr, font, 10);
                            weightText.setPosition(midX, midY);
                            weightText.setFillColor(sf::Color(150, 0, 0));
                            window.draw(weightText);
                        }
                    }
                }
            }

            // Draw nodes
            for (int i = 0; i < nodeCount; i++) {
                sf::Vector2f pos = latLonToScreen(lats[i], lons[i],
                    minLat, maxLat, minLon, maxLon);

                sf::CircleShape node(12);
                node.setPosition(pos.x - 12, pos.y - 12);
                node.setFillColor(sf::Color(70, 130, 180));
                node.setOutlineColor(sf::Color::Black);
                node.setOutlineThickness(2);
                window.draw(node);

                // Draw node name
                if (fontLoaded) {
                    sf::Text nameText(nodes[i], font, 11);
                    nameText.setPosition(pos.x + 15, pos.y - 8);
                    nameText.setFillColor(sf::Color::Black);
                    nameText.setStyle(sf::Text::Bold);
                    window.draw(nameText);
                }
            }

            window.display();
        }
    }

    // BUS ROUTE VISUALIZATION
    static void visualizeBusRoute(const string& busNumber,
        const string stops[],
        const double lats[],
        const double lons[],
        int stopCount,
        int currentStopIndex) {
        sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
            "Bus Route: " + busNumber);

        if (stopCount == 0) {
            cout << "No stops in route!" << endl;
            return;
        }

        // Find bounds
        double minLat = lats[0], maxLat = lats[0];
        double minLon = lons[0], maxLon = lons[0];

        for (int i = 1; i < stopCount; i++) {
            if (lats[i] < minLat) minLat = lats[i];
            if (lats[i] > maxLat) maxLat = lats[i];
            if (lons[i] < minLon) minLon = lons[i];
            if (lons[i] > maxLon) maxLon = lons[i];
        }

        double latPadding = (maxLat - minLat) * 0.15;
        double lonPadding = (maxLon - minLon) * 0.15;
        minLat -= latPadding; maxLat += latPadding;
        minLon -= lonPadding; maxLon += lonPadding;

        sf::Font font;
        bool fontLoaded = font.loadFromFile("arial.ttf");

        // Animation state
        sf::Clock clock;

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(sf::Color(245, 245, 220)); // Beige

            // Title
            if (fontLoaded) {
                sf::Text title("Bus Route: " + busNumber, font, 28);
                title.setPosition(10, 10);
                title.setFillColor(sf::Color(0, 0, 128));
                title.setStyle(sf::Text::Bold);
                window.draw(title);

                sf::Text subtitle(to_string(stopCount) + " stops", font, 16);
                subtitle.setPosition(10, 45);
                subtitle.setFillColor(sf::Color(100, 100, 100));
                window.draw(subtitle);
            }

            // Draw route path
            for (int i = 0; i < stopCount - 1; i++) {
                sf::Vector2f pos1 = latLonToScreen(lats[i], lons[i],
                    minLat, maxLat, minLon, maxLon);
                sf::Vector2f pos2 = latLonToScreen(lats[i + 1], lons[i + 1],
                    minLat, maxLat, minLon, maxLon);

                // Thicker line for route
                sf::RectangleShape routeLine(sf::Vector2f(
                    sqrt(pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y, 2)), 4));
                routeLine.setPosition(pos1);
                float angle = atan2(pos2.y - pos1.y, pos2.x - pos1.x) * 180 / 3.14159f;
                routeLine.setRotation(angle);
                routeLine.setFillColor(sf::Color(0, 100, 200, 150));
                window.draw(routeLine);

                // Arrow
                sf::CircleShape arrow(5, 3);
                arrow.setPosition((pos1.x + pos2.x) / 2 - 5, (pos1.y + pos2.y) / 2 - 5);
                arrow.setRotation(angle);
                arrow.setFillColor(sf::Color(0, 0, 200));
                window.draw(arrow);
            }

            // Draw stops
            for (int i = 0; i < stopCount; i++) {
                sf::Vector2f pos = latLonToScreen(lats[i], lons[i],
                    minLat, maxLat, minLon, maxLon);

                // Stop marker
                sf::CircleShape stopMarker(15);
                stopMarker.setPosition(pos.x - 15, pos.y - 15);

                if (i == currentStopIndex) {
                    // Current stop - pulsing green
                    float pulse = 0.7f + 0.3f * sin(clock.getElapsedTime().asSeconds() * 3);
                    stopMarker.setFillColor(sf::Color(0, 255 * pulse, 0));
                    stopMarker.setOutlineThickness(3);
                }
                else if (i < currentStopIndex) {
                    // Passed stop
                    stopMarker.setFillColor(sf::Color(150, 150, 150));
                    stopMarker.setOutlineThickness(2);
                }
                else {
                    // Future stop
                    stopMarker.setFillColor(sf::Color::White);
                    stopMarker.setOutlineThickness(2);
                }

                stopMarker.setOutlineColor(sf::Color::Black);
                window.draw(stopMarker);

                // Stop number
                if (fontLoaded) {
                    char numStr[5];
                    snprintf(numStr, sizeof(numStr), "%d", i + 1);
                    sf::Text numText(numStr, font, 14);
                    numText.setPosition(pos.x - 6, pos.y - 8);
                    numText.setFillColor(sf::Color::Black);
                    numText.setStyle(sf::Text::Bold);
                    window.draw(numText);

                    // Stop name
                    sf::Text nameText(stops[i], font, 12);
                    nameText.setPosition(pos.x + 20, pos.y - 6);
                    nameText.setFillColor(sf::Color::Black);

                    if (i == currentStopIndex) {
                        nameText.setStyle(sf::Text::Bold);
                        nameText.setFillColor(sf::Color(0, 150, 0));
                    }

                    window.draw(nameText);
                }
            }

            // Draw bus icon at current position
            if (currentStopIndex >= 0 && currentStopIndex < stopCount) {
                sf::Vector2f busPos = latLonToScreen(lats[currentStopIndex], lons[currentStopIndex],
                    minLat, maxLat, minLon, maxLon);

                // Animated bus
                float bobbing = 3 * sin(clock.getElapsedTime().asSeconds() * 4);

                // Bus body
                sf::RectangleShape bus(sf::Vector2f(30, 20));
                bus.setPosition(busPos.x - 15, busPos.y - 30 + bobbing);
                bus.setFillColor(sf::Color::Yellow);
                bus.setOutlineColor(sf::Color::Black);
                bus.setOutlineThickness(2);
                window.draw(bus);

                // Bus windows
                sf::RectangleShape window1(sf::Vector2f(8, 6));
                window1.setPosition(busPos.x - 10, busPos.y - 27 + bobbing);
                window1.setFillColor(sf::Color::Cyan);
                window.draw(window1);

                sf::RectangleShape window2(sf::Vector2f(8, 6));
                window2.setPosition(busPos.x + 2, busPos.y - 27 + bobbing);
                window2.setFillColor(sf::Color::Cyan);
                window.draw(window2);
            }

            // Info panel
            if (fontLoaded && currentStopIndex >= 0 && currentStopIndex < stopCount) {
                sf::RectangleShape infoBox(sf::Vector2f(280, 80));
                infoBox.setPosition(WINDOW_WIDTH - 300, WINDOW_HEIGHT - 100);
                infoBox.setFillColor(sf::Color(255, 255, 255, 230));
                infoBox.setOutlineColor(sf::Color::Black);
                infoBox.setOutlineThickness(2);
                window.draw(infoBox);

                sf::Text infoTitle("Current Location", font, 16);
                infoTitle.setPosition(WINDOW_WIDTH - 290, WINDOW_HEIGHT - 95);
                infoTitle.setFillColor(sf::Color::Black);
                infoTitle.setStyle(sf::Text::Bold);
                window.draw(infoTitle);

                sf::Text currentStop("Stop: " + stops[currentStopIndex], font, 14);
                currentStop.setPosition(WINDOW_WIDTH - 290, WINDOW_HEIGHT - 70);
                currentStop.setFillColor(sf::Color(0, 100, 0));
                window.draw(currentStop);

                char progressStr[50];
                snprintf(progressStr, sizeof(progressStr), "Progress: %d / %d stops", currentStopIndex + 1, stopCount);
                sf::Text progress(progressStr, font, 12);
                progress.setPosition(WINDOW_WIDTH - 290, WINDOW_HEIGHT - 48);
                progress.setFillColor(sf::Color(100, 100, 100));
                window.draw(progress);
            }

            window.display();
        }
    }

    // POPULATION HEATMAP
    static void visualizePopulationHeatmap(const string sectors[],
        const int populations[],
        int sectorCount) {
        sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
            "Population Heatmap");

        sf::Font font;
        bool fontLoaded = font.loadFromFile("arial.ttf");

        // Find max population for scaling
        int maxPop = 0;
        for (int i = 0; i < sectorCount; i++) {
            if (populations[i] > maxPop) maxPop = populations[i];
        }

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(sf::Color::White);

            // Title
            if (fontLoaded) {
                sf::Text title("Population Distribution by Sector", font, 26);
                title.setPosition(10, 10);
                title.setFillColor(sf::Color::Black);
                title.setStyle(sf::Text::Bold);
                window.draw(title);
            }

            // Draw bars
            int barWidth = (WINDOW_WIDTH - 100) / sectorCount;
            if (barWidth > 150) barWidth = 150;
            int spacing = (WINDOW_WIDTH - 100 - barWidth * sectorCount) / (sectorCount + 1);

            for (int i = 0; i < sectorCount; i++) {
                int x = 50 + spacing + i * (barWidth + spacing);
                int barHeight = (maxPop > 0) ? (populations[i] * 500 / maxPop) : 0;
                int y = WINDOW_HEIGHT - 150 - barHeight;

                // Color based on population
                sf::Color barColor;
                float intensity = (float)populations[i] / maxPop;
                if (intensity > 0.7f) {
                    barColor = sf::Color(255, 50, 50); // Red high
                }
                else if (intensity > 0.4f) {
                    barColor = sf::Color(255, 200, 50); // Orange medium
                }
                else {
                    barColor = sf::Color(100, 200, 100); // Green low
                }

                // Bar
                sf::RectangleShape bar(sf::Vector2f(barWidth - 10, barHeight));
                bar.setPosition(x, y);
                bar.setFillColor(barColor);
                bar.setOutlineColor(sf::Color::Black);
                bar.setOutlineThickness(2);
                window.draw(bar);

                if (fontLoaded) {
                    // Population count on bar
                    char popStr[20];
                    snprintf(popStr, sizeof(popStr), "%d", populations[i]);
                    sf::Text popText(popStr, font, 12);
                    popText.setPosition(x + (barWidth - 10) / 2 - 15, y - 20);
                    popText.setFillColor(sf::Color::Black);
                    popText.setStyle(sf::Text::Bold);
                    window.draw(popText);

                    // Sector name
                    sf::Text sectorText(sectors[i], font, 14);
                    sectorText.setPosition(x + (barWidth - 10) / 2 - 15,
                        WINDOW_HEIGHT - 140);
                    sectorText.setFillColor(sf::Color::Black);
                    sectorText.setStyle(sf::Text::Bold);
                    window.draw(sectorText);
                }
            }

            // Legend
            if (fontLoaded) {
                sf::Text legend("Color intensity shows population density", font, 14);
                legend.setPosition(10, WINDOW_HEIGHT - 40);
                legend.setFillColor(sf::Color(100, 100, 100));
                window.draw(legend);
            }

            window.display();
        }
    }

    // STATISTICS DASHBOARD
    static void visualizeStatsDashboard(int totalPop, int buses, int hospitals,
        int schools, int malls, int facilities) {
        sf::RenderWindow window(sf::VideoMode(1000, 700), "City Statistics Dashboard");

        sf::Font font;
        bool fontLoaded = font.loadFromFile("arial.ttf");

        sf::Clock clock;

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(sf::Color(240, 240, 245));

            // Title
            if (fontLoaded) {
                sf::Text title("ISLAMABAD CITY DASHBOARD", font, 32);
                title.setPosition(250, 20);
                title.setFillColor(sf::Color(0, 0, 100));
                title.setStyle(sf::Text::Bold);
                window.draw(title);
            }

            // Stats cards
            string labels[] = { "Population", "Buses", "Hospitals", "Schools", "Malls", "Facilities" };
            int values[] = { totalPop, buses, hospitals, schools, malls, facilities };
            sf::Color colors[] = {
                sf::Color(100, 150, 255),
                sf::Color(255, 200, 100),
                sf::Color(255, 100, 100),
                sf::Color(100, 255, 150),
                sf::Color(255, 150, 255),
                sf::Color(150, 200, 255)
            };

            for (int i = 0; i < 6; i++) {
                int row = i / 3;
                int col = i % 3;
                int x = 50 + col * 310;
                int y = 120 + row * 250;

                // Card background with shadow
                sf::RectangleShape shadow(sf::Vector2f(280, 200));
                shadow.setPosition(x + 5, y + 5);
                shadow.setFillColor(sf::Color(0, 0, 0, 50));
                window.draw(shadow);

                // Card
                sf::RectangleShape card(sf::Vector2f(280, 200));
                card.setPosition(x, y);
                card.setFillColor(sf::Color::White);
                card.setOutlineColor(colors[i]);
                card.setOutlineThickness(3);
                window.draw(card);

                // Icon area
                sf::RectangleShape iconBg(sf::Vector2f(280, 80));
                iconBg.setPosition(x, y);
                iconBg.setFillColor(colors[i]);
                window.draw(iconBg);

                if (fontLoaded) {
                    // Label
                    sf::Text label(labels[i], font, 20);
                    label.setPosition(x + 20, y + 25);
                    label.setFillColor(sf::Color::White);
                    label.setStyle(sf::Text::Bold);
                    window.draw(label);

                    // Value with animation
                    float pulse = 0.9f + 0.1f * sin(clock.getElapsedTime().asSeconds() * 2 + i);
                    char valStr[20];
                    snprintf(valStr, sizeof(valStr), "%d", values[i]);
                    sf::Text value(valStr, font, 48);
                    value.setPosition(x + 140 - value.getLocalBounds().width / 2, y + 105);
                    value.setFillColor(colors[i]);
                    value.setScale(pulse, pulse);
                    value.setStyle(sf::Text::Bold);
                    window.draw(value);
                }
            }

            window.display();
        }
    }

    // TRANSPORT HUBS VISUALIZATION

    static void visualizeTransportHubs(const string hubIDs[], const string names[],
        const double lats[], const double lons[], const int types[],
        const bool operational[], int count) {
        if (count == 0) {
            cout << "No transport hubs to visualize!" << endl;
            return;
        }

        sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
            "Transport Hubs");

        // Find bounds
        double minLat = lats[0], maxLat = lats[0];
        double minLon = lons[0], maxLon = lons[0];

        for (int i = 1; i < count; i++) {
            if (lats[i] < minLat) minLat = lats[i];
            if (lats[i] > maxLat) maxLat = lats[i];
            if (lons[i] < minLon) minLon = lons[i];
            if (lons[i] > maxLon) maxLon = lons[i];
        }

        double latPadding = (maxLat - minLat) * 0.1;
        double lonPadding = (maxLon - minLon) * 0.1;
        minLat -= latPadding; maxLat += latPadding;
        minLon -= lonPadding; maxLon += lonPadding;

        sf::Font font;
        bool fontLoaded = font.loadFromFile("arial.ttf");

        sf::Color hubColors[] = {
            sf::Color::Blue,      // Airport
            sf::Color::Green,     // Railway
            sf::Color::Yellow,    // Metro
            sf::Color::Magenta    // Bus Terminal
        };

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(sf::Color(245, 245, 255));

            // Title
            if (fontLoaded) {
                sf::Text title("Transport Hubs - Airport/Rail Integration", font, 24);
                title.setPosition(10, 10);
                title.setFillColor(sf::Color::Black);
                title.setStyle(sf::Text::Bold);
                window.draw(title);
            }

            // Draw hubs
            for (int i = 0; i < count; i++) {
                sf::Vector2f pos = latLonToScreen(lats[i], lons[i],
                    minLat, maxLat, minLon, maxLon);

                // Hub icon
                sf::CircleShape hub(20);
                hub.setPosition(pos.x - 20, pos.y - 20);
                if (types[i] >= 0 && types[i] < 4) {
                    hub.setFillColor(hubColors[types[i]]);
                }
                else {
                    hub.setFillColor(sf::Color::Blue);
                }
                if (!operational[i]) {
                    hub.setFillColor(sf::Color(100, 100, 100));
                }
                hub.setOutlineColor(sf::Color::Black);
                hub.setOutlineThickness(2);
                window.draw(hub);

                // Hub name
                if (fontLoaded) {
                    sf::Text nameText(names[i], font, 14);
                    nameText.setPosition(pos.x + 25, pos.y - 10);
                    nameText.setFillColor(sf::Color::Black);
                    nameText.setStyle(sf::Text::Bold);
                    window.draw(nameText);
                }
            }

            window.display();
        }
    }

    // EMERGENCY VEHICLES VISUALIZATION
    static void visualizeEmergencyVehicles(const string vehicleIDs[],
        const string locations[], const double lats[], const double lons[],
        const int types[], const bool available[], int count) {
        if (count == 0) {
            cout << "No emergency vehicles to visualize!" << endl;
            return;
        }

        sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
            "Emergency Vehicles");

        // Find bounds
        double minLat = lats[0], maxLat = lats[0];
        double minLon = lons[0], maxLon = lons[0];

        for (int i = 1; i < count; i++) {
            if (lats[i] != 0 && lons[i] != 0) {
                if (lats[i] < minLat) minLat = lats[i];
                if (lats[i] > maxLat) maxLat = lats[i];
                if (lons[i] < minLon) minLon = lons[i];
                if (lons[i] > maxLon) maxLon = lons[i];
            }
        }

        double latPadding = (maxLat - minLat) * 0.1;
        double lonPadding = (maxLon - minLon) * 0.1;
        minLat -= latPadding; maxLat += latPadding;
        minLon -= lonPadding; maxLon += lonPadding;

        sf::Font font;
        bool fontLoaded = font.loadFromFile("arial.ttf");

        sf::Color vehicleColors[] = {
            sf::Color::Red,         // Ambulance
            sf::Color(255, 100, 0), // Fire Truck
            sf::Color::Blue,        // Police
            sf::Color::Yellow       // Rescue
        };

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(sf::Color(255, 240, 240)); // Light red background

            // Title
            if (fontLoaded) {
                sf::Text title("Emergency Vehicles", font, 28);
                title.setPosition(10, 10);
                title.setFillColor(sf::Color(150, 0, 0));
                title.setStyle(sf::Text::Bold);
                window.draw(title);
            }

            // Draw vehicles
            for (int i = 0; i < count; i++) {
                if (lats[i] == 0 || lons[i] == 0) continue;

                sf::Vector2f pos = latLonToScreen(lats[i], lons[i],
                    minLat, maxLat, minLon, maxLon);

                // Vehicle icon
                sf::RectangleShape vehicle(sf::Vector2f(35, 20));
                vehicle.setPosition(pos.x - 17, pos.y - 10);
                if (types[i] >= 0 && types[i] < 4) {
                    vehicle.setFillColor(vehicleColors[types[i]]);
                }
                else {
                    vehicle.setFillColor(sf::Color::Magenta);
                }
                if (!available[i]) {
                    vehicle.setFillColor(sf::Color(150, 150, 150));
                }
                vehicle.setOutlineColor(sf::Color::Black);
                vehicle.setOutlineThickness(2);
                window.draw(vehicle);

                // Availability indicator
                sf::CircleShape indicator(5);
                indicator.setPosition(pos.x + 20, pos.y - 10);
                indicator.setFillColor(available[i] ? sf::Color::Green : sf::Color::Red);
                window.draw(indicator);

                // Vehicle ID
                if (fontLoaded) {
                    sf::Text vehicleText(vehicleIDs[i], font, 11);
                    vehicleText.setPosition(pos.x - 15, pos.y - 30);
                    vehicleText.setFillColor(sf::Color::Black);
                    vehicleText.setStyle(sf::Text::Bold);
                    window.draw(vehicleText);
                }
            }

            window.display();
        }
    }
};

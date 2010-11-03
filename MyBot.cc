#include <iostream>
#include <fstream>
#include <algorithm>
#include "PlanetWars.h"
#include "PlanetScore.h"
using namespace std;

double compute_score(int growth_rate, int num_ships, int distance) {
  if(num_ships == 0) num_ships = 1;
  if(distance == 0) distance = 1;
  double growth_rate_score = 40 * growth_rate;
  double num_ships_score = 5000 / num_ships / 2;
  double distance_score = 1000 / distance;
  return growth_rate_score + num_ships_score + distance_score;
}

void SendShips(const PlanetWars& pw, int my_planet, int my_planet_ships) {
  // (1) Find most interesting planets to send fleets
  vector<Planet> not_my_planets = pw.NotMyPlanets();
  vector<PlanetScore> not_my_planets_score;
  for(vector<Planet>::const_iterator it = not_my_planets.begin();
      it < not_my_planets.end(); ++it) {
    int num_ships = it->NumShips();
    int growth_rate = it->GrowthRate();
    int distance = pw.Distance(it->PlanetID(), my_planet);
    double score = compute_score(growth_rate, num_ships, distance);
    PlanetScore* ps = new PlanetScore(*it, score);
    not_my_planets_score.push_back(*ps);
  }
  sort(not_my_planets_score.begin(), not_my_planets_score.end());

  // (2) Send ships from my planet to the weakest planets that I don't own
  for(vector<PlanetScore>::iterator it = not_my_planets_score.begin();
      it < not_my_planets_score.end(); ++it) {
    Planet p = *(it->GetPlanet());
    if(my_planet_ships > p.NumShips() + 5) {
      pw.IssueOrder(my_planet, p.PlanetID(), p.NumShips()+1);
      my_planet_ships -= p.NumShips();
    }
    else break;
  }
}

void DoTurn(const PlanetWars& pw, int turn) {
  // Per each of my planets try to send ships to other planets
  //ofstream myfile;
  //myfile.open("output",ios::app);
  int strongest_planet;
  int strongest_planet_ships;
  vector<Planet> my_planets = pw.MyPlanets();
  for (int i = 0; i < my_planets.size(); ++i) {
    SendShips(pw, my_planets[i].PlanetID(), my_planets[i].NumShips());
  }
  //myfile.close();
}


// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main(int argc, char *argv[]) {
  std::string current_line;
  std::string map_data;
  int turn = 0;
  ofstream myfile;
  //  myfile.open("output");
  //  myfile.close();
  while (true) {
    int c = std::cin.get();
    current_line += (char)c;
    if (c == '\n') {
      if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
	turn++;
        PlanetWars pw(map_data);
        map_data = "";
        DoTurn(pw, turn);
	pw.FinishTurn();
      } else {
        map_data += current_line;
      }
      current_line = "";
    }
  }
  return 0;
}
#ifndef CITY_HPP
#define CITY_HPP

#include <string>

#include "../utils.hpp"
#include "../province/province.hpp"

class City {
public:
  typedef struct {
    std::string name;
    Province &province;
    vec2f position;
    int population;
    int wealth;
    int food;
    int industry;
  } Data;

  City(Data data) : data(data) {}
  ~City() = default;

  std::string getName() const { return data.name; }
  Province &getProvince() const { return data.province; }
  vec2f getPosition() const { return data.position; }
  int getPopulation() const { return data.population; }
  int getWealth() const { return data.wealth; }
  int getFood() const { return data.food; }
  int getIndustry() const { return data.industry; }

private:
  Data data;
};

#endif // CITY_HPP

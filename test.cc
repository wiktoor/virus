#include "virus_genealogy.h"
using namespace std;

#include <iterator>
#include <iostream>

class Virus {
  public:
    using id_type = std::string;
    Virus(id_type const &_id) : id(_id) {
    }
    id_type get_id() const {
      return id;
    }
  private:
    id_type id;
};

int main() {
    VirusGenealogy<Virus> gen("1");
    auto x = gen["1"];
    cout << gen.exists("1") << endl;
}
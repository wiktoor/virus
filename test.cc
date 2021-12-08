#include "virus_genealogy.h"
using namespace std;

#include <iterator>
#include <iostream>

template<typename T>
requires bidirectional_iterator<T>
void vectorBidirectional(T it, T end) {
    for (; it != end; ++it) cout << *it;
}

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
    vector<int> v({1, 2, 3});
    vector<int>::iterator it = v.begin();
    vector<int>::iterator end = v.end();
    vectorBidirectional(it, end);

    VirusGenealogy<Virus> gen("1");
    gen.print("1");
    cout << gen.get_stem_id() << endl;
}
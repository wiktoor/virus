#include "virus_genealogy.h"
using namespace std;

#include <iterator>
#include <iostream>
#include <vector>
#include <concepts>
#include <type_traits>

class Virus {
  public:
    using id_type = std::string;
    Virus(id_type const &_id) : id(_id) {
    }
    id_type get_id() const {
      return id;
    }
    ~Virus() {
      cout << "Usunięty: " << id << endl;
    }
  private:
    id_type id;
};

template <typename T>
concept correctType = 
requires (T x) {
  { *x } -> same_as<const Virus &>;
};

template <typename T>
concept correctChildrenIterator =  bidirectional_iterator<T> /* && correctType<T> */;

template <typename T>
requires correctChildrenIterator<T>
void f(T x) { }

int main() {
    VirusGenealogy<Virus> gen("1");
    f(gen.get_children_begin("1"));
    if (gen.get_children_begin("1") == gen.get_children_begin("1")) {
      cout << "-------------------ok-------------------------------\n";
    }
    //auto x = gen["1"];
    //cout << gen.exists("1") << endl;
    gen.create("2", "1");
    gen.create("3", "1");
    gen.create("4", "2");
    gen.create("5", "2");
    gen.create("6", vector<typename Virus::id_type>{"2", "3"});
    gen.create("7", "4");
    gen.printTree("1");
    /*size_t size;
    VirusGenealogy<Virus>::children_iterator ch_it;
    for (size = 0, ch_it = gen.get_children_begin("1");
         ch_it != gen.get_children_end("1");
         ++size, ++ch_it) 
    {
      cout << ch_it->get_id() << endl;
    }
    gen.connect("2", "3");
    cout << endl;
    for (size = 0, ch_it = gen.get_children_begin("3");
         ch_it != gen.get_children_end("3");
         ++size, ++ch_it) 
    {
      cout << ch_it->get_id() << endl;
    }*/
    gen.remove("2");
    cout << "------------------------------------------------------\n";
    gen.printTree("1");
    try {
      //gen.create("3", "6");
      gen.remove("1");
      gen.remove("2");
    }
    catch (std::exception &e) {
      cout << e.what() << endl;
    }
    catch (...) {
      cout << "halo\n";
    }
}
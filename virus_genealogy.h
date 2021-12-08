#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <vector>
#include <memory>
#include <map>
#include <iostream> // do usunięcia

template<typename Virus>
class VirusGenealogy {
    private:
        Virus::id_type stem_id;
        class Node {
            private:
                std::shared_ptr<Virus> virus_ptr;
                std::vector<typename Virus::id_type> parents;
                std::vector<std::shared_ptr<Virus>> children;
            public:
                Node(const Virus::id_type &id) : virus_ptr(std::make_shared<Virus>(id)) {}
                // funkcje do testowania
                void printNode() {
                    std::cout << virus_ptr->get_id() << std::endl;
                    for (auto id : parents) std::cout << id << " ";
                    std::cout << std::endl;
                    for (auto ptr : children) std::cout << ptr->get_id() << " ";
                    std::cout << std::endl;
                }
        };
        std::map<typename Virus::id_type, Node> nodes;
    public:
        VirusGenealogy(Virus::id_type const &stem_id) : stem_id(stem_id) {
            nodes.emplace(stem_id, Node(stem_id));
        }
        Virus::id_type get_stem_id() const {
            return stem_id;
        }
        // funkcje do testowania
        void print(Virus::id_type const &id) {
            nodes.at(id).printNode();
        }
};

#endif /* VIRUS_GENEALOGY_H */
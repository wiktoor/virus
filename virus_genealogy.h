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
        struct Node {
            Virus virus;
            std::vector<typename Virus::id_type> parents;
            std::vector<std::shared_ptr<Node>> children;
            Node(const Virus::id_type &id) : virus(Virus(id)) {}
        };
        std::map<typename Virus::id_type, Node> nodes;
    public:
        // using children_iterator = std::vector<std::shared_ptr<Virus>>::iterator;
        VirusGenealogy(Virus::id_type const &stem_id) : stem_id(stem_id) {
            nodes.emplace(stem_id, Node(stem_id));
        }
        Virus::id_type get_stem_id() const {
            return stem_id;
        }
        std::vector<typename Virus::id_type> get_parents(Virus::id_type const &id) const {
            // TODO: sprawdzić, czy jest w tej mapie
            return nodes.at(id).parents;
        }
        bool exists(Virus::id_type const &id) const {
            return nodes.contains(id);
        }
        const Virus& operator[](typename Virus::id_type const &id) const {
            // TODO: sprawdzić, czy jest w tej mapie
            return nodes.at(id).virus;
        }
        // void create(Virus::id_type const &id, Virus::id_type const &parent_id) {
        //     Node node(id);
        //     Node parent = nodes.at(parent_id);
        //     //auto ptr = std::weak_ptr<Node>(parent);
        //     // node.parents.push_back(std::weak_ptr<Node>(nodes.at(parent_id)));
        // }
};

#endif /* VIRUS_GENEALOGY_H */
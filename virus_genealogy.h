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
            public:
                Virus virus;
                std::vector<std::weak_ptr<Node>> parents;
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
            Node node = nodes.at(id);
            std::vector<typename Virus::id_type> result;
            for (auto parent : node.parents) {
                result.push_back(parent->virus.get_id());
            }
            return result;
        }
};

#endif /* VIRUS_GENEALOGY_H */
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
            std::vector<std::shared_ptr<Virus>> children;
            Node(const Virus::id_type &id) : virus(Virus(id)) {}
        };
        std::map<typename Virus::id_type, Node> nodes;
    public:
        class children_iterator {
            private:
                std::shared_ptr<std::vector<std::shared_ptr<Virus>>> vec;
                size_t position;
            public:
                children_iterator() {}
                children_iterator(const std::vector<std::shared_ptr<Virus>>& vec, size_t position) : 
                    vec(std::make_shared<std::vector<std::shared_ptr<Virus>>>(vec)), position(position) {}
                children_iterator& operator=(const children_iterator& it) {
                    position = it.position;
                    vec = it.vec;
                    return *this;
                }
                Virus& operator*() const { return *(vec->at(position)); }
                std::shared_ptr<Virus> operator->() { return vec->at(position); }
                children_iterator& operator++() { position++; return *this; }
                children_iterator operator++(int) { children_iterator tmp = *this; ++(*this); return tmp; }
                children_iterator& operator--() { position--; return *this; }
                children_iterator operator--(int) { children_iterator tmp = *this; --(*this); return tmp; }
                bool operator==(const children_iterator& a) { return this->position == a.position; }
                bool operator!=(const children_iterator& a) { return !(*this == a); }
        };
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
        VirusGenealogy<Virus>::children_iterator get_children_begin(Virus::id_type const &id) const {
            return children_iterator(nodes.at(id).children, 0);
        }
        VirusGenealogy<Virus>::children_iterator get_children_end(Virus::id_type const &id) const {
            return children_iterator(nodes.at(id).children, nodes.at(id).children.size());
        }
        void create(Virus::id_type const &id, Virus::id_type const &parent_id) {
            // TODO: co jezeli klucz id jest juz w mapie albo nie ma parent_id
            nodes.emplace(id, Node(id));
            nodes.at(id).parents.push_back(parent_id);
            nodes.at(parent_id).children.push_back(make_shared<Virus>(nodes.at(id).virus));
        }
};

#endif /* VIRUS_GENEALOGY_H */
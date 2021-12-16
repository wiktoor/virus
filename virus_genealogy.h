#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <exception>
#include <iostream> // do usunięcia

struct VirusAlreadyCreated : public std::exception {
    const char* what() const throw() {
        return "VirusAlreadyCreated";
    }
};

struct VirusNotFound : public std::exception {
    const char* what() const throw() {
        return "VirusNotFound";
    }
};

struct TriedToRemoveStemVirus : public std::exception {
    const char* what() const throw() {
        return "TriedToRemoveStemVirus";
    }
};

template<typename Virus>
class VirusGenealogy {
private:

    Virus::id_type stem_id;

    using Virus_ptr_t = std::shared_ptr<Virus>;
    using Virus_ptrs_t = std::vector<Virus_ptr_t>;
    // USINGI!!!

    struct Node {

        Virus_ptr_t virus_ptr = nullptr;
        std::vector<typename Virus::id_type> parents;
        Virus_ptrs_t children;

        // Jeśli make_shared lub konstruktor wirusa rzuci wyjątek, virus_ptr będzie
        // równy nullptr.
        Node(const Virus::id_type &id) : virus_ptr(std::make_shared<Virus>(id)) {}

        // funkcje do testowania
        void printNode() {
            std::cout << "Jestem wierzchołkiem: " << virus_ptr->get_id() << std::endl;
            std::cout << "Moi rodzice to: " << std::endl;
            for (auto parent : parents) {
                std::cout << parent << " ";
            }
            std::cout << std::endl << "Moi synowie to: " << std::endl;
            for (auto ptr : children) {
                std::cout << ptr->get_id() << " ";
            }
            std::cout << std::endl << std::endl;
        }

    };

    std::map<typename Virus::id_type, Node> nodes;

public:

    class children_iterator {
    private:

        std::shared_ptr<Virus_ptrs_t> vec;
        size_t position;

    public:

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Virus;
        using difference_type = std::ptrdiff_t;
        using pointer = std::shared_ptr<Virus>;
        using reference = const Virus &;

        children_iterator() {}

        children_iterator(const Virus_ptrs_t &vec, size_t position) :
            vec(std::make_shared<Virus_ptrs_t>(vec)), position(position) {}

        children_iterator& operator=(const children_iterator &it) {
            position = it.position;
            vec = it.vec;
            return *this;
        }

        const Virus& operator*() const {
            return *(vec->at(position));
        }

        std::shared_ptr<Virus> operator->() {
            return vec->at(position);
        }

        children_iterator& operator++() {
            position++;
            return *this;
        }

        children_iterator operator++(int) {
            children_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        children_iterator& operator--() {
            position--;
            return *this;
        }

        children_iterator operator--(int) {
            children_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const children_iterator &a) const {
            return this->position == a.position && *(this->vec) == *(a.vec);
        }

        bool operator!=(const children_iterator &a) const {
            return !(*this == a);
        }

    };

    VirusGenealogy(const Virus::id_type &stem_id) : stem_id(stem_id) {
        nodes.emplace(stem_id, Node(stem_id));
    }

    Virus::id_type get_stem_id() const {
        return stem_id;
    }

    // Pozmieniać na iteratory, bo kilka odwołań do tego samego klucza.
    std::vector<typename Virus::id_type> get_parents(Virus::id_type const &id) const {
        if (!nodes.contains(id)) throw VirusNotFound();
        return nodes.at(id).parents;
    }

    bool exists(Virus::id_type const &id) const {
        return nodes.contains(id);
    }

    const Virus& operator[](typename Virus::id_type const &id) const {
        if (!nodes.contains(id))
            throw VirusNotFound();

        return nodes.at(id).virus;
    }

    VirusGenealogy<Virus>::children_iterator get_children_begin(Virus::id_type const &id) const {
        if (!nodes.contains(id))
            throw VirusNotFound();

        return children_iterator(nodes.at(id).children, 0);
    }

    VirusGenealogy<Virus>::children_iterator get_children_end(Virus::id_type const &id) const {
        if (!nodes.contains(id))
            throw VirusNotFound();

        return children_iterator(nodes.at(id).children, nodes.at(id).children.size());
    }

    void create(Virus::id_type const &id, Virus::id_type const &parent_id) {
        // Jeśli contains rzuci wyjątek, to nie będzie to miało żadnych skutków ubocznych.
        if (nodes.contains(id))
            throw VirusAlreadyCreated();
        if (!nodes.contains(parent_id))
            throw VirusNotFound();

        // Jeśli konstruktor rzuci wyjątek, to nie ma efektu.
        Node node(id);
        node.parents.push_back(parent_id);
        auto parent_it = nodes.find(parent_id);
        (parent_it->second).children.push_back(node.virus_ptr);

        try {
            nodes.emplace(id, Node(id));
        }
        catch (...) {
            // pop_back() jest noexpept na niepustym vectorze
            (parent_it->second).children.pop_back();
            throw;
        }
    }

    void create(typename Virus::id_type const &id, std::vector<typename Virus::id_type> const &parent_ids) {
        if (nodes.contains(id))
            throw VirusAlreadyCreated();
        for (typename Virus::id_type parent : parent_ids) {
            if (!nodes.contains(parent))
                throw VirusNotFound();
        }

        Node node(id);
        for (typename Virus::id_type parent_id : parent_ids) {
            node.parents.push_back(parent_id);
        }

        std::vector<typename std::map<typename Virus::id_type, Node>::iterator> parents_it(parent_ids.size());
        size_t next_id = 0;
        try {
            for (; next_id < parent_ids.size(); next_id++) {
                parents_it[next_id] = nodes.find(parent_ids[next_id]);
                // czy -> rzuca wyjątek?
                (parents_it[next_id]->second).children.push_back(node.virus_ptr);
            }
        }
        catch (...) {
            for (size_t j = 0; j < next_id; j++) {
                (parents_it[j]->second).children.pop_back();
            }
            throw;
        }

        try {
            nodes.emplace(id, Node(id));
        }
        catch (...) {
             for (size_t j = 0; j < parent_ids.size(); j++) {
                (parents_it[j]->second).children.pop_back();
            }
            throw;
        }
    }

    void connect(Virus::id_type const &child_id, Virus::id_type const &parent_id) {
        if (!nodes.contains(child_id) || !nodes.contains(parent_id))
            throw VirusNotFound();

        std::vector<typename Virus::id_type>& parent_ids = nodes.at(child_id).parents;
        if (std::find(parent_ids.begin(), parent_ids.end(), parent_id) != parent_ids.end())
            return;

        parent_ids.push_back(parent_id);
        try {
            nodes.at(parent_id).children.push_back(nodes.at(child_id).virus);
        }
        catch (...) {
            parent_ids.pop_back();
        }
    }

    void remove(Virus::id_type const &id) {
        if (id == stem_id) throw TriedToRemoveStemVirus();
        if (!nodes.contains(id)) throw VirusNotFound();
        Node& node = nodes.at(id);
        // usuwamy krawędzie od rodziców
        for (typename Virus::id_type parent_id : node.parents) {
            Node& parent = nodes.at(parent_id);
            for (size_t i = 0; i < parent.children.size(); i++) {
                if (parent.children[i]->get_id() == id) {
                    swap(parent.children[i], parent.children.back());
                    parent.children.pop_back();
                    break; // bo nie ma multikrawędzi
                }
            }
        }
        for (auto ptr : node.children) {
            Node& child = nodes.at(ptr->get_id());
            auto it = std::find(child.parents.begin(), child.parents.end(), id);
            child.parents.erase(it);
            if (!child.parents.size()) remove(child.virus_ptr->get_id());
        }
        nodes.erase(id);
    }

    // funkcje do testowania
    void printTree(Virus::id_type const &id) {
        Node& node = nodes.at(id);
        node.printNode();
        for (auto ptr : node.children) {
            printTree(ptr->get_id());
        }
    }

};

#endif /* VIRUS_GENEALOGY_H */
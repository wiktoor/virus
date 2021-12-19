#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <vector>
#include <memory>
#include <map>
#include <set>
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

    Virus::id_type stem_id;

    using Virus_ptr_t = std::shared_ptr<Virus>;
    using Virus_ptrs_t = std::set<Virus_ptr_t>;
    using Virus_ids_t = std::set<typename Virus::id_type>;
    // Przydałyby się jeszcze usingi na Virus::id_type (bo typename'y)
    // i na std::vector<typename Virus::id_type>. Tylko używając ich
    // zmieniamy wygląd interfejsu. Na przykład get_parents zwraca
    // std::vector<typename Virus::id_type>, a jeśli zrobimy usinga,
    // to będziemy mieć inną nazwę zwracanego typu. Nie wiem, czy tak można.

    struct Node {

        // Tutaj typ zmienił się na pointer do wirusa. Możliwe, że wcześniej źle
        // źle korzystaliśmy z shared pointerów. Funkcja make_shared tworzy nowy
        // obiekt i zwraca wskaźnik do niego. Czyli nie powinniśmy jej używać,
        // gdy chcemy stworzyć kolejny wskaźnik do już istniejącego obiektu,
        // bo stworzyłaby nam nowy, identyczny obiekt. A przynajmniej w teorii
        // tak mogłoby się stać, bo tego nie wykryliśmy. Tak czy siak, tak jak teraz
        // mamy na pewno jest dobrze. Wywołujemy make_shared raz w konstruktorze
        // Noda, a jak potem chcemy nowy wskaźnik do istniejącego wirusa, to
        // kopiujemy wskaźnik virus_ptr.
        Virus_ptr_t virus_ptr = nullptr;
        Virus_ids_t parents;
        Virus_ptrs_t children;

        // Jeśli make_shared lub konstruktor wirusa rzuci wyjątek, virus_ptr będzie
        // równy nullptr. Nie utworzymy wirusa, wszystko jest ok.
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

    using Nodes_map_t = std::map<typename Virus::id_type, Node>;

    Nodes_map_t nodes;

public:

    class children_iterator {

        Virus_ptrs_t::const_iterator it_to_virus_ptr;

    public:

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Virus;
        using difference_type = std::ptrdiff_t;
        using pointer = Virus_ptr_t;
        using reference = const Virus &;

        children_iterator() {}

        children_iterator(Virus_ptrs_t::const_iterator it) : it_to_virus_ptr(it) {}

        children_iterator& operator=(const children_iterator &other) {
            it_to_virus_ptr = other.it_to_virus_ptr;
            return *this;
        }

        const Virus& operator*() const {
            return **it_to_virus_ptr;
        }

        Virus_ptr_t operator->() {
            return *it_to_virus_ptr;
        }

        children_iterator& operator++() {
            it_to_virus_ptr++;
            return *this;
        }

        children_iterator operator++(int) {
            children_iterator temp = *this;
            ++(*this);
            return temp;
        }

        children_iterator& operator--() {
            it_to_virus_ptr--;
            return *this;
        }

        children_iterator operator--(int) {
            children_iterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const children_iterator &other) const {
            return it_to_virus_ptr == other.it_to_virus_ptr;
        }

        bool operator!=(const children_iterator &other) const {
            return !(*this == other);
        }

    };

    VirusGenealogy(const VirusGenealogy &other) = delete;

    VirusGenealogy& operator=(const VirusGenealogy &other) = delete;

    VirusGenealogy(const Virus::id_type &stem_id) : stem_id(stem_id) {
        nodes.emplace(stem_id, Node(stem_id));
    }

    Virus::id_type get_stem_id() const {
        return stem_id;
    }

    std::vector<typename Virus::id_type> get_parents(Virus::id_type const &id) const {
        auto node_it = nodes.find(id);
        if (node_it == nodes.end())
            throw VirusNotFound();

        std::vector<typename Virus::id_type> result;
        for (typename Virus::id_type parent_id : (node_it->second).parents) {
            result.push_back(parent_id);
        }
        //return std::vector<typename Virus::id_type>((node_it->second).parents.begin(), (node_it->second).parents.end());
        return result;
    }

    bool exists(Virus::id_type const &id) const {
        return nodes.contains(id);
    }

    const Virus& operator[](typename Virus::id_type const &id) const {
        auto node_it = nodes.find(id);
        if (node_it == nodes.end())
            throw VirusNotFound();

        return *((node_it->second).virus_ptr);
    }

    VirusGenealogy<Virus>::children_iterator get_children_begin(Virus::id_type const &id) const {
        auto node_it = nodes.find(id);
        if (node_it == nodes.end())
            throw VirusNotFound();

        return children_iterator((node_it->second).children.begin());
    }

    VirusGenealogy<Virus>::children_iterator get_children_end(Virus::id_type const &id) const {
        auto node_it = nodes.find(id);
        if (node_it == nodes.end())
            throw VirusNotFound();

        return children_iterator((node_it->second).children.end());
    }

    void create(Virus::id_type const &id, Virus::id_type const &parent_id) {
        auto parent_it = nodes.find(parent_id);
        if (nodes.contains(id))
            throw VirusAlreadyCreated();
        if (!nodes.contains(parent_id))
            throw VirusNotFound();

        Node node(id);
        node.parents.insert(parent_id);
        auto it = (parent_it->second).children.insert(node.virus_ptr);

        try {
            nodes.emplace(id, node);
        }
        catch (...) {
            // pop_back() jest noexpept na niepustym vectorze
            (parent_it->second).children.erase(it.first);
            throw;
        }
    }

    void create(typename Virus::id_type const &id, std::vector<typename Virus::id_type> const &parent_ids) {
        std::vector<typename Nodes_map_t::iterator> parent_its;
        for (typename Virus::id_type parent_id : parent_ids) {
            parent_its.push_back(nodes.find(parent_id));
        }

        if (nodes.contains(id))
            throw VirusAlreadyCreated();
        for (auto parent_it : parent_its) {
            if (parent_it == nodes.end())
                throw VirusNotFound();
        }
        if (parent_ids.empty())
            return;

        Node node(id);
        for (typename Virus::id_type parent_id : parent_ids) {
            node.parents.insert(parent_id);
        }

        std::vector<std::pair<typename Virus_ptrs_t::iterator, bool>> its(parent_ids.size());
        try {
            for (size_t next_id = 0; next_id < parent_ids.size(); next_id++) {
                its[next_id] = (parent_its[next_id]->second).children.insert(node.virus_ptr);
            }
            nodes.emplace(id, node);
        }
        catch (...) {
            for (size_t j = 0; j < parent_ids.size(); j++) {
                if (!its[j].second)
                    break;
                (parent_its[j]->second).children.erase(its[j].first);
            }
            throw;
        }
    }

    void connect(Virus::id_type const &child_id, Virus::id_type const &parent_id) {
        auto child_it = nodes.find(child_id);
        auto parent_it = nodes.find(parent_id);

        if (child_it == nodes.end() || parent_it == nodes.end())
            throw VirusNotFound();

        Virus_ids_t& parent_ids = (child_it->second).parents;
        if (parent_ids.find(parent_id) != parent_ids.end())
            return;

        auto parent_id_it = parent_ids.insert(parent_id);
        try {
            (parent_it->second).children.insert((child_it->second).virus_ptr);
        }
        catch (...) {
            parent_ids.erase(parent_id_it.first);
            throw;
        }
    }

    /*void remove(Virus::id_type const &id) {
        auto node_it = nodes.find(id);

        if (id == stem_id)
            throw TriedToRemoveStemVirus();
        if (node_it == nodes.end())
            throw VirusNotFound();

        Nodes_map_t nodes_copy = nodes;

        try {
            remove_edges_from_parents(id);
            remove_descendant(id);
        }
        catch (...) {
            swap(nodes, nodes_copy);
            throw;
        }
    }*/

    void remove(Virus::id_type const &id) {
        auto node_it = nodes.find(id);

        if (id == stem_id)
            throw TriedToRemoveStemVirus();
        if (node_it == nodes.end())
            throw VirusNotFound();

        std::vector<typename Nodes_map_t::iterator> nodes_to_remove;
        std::map<typename Virus::id_type, std::vector<typename Virus_ids_t::iterator>> edges;
        find_nodes_to_remove(id, edges, nodes_to_remove);

        std::vector<typename Nodes_map_t::iterator> parents;
        std::vector<typename Virus_ptrs_t::iterator> id_pos;
        for (typename Virus::id_type parent_id : (node_it->second).parents) {
            parents.push_back(nodes.find(parent_id));
        }
        for (auto parent_it : parents) {
            auto child_it = (parent_it->second).children.begin();
            for (; child_it != (parent_it->second).children.end(); child_it++) {
                if ((*child_it)->get_id() == id) {
                    id_pos.push_back(child_it);
                    break;
                }
            }
        }

        std::vector<typename Nodes_map_t::iterator> nodes_in_subtree;
        std::vector<std::vector<typename Virus_ids_t::iterator>> edges_to_remove;
        for (auto vert : edges) {
            nodes_in_subtree.push_back(nodes.find(vert.first));
            edges_to_remove.push_back(vert.second);
        }

        for (size_t i = 0; i < nodes_in_subtree.size(); i++) {
            for (size_t j = 0; j < edges_to_remove[i].size(); j++) {
                (nodes_in_subtree[i]->second).parents.erase(edges_to_remove[i][j]);
            }
        }
        for (size_t i = 0; i < parents.size(); i++) {
            (parents[i]->second).children.erase(id_pos[i]);
        }
        for (auto it : nodes_to_remove) {
            nodes.erase(it);
        }
    }

private:

    void find_nodes_to_remove(Virus::id_type const &id,
                              std::map<typename Virus::id_type, std::vector<typename Virus_ids_t::iterator>> &edges,
                              std::vector<typename Nodes_map_t::iterator> &nodes_to_remove) {
        nodes_to_remove.push_back(nodes.find(id));

        for (const Virus_ptr_t &virus_ptr : nodes.at(id).children) {
            Node& child = nodes.at(virus_ptr->get_id());
            typename Virus::id_type child_id = child.virus_ptr->get_id();
            if (edges.find(child_id) == edges.end()) {
                edges.emplace(child_id, std::vector<typename Virus_ids_t::iterator>());
            }
            edges.at(child_id).push_back(child.parents.find(id));
            
            if (edges.at(child_id).size() == child.parents.size())
                find_nodes_to_remove(child_id, edges, nodes_to_remove);
        }
    }

    void remove_edges_from_parents(Virus::id_type const &id) {
        for (typename Virus::id_type parent_id : nodes.at(id).parents) {
            Node& parent = nodes.at(parent_id);
            for (size_t i = 0; i < parent.children.size(); i++) {
                if (parent.children[i]->get_id() == id) {
                    swap(parent.children[i], parent.children.back());
                    parent.children.pop_back();
                    break;
                }
            }
        }
    }

    void remove_descendant(Virus::id_type const &id) {
        for (Virus_ptr_t virus_ptr : nodes.at(id).children) {
            Node& child = nodes.at(virus_ptr->get_id());
            child.parents.erase(std::find(child.parents.begin(), child.parents.end(), id));
            if (child.parents.size() == 0)
                remove_descendant(child.virus_ptr->get_id());
        }

        nodes.erase(id);
    }


    // funkcje do testowania
public:    void printTree(Virus::id_type const &id) {
        Node& node = nodes.at(id);
        node.printNode();
        for (auto ptr : node.children) {
            printTree(ptr->get_id());
        }
    }

};

#endif /* VIRUS_GENEALOGY_H */
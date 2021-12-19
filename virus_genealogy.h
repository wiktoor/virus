/*
    Rozwiązanie piątego zadania zaliczeniowego z JNP
    Magdalena Nieszporska nr indeksu 429982
    Patryk Jędrzejczak nr indeksu 429285
    Wiktor Chmielewski nr indeksu 429131
*/

#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <vector>
#include <memory>
#include <map>
#include <set>
#include <exception>

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

    struct Node {

        Virus_ptr_t virus_ptr = nullptr;
        Virus_ids_t parents;
        Virus_ptrs_t children;

        // Jeśli make_shared lub konstruktor wirusa rzuci wyjątek, virus_ptr będzie
        // równy nullptr. Nie utworzymy wirusa, nie będzie żadnych skutków ubocznych.
        Node(const Virus::id_type &id) : virus_ptr(std::make_shared<Virus>(id)) {}

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

    // Jeżeli gdzieś zostanie rzucony wyjątek, to nie dodamy nic do mapy,
    // więc nie będzie żadnych skutków ubocznych.
    VirusGenealogy(const Virus::id_type &stem_id) : stem_id(stem_id) {
        nodes.emplace(stem_id, Node(stem_id));
    }

    // Poniżej nie wprowadzamy żadnych zmian, więc mamy silną odporność na wyjątki.
    Virus::id_type get_stem_id() const {
        return stem_id;
    }

    // Poniżej nie wprowadzamy żadnych zmian, więc mamy silną odporność na wyjątki.
    std::vector<typename Virus::id_type> get_parents(const Virus::id_type &id) const {
        auto node_it = nodes.find(id);
        if (node_it == nodes.end())
            throw VirusNotFound();

        return std::vector<typename Virus::id_type>((node_it->second).parents.begin(), (node_it->second).parents.end());
    }

    // Poniżej nie wprowadzamy żadnych zmian, więc mamy silną odporność na wyjątki.
    bool exists(const Virus::id_type &id) const {
        return nodes.contains(id);
    }

    // Poniżej nie wprowadzamy żadnych zmian, więc mamy silną odporność na wyjątki.
    const Virus& operator[](const typename Virus::id_type &id) const {
        auto node_it = nodes.find(id);
        if (node_it == nodes.end())
            throw VirusNotFound();

        return *((node_it->second).virus_ptr);
    }

    // Poniżej nie wprowadzamy żadnych zmian, więc mamy silną odporność na wyjątki.
    VirusGenealogy<Virus>::children_iterator get_children_begin(const Virus::id_type &id) const {
        auto node_it = nodes.find(id);
        if (node_it == nodes.end())
            throw VirusNotFound();

        return children_iterator((node_it->second).children.begin());
    }

    // Poniżej nie wprowadzamy żadnych zmian, więc mamy silną odporność na wyjątki.
    VirusGenealogy<Virus>::children_iterator get_children_end(const Virus::id_type &id) const {
        auto node_it = nodes.find(id);
        if (node_it == nodes.end())
            throw VirusNotFound();

        return children_iterator((node_it->second).children.end());
    }

    void create(const Virus::id_type &id, const Virus::id_type &parent_id) {
        auto parent_it = nodes.find(parent_id);
        if (nodes.contains(id))
            throw VirusAlreadyCreated();
        if (!nodes.contains(parent_id))
            throw VirusNotFound();

        Node node(id);
        // Do tego momentu nie wprowadzaliśmy żadnych zmian, więc była silna odporność.
        // Poniższy insert jest w lokalnym obiekcie, więc w przypadku rzucenia wyjątku nie musimy go cofać.
        node.parents.insert(parent_id);
        auto insert_result = (parent_it->second).children.insert(node.virus_ptr);

        try {
            // Jeżeli poniższy emplace wyrzuci wyjątek, to musimy cofnąć powyższego inserta.
            nodes.emplace(id, node);
        }
        catch (...) {
            // erase() na secie jest noexcept, gdy parametrem jest iterator
            (parent_it->second).children.erase(insert_result.first);
            throw;
        }
    }

    void create(const typename Virus::id_type &id, const std::vector<typename Virus::id_type> &parent_ids) {
        // Poniższy obiekt jest lokalny, więc w przypadku rzucenia wyjątku nie musimy ich cofać zmian na nim.
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

        // Poniższy obiekt jest lokalny, więc w przypadku rzucenia wyjątku nie musimy ich cofać zmian na nim.
        Node node(id);
        for (typename Virus::id_type parent_id : parent_ids) {
            node.parents.insert(parent_id);
        }

        std::vector<std::pair<typename Virus_ptrs_t::iterator, bool>> insert_results(parent_ids.size());
        try {
            // Jeżeli poniższy kod rzuci wyjątek, to musimy cofnąć zmiany, które zostały przez niego wprowadzone
            for (size_t next_id = 0; next_id < parent_ids.size(); next_id++) {
                insert_results[next_id] = (parent_its[next_id]->second).children.insert(node.virus_ptr);
            }
            nodes.emplace(id, node);
        }
        catch (...) {
            for (size_t j = 0; j < parent_ids.size(); j++) {
                if (!insert_results[j].second)
                    break;
                // erase() na secie jest noexcept, gdy parametrem jest iterator
                (parent_its[j]->second).children.erase(insert_results[j].first);
            }
            throw;
        }
    }

    void connect(const Virus::id_type &child_id, const Virus::id_type &parent_id) {
        auto child_it = nodes.find(child_id);
        auto parent_it = nodes.find(parent_id);

        if (child_it == nodes.end() || parent_it == nodes.end())
            throw VirusNotFound();

        Virus_ids_t& parent_ids = (child_it->second).parents;
        if (parent_ids.find(parent_id) != parent_ids.end())
            return;

        // Do tego momentu wprowadzaliśmy zmiany jedynie na lokalnych obiektach, więc była silna odporność.
        auto parent_id_it = parent_ids.insert(parent_id);
        try {
            // Jeżeli poniższy insert się nie powiedzie, to musimy cofnąć powyższy insert.
            (parent_it->second).children.insert((child_it->second).virus_ptr);
        }
        catch (...) {
            parent_ids.erase(parent_id_it.first);
            throw;
        }
    }

    // Najpierw znajdujemy iteratory do wszystkich usuwanych obiektów (tu może zostać rzucony wyjątek),
    // a potem usuwamy wszystko noexcept korzystając z tych iteratorów.
    void remove(const Virus::id_type &id) {
        if (id == stem_id)
            throw TriedToRemoveStemVirus();
        if (!nodes.contains(id))
            throw VirusNotFound();

        // W poniższym fragmencie kodu znajdujemy iteratory obiektów do usunięcia.
        std::vector<typename Nodes_map_t::iterator> nodes_to_remove;
        std::map<typename Virus::id_type, std::vector<typename Virus_ids_t::iterator>> parent_ids_to_remove;
        find_nodes_to_remove(id, parent_ids_to_remove, nodes_to_remove);

        std::vector<typename Nodes_map_t::iterator> parent_its;
        std::vector<typename Virus_ptrs_t::iterator> position_in_children;
        find_parents_of_node_to_remove(id, parent_its, position_in_children);

        std::vector<typename Nodes_map_t::iterator> affected_nodes;
        std::vector<std::vector<typename Virus_ids_t::iterator>> deleted_parents;
        find_affected_nodes(parent_ids_to_remove, affected_nodes, deleted_parents);

        // Od tego momentu zaczynamy wprowadzać zmiany, ale robimy to noexcept.
        update_parents_of_node_to_remove(parent_its, position_in_children);
        delete_parents_of_affected_nodes(affected_nodes, deleted_parents);
        delete_nodes(nodes_to_remove);
    }

private:

    void find_nodes_to_remove(const Virus::id_type &id,
                              std::map<typename Virus::id_type, std::vector<typename Virus_ids_t::iterator>> &parent_ids_to_remove,
                              std::vector<typename Nodes_map_t::iterator> &nodes_to_remove) {
        nodes_to_remove.push_back(nodes.find(id));

        for (const Virus_ptr_t &virus_ptr : nodes.at(id).children) {
            Node& child = nodes.at(virus_ptr->get_id());
            typename Virus::id_type child_id = child.virus_ptr->get_id();
            if (parent_ids_to_remove.find(child_id) == parent_ids_to_remove.end()) {
                parent_ids_to_remove.emplace(child_id, std::vector<typename Virus_ids_t::iterator>());
            }

            parent_ids_to_remove.at(child_id).push_back(child.parents.find(id));
            
            if (parent_ids_to_remove.at(child_id).size() == child.parents.size())
                find_nodes_to_remove(child_id, parent_ids_to_remove, nodes_to_remove);
        }
    }

    void find_parents_of_node_to_remove(const Virus::id_type &id,
                                        std::vector<typename Nodes_map_t::iterator> &parent_its,
                                        std::vector<typename Virus_ptrs_t::iterator> &position_in_children) {
        for (typename Virus::id_type parent_id : nodes.at(id).parents) {
            parent_its.push_back(nodes.find(parent_id));
        }
        for (auto parent_it : parent_its) {
            auto child_it = (parent_it->second).children.begin();
            for (; child_it != (parent_it->second).children.end(); child_it++) {
                if ((*child_it)->get_id() == id) {
                    position_in_children.push_back(child_it);
                    break;
                }
            }
        }
    }

    void find_affected_nodes(const std::map<typename Virus::id_type, std::vector<typename Virus_ids_t::iterator>> &parent_ids_to_remove,
                             std::vector<typename Nodes_map_t::iterator> &affected_nodes,
                             std::vector<std::vector<typename Virus_ids_t::iterator>> &deleted_parents) {
        for (auto affected_node : parent_ids_to_remove) {
            affected_nodes.push_back(nodes.find(affected_node.first));
            deleted_parents.push_back(affected_node.second);
        }
    }

    void delete_parents_of_affected_nodes(const std::vector<typename Nodes_map_t::iterator> &affected_nodes,
                                          const std::vector<std::vector<typename Virus_ids_t::iterator>> &deleted_parents) {
        for (size_t i = 0; i < affected_nodes.size(); i++) {
            for (size_t j = 0; j < deleted_parents[i].size(); j++) {
                (affected_nodes[i]->second).parents.erase(deleted_parents[i][j]);
            }
        }
    }

    void update_parents_of_node_to_remove(const std::vector<typename Nodes_map_t::iterator> &parent_its,
                                          const std::vector<typename Virus_ptrs_t::iterator> &position_in_children) {
        for (size_t i = 0; i < parent_its.size(); i++) {
            (parent_its[i]->second).children.erase(position_in_children[i]);
        }
    }

    void delete_nodes(const std::vector<typename Nodes_map_t::iterator> &nodes_to_remove) {
        for (auto it : nodes_to_remove) {
            nodes.erase(it);
        }
    }

};

#endif /* VIRUS_GENEALOGY_H */